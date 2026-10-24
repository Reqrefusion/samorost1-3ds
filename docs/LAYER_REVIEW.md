# Complete layer review

All source definitions and frames were structurally inspected; semantic assemblies and their contact masks were reviewed separately. Shared definitions inherit their actual actor context; unreachable source assets are listed separately. Depth placement is an artistic draft, not recovered physical distance.

| Scene | Images | Timelines | Vectors | Frames | Named objects | Groups | Unused definitions |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Univerzal | 31 | 1 | 0 | 1 | 0 | 1 | 20 |
| Start | 154 | 21 | 0 | 2028 | 1 | 1 | 1 |
| Lyzar | 493 | 77 | 115 | 4076 | 13 | 3 | 23 |
| Rybar | 734 | 183 | 0 | 3178 | 17 | 4 | 100 |
| Vcely | 728 | 47 | 781 | 4660 | 16 | 6 | 2 |
| Strom | 364 | 53 | 270 | 2661 | 14 | 5 | 2 |
| Vodarna | 247 | 35 | 289 | 2481 | 26 | 7 | 1 |
| Strojovna | 195 | 42 | 0 | 2070 | 20 | 4 | 26 |

## Placement decisions

* Connected photo tiles remain on one base plane so seams do not open.
* Actor poses, wings, fingers, mouths and carried objects inherit one assembly.
* Shadows, ropes, knobs, printed labels and their supporting objects share contact planes.
* Translucent smoke, light and swarm frames do not gain depth from alpha or frame index.
* Existing rocket and bird approach curves remain active. Other assemblies use stable depths unless an authored approach cue has been reviewed.
* Runtime fades and credits use the screen plane.
* Original paint order is retained; depth profiles never sort transparent objects.

`layer-profiles.json` is the editable source. `layer-inventory.json` contains every definition and its contextual assignments. Inherited animation parts are reviewed and deliberately grouped, rather than assigned arbitrary independent depth.

## Start

| Assembly | Resting plane | Relief gain | Members |
| --- | ---: | ---: | --- |
| credits | 0 | 0 | sprite 600 |

## Lyzar

| Assembly | Resting plane | Relief gain | Members |
| --- | ---: | ---: | --- |
| collectors on moss | -0.24 | 0 | Sberac1, Sberac2, Sberac3, Sberac4, Sberac5, Sberac6, Sberac7 |
| zipline, carrier and lock | -0.16 | 0 | Cedulka, Lyzar, Cudl, image 1786, image 1788, image 1790, image 1787, image 1789, image 1662, image 1664, image 1777, sprite 638, sprite 675 |
| smoker, hand, chair, shadow and pipe | -0.05 | 0 | Tabak, Kurak, Kour, image 1302, image 1301, image 1497, sprite 642 |

## Rybar

| Assembly | Resting plane | Relief gain | Members |
| --- | ---: | ---: | --- |
| fisher and fishing equipment | -0.3 | 0 | Rybar, Voda, Kostricka, Kour |
| newts and goats on the plate | -0.28 | 0 | Mloci, Koza1, Koza2, Koza3, Mlok1, Mlok2, Mlok3, Mlok4, Mlok5, Mlok6 |
| bee in front of the plate | 0.08 | 0 | Vcela |
| waiting passenger | 0.1 | 0 | Lyzar |

## Vcely

| Assembly | Resting plane | Relief gain | Members |
| --- | ---: | ---: | --- |
| lift, support and passenger | 0.02 | 0 | Gondola, GondolaObj, Skret, image 1839, sprite 625 |
| tree, trunk cover and articulated sucker | -0.3 | 0 | Strom, Sosak, SosakTam, KrytkaSosakStrom, Zasek, image 1838, sprite 610, sprite 620 |
| snail on bark | -0.2 | 0 | SnekLeze, Snek |
| flowers and pollinating swarm | 0.18 | 0 | Kvety, Vcely |
| bulb, light and switch | -0.12 | 0 | Zarovka, Svetlo, Vypinac |
| skull and small ambient motion | -0.26 | 0 | Lebka, sprite 645 |

## Strom

| Assembly | Resting plane | Relief gain | Members |
| --- | ---: | ---: | --- |
| front hollow cover, record player, cabinet and squirrel | -0.1 | 0 | Gramec, Veverka, Dutina, image 1312 |
| rear bark plate | -0.35 | 0 | image 1301 |
| branch, rope, gondola, owl and passenger | -0.08 | 0 | Lano, Gondola, Vetev, SovaPozice, Sova, Skret, image 1302, image 1305 |
| woodpecker and worms | -0.2 | 0 | Datel, Cerv1, Cerv2, Cerv3, Cerv4 |
| small background motion | -0.35 | 0 | sprite 600 |

## Vodarna

| Assembly | Resting plane | Relief gain | Members |
| --- | ---: | ---: | --- |
| rock abutment at the bridge | -0.12 | 0 | image 1301 |
| bridge, railings, crank, ants and hero | -0.12 | 0 | Mravenec, Mravec, Klika, Skret, GondolaPad, Gondola, image 1312, image 1315, image 1445, image 1353, image 1542, image 1311, image 1404 |
| door at the back of the bridge | -0.08 | 0 | Dvere |
| switchboard and all six knobs | -0.18 | 0 | Rozvadec, Knob1, Knob2, Knob3, Knob4, Knob5, Knob6 |
| mushroom stems and caps | -0.1 | 0 | Houba1, Houba2, Houba3, Houba4, Houba5, Houba6, Houba7, Houba8, Houba9, Houba10 |
| first moth | 0.05 | 0 | Mura2 |
| second moth | 0.1 | 0 | Mura1 |

## Strojovna

| Assembly | Resting plane | Relief gain | Members |
| --- | ---: | ---: | --- |
| wall panels | -0.45 | 0.35 | image 1492, image 1493, image 1302, image 1303 |
| connected machine, wheel shadow, ladder, dials, labels and hero | -0.1 | 0 | KoloStin, Kolo, Zebrik, Paka, Skret, Rafika1, Rafika2, Vypinac, Dioda, BtnPlus, BtnMinus, BtnZebr, BtnKour1, BtnKour2, BtnKour3, Kour0, Kour1, Kour2, Kour3, image 1307, image 1314, image 1305, image 1424, image 1301, image 1308, image 1310, image 1426, image 1414, image 1491, image 1490, image 1439, image 1304, image 1309, image 1311, image 1468 |
| hanging bulb and its light | 0.06 | 0 | Svetlo, image 1488 |
| exterior planet rotation | -0.2 | 0.65 | sprite 601 |

## Animation handoffs

* Rybar passenger blends from his waiting plane to the bird during jump frames 10..35.
* Rybar bone blends to the incoming bird during arrival frames 200..246.

## Contact-mask checks

`check_layer_contacts.py` compares first-pose alpha silhouettes and vector masks for the six interactive scenes. This detected and corrected the tree-hollow cover, branch/bark joint and bridge rock/railing contact. It is an initial-pose geometric check, not a claim that every animation frame has been visually reviewed on hardware.
