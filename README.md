# OLIMEX RK3188 SOM 4GB / RK3188 SOM EVB 

Full kernel build tree for the OLIMEX RockChip RK3188 development board

## OLIMEX I2C Connections

### Real Time Clock (RTC)
```
Chip..........: HYM8563
Address.......: 0x51
Bus...........: i2c-1 
Kernel Driver.: drivers/rtc/rtc-HYM8563.c
```
### Power Regulator
```
Chip..........: ACT8846
Address.......: 0x5a
Bus...........: i2c-1
Kernel Driver.: arch/arm/mach-rk30/board-pmu-act8846.c
```
### HDMI
```
Chip..........: CAT66121
Address.......: 0x4c
Bus...........: i2c-2
Kernel Driver.: drivers/video/rockchip/hdmi/chips/cat66121/cat66121_hdmi.c
```
### Touchscreen Panel
```
Chip..........: AR1020
Address.......: 0x4d
Bus...........: i2c-3
Kernel Driver.: drivers/input/touchscreen/ar1020-i2c.c
```
### MOD-IRDA
```
Chip..........: 
Address.......: 0x24
Bus...........: i2c-0
UEXT Connector: UEXT1
Kernel Driver.: drivers/media/rc/irda/olimex-irda.c
```
### Notes
```
Power down button (unhandled):
Jan  2 04:55:56 eofmc kernel: [ 1657.334381] evbug: Event. Dev: input0, Type: 1, Code: 116, Value: 1
Jan  2 04:55:56 eofmc kernel: [ 1657.334423] evbug: Event. Dev: input0, Type: 0, Code: 0, Value: 0
Jan  2 04:55:57 eofmc kernel: [ 1657.614384] evbug: Event. Dev: input0, Type: 1, Code: 116, Value: 0
Jan  2 04:55:57 eofmc kernel: [ 1657.614409] evbug: Event. Dev: input0, Type: 0, Code: 0, Value: 0
```

