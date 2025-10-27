# ch32fun_isler
Industry Standard Low Energy Radio (iSLER) RX/TX for ch32fun

### supported MCUs:
This code aims to support all WCH RF enabled microcontrollers, and is currently tested and working on ch570, ch582, ch592 and ch32v208,
and development aims for additional support for ch573 (which is a pain in the neck!) and ch585, and will be extended to future RF chips from WCH.
The 1M and 2M PHY's work on all the supported chips, and Coded Phy S2 and S8 is in development.

| Chip       | RX | TX | 1M | 2M | S2 | S8 |
|------------|----|----|----|----|----|----|
| ch570/2    | √  | √  | √  | √  | x  | x  |
| ch571/3*   | ~  | x  | ~  | x  | x  | x  |
| ch582/3    | √  | √  | √  | √  | √  | √  |
| ch584/5    | √  | √  | √  | √  | x  | x  |
| ch591/2    | √  | √  | √  | √  | x  | x  |
| ch23v208   | √  | √  | √  | √  | √  | √  |

\* ch571/3 is a completely different creature in many respects, and iSLER support is very challenging. Current state is that
the core init seems ok, but TX does not work and RX seems to receive but since DMA has to be handled manually here
I don't know where to pull the frame data from. It's possible the TX issues are also from the DMA part, but the LLE_IRQHandler
is also sus.

### Links for low-cost dev boards:
If you don't know which one to get, the ch582 from quason (the first ch573/ch582 link) is the cheapest and supports Long Range Coded Phy (S2/S8).

CH570/2: \
https://vi.aliexpress.com/item/1005009123215735.html \
https://vi.aliexpress.com/item/1005009071651737.html

CH571/3, CH582/3: \
https://vi.aliexpress.com/item/1005008586545355.html \
https://vi.aliexpress.com/item/1005008586177888.html

CH591/2: \
https://vi.aliexpress.com/item/1005006117859297.html

CH32V208: \
https://vi.aliexpress.com/item/1005007751052802.html \
https://vi.aliexpress.com/item/1005007751228107.html \
https://vi.aliexpress.com/item/1005007257716209.html \
https://vi.aliexpress.com/item/1005006177751678.html
