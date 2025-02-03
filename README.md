# Anchors and Tags EUI naming rules

## Anchors

char EUI[] = "AA:BB:CC:DD:EE:FF:00:0X";

## Tags

char EUI[] = "AA:BB:CC:DD:EE:FF:0Y:0Y";

# Ranging data form

Range: 0.16 m    RX power: -70.59 dBm    distance from anchor/tag: 30   Sampling: 2.09 Hz    Anchor:X:0X

String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance from anchor/tag:";
rangeString += recv_data[2]; rangeString += recv_data[3];
rangeString += "\t Sampling: "; rangeString += samplingRate; rangeString += " Hz    Anchor:" ; rangeString  += EUI[18]; EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];

# States

built_coord_1, --> Anchor A v.s. TWR of Anchor B & C

build_coord_2, --> Anchor A & B v.s TWR of Anchor C ( after this we can get a 3D coordinate)

self_calibration --> Anchor A & B & C v.s. TWR of other Anchors ( After this we get all Anchors' position and hence self calibration cpmplete)

flying --> All Anchors v.s TWR of Tags

# TODO

* [ ]  add class stateMchine into uwb_common
