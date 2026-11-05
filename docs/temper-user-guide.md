# FunkeeB Temper — user guide

This guide is adapted from the [FunkeeB user guide](https://funkeeb.vercel.app/?guide=user)
and updated for the `finestedm/temper-zmkstudio` firmware repository.

## 1. Fork the firmware repository

Fork [finestedm/temper-zmkstudio](https://github.com/finestedm/temper-zmkstudio)
to your own GitHub account. Work in your fork so that your keymap changes are
kept separate from the upstream repository.

For the xphstos-style layout and the current display/animation setup, use the
[`xphstos-layout`](https://github.com/finestedm/temper-zmkstudio/tree/xphstos-layout)
branch. The default branch may contain a different keymap variant.

## 2. Edit your keymap

Open [Nick Coutsos' ZMK Keymap Editor](https://nickcoutsos.github.io/keymap-editor/)
and sign in with GitHub. Open `config/temper.keymap` from your fork, edit the
bindings, and save the file back to the repository.

The keymap is plain ZMK devicetree and is also compatible with ZMK Studio. The
editor geometry is described by `config/info.json`.

## 3. Build the firmware

Saving a change to GitHub starts the repository's **Build ZMK firmware** action.
Open the **Actions** tab in your fork and wait for the newest workflow run to
finish. A normal build takes a few minutes.

## 4. Download the artifacts

Open the completed workflow run and download its **firmware** artifact. It
contains three UF2 files:

- `temper-left-niceview-bongo.uf2` — firmware for the left/central half;
- `temper-right-niceview-status.uf2` — firmware for the right/peripheral half;
- `settings-reset.uf2` — temporary settings-reset firmware.

Do not leave `settings-reset.uf2` installed as the normal firmware. It is used
only for clearing stored Bluetooth and runtime keymap settings.

## 5. Optional reset after changing layouts

When moving between substantially different layouts, first flash
`settings-reset.uf2` to each half. Then flash the correct left or right UF2 to
that half. This clears stored ZMK Studio settings and Bluetooth bonds, so the
keyboard must be paired again afterwards.

## 6. Connect the USB cable

For the normal left/central firmware, connect the USB-C cable to the **left
half**. The left half is the central device and is the side used for ZMK Studio.

## 7. Enter bootloader mode

Disconnect the battery or switch it off. On the nice!nano/Pro Micro controller,
briefly short the `GND` and `RST` pins twice in quick succession with a
conductive object. The red LED should flash and the computer should show a
`NICENANO` drive.

The repository also provides bootloader shortcuts on the BLT layer in the
xphstos layout: hold the two inner thumb keys, then press the outermost top key
on the half you want to reset.

## 8. Flash the correct UF2

Drag the UF2 file onto the `NICENANO` drive:

- left half → `temper-left-niceview-bongo.uf2`;
- right half → `temper-right-niceview-status.uf2`.

The drive normally disappears and the controller reboots automatically. Wait
for the copy to finish before disconnecting the cable. Never flash the left
file to the right half or vice versa; it will not normally damage the board,
but the split roles and key behavior will be wrong.

After flashing, disconnect USB, reconnect the battery if needed, and power the
keyboard on again.

## 9. Use ZMK Studio

On the xphstos layout, hold both inner thumb keys to activate the **BLT** layer,
then press **T** — the innermost top-row key on the left half, closest to the
split. Connect the left half over USB and open [ZMK Studio](https://zmk.studio/).

ZMK Studio changes are stored on the keyboard and do not automatically update
`config/temper.keymap`. After flashing a keymap edited in Nick Coutsos' editor,
use **Restore Stock Settings** in ZMK Studio if an older runtime keymap is still
active.

## Display notes

The current firmware is configured for Sharp LS011B7DH03/nice!view-compatible
160×68 SPI displays. The left display shows Bongo Cat, indicators and WPM; the
right display shows the status screen and FunkeeB branding. Generic I²C OLED
modules are not drop-in replacements for these displays without hardware and
firmware changes.

