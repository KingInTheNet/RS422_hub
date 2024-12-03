How to build PlatformIO based project
=====================================

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-ststm32/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Change directory to example
$ cd platform-ststm32/examples/mbed-rtos-serial

# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Build specific environment
$ pio run -e disco_f769ni

# Upload firmware for the specific environment
$ pio run -e disco_f769ni --target upload

# Clean build files
$ pio run --target clean
```

Basic functions: slave1 initialize on UART5 (PC_12, PD_2, 38400)
                 slave2 initialize on UART7 (PE_8, PE_7,38400)
                 master initialize on UART2 (PD_5, PD_6, 38400)
Each functions have read Callback attached to RxIrq
Since there is no way to specify which slave is being communicated with, I included a switch command specifically for hub (0xFA) that automatically switch the current slave to next slave. This byte is not transmitted to slave.
Further optimization can be done with DMA that might theoretically increase transmission speed, but as of now speed is being bottle necked by slave and master on windows machine
