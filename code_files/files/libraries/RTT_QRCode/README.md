# RT-Thread QR Code App

## QRCode

This project is forked from [RT-Thread Package QRCode](https://github.com/RT-Thread-packages/qrcode), which in turn is forked from [Arduino Library QRCode](https://github.com/ricmoo/QRCode).


## Dependence
- [RT-Thread Library](https://github.com/onelife/Arduino_RT-Thread)


## Supported Architectures
- SAM (ARM Cortex-M3, Tested with Arduino Due)
- SAMD (ARM Cortex-M0+, Tested with Arduino MKRZero)


## Build As MSH Command

- Uncomment the line with `// RT_T.begin();` in "QRCode.ino"
```
void setup() {
  // RT_T.begin();
}
```

- Add the following line to "shell_cmd.h" (in RT-Thread Library)
```
ADD_MSH_CMD(qr, qrcode generator, qrcode, int, uint8_t argc, char **argv)
```


## Build As RTT Arduino App
- Compile
    - You may copy the compiling command from Arduino IDE's output window (select "File-> Preferences-> Show verbose output during: compilation" if you can't see the command)
    - Insert options `-mlong-calls -fPIC`

- Link
    - You may copy the link command from Arduino IDE's output window
    - Keep only "QRCode.ino.cpp.o" and "qrcode.c.o`
    - Remove option `-Wl,--unresolved-symbols=report-all"
    - Remove option `-L...`
    - Remove option `-T...`
    - Remove option `-Wl,--start-group ... -Wl,--end-group`
    - Insert options "-shared -fPIC -nostdlib -Wl,-marmelf -Wl,-z,max-page-size=0x4"
    - Insert entry point option (e.g. "-Wl,-eqrcode")

- Strip
    - Use `arm-none-eabi-strip` to strip unused sections, e.g. `.hash`, `.comment` and `.ARM.attributes`
