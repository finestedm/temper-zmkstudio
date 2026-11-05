# FunkeeB Temper — user guide

This guide is adapted from the [FunkeeB user guide](https://funkeeb.vercel.app/?guide=user)
and updated for the `finestedm/temper-zmkstudio` firmware repository.

## 1. Fork the firmware repository

Fork [finestedm/temper-zmkstudio](https://github.com/finestedm/temper-zmkstudio)
to your own GitHub account. Work in your fork so that your keymap changes are
kept separate from the upstream repository.

Use the repository's default [`main`](https://github.com/finestedm/temper-zmkstudio/tree/main)
branch for the supported keymap, displays, Bongo Cat animation, ZMK Studio and
Nick Coutsos editor integration.

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

The firmware also provides keyboard shortcuts for entering the bootloader:

- **left half:** hold the thumb key that types `Esc` to activate **Nav**, then
  press `Q` (the outermost top key on the left half);
- **right half:** hold the thumb key that types `Backspace` to activate
  **Num**, then press `P` (the outermost top key on the right half).

The bootloader action is source-specific, so it resets the half on which `Q` or
`P` is physically pressed. The right half must be paired and connected to the
left half for its shortcut to be processed.

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

Connect the left/central half over USB. Hold the thumb key that types `Esc` to
activate **Nav**, then press `T` — the innermost top-row key on the left half,
closest to the split. This invokes the `studio_unlock` binding. Open
[ZMK Studio](https://zmk.studio/) immediately afterwards.

ZMK Studio changes are stored on the keyboard and do not automatically update
`config/temper.keymap`. After flashing a keymap edited in Nick Coutsos' editor,
use **Restore Stock Settings** in ZMK Studio if an older runtime keymap is still
active.

## Layer shortcuts

Most layers are momentary: hold the listed key to use the layer and release it
to return to the previous layer. A quick tap still sends the key printed in the
second column.

| Layer | Hold this key | Typical purpose |
| --- | --- | --- |
| Base | — | Default QWERTY layer |
| Button | `Z` or `/` | Mouse buttons and editing actions |
| Nav | `Esc` thumb key | Navigation, Studio unlock and left bootloader |
| Mouse | `Space` thumb key | Keypad directions and mouse buttons |
| Media | `Tab` thumb key | Media, Bluetooth, output and display controls |
| Num | `Backspace` thumb key | Numbers and right bootloader |
| Sym | `Enter` thumb key | Symbols |
| Fun | `Delete` thumb key | Function keys |

The **Extra** and **Tap** layouts are selected with guarded double-taps rather
than a direct hold key. The two presses must be made within 200 ms:

- while Nav, Mouse or Media is active: double-tap `W` → Tap, `E` → Extra,
  `R` → Base;
- while Num, Sym or Fun is active: double-tap `U` → Base, `I` → Extra,
  `O` → Tap.

There are also paired persistent layer selectors:

- Nav: double-tap `C` → Num, `V` → Nav;
- Mouse: double-tap `C` → Sym, `V` → Mouse;
- Media: double-tap `C` → Fun, `V` → Media;
- Num: double-tap `M` → Num, `,` → Nav;
- Sym: double-tap `M` → Sym, `,` → Mouse;
- Fun: double-tap `M` → Fun, `,` → Media.

The current Tap layer contains only plain key presses and no layer-return
binding. If it is selected persistently, restart the keyboard to return to the
default Base layer. This is an existing limitation of the stock keymap.

## Display notes

The current firmware is configured for Sharp LS011B7DH03/nice!view-compatible
160×68 SPI displays. The left display shows Bongo Cat, indicators and WPM; the
right display shows the status screen and FunkeeB branding. Generic I²C OLED
modules are not drop-in replacements for these displays without hardware and
firmware changes.

