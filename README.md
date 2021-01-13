# kernel_cmdline_extractor
Outputs data to stdout that is associated with a label i.e shredos="xxxxx xxxxx xxxx" from /proc/cmdline. Specically written for ShredOS but could be used elsewhere.

Given a specific label from the kernel command line (cat /proc/cmdline) as input, this program will read the kernel command line
searching for the specified label and output the associated data. If the data associated with the label is double quoted then the
data can include embedded whitespace characters. If not quoted then only the first word after the = symbol is read.

### Compilation:
```
./autogen.sh
./configure
make
```

### Syntax:
kernel_cmdline_extractor label
   where label is a word followed by a equals symbol in /proc/cmdline. i.e. shredos=--autonuke
   the label option specified after 'kernel_cmdline_extractor' should not include the '=' symbol.

### Examples:
If the kernel command line is as follows
```
linux /boot/shredos console=tty3 loglevel=3 shredos="--autopoweroff --nousb" loadkeys=uk
```
then specifing shredos as the argument to kernel_cmdline_extractor would output ..
```
#> kernel_cmdline_extractor shredos
#> --autopoweroff --nousb
```
For options that can be used with shredos, see nwipe's help, i.e. nwipe --help

In addition the keyboard could be set from the data associated with the label loadkeys
```
#> kernel_cmdline_extractor loadkeys
#> uk
```
#### Return codes:
* 0 = success
* 1 = unable to open /proc/cmdline
* 2 = unable to read /proc/cmdline
* 3 = No label specified by user
* 4 = label not found in kernel command line
* 5 = 4096 byte kernel command line was actually read. Not very likely!
