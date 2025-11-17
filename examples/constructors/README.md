# Constructors example

You can implement constructor functions by tagging them with `__attribute__((constructor))`. On most toolchains, these functions will be automatically invoked by the startup routines before entering `main()`, but this works slightly different in ch32fun. To run the constructor functions, you have two options:

1. Add a `#define FUNCONF_SUPPORT_CONSTRUCTORS 1` to `funconfig.h`. If you do this, constructor functions will be invoked by the `SystemInit()` routine. This is most likely what you want.
2. Manually invoke `CallConstructors()` when you want to run them. This is a bit more flexible because it allows you to decide when to run them, and also allows to run them several times in case you need it.

Note both options can be combined, so you could `#define FUNCONF_SUPPORT_CONSTRUCTORS 1`, and then if you need to run the functions again some other time, also invoke `CallConstructors()`.

The example code demonstrates the first option. It implements two contructor functions that just increment a global counter.

NOTE: on some toolchains, in addition to the `__attribute__((constructor))`, you also have to add an `__attribute__((used))` tag. Otherwise the linker will discard the constructor functions (since they are not directly referenced by the code) during garbage collection. It seems this is not needed in ch32fun, but if you want to also add it to your constructors, it doesn't hurt.

# Usage

Plug your favorite CH32V003 board and programmer, and run `make flash && make monitor`. You should see the monitor outputting `Constructors called: 2`. If you comment out the `#define FUNCONF_SUPPORT_CONSTRUCTORS 1` (or set it to 0) and run again `make flash && make monitor`, the constructors will not be invoked, and thus `Constructors called: 0` will be printed.
