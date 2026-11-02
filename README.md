# Auto scanner device

A motor-driven surface probe that rides on the spindle of a PrintNC CNC router. A NEMA 17 drives a rack and
pinion that lowers a probe tip onto the work; an optical switch sees the touch, and a second one the end of
travel. An Arduino Pro Micro runs it through an A4988 driver and talks to the sender over USB, so the CNC can
scan the surface of the stock and level the G-code to it.

> **Made to work with [FluidNCSender](https://github.com/mikeoverbay/FluidNCSender)**, my G-code sender for FluidNC.
> Its Scan panel connects to the probe, moves the CNC over a grid of points, probes each one (allowing for the
> probe's offset from the spindle), and levels the G-code with the scan.

## Parts list

See **[BOM.md](BOM.md)**: the bought parts with where they came from and how many, the parts to make, and how the
assembly goes together.

## Code

The firmware is **[firmware/PROBE_OS/PROBE_OS.ino](firmware/PROBE_OS/PROBE_OS.ino)**, for the Arduino Pro Micro
(ATmega32U4: select *Arduino Leonardo* or a Pro Micro board in the Arduino IDE). It needs the **FastLED** library.
Over USB it shows up as "AutoProbe" by "Coffee CNC", a serial port at 115200 baud.

> ⚠ This is the copy from the CNC PC (2025-12-21). The copy in `OneDrive\Documents\Arduino\PROBE_OS` on the
> Windows PC may be newer; if it is, it replaces this one. This copy reads commands only between moves, and it
> ignores `A` (the abort FluidSender sends when a scan is stopped).

### Pins

| Pro Micro pin | Goes to |
|---|---|
| 5 | RGB LED (WS2811-type data line) |
| 21 (A3) | Home opto switch (`opto travel`) |
| 20 (A2) | Touch opto switch (`opto touch`) |
| 16 | A4988 STEP |
| 10 | A4988 DIR |
| 14 (A0) | A4988 MS3 |

### Serial commands (one per line)

| Command | Does | Answers |
|---|---|---|
| `H` | Home: retract fully. The next touch reads 0.0 | `homed` |
| `ZR:0.200` | How far to back off after a touch (inches) | `OK ZR=0.200` |
| `P` | Probe down to the surface | the height in inches, relative to the first touch after `H`; `ERROR` on a miss |
| `I` | Start probing (first touch) | |
| `X` | Go idle | |

Travel is 4.5 in. One step moves `0.03937 × 16 × π / 1600` in: the code takes the pinion's pitch circle as 16 mm
across and the motor as 1600 steps per turn.
