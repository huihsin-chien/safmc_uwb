# SAFMC UWB Multi-UAV Indoor Localization

Indoor positioning system for multi-UAV coordination using Ultra-Wideband (UWB) ranging.
**Result: <10 cm accuracy · 4th Place, Singapore Amazing Flying Machine Competition (SAFMC 2025)**

## Overview

GPS fails indoors. This system uses UWB radio ranging between anchor nodes to localize multiple UAVs simultaneously in a 20 m × 20 m space — no external infrastructure needed beyond the anchor nodes.

## System Architecture

```
[UAV tag] ──TWR──> [Anchor A]
                   [Anchor B]  →  Multilateration (least-squares)  →  XYZ position
                   [Anchor C]
```

1. **Two-Way Ranging (TWR)** — ESP32-controlled UWB modules exchange timestamps to compute precise inter-node distance, cancelling clock drift
2. **Multilateration** — distances from ≥3 anchors solved via least-squares optimization in Python
3. **ROS publisher** — real-time XYZ position broadcast for UAV flight controller

## Hardware

- ESP32 microcontrollers + UWB radio modules (one per UAV + fixed anchors)
- Fixed anchor nodes at known positions in the space

## Stack

- **Python** — least-squares multilateration solver, ROS topic publisher
- **ESP32 firmware** — Two-Way Ranging (TWR) protocol
- **ROS** — inter-system position broadcast

## Results

| Metric | Value |
|--------|-------|
| Positioning accuracy | **< 10 cm** |
| Coverage area | 20 m × 20 m |
| Competition | SAFMC 2025 — Multi-UAV Collaboration |
| Final standing | **4th Place** |

## Competition

Developed for the [Singapore Amazing Flying Machine Competition (SAFMC) 2025](https://www.safmc.sg/), placing **4th** in the Multi-UAV Collaboration category.
