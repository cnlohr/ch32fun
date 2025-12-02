# Simple example of using a timer in CH570/2 in encoder mode

Currently only CH570/2 among CH5xx family has encoder mode in it's only timer peripheral. It is very easy to setup and use, and it works pretty well.

This example also shows how to configure an external interrupt on a GPIO pin for a button that is present in most generic rotary encoders.

Connect your rotary encoder in such manner:

- ENC A/B pins to PA7/PA2 or PA4/PA9 (set corresponding remap option in code)
- ENC common and one of button pins to GND or VCC (set which one you've chosen in code)

``make terminal`` and watch values change.

## Reset pin conflict

It can happen that your encoder setup can conflict with your current reset pin config.
Do ``minichlink -i`` to check info about your MCU. If it says:

```
Reset - enabled
Reset pin - PA7
```

you need to either disable Reset pin by doing ``minichlink -D`` or chose different set of pins for your encoder and use remap option in code.
