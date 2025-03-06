# QMK Firmware for Work Louder Creator Micro

My board is a figma version but the hardware is the same.

## Flash Guide

1. Hold left top encoder and plug the board in, or press the bottom right key on layter 3.
2. Clear EEPROM in QMK Toolbox
3. Flash the firmware

### Or use QMK CLI to flash
`qmk compile -kb work_louder/micro -km shigure -j 32`

`qmk flash -kb work_louder/micro -km shigure -j 32`

### Known Issues
- Sometimes Windows driver will need to be replaced with the correct `WinUSB` driver with `zadig`.
