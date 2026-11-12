
# Temper custom split ergo keyboard software

Based on [temper](https://github.com/raeedcho/temper), which is itself a
continuation of [chocofi](https://github.com/pashutk/chocofi/).

## Firmware

The firmware targets nRF52840 Pro Micro-compatible controllers using ZMK. A
GitHub Actions workflow builds separate UF2 files for the left and right halves.

For flashing and day-to-day setup, see the [Temper user guide](docs/temper-user-guide.md).

## Case models

Editable STEP models for the split case are available in
[`case/temper-tps43`](case/temper-tps43). The set contains separate top and
bottom parts for both halves plus a dedicated right-side TPS43 holder. These
files can be adjusted in CAD or exported to STL/3MF for printing.

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
support. On this `private` branch, connect the left half over USB, hold the two
inner thumb keys to activate **BLT**, press the left inner top key to invoke
`studio_unlock`, and connect from ZMK Studio.

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

## Optional TPS43 right half

GitHub Actions also builds `temper-right-tps43-touchpad.uf2` as an alternative
firmware for a right half fitted with a TPS43/IQS5xx touchpad instead of the
nice!view. Keep using `temper-left-niceview-bongo.uf2` on the left half; it is
prepared to receive pointing events from either right-half variant.

The touchpad variant uses the same five-pin header and pin assignment as the
tested FunkeeB Corne V4 integration: SDA on Pro Micro pin 2, SCL on pin 3, RDY
on pin 1, plus VCC and GND. It supports pointer movement, one-finger left click,
two-finger right click, press-and-hold dragging, two-finger scrolling and the
driver's low-power mode. Do not flash the TPS43 artifact onto a right half with
a display; use `temper-right-niceview-status.uf2` for that hardware.
