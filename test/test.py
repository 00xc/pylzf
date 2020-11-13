import os
import random
import time
import sys
from pylzf import compress, decompress

SIZES = (2, 3, 4, 5, 6, 7, 8, 9, 10, 100, 1_000, 10_000, 100_000, 1_000_000, 5_000_000, 10_000_000, 50_000_000)

if __name__ == '__main__':

	for size in SIZES:

		print("[*] Obtaining random bytes", end="\r")
		original = os.urandom(size)

		t0 = time.perf_counter()
		cmpr = compress(original)
		t0 = time.perf_counter() - t0

		assert(original == decompress(cmpr))
		print("[+] Compressed/decompressed {:8d} random bytes in {} ms".format(size, round(t0*1000, 3)))
