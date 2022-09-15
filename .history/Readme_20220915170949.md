# STM32 PlatformIO Template

## Folder Structure

- BSP
跟硬體相關的設定
    > STM32CubeMX設定檔
    >
    > - Inc
    >   - HardwareConfig.h
    >   - my_script.h
    > - Src
    >   - my_script.cpp

- Include
  >User header file
- Src
  >User source code
- USB_HID_Driver_F411
  > HID Driver
- jlink
  > Jlink command

## Platformio.ini

- 修改firmware name

```clike
; set firmware name and version
custom_option1 = motor_testkit
custom_prog_version = 1.0.1.0
```
