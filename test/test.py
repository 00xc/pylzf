import os
import time
from pylzf import compress, decompress

SIZES = (2, 3, 4, 5, 6, 7, 8, 9, 10, 100, 1_000, 10_000, 100_000, 1_000_000, 5_000_000, 10_000_000, 50_000_000)

if __name__ == '__main__':

	# Try a wide range of sizes with random data
	for size in SIZES:

		print("[*] Obtaining random bytes", end="\r")
		original = os.urandom(size)

		t0 = time.perf_counter()
		result = decompress(compress(original))
		t0 = time.perf_counter() - t0

		assert(original == result)
		print("[+] Compressed/decompressed random bytes in {} ms ({} bytes)".format(round(t0*1000, 3), size))

	# A badly written decompression wrapper will not allocate enough memory since the compression ratio will be very high.
	original = ("\x00"*10000000 + "B"*10000000).encode()
	t0 = time.perf_counter()
	result = decompress(compress(original))
	t0 = time.perf_counter() - t0

	assert(original == result)
	print("[+] Compressed/decompressed homogeneous data in {} ms ({} bytes)".format(round(t0*1000, 3), len(original)))