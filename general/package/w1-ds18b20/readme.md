# w1-ds18b20

This app allows reading ds18b20 sensors in bitbang mode using direct GPIO memory access.
Currently supports only HiSilicon-style GPIO offsets. Pull-up resistor is required.

## Usage

Example: reading temperature using GPIO8_2 on Gk7205V300 (consult datasheet or ipctool output)

```
# ./w1-ds18b20 -base 0x120B8000 -gpio 2
28-4A4E3C1E64FF :  22.44 C
```
