
# Temper custom split ergo keyboard software

Based on [temper](https://github.com/raeedcho/temper), which is itself a
continuation of [chocofi](https://github.com/pashutk/chocofi/).

## Firmware

The firmware targets nRF52840 Pro Micro-compatible controllers using ZMK. A
GitHub Actions workflow builds separate UF2 files for the left and right halves.

## Configuring the keymap

The active [`config/temper.keymap`](config/temper.keymap) is plain ZMK
devicetree. It retains the previous QWERTY/Miryoku-derived layers and behaviors,
but no longer requires the Miryoku preprocessor to generate them. This makes a
single firmware source usable with either editor below.

### Nick Coutsos' ZMK Keymap Editor

1. Open [ZMK Keymap Editor](https://nickcoutsos.github.io/keymap-editor/).
2. Choose **GitHub** and authorize this repository, or choose **File System**
   and open `config/temper.keymap` locally.
3. Edit the bindings and save the keymap.
4. Let GitHub Actions build the firmware, then flash the new left-half UF2.

The keyboard geometry used by the editor is stored in `config/info.json`.
Custom hold-taps, tap dances, Bluetooth profile actions, bootloader bindings,
and the Studio unlock binding are declared directly in the keymap so the editor
can preserve them.

### ZMK Studio

The left/central firmware is built with [ZMK Studio](https://zmk.studio/)
support. Connect the left half over USB, invoke the `studio_unlock` binding on
the **Nav** layer, and connect from ZMK Studio.

ZMK Studio stores runtime keymap changes on the keyboard. Those changes do not
flow back into `config/temper.keymap`. After flashing a keymap changed with the
Nick Coutsos editor, use **Restore Stock Settings** in ZMK Studio so the newly
flashed keymap becomes active.

## Displays

The left half remains the split central and uses the local `nice_view_bongo`
shield to show a Bongo Cat that reacts to key presses from both halves. The
widget uses the current ZMK/LVGL 9 display API and rotates its portrait UI for
the keyboard's display mounting. The right half uses a matching custom
nice!view screen with a large battery gauge, Bluetooth link state, and a
lengthwise `FunkeeB` wordmark. Connect the left half to the host when using USB
or ZMK Studio. After one minute without a key press, the cat closes its eyes and
displays `Zzzz`; the next key press wakes it immediately. A rolling 16-second
WPM graph fills the space between the status indicators and the bottom-aligned
cat. The seven animation frames are adapted from
[SamIAm2000/zmk](https://github.com/SamIAm2000/zmk) (MIT): they are scaled for
this portrait layout and the flower is removed.
