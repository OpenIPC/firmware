# autonight

## Before usage
### rebuild uImage and rootfs
```
make BOARD=t31_lite br-linux-menuconfig
    Device Drivers
        Multifunction device drivers
            <*> Support for the XBurst SADC AUX
            <*> Support for the XBurst SADC CORE
make BOARD=t31_lite br-linux-rebuild
make BOARD=t31_lite br-all
```
### or change `br-ext-chip-ingenic/board/t31/kernel/t31.generic.config` before building
```
- # CONFIG_MFD_JZ_SADC_V13 is not set
- # CONFIG_MFD_JZ_SADC_AUX is not set
+ CONFIG_MFD_JZ_SADC_V13=y
+ CONFIG_MFD_JZ_SADC_AUX=y

```

## Options
```
-D - sets jz_adc_aux device
-c - sets the command to call to set night mode
-d - delay (in seconds) between ADC reads
-O - turn on night mode when ADC value drops below this threshold
-F - turn off night mode when ADC value goes above this threshold
-h - print usage statement and exit
```

## Examples usage
```
autonight -D /dev/jz_adc_aux_0
autonight -d 3
autonight -h
```

## TODO
### Need to choose the correct default values of `thresholdOn` and `thresholdOff`
