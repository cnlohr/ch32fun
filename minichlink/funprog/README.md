# Funprog programmer core files

Here live the core files that implement common funrprog functions for each and every implementation of the programmer.

Currently we have these variants:

- [ESP32-S2](https://github.com/cnlohr/esp32s2-cookbook/tree/master/ch32v003programmer)
- [CH32V003](https://github.com/cnlohr/rv003usb/tree/master/rvswdio_programmer)
- [CH570](https://github.com/cnlohr/ch570funprog/)

Minichlink functions related to this type of programmers can be found here in [minichlink/pgm-esp32s2-ch32xx.c](../pgm-esp32s2-ch32xx.c)

## Porting

You can add your own implementation to this list. To do this you will have to write these files to your project:

- programmer_config.h - config where you can select parts you need
- funprog_port.h - platform specific general functions
- bitbang_rvswdio.h - low-level implementation of the RVSWDIO protocol(s)

and these optional, if you want UPDI support (currently implemented only on ESP32-S2):

- bitbang_updi.h - low-level implemention of UPDI protocol

Best way to point to common files and make sure you can easily pull updates is to use git submodule of ch32fun and include them directly from this directory.
Then include everything in your main file in this order:

```C
#include "funprog_config.h"

#include "funprog_port.h"

#include "ch32fun/minichlink/funprog/funprog_rvswdio.h"

#include "bitbang_rvswdio.h"

#if CH5xx_SUPPORT
#include "ch32fun/minichlink/funprog/funprog_ch5xx.h"
#endif

#if UPDI_SUPPORT
#include "ch32fun/minichlink/funprog/funprog_updi.h"
#include "bitbang_updi.h"
#endif

#include "ch32fun/minichlink/funprog/funprog_logic.h"
```

You need to call ``HandleCommandBuffer`` function from [funprog_logic.h](./funprog_logic.h) to process incoming commands. Check out available implementation to get the idea.
