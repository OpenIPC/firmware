## Introduction

This package is created and intended for unification and use in all firmware and on various processors where the method of connecting motors via GPIO is used.


## Configuring known devices

#### GK7205V200 (unknown model)
```
fw_setenv gpio_motors '52 53 56 57 69 70 59 58'
```

#### GM8136, Faleemi
```
fw_setenv gpio_motors '51 52 53 54 55 28 29 30'
```

#### HI3516CV200, FDT
```
fw_setenv gpio_motors '60 61 37 38 54 55 56 57'
```

#### SSC337DE, Foscam
```
fw_setenv gpio_motors '1 2 12 13 62 63 64 65'
```

We will add more information as soon as possible after testing.

#### T31, Jooan Q3R-U
```
fw_setenv gpio_motors 53 52 54 14 17
```
This type of device has only 4 GPIOs intended for controlling motors at one time and 1 additional GPIO for switching the Horizontal/Vertical mode. 
It is on this device that temporary incorrect operation of the camera's diagonal movement is possible.

## Using the utility
```
gpio-motors <pan steps> <tilt steps> <delay (ms)>
```

```
gpio-motors 20 0 30
gpio-motors -20 0 30
#
gpio-motors 0 20 30
gpio-motors 0 -20 30
#
gpio-motors 20 10 30
gpio-motors -20 10 30
```

## More info

- [Our Wiki](https://github.com/OpenIPC/wiki/blob/master/en/motors.md)
