# Temper case with TPS43 support

This folder contains editable STEP models for the Temper split keyboard case
and its optional right-side TPS43 touchpad holder.

| File | Part |
| --- | --- |
| `Top-Left.step` | Left-half top shell |
| `Bottom-Left.step` | Left-half bottom shell |
| `Top-Right.step` | Right-half top shell |
| `Bottom-Right.step` | Right-half bottom shell |
| `TPS43-Right-Holder.step` | TPS43 holder for the right half |

The models are provided as CAD sources so they can be modified before being
exported to a slicer format. Check orientation, clearances, and printer-specific
tolerances before producing the final parts.

## Required hardware

- 10 heat-set inserts: M2 internal thread, 3.2 mm outer diameter, 2 mm length
- 10 M2x6 screws
- Male pin headers with `H = 7.5 mm`
- Female pin sockets with `H = 3.5 mm`
- Five insulated wires for the TPS43 connection

Use the specified header and socket heights. Different sizes change the stack
height and may collide with the TPS43 holder.

## Connecting the TPS43 to the five-pin display socket

The TPS43 variant reuses the keyboard's five-pin nice!view connector. Match the
signals by their PCB labels; do not rely on wire colours or on a left-to-right
order from a photograph, because the order appears reversed when either board
is turned over.

| Five-pin display socket | TPS43 pad | Function |
| --- | --- | --- |
| `GND` | `GND` | Ground |
| `VCC` | `3V3` / `VDDHI` | 3.3 V supply |
| `SCL` / `SCK` (Pro Micro pin 3) | `SCL` | I2C clock |
| `MOSI` (Pro Micro pin 2) | `SDA` | I2C data |
| `CS` (Pro Micro pin 1) | `RDY` | Touchpad ready/interrupt |

The TPS43 `NRST`/`RST` pad is not part of this five-wire connection and remains
unconnected in this build.

### Preparing the cable

1. Disconnect USB and the battery before soldering or inserting any wires.
2. On the touchpad end, solder the five wires directly to the labelled TPS43
   pads: `GND`, `3V3`/`VDDHI`, `SCL`, `SDA`, and `RDY`.
3. On the keyboard end, solder each wire to an individual bare male header pin.
   Remove the plastic header carrier and do not use a five-position plastic
   plug housing on this end.
4. Insulate every solder joint, for example with narrow heat-shrink tubing.
5. Insert the five pins separately into the matching positions of the five-pin
   female display socket, following the signal table above.
6. Before powering the keyboard, check every connection for continuity and
   verify that `3V3` is not shorted to `GND`. Reversing the supply can damage
   the touchpad or controller.

The five-pin connector is intended for either the right-side nice!view or the
TPS43 cable, depending on the installed firmware and case configuration. Do not
connect or disconnect the touchpad while the keyboard is powered.

## Mechanical assembly notes

Install the ten M2 heat-set inserts flush with the printed surfaces, using the
lowest practical tool temperature for the chosen filament. Assemble the case
with ten M2x6 screws. Test-fit the `H = 7.5 mm` male headers, `H = 3.5 mm`
female sockets, cable, and touchpad holder before tightening the case; a taller
connector stack can foul the holder.
