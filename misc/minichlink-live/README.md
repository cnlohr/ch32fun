# Minichlink live command server

Minichlink now supports a command server on port 4444.
Currently, there are only two commands implemented:
 - `-s` Set/Write command
 - `-m` Read command

The format of these commands is the same as the command line interface commands with the same name:
```sh
-s [debug register] [value]
-m [debug register]
```
> [!WARNING]
> All values MUST be expressed in hexadecimal:
```sh
-s 0x20 0xc02a717d
```

Multiple commands can be chained together, the results of the read operations will be returned as a key value pair with the key being the register address:
```sh
04: 1234bees5
04: f00cac1a
```

The QingKe V4 Processor manual describes how to build these commands, but there are also 4 example scripts that you can use:
```sh
./read.sh dma_count # reads contents of memory of `debug_count` symbol

./write.sh dma_count 123 # writes the value 123 at `dma_count` symbol address

./batch_read.sh dma_buffer # reads the contents of the symbol `dma_buffer`
                           # size is deduced automatically, but can also be specified with a second argument

./plot.py dma_count # continuously read a value an plot it
```
> [!WARNING]
> All of these scripts are examples, not tools, treat appropriately
