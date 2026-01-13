# Stack Info Example

This example shows how to measure runtime stack usage using a canary value.
The stack is filled with a canary value (0xDEADBEEF) and after a few operations,
the stack is scanned to see how much of the canary value has been overwritten,
indicating the maximum stack depth used.

The second part of the example shows a call trace (frame pointer backtrace) of a recursive
function with random depth.

RA is the return address. You can use the following command to get the function name from the
address (replace `<address>` with the actual address):

```sh
riscv32-unknown-elf-addr2line -f -e *.elf <address>
```
> [!NOTE]
> Your toolchain prefix may vary.
