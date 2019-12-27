# libISP2 GUI example

This GTK application opens a file or serial port and reads any ISP data.
It formats this data into a simple gauge output that's updated constantly.

<!--TODO: Add image -->

## TODO

- [ ] Figure out GTK event loop
- [ ] Figure out best way to get data from libISP (pthread, GThread, `g_idle_add()`, etc.)
- [ ] Figure out feature roadmap

## Usage

`libISP2_gtk [-d] [-s] file`

### Examples

#### Use raw serial dump

`libISP2_gtk -d data/dual-lc2-warmup.bin`

#### Use a serial port for real data

`libISP2_gtk -s /dev/ttyS0`

***Note: Don't use `-d` with `-s`***

### Dependencies

* GTK+-3.0
