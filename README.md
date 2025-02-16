# Anchors and Tags EUI naming rules

## Anchors

char EUI[] = "AA:BB:CC:DD:EE:FF:00:0X";

## Tags

char EUI[] = "AA:BB:CC:DD:EE:FF:0Y:0Y";

# Ranging data form

Range: 0.00 m    RX power: -60.77 dBm distance between anchor/tag:11 from Anchor 00:03

```c
String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
rangeString += recv_data[2]; rangeString += recv_data[3];
rangeString += " from Anchor ";rangeString  += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];
```

# States

built_coord_1, --> Anchor A 當anchor v.s. TWR of Anchor B & C 當tag

build_coord_2, --> Anchor A & B 當anchor v.s TWR of Anchor C 當tag( after this we can get a 3D coordinate)

self_calibration --> Anchor A & B & C & D 當anchor v.s. TWR of other Anchors 當tag( After this we get all Anchors' position and hence self calibration cpmplete)

flying --> All Anchors v.s TWR of Tags


* [ ]  add class stateMchine into uwb_common


Error Message:
Exception in thread Thread-12 (processing_thread):
Traceback (most recent call last):
  File "C:\Users\User\AppData\Local\Programs\Python\Python312\Lib\threading.py", line 1075, in _bootstrap_inner
    self.run()
  File "C:\Users\User\AppData\Local\Programs\Python\Python312\Lib\threading.py", line 1012, in run
    self._target(*self._args, **self._kwargs)
  File "c:\Gozzz\2025SAFMC\safmc_uwb\Serial_testing\readingSerialUwbData\main.py", line 432, in processing_thread
    build_3D_coord.build_3D_coord(anchor_list)
  File "c:\Gozzz\2025SAFMC\safmc_uwb\Serial_testing\readingSerialUwbData\build_3D_coord.py", line 121, in build_3D_coord
    D = build_distance_matrix(anchorABCD_distance)
        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  File "c:\Gozzz\2025SAFMC\safmc_uwb\Serial_testing\readingSerialUwbData\build_3D_coord.py", line 20, in build_distance_matrix
    D[i,j] = anchorABCD_distance[(chr(i+65), chr(j+65))]
             ~~~~~~~~~~~~~~~~~~~^^^^^^^^^^^^^^^^^^^^^^^^
TypeError: list indices must be integers or slices, not tuple