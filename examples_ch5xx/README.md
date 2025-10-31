# CH5xx examples

The CH5xx examples currently focus on the BLE chips 57x, 58x and 59x, but it is definitely possible to add support for other RISC-V models in the ch5xx range.\

The following table details which demo is working on which chip, since some functionality is still being implemented:

|                     | ch570/2 | ch571/3 | ch582/3 | ch584/5 | ch59x |
|---------------------|---------|---------|---------|---------|-------|
| blink               |    √    |    √    |    √    |    √    |   √   |
| ble_beacon          |    √    |    ×    |    √    |    √    |   √   |
| boot_button         |    √    |    ×    |    √    |    √    |   √   |
| comparator_adc_demo |    √    |    ×    |    ×    |    ×    |   ×   |
| debugprintfdemo     |    √    |    √    |    √    |    √    |   √   |
| iSLER               |    √    |    ×    |    √    |    √    |   √   |
| lowpower            |    √    |    ×    |    √    |    √    |   √   |
| RTC_irq             |    √    |    √    |    √    |    √    |   √   |
| systick_irq         |    √    |    ×    |    √    |    √    |   √   |
| uartdemo            |    √    |    √    |    √    |    √    |   √   |
| ws2812bdemo         |    √    |    √    |    √    |    √    |   √   |
| spi_tx              |    ?    |    √    |    ?    |    ?    |   ?   |

The Makefiles default to ch570, if you want to compile for another chip only the `TARGET_MCU` and `TARGET_MCU_PACKAGE` need to be changed.
