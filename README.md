# ws2txt

Version 0.1

A rudimentary utility for converting WordStar (v4 and earlier) files to plain
ASCII text, or viewing them on screen. I wrote this utility mostly to
experiment with ways of running the HI-TECH C compiler on Linux, under a CP/M
emulator.

WordStar version 5 and later saves files that are almost human-readable --
at least when only ASCII characters are used. Version 4 and earlier
use the high bit of some bytes in the document to carry formatting
hints. It is these spurious high bits that make WS documents difficult
to read as plain text.

## Usage

    ws2txt [/v] [/h] [/p] [/o text_file] {wordstar_file}

The switches are case-insensitive (which is unavoidable on CP/M), and
you can use `-` rather than `/` if preferred. 

If no `/o` (output) switch is given, output is to standard out. 

The `/p` switch specifies that output
is to written a page (screen) at a time, with a prompt to display
the next line or page. This switch has no effect if output is to a
file.

`/h` shows the help message; `\v` shows the version. 

## Building

`ws2txt` can be built in any of these three ways:

- On CP/M for CP/M
- On Linux for CP/M
- On Linux for Linux

The CP/M builds, whether building on Linux or CP/M, are designed to 
use the 3.09 version of HI-TECH C for Z80, which is maintained by
Tony Nicholson, and available in his GitHub repository:

https://github.com/agn453/HI-TECH-Z80-C

Most likely the original HI-TECH distribution would work as well, but
you would need to change `linq` to `link` in the CP/M `Makefile`.

### Building for CP/M on CP/M

You'll need to install the HI-TECH C binaries on a drive, and the
program source in (ideally) a different drive. Tony Nicholson's
version of the compiler is designed (I think) to be installed on 
drive C:, rather than the A: of the original. To use a different
drive than C:, you can create a file `ENVIRON` in the drive
that contains the source code. This file should indicate where the
compiler is installed. So, with the compiler files on drive H:, and
program files on B:, `B:ENVIRON` should contain: 

    HITECH=H:

Then you should be able to build `ws2txt.com` as follows:

    B> H:c309 -cpm -ows2txt.com main.c getopt.c compat.c

### Building on Linux for CP/M

The Linux build for CP/M is controlled by the `Makefile`. This
chains together all the various build steps using the HI-TECH
tools. A bunch of shell scripts run these tools in ways that can make
sense in a `Makefile`.

The Linux build uses Joseph Allen's CP/M emulator for Linux. It is
available here:

https://codeload.github.com/jhallen/cpm/zip/refs/heads/master 

You'll need to build and install this before building `ws2txt`.

You can copy  the HI-TECH C binaries directly in the program's source
directory. Alternatively, if you have `lbrate` on your system, you
can do `make prepare` to download and unpack the latest version
of the HI-TECH tools from GitHub.  

`make unprepare` will remove the HI-TECH tools without cleaning
anything else.

The file `ENVIRON` in the source directory is necessary, because
the CP/M emulator has only an 'A' drive, so the HI-TECH C compiler
must look on `A:`, regardless of its defaults.

All being well, running `make` should do everything that is required.
Even under emulation, its about a hundred times faster to build on
a modern Linux system than on CP/M -- but a key part of this whole
exercise is that the source code is the same, and the utility 
_will_ build on CP/M.

### Building on Linux for Linux

Just run:

    make -f Makefile.linux

The Linux version runs essentially the same source as the CP/M version.
Notable differences are the terminal handling -- GCC does not have a
native function to read a character without echoing it -- and
end-of-line handling. When writing the converted document to a file,
the Linux version removes carriage-return characters, while the
CP/M version leaves them in. 

## Limitations

`ws2txt` does not handle WordStar 'dot commands', that is, formatting
instructions that occupy a whole line starting with '.'. 
These lines are passed through to the output. In general, dot 
commands don't make the document unreadable as text. It wouldn't be
hard to add support to the program for handling these commands but,
other than eliminating them from the output, there probably isn't much
that can be done with them.

## CP/M notes

`WS2TXT.COM` is about 11kB in size. This is _enormous_, for a utility as
simple as this. To understand why this is, see

https://kevinboone.me/cpm-c2.html

