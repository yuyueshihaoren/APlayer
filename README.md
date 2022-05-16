# APlayer - Play an encrypted media

Notice: This software is not stable yet! Please consider using it after the first release.

## Usage

Usage:
```
Usage: aplayer [OPTION...] MEDIA_FILE
A simple program using libmpv, defining custom mpv protocols
Modes include:
----------------------
	"popen-gpg"
		Same as --custom-popen-command="gpg -d", decrypt and play GnuPG via pipe (see
security note)
	"popen-cat"
		Who doesn't want to popen a cat?, same as `aplayer --dangerous
--mode=custom-popen-command --custom-command=cat [MEDIA_FILE]`
	"regular-file"
		Play a regular multimedia fileThe following modes need --dangerous
	"custom-popen-command"
		command-string will be executed in a shell and its stdout will be feed in
mpv, [MEDIA_FILE] will be where to specify the command
	"binary-split"
		play a folder filled with output of command split -b=bytes, file names must
start with x and ordered alphabetically. bytes option takes integer
----EXAMPLES-----------
$ ./aplayer --mode=popen-gpg --blk-size=$((5*1024*1024))
~/Videos/free_software_forever.mp4

  -b, --blk-size=blk_size    Integer block size (bytes)
  -C, --custom-command=command   (Dangerous) custom command string
  -D, --dangerous            See popen-command
  -L, --log-level=level      Log level in integers
                             NONE = 0,FATAL = 10,ERROR = 20,WARN = 30,INFO =
                             40,V = 50,DEBUG = 60,TRACE = 70,
  -M, --mode=mode-string     Specify modes, see --help for all modes, default
                             regular-file
  -v, --verbose              Mpv verbose output
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to [Github Issues].
```

## Build

### Ubuntu

#### Dependencies
- libmpv
- libmpv-dev
- cmake

Iall dependencies

```shell
sudo apt update
sudo apt install build-essential cmake libmpv libmpv-dev
```

#### Build with cmake

```shell
cd aplayer # go to the project directory
mkdir build # or any directory name you like
cd build
```

```shell
cmake ..
cmake --build .
```


## License

Copyright 2022 Yue Yu

This file is part of APlayer.

APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.