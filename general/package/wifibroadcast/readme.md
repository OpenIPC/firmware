# ATHEROS

1. Для сборки с драйверами Atheros включить опции ядра в соотвествующем плате конфиге:

```
br-ext-chip-goke/board/gk7205v200/kernel/gk7205v200.generic-fpv.config
br-ext-chip-goke/board/gk7205v200/kernel/gk7205v300.generic-fpv.config

CONFIG_WLAN_VENDOR_ATH=y
CONFIG_ATH9K_HTC=m
```

2. Выбрать необходимые пакеты в конфиге соответствующем плате:

```
br-ext-chip-goke/configs/unknown_unknown_gk7205v200_fpv_defconfig
br-ext-chip-goke/configs/unknown_unknown_gk7205v300_fpv_defconfig

BR2_PACKAGE_LINUX_FIRMWARE_OPENIPC_ATHEROS_9271=y
```

# REALTEK

1. Выбрать необходимые пакеты в конфиге соответствующем плате:

```
br-ext-chip-goke/configs/unknown_unknown_gk7205v200_fpv_defconfig
br-ext-chip-goke/configs/unknown_unknown_gk7205v300_fpv_defconfig


BR2_PACKAGE_RTL8812AU_OPENIPC=y
```


!!!ВАЖНО!!! Оба драйвера с большой вероятностью не поместятся в сборку, потому собирать по принципу 1 прошивка 1 драйвер. Majestic не включен в стандартную сборку, после прошивки его необходимо загрузить отдельно.


# Загрузка модулей

## Realtek:

```
modprobe cfg80211
modprobe 88XXau
```
## Atheros:

```
modprobe cfg80211
modprobe mac80211
modprobe ath9k_htc
```

# Настройка интерфейса

```
iw set reg BO
ifconfig wlan0 up
iwconfig wlan0 mode monitor
iwconfig wlan0 channel 6
```
