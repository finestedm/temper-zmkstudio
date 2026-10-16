
# Temper custom split ergo keyboard software
Based on [temper](https://github.com/raeedcho/temper) that itself is a great continuation of [chocofi](https://github.com/pashutk/chocofi/).

## Firmware
Use boards based on nrf52840 like ProMicro or Nice!Nanov2. This means that I use zmk. Compared to original Temper software this version supports [zmk.studio](https://zmk.dev/docs/features/studio) , so we can change our keymaps on the fly. The firmware will be created in an github action and you can use zmk.studio.

## Private layout

The `private` branch uses the current keymap and keyboard tuning from
[`finestedm/Temper_zmk`](https://github.com/finestedm/Temper_zmk), while keeping
this repository's nice!view and ZMK Studio build. Hold the two inner thumb keys
to open the **BLT** layer, then press its left inner top key to unlock ZMK
Studio.
