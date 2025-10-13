/***************************************************************************//**
   @file    QRCode.ino
   @brief   Arduino RTT QRCode App
   @author  onelife <onelife.real[at]gmail.com>
 ******************************************************************************/
#include <rtt.h>
#include "qrcode.h"

/* ATTENTION
    The following flags in "rtconfig.h" must be turned on:
    - CONFIG_USING_MODULE
 */

/* NOTES
    To build code as "MO" file:
    1. Compile
      - you may copy the compiling command from Arduino's output window (select
      [File-> Preferences-> Show verbose output during: compilation] if you
      can't see the command)
      - add options "-mlong-calls -fPIC"

    2. Link
    - you may copy the compiling command from Arduino's output window
    - keep only "QRCode.ino.cpp.o" and "qrcode.c.o"
    - remove option "-Wl,--unresolved-symbols=report-all"
    - remove option "-LC:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579"
    - remove option "-TC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\hardware\\samd\\1.6.21\\variants\\mkrzero/linker_scripts/gcc/flash_with_bootloader.ld"
    - remove option "-Wl,--start-group ... -Wl,--end-group"
    - add options "-shared -fPIC -nostdlib -Wl,-marmelf -Wl,-z,max-page-size=0x4"
    - add option to set entry point (e.g. "-Wl,-esay_hello")

    3. Strip
    - use "arm-none-eabi-strip"

    To build code as "SO" file:
    1. Compile
      - same as above

    2. Link
    - same as above, but
    - add option to set entry point to NULL (e.g. "-Wl,-e0")

    3. Strip
    - use "arm-none-eabi-strip"
 */

#define DEFAULT_QR_VERSION 3
#define DEFAULT_QR_STRING  "HELLO WORLD"


extern "C" {

  int qrcode(rt_uint8_t argc, char **argv) {
    int ret;
    rt_uint32_t qrver;
    const char *qrstr;
    rt_uint8_t *qrcodeBytes;
    QRCode qrc;
    rt_uint8_t x, y;

    if (argc > 1) qrstr = argv[1];
    else
      qrstr = DEFAULT_QR_STRING;

    if (argc > 2) qrver = (rt_uint32_t)argv[2];
    else
      qrver = DEFAULT_QR_VERSION;

    ret = RT_EOK;

    do {
      qrcodeBytes = (rt_uint8_t *)rt_calloc(1, qrcode_getBufferSize(qrver));
      if (!qrcodeBytes) {
        ret = -RT_ENOMEM;
        break;
      }

      ret = qrcode_initText(&qrc, qrcodeBytes, qrver, ECC_LOW, qrstr);
      if (ret < 0) break;

      rt_kprintf("\n");
      for (y = 0; y < qrc.size; y++) {
        for (x = 0; x < qrc.size; x++) {
          if (qrcode_getModule(&qrc, x, y))
            rt_kprintf("\xdb\xdb");
          else
            rt_kprintf("  ");
        }
        rt_kprintf("\n");
      }
      rt_kprintf("\n");

    } while (0);

    rt_free(qrcodeBytes);
    if (RT_EOK != ret)
      LOG_E("ERR %d", ret);

    return ret;
  }

} /* extern "C"{ */


void setup() {
  // RT_T.begin();
}

// this function will be called by "Arduino" thread
void loop() {
  // may put some code here that will be run repeatedly
}

/* EXAMPLE

"C:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\arm-none-eabi-gcc\\4.8.3-2014q1/bin/arm-none-eabi-g++" -mcpu=cortex-m0plus -mthumb -c -g -Os -mlong-calls -fPIC -Wall -Wextra -std=gnu++11 -ffunction-sections -fdata-sections -fno-threadsafe-statics -nostdlib --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -MMD -DF_CPU=48000000L -DARDUINO=10809 -DARDUINO_SAMD_MKRZERO -DARDUINO_ARCH_SAMD -DUSE_ARDUINO_MKR_PIN_LAYOUT -D__SAMD21G18A__ -DUSB_VID=0x2341 -DUSB_PID=0x804f -DUSBCON "-DUSB_MANUFACTURER=\"Arduino LLC\"" "-DUSB_PRODUCT=\"Arduino MKRZero\"" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\CMSIS\\4.5.0/CMSIS/Include/" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\CMSIS-Atmel\\1.1.0/CMSIS/Device/ATMEL/" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\hardware\\samd\\1.6.21\\cores\\arduino" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\hardware\\samd\\1.6.21\\variants\\mkrzero" "-IC:\\Users\\onelife\\Documents\\Arduino\\libraries\\RT-Thread\\src" "-IC:\\Users\\onelife\\Documents\\Arduino\\libraries\\RTT-QRCode\\src" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\hardware\\samd\\1.6.21\\libraries\\SPI" "C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579\\sketch\\QRCode.ino.cpp" -o "C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579\\sketch\\QRCode.ino.cpp.o"

"C:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\arm-none-eabi-gcc\\4.8.3-2014q1/bin/arm-none-eabi-gcc" -mcpu=cortex-m0plus -mthumb -c -g -Os -mlong-calls -fPIC -Wall -Wextra -std=gnu11 -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -MMD -DF_CPU=48000000L -DARDUINO=10809 -DARDUINO_SAMD_MKRZERO -DARDUINO_ARCH_SAMD -DUSE_ARDUINO_MKR_PIN_LAYOUT -D__SAMD21G18A__ -DUSB_VID=0x2341 -DUSB_PID=0x804f -DUSBCON "-DUSB_MANUFACTURER=\"Arduino LLC\"" "-DUSB_PRODUCT=\"Arduino MKRZero\"" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\CMSIS\\4.5.0/CMSIS/Include/" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\CMSIS-Atmel\\1.1.0/CMSIS/Device/ATMEL/" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\hardware\\samd\\1.6.21\\cores\\arduino" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\hardware\\samd\\1.6.21\\variants\\mkrzero" "-IC:\\Users\\onelife\\Documents\\Arduino\\libraries\\RT-Thread\\src" "-IC:\\Users\\onelife\\Documents\\Arduino\\libraries\\RTT-QRCode\\src" "-IC:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\hardware\\samd\\1.6.21\\libraries\\SPI" "C:\\Users\\onelife\\Documents\\Arduino\\libraries\\RTT-QRCode\\src\\qrcode.c" -o "C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579\\libraries\\RTT-QRCode\\qrcode.c.o"

"C:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\arm-none-eabi-gcc\\4.8.3-2014q1/bin/arm-none-eabi-g++" -Os -shared -fPIC -nostdlib -Wl,-eqrcode -Wl,-marmelf -Wl,-z,max-page-size=0x4 -Wl,--gc-sections -save-temps "-Wl,-Map,C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579/qr.map" --specs=nano.specs --specs=nosys.specs -mcpu=cortex-m0plus -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections  -Wl,--warn-common -Wl,--warn-section-align -o "C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579/qr.elf" "C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579\\sketch\\QRCode.ino.cpp.o" "C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579\\libraries\\RTT-QRCode\\qrcode.c.o"

"C:\\Users\\onelife\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\arm-none-eabi-gcc\\4.8.3-2014q1/bin/arm-none-eabi-strip" -R .hash -R .comment -R .ARM.attributes "C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579/qr.elf" -o "C:\\Users\\onelife\\AppData\\Local\\Temp\\arduino_build_242579/qr.mo"

 */
