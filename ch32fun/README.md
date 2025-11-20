## Update Status Overview
|PERIPHERAL    |V003|V00x|V10x|V205|V20x|V30x|X035(643\*)|L103(M103\*)|M030| 641|
|:-------------|:--:|:--:|:--:|:--:|:--:|:--:|:---------:|:----------:|:--:|:--:|
|DPAL Header\* |2.0 | ×  |2.7 | ×  | ×  | ×  | √         |1.5         | ×  |1.3 |
|ADC           |1.9 | ×  |2.1 | ×  | ×  | ×  |1.3        |1.5         | ×  | √  |
|AWU           |N/A |N/A |N/A |N/A |N/A |N/A | √         |N/A         |N/A |N/A |
|BKP           |N/A |N/A |2.1 | ×  | ×  | ×  |N/A        |1.5         |N/A |N/A |
|CAN           |N/A |N/A |N/A | ×  | ×  | ×  |N/A        |1.5         |N/A |N/A |
|CRC           |N/A |N/A |2.1 | ×  | ×  | ×  |N/A        |1.5         |N/A |N/A |
|DAC           |N/A |N/A |N/A |N/A |N/A | ×  |N/A        |N/A         |N/A |N/A |
|DBGMCU        |1.5 | ×  |2.1 | ×  | ×  | ×  | √         |1.5         | ×  |1.2 |
|DMA           | √  | ×  | √  | ×  | ×  | ×  | √         |1.5         | ×  | √  |
|DVP           |N/A |N/A |N/A |N/A |N/A | ×  |N/A        |N/A         |N/A |N/A |
|ETH           |N/A |N/A |N/A |N/A |N/A | ×  |N/A        |N/A         |N/A |N/A |
|EXIT          | √  | ×  |2.4 | ×  | ×  | ×  | √         |1.5         | ×  | √  |
|FLASH         | √  | ×  |2.7 | ×  | ×  | ×  |1.4        |1.5         | ×  |1.1 |
|FSMC          |N/A |N/A |N/A | ×  |N/A | ×  |N/A        |N/A         |N/A |N/A |
|GPIO          |2.0 | ×  |2.7 | ×  | ×  | ×  |1.6        |1.5         | ×  |1.2 |
|I2C           | √  | ×  | √  | ×  | ×  | ×  |1.7        |1.5         | ×  |1.2 |
|IWDG          | √  | ×  | √  | ×  | ×  | ×  | √         |1.5         |N/A |N/A |
|LPTIM         |N/A |N/A |N/A |N/A |N/A |N/A |N/A        |1.5         |N/A |N/A |
|MISC          | √  | ×  |2.4 |N/A | ×  | ×  |1.6        |1.5         |N/A |1.1 |
|OPA           | √  | ×  |N/A | ×  | ×  | ×  |1.3        |1.5         | ×  |N/A |
|PWR           |1.9 | ×  |2.6 | ×  | ×  | ×  |1.7        |1.5         | ×  | √  |
|QSPI          |N/A |N/A |N/A | ×  |N/A |N/A |N/A        |N/A         |N/A |N/A |
|RCC           |1.8 | ×  |2.7 | ×  | ×  | ×  | √         |1.5         | ×  |1.1 |
|RNG           |N/A |N/A |N/A |N/A |N/A | ×  |N/A        |N/A         |N/A |N/A |
|RTC           |N/A |N/A | √  | ×  | ×  | ×  |N/A        |1.5         |N/A |N/A |
|SPI           |1.9 | ×  |2.7 | ×  | ×  | ×  |1.7        |1.5         | ×  |N/A |
|TIM           |1.6 | ×  | √  | ×  | ×  | ×  | √         |1.5         | ×  | √  |
|TKEY          |N/A |N/A |N/A | ×  |N/A |N/A |N/A        |N/A         |N/A |N/A |
|USART         | √  | ×  |2.4 | ×  | ×  | ×  | √         |1.5         | ×  | √  |
|USB           |N/A |N/A | √  | ×  | ×  | ×  |1.8        |1.5         | ×  |N/A |
|USB_HOST      |N/A |N/A | √  |N/A |N/A |N/A |N/A        |N/A         |N/A |N/A |
|USBPD         |N/A |N/A |N/A |N/A |N/A |N/A |1.4        |1.5         | ×  |1.2 |
|WWWDG         | √  | ×  | √  | ×  | ×  | ×  | √         |1.5         | ×  | √  |
|**chxxxhw.h** | √  | ×  | √  | ×  | √  | √  | √         | √          | ×  | √  |
|**minichlink**| √  | ×  | +  | ×  | √  | √  | √  ( √   )| √  ( ×    )| ×  | √  |

* n.m:  Last commit message of the header file in ch32xxx/EVT/EXAM/SRC/Peripheral/inc
* √:    Merged in , version unspecified
* ×:    Not merged / Unchecked
* +:    Work in progress
* N/A:  No header file with this suffix in EVT, does not mean that the feature is not supported

\* X035(643): They are the same except electrical characteristics, LEDPWM, remapping, and ADC channel numbers.\
\* L103(M103): They use the same register map and header files.\
\* DPAL Header: Device Peripheral Access Layer Header File, normally named as ch32xxx.h
