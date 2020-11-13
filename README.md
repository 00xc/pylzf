# pylzf #
Python 3 bindings for [liblzf](http://oldhome.schmorp.de/marc/liblzf.html). From the liblzf homepage:
> LibLZF is a very small data compression library. It consists of only two .c and two .h files and is very easy to incorporate into your own programs. The compression algorithm is very, very fast, yet still written in portable C.


> LibLZF is an ideal library for the following applications:
> * Fast, real-time compression, where liblzf can save I/O without costing too much CPU time.
> * Block compression. If you have repeated blocks of data (IP packets, application state etc..), then liblzf is ideal to decrease the size at a very small cost, as liblzf is basically free on modern cpus.

## Installation ##
`$ git clone https://github.com/00xc/pylzf.git`\
`$ cd pylzf && python3 -m pip install .`

## Usage ##
Using pylzf is extremely simple. Its API consists of two functions, `compress` and `decompress`:
```Python
import os
import pylzf

random_data = os.urandom(1024)
compressed_data = pylzf.compress(random_data)
decompressed_data = pylzf.decompress(compressed_data)

assert(random_data == decompressed_data)
```