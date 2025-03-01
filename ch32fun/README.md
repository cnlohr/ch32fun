## Update Status Overview
|PERIPHERAL    |V003|V00x|V10x|V20x|V30x|X035|L103|M030|
|:-------------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|DPAL Header\* |2.0 | x  |2.7 | x  | x  | √  | ×  | ×  |
|ADC           |1.9 | x  |2.1 | x  | x  |1.3 | ×  | ×  |
|AWU           |N/A |N/A |N/A |N/A |N/A | √  |N/A |N/A |
|BKP           |N/A |N/A |2.1 | x  | x  |N/A | ×  |N/A |
|CAN           |N/A |N/A |N/A | x  | x  |N/A | ×  |N/A |
|CRC           |N/A |N/A |2.1 | x  | x  |N/A | ×  |N/A |
|DAC           |N/A |N/A |N/A |N/A | x  |N/A |N/A |N/A |
|DBGMCU        |1.5 | x  |2.1 | x  | x  | √  | ×  | ×  |
|DMA           | √  | x  | √  | x  | x  | √  | ×  | ×  |
|DVP           |N/A |N/A |N/A |N/A | x  |N/A |N/A |N/A |
|ETH           |N/A |N/A |N/A |N/A | x  |N/A |N/A |N/A |
|EXIT          | √  | x  |2.4 | x  | x  | √  | ×  | ×  |
|FLASH         | √  | x  |2.7 | x  | x  |1.4 | ×  | ×  |
|FSMC          |N/A |N/A |N/A |N/A | x  |N/A |N/A |N/A |
|GPIO          |2.0 | x  |2.7 | x  | x  |1.6 | ×  | ×  |
|I2C           | √  | x  | √  | x  | x  |1.7 | ×  | ×  |
|IWDG          | √  | x  | √  | x  | x  | √  | ×  |N/A |
|LPTIM         |N/A |N/A |N/A |N/A |N/A |N/A | ×  |N/A |
|MISC          | √  | x  |2.4 | x  | x  |1.6 | ×  |N/A |
|OPA           | √  | x  |N/A | x  | x  |1.3 | ×  | ×  |
|PWR           |1.9 | x  |2.6 | x  | x  |1.7 | ×  | ×  |
|RCC           |1.8 | x  |2.7 | x  | x  | √  | ×  | ×  |
|RNG           |N/A |N/A |N/A |N/A | x  |N/A |N/A |N/A |
|RTC           |N/A |N/A | √  | x  | x  |N/A | ×  |N/A |
|SPI           |1.9 | x  |2.7 | x  | x  |1.7 | ×  | ×  |
|TIM           |1.6 | x  | √  | x  | x  | √  | ×  | ×  |
|USART         | √  | x  |2.4 | x  | x  | √  | ×  | ×  |
|USB           |N/A |N/A | √  | x  | x  |1.8 | ×  | ×  |
|USB_HOST      |N/A |N/A | √  |N/A |N/A |N/A |N/A |N/A |
|USBPD         |N/A |N/A |N/A |N/A |N/A | x  | ×  | ×  |
|WWWDG         | √  | x  | √  | x  | x  | √  | ×  | ×  |
|**chxxxhw.h** | √  | x  | √  | √  | √  | √  | x  | x  |
|**minichlink**| √  | x  | √  | √  | √  | √  | x  | x  |

* n.m:  Last commit message of the header file in ch32xxx/EVT/EXAM/SRC/Peripheral/inc
* √:    Merged in , version unspecified
* ×:    Not merged / Unchecked
* +:    Work in progress
* N/A:  No header file with this suffix in EVT, does not mean that the feature is not supported

\* DPAL Header: Device Peripheral Access Layer Header File, normally named as ch32xxx.h
