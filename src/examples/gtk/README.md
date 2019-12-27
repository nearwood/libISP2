# libISP2 GUI example

This console application opens a file or serial port and reads any ISP data.
It formats this data into a simple curses output that's updated constantly.

<!--TODO: Add image -->

## Usage

`libISP2_gtk [-d] [-s] file`

### Examples

#### Use raw serial dump

`libISP2_gtk -d data/dual-lc2-warmup.bin`

#### Use a serial port for real data

`libISP2_gtk -s /dev/ttyS0`

***Note: Don't use `-d` with `-s`***
