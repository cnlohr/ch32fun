/*
  Flash library for v20x/v30x. Use the following functions:

  * ch20x_30x_flash_cmd_read(): Read from flash
  * ch20x_30x_flash_cmd_write(): Write to flash
  * ch20x_30x_flash_cmd_erase(): Erase flash
  * ch20x_30x_flash_cmd_verify(): Verify written data.

  Flash erase operations must be in page granularity: addr must be page aligned,
  and length must be multiple of the page size (256 bytes).

  For flash write operations, addr must be page aligned, and both buf and len
  must be aligned to 32 bits.

  Read and verify operations do not have alignment restrictions.
*/

#include <stdint.h>
#include <string.h> // For memcmp()

#include "ch32fun.h"

#define CH20X_30X_FLASH_PAGE_LEN 256
#define CH20X_30X_FLASH_PAGE_U32LEN (CH20X_30X_FLASH_PAGE_LEN / sizeof(uint32_t))
#define CH20X_30X_FLASH_ALIGN_MASK (CH20X_30X_FLASH_PAGE_LEN - 1)

enum {
	FLASH_OK = 0,           // Success
	FLASH_ERR_PARAM,        // Parameter error (check alignment!)
	FLASH_ERR_CLK,          // Clock is too high (use HPRE or lower system clock below 120MHz)
	FLASH_ERR_VERIFY,       // Verify has failed
	FLASH_ERR_PROTECT,	// Write protection error
};

void _ch20x_30x_flash_unlock(void)
{
	__disable_irq();
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;

	FLASH->MODEKEYR = FLASH_KEY1;
	FLASH->MODEKEYR = FLASH_KEY2;
}

void _ch20x_30x_flash_lock(void)
{
	FLASH->CTLR = FLASH_CTLR_LOCK | FLASH_CTLR_FAST_LOCK;
	__enable_irq();
}

void ch20x_30x_flash_cmd_read(uintptr_t addr, uint8_t *buf, uint32_t len)
{
	memcpy(buf, (void*)addr, len);
}

uint32_t _ch20x_30x_flash_busy_poll(void)
{
	while (FLASH->STATR & FLASH_STATR_BSY);

	const uint32_t err = FLASH->STATR & FLASH_STATR_WRPRTERR;
	// Clear status flags (EOP/WRPRTERR)
	FLASH->STATR = FLASH_STATR_EOP | FLASH_STATR_WRPRTERR;

	return err ? FLASH_ERR_PROTECT : FLASH_OK;
}

// Writes one page at most
uint32_t _ch20x_30x_flash_page_write(uintptr_t addr, const uint32_t *data, uint32_t u32len)
{
	volatile uint32_t *dst = (volatile uint32_t*)addr;

	FLASH->CTLR = CR_PAGE_PG;  // FTPG, quick program 64-dword page
	// Write data to buffer (must point to appropriate address
	for (uint_fast8_t i = 0; i < u32len; i++) {
		dst[i] = data[i];
		while (FLASH->STATR & 0x02); // WRBSY
	}

	// Perform buffer to flash program
	FLASH->CTLR = CR_PAGE_PG | CR_PG_STRT;

	// Busy poll and return
	return _ch20x_30x_flash_busy_poll();
}

// addr must be page aligned (256 bytes).
// buf and len must be aligned to 32 bits
uint32_t ch20x_30x_flash_cmd_write(uintptr_t addr, const uint8_t *buf, uint32_t len )
{
	uint32_t err = FLASH_OK;

	// Check SYSCLK / HB prescaler is not greater than 120 MHz
#if (FUNCONF_SYSTEM_CORE_CLOCK > 120000000)
	if (!(RCC->CFGR0 & RCC_HPRE_3)) {
		return FLASH_ERR_CLK;
	}
#endif
	if (addr & CH20X_30X_FLASH_ALIGN_MASK || (uintptr_t)buf & 3 || len & 3) {
		return FLASH_ERR_PARAM;
	}

	_ch20x_30x_flash_unlock();
	while (!err && len) {
		const uint32_t to_write = len < CH20X_30X_FLASH_PAGE_LEN ?
			len : CH20X_30X_FLASH_PAGE_LEN;
		err = _ch20x_30x_flash_page_write(addr, (uint32_t*)buf,
				to_write / sizeof(uint32_t));
		addr += to_write;
		buf += to_write;
		len -= to_write;
	}
	FLASH->CTLR = 0;  // Clear FTPG
	_ch20x_30x_flash_lock();

	return err;
}

uint32_t _ch20x_30x_flash_page_erase(uintptr_t addr)
{
	FLASH->CTLR = CR_PAGE_ER; // FTER, erase a full page
	FLASH->ADDR = addr;
	FLASH->CTLR = CR_STRT_Set | CR_PAGE_ER;

	// Busy poll and return
	return _ch20x_30x_flash_busy_poll();
}

// addr and len must be page aligned (256 bytes)
uint32_t ch20x_30x_flash_cmd_erase(uintptr_t addr, uint32_t len)
{
	uint32_t err = FLASH_OK;

	// Check SYSCLK / HB prescaler is not greater than 120 MHz
#if (FUNCONF_SYSTEM_CORE_CLOCK > 120000000)
	if (!(RCC->CFGR0 & RCC_HPRE_3)) {
		return FLASH_ERR_CLK;
	}
#endif
	if (addr & CH20X_30X_FLASH_ALIGN_MASK || len & CH20X_30X_FLASH_ALIGN_MASK) {
		return FLASH_ERR_PARAM;
	}

	const uint32_t pages = len / CH20X_30X_FLASH_PAGE_LEN;
	_ch20x_30x_flash_unlock();
	for (uint32_t i = 0; !err && i < pages; i++) {
		err = _ch20x_30x_flash_page_erase(addr);
		addr += CH20X_30X_FLASH_PAGE_LEN;
	}
	FLASH->CTLR = 0; // Clear FTER
	_ch20x_30x_flash_lock();

	return err;
}

uint32_t ch20x_30x_flash_cmd_verify(uintptr_t addr, uint8_t *buf, int len)
{
	return memcmp((void*)addr, buf, len) ? FLASH_ERR_VERIFY : FLASH_OK;
}
