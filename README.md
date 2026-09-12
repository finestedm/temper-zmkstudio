
# Temper custom split ergo keyboard software
Based on [temper](https://github.com/raeedcho/temper) that itself is a great continuation of [chocofi](https://github.com/pashutk/chocofi/).

## Firmware
Use boards based on nrf52840 like ProMicro or Nice!Nanov2. This means that I use zmk. Compared to original Temper software this version supports [zmk.studio](https://zmk.dev/docs/features/studio) , so we can change our keymaps on the fly. The firmware will be created in an github action and you can use zmk.studio.

## Displays

The left half remains the split central and uses the local `nice_view_bongo`
shield to show a Bongo Cat that reacts to key presses from both halves. The
widget uses the current ZMK/LVGL 9 display API and rotates its portrait UI for
the keyboard's display mounting. The right half uses ZMK's native nice!view
peripheral status screen. Connect the left half to the host when using USB or
ZMK Studio. After one minute without a key press, the cat closes its eyes and
displays `Zzzz`; the next key press wakes it immediately.
