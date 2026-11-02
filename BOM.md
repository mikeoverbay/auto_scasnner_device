# Parts list

Taken from the Shapr3D model `Auto Probe Addon.step` (exported 2026-10-17). In the model each part's name is
followed by where it came from; that link is the **Source** column. Quantities are how many times the assembly
uses each part.

## Bought parts

| Part | Qty | Source | In the model |
|---|---|---|---|
| NEMA 17 stepper motor | 1 | [Amazon B01N30ISYC](https://www.amazon.com/dp/B01N30ISYC) | `NEAM 17` |
| Pinion (on the motor shaft) | 1 | [Amazon B0B9L1RQJ5](https://www.amazon.com/dp/B0B9L1RQJ5) | `pinion` |
| Guide rod | 2 | [Amazon B082ZMGJ9B](https://www.amazon.com/dp/B082ZMGJ9B) | `rod L`, `rod R` |
| LM8LUU linear ball bearing: 8 mm bore, 15 mm OD, 45 mm long, rubber seals both sides (sold as a 2-pack) | 2 | [Amazon B0D5BKMLSJ](https://www.amazon.com/dp/B0D5BKMLSJ) | `bearing 1`, `bearing 2` |
| Optical endstop switch | 2 | [Amazon B07PMW2QMT](https://www.amazon.com/dp/B07PMW2QMT) | `opto travel`, `opto touch` |
| RGB LED, 5 mm, 4 pins | 1 | [Pololu 2535](https://www.pololu.com/product/2535) | `LED_D5.0mm-4_RGB_Wide_Pins` |
| Prototype board (the probe board) | 1 | [Amazon B07R49F3MH](https://www.amazon.com/dp/B07R49F3MH) | `probe board_PCB` |
| Arduino Pro Micro | 1 | [Amazon B01MTU9GOB](https://www.amazon.com/dp/B01MTU9GOB) | `ARDUINO PRO MICRO.stp` |
| A4988 stepper driver | 1 | [Amazon B07PFCQH84](https://www.amazon.com/dp/B07PFCQH84) | `A4988-Stepper-Driver-WithPins.stp` |
| Screw terminal block, 2 pin | 1 | [Amazon B09NCSMXJY](https://www.amazon.com/dp/B09NCSMXJY) | `2 pin terminal block.STEP` |
| JST-XH connector, 3 pin (female, with mating plug) | 2 | [Amazon B07Q2MJRBG](https://www.amazon.com/dp/B07Q2MJRBG) | `JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical` |
| JST-XH connector, 4 pin (female, with mating plug) | 1 | [Amazon B07XNW1LG8](https://www.amazon.com/dp/B07XNW1LG8) | `JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical` |
| Electrolytic capacitor, 100 µF, 10 × 17 mm, 5 mm pitch | 1 | [Amazon B0C3C4LKTZ](https://www.amazon.com/dp/B0C3C4LKTZ) | `CP_Radial_D10.0mm_P5.00mm` |
| Film capacitor, 7 × 2 mm, 5 mm pitch | 1 | [Amazon B00NWFDJ1W](https://www.amazon.com/dp/B00NWFDJ1W) | `C_Rect_L7.0mm_W2.0mm_P5.00mm` |
| M5 threaded rod, 200 mm | 1 | [Amazon B0D4VJP383](https://www.amazon.com/dp/B0D4VJP383) | `5mm threaded rod` |
| M5 nut | 1 | *(no link in the model)* | `5mm nut` |

## Made parts (3D printed or machined)

| Part | Qty | In the model |
|---|---|---|
| Cover | 1 | `cover` |
| LED bezel | 1 | `LED Bezel` |
| Wire way | 1 | `wire way` |
| Frame | 2 | `frame` (two different bodies with the same name) |
| Slide | 1 | `slide` |
| Rack | 1 | `rack` |
| Probe tip | 1 | `Probe tip` |

## How it goes together (the model's assembly tree)

```
probe assembly                 (on the PrintNC's Spindle Assembly)
├── cover          cover, LED bezel
├── frame ×2, wire way, rod L, rod R
├── opto travel
├── NEMA 17        with the pinion
├── Probe Assembly slide, rack, probe tip, M5 threaded rod, M5 nut, bearing 1, bearing 2, opto touch
└── probe board    prototype board, Arduino Pro Micro, A4988, RGB LED,
                   JST-XH 3 pin ×2, JST-XH 4 pin, 2-pin terminal block, two capacitors
```

The model's other bodies (named `Imported…`, `brep_…`, `CirPattern1`, `OuterRace`, `Kes-…`, `Çizgiden Bükme…`,
`LPattern2` and the like) are pieces inside the vendors' models of the motor, the board and the connectors, not
separate parts.
