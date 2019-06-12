# gbfix
Fixes GameBoy ROMs internal Header Data, and allows setting Parameters in the Header
# Compile
```
g++ -o gbfix gbfix.cpp
```
# Usage
```
gbfix - GameBoy Header Fixer, by El Seyf
        Fixes GameBoy ROMs internal Header Data
Usage: gbfix [options] "in_file"

Options:
	-h		Show this help and quit
	-i <File>	Use <File> as Input
	-o <File>	Output file to <File> (Default: <in_file>_fix)
	-n <Name>	Use <Name> as Header Name
				(up to 15 characters) (Default: "")
	-gbc		Set GBC Flag in Header (Default: False)
	-l <Code>	Use <Code> as Header License Code
				(Dec (N) or Hex (0xNN)) (Default: 0)
	-c <Type>	Use <Type> as Header Cartridge Type
				(Dec (N) or Hex (0xNN)) (Default: 0)
	-rom <Banks>	Use <Banks> as Header Rom Banks Number
				(Dec (N) or Hex (0xNN)) (Default: 0)
	-ram <Size>	Use <Size> as Header Ram Size
				(Valid <Size>: 0k, 2k, 8k, 32k, 64k, 128k) (Default: 0k)
```
