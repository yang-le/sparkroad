# SparkRoad FPGA

## FPGA EG4S20NG88

- In-chip riscv core, support RV32IMC, running at 24MHz
- 8KB BRAM, which can be used for the ZSBL
- UART @ 115200, 8N1

### Memory map

| start         | stop | size | usage     |
| ------------- | ---- | ---- | --------- |
| 0x00000000 | 0x00001FFF | 8KB | BRAM |
| 0x00002000 | 0x0FFFFFFF | ~255MB | unused     |
| 0x10000000 | 0x10000000 | 1B | GPIO     |
| 0x10000001 | 0x1000000F | 16B | unused     |
| 0x10000010 | 0x1000001F | 16B | UART     |
| 0x10000020 | 0xFFFFFFFF | ~4GB | unused     |

## User Flash 8Mbit

| description | pin number |
| ----------- | ---------- |
| CS | P54  |
| MISO | P55  |
| WP | P57  |
| HOLD | P61  |
| CLK | P60  |
| MOSI | P59  |

## RGB LED

| description | pin number |
| ----------- | ---------- |
| BLUE | P19  |
| GREEN | P23  |
| RED | P17  |

## User Button

| description | pin number |
| ----------- | ---------- |
| A (reset) | P16  |
| B | P18  |

## SD Card

| description | pin number |
| ----------- | ---------- |
| D0 | P63  |
| D1 | P62  |
| D2 | P75  |
| D3 | P74  |
| CLK | P64  |
| CMD | P66  |
