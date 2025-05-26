# Low Power Configuration

## Power Consumption

For CH570/2:

- In sleep mode with RAM retention, the MCU consumes approximately **0.9 µA**.  
- In sleep mode with LSI, RTC and RAM retention, it consumes arnoud **1.2 µA**.
- In shutdown mode with all functions disabled, it consumes about **0.6 µA**.

CH570 and CH572 have identical low-power modes and configuration options.

## Pin Configuration

This apply to all WCH chips.

**Do not leave any pins floating.**  
A floating pin with digital input enabled can draw extra current due to the internal Schmitt trigger.

To avoid unnecessary power consumption, you can either:

1. **Enable all internal pull-up resistors:**

2. **Use external pull-up resistors.**

3. **Disable all digital functions on the pins:**

After performing one of the above configurations, you can set up pin functions as needed for your application.

---

## CH570/572 LDO Configuration

The CH570 and CH572 include an integrated 5V LDO. Configure `POWERED_BY_V5PIN` in the funconfig.h according to which pin you use to power the MCU. 

### If powered via the V5 pin

`POWERED_BY_V5PIN = 1` will set the `RB_PWR_LDO5V_EN` bit (in safe access):

---

### If powered via the VCC pin

No additional setting required.

---

If not configured correctly, the sleep current may increase by approximately 100 µA.
Although connecting a 1 MΩ resistor between VCC and V5 can help reduce this excess current, this workaround is not recommended.

## System clock frequency


For CH570/2, if the clock frequency is below 60 Mhz, one can simply configure `R8_SLP_POWER_CTRL`, `R16_POWER_PLAN` and then call `__WFI()`.

**For higher clock frequency like 100 Mhz, the flash will become unstable after waking up**

Either lower the clock frequency before going into sleep mode, or let the code run in ram until flash is stable. Even when `RB_WAKE_DLY_MOD = 0x40`, another 40 µs or so is required before code can run in flash. Longer time is required for other parts like systick. So don't use `Delay_Us()` immedately, run some `__NOP()` in loops before you do anything else.

## Unofficial test results

- For CH570/2, datasheet asked for a 1.5k resistor between VCC and V5. Preliminary tests suggest that it is not necessary to do so. The MCU works fine without.
