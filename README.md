
# Temper custom split ergo keyboard software
Based on [temper](https://github.com/raeedcho/temper) that itself is a great continuation of [chocofi](https://github.com/pashutk/chocofi/).

## Firmware
Use boards based on nrf52840 like ProMicro or Nice!Nanov2. This means that I use zmk. Compared to original Temper software this version supports [zmk.studio](https://zmk.dev/docs/features/studio) , so we can change our keymaps on the fly. The firmware will be created in an github action and you can use zmk.studio.

## Displays

The left half remains the split central and uses
[`dsifry/nice-view-mod`](https://github.com/dsifry/nice-view-mod) to show a Bongo
Cat that reacts to key presses from both halves. The right half uses ZMK's
lightweight built-in peripheral screen with a large battery percentage and a
connection icon. Connect the left half to the host when using USB or ZMK
Studio.

ZMK and the build workflow are pinned to `v0.3.0` for compatibility with the
Bongo Cat widget.
