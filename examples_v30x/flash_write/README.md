# Flash write example for v20x/v30x

This example shows how to use the `ch20x_30x_flash.h` in extralibs to read/erase/write data to flash. The example just writes two 32 bit values, a magic word to know if we saved previously, and a run count that is incremented each run.

# Usage

Plug your favorite v20x/v30x board and programmer, and run `make && make monitor`. You should see the monitor outputting something like:

```
FLASH DEMO START
Run count: 2
Saving run count...
Done. Reset to check if run count is properly updated!
```

Each time you reset the board, the run count should increase once.
