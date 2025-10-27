# USBHS test device

This is a simple example of basic USBHS functionality. It creates two HID devices - a mouse and a keyboard, and a dummy device for testing a bulk transfers on EP4 and EP5. *Keyboard* send 3 presses of ``8`` and *mouse* slowly crawls the cursor diagonally to the bottom right corner of a screen.

To test bulk transfers you can use a ``testtop`` program that can be found in a corresponding subdirectory. By default it's set for RX bulk transfers, but it can be changed by editing ``testtop.c`` file and recompiling.

Tested on **CH585** and **CH32V307** devboards. Should also work on other USBHS targets.

Measured performance results: **31.2 MB/s** for **CH585**@78MHz and **38.4 MB/s** for **CH32V307**@144MHz.

## Note about *fast_memcpy*

To increase performance on **CH584/5** chips the custom memcpy function is used to copy buffers. It uses proprietary ``mcpy`` instruction that is found in some WCH chips. The caveat is that it works only for 4 bytes aligned data. Built-in endpoint buffers are already aligned, if you're planning on using other buffers in your code please align them too using ``__attribute__ ((aligned(4)))`` before array declaration, or simply use ``uint32_t`` arrays.

## Notes about performance on CH585

CH585 doesn't have a cached flash like CH32. To achieve comparable performance EVT puts all critical functions into RAM. This increases performance significantly, but it's still worse than on CH32 models. RAM portion designated for functions in ch32fun is called ``highcode``. You can assign a function to this area using ``__HIGH_RAM`` macro. Surprisingly there are some inconsistencies in this method - putting some functions there improves the result, some other may have detrimental effect. Disabling user functions by turning ``FUSB_USER_HANDLERS`` and/or ``FUSB_HID_USER_REPORTS`` also increased throughput in my tests.

Also to achieve maximum performance main loop is placed in a dedicated function that is also placed in RAM. This gave ~4.5 MB/s increase.
