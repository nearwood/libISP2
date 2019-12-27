# libISP2 CLI example

This console application opens a file or serial port and reads any ISP data.
It formats this data into a simple curses output that's updated constantly.

## Usage

`libISP2_ncurses [-d] [-s] file`

### Examples

#### Use raw serial dump

`libISP2_ncurses -d data/dual-lc2-warmup.bin`

#### Use a serial port for real data

`libISP2_ncurses -s /dev/ttyS0`

***Note: Don't use `-d` with `-s`***

## Example output

```
Reading: ../../data/dual-lc2-warmup.bin
RECORDING: [OFF] LOGGING: [NOPE?]
> Sensor 0
  Status: WARMING UP
  Warmup: 080


> Sensor 1
  Status: WARMING UP
  Warmup: 079
```

```
Reading: ../../data/dual-lc2-warmup.bin
RECORDING: [OFF] LOGGING: [NOPE?]
> Sensor 0
  Status: O2 READING
  Oxygen: 20.9%


> Sensor 1
  Status: O2 READING
  Oxygen: 20.9%
```

### Dependencies

* ncurses
