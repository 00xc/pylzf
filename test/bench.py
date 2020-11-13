import random
import string
import time

import pylzf
import zlib

def mean(x):
	return sum(x)/len(x)


if __name__ == '__main__':

	NTESTS = 20

	with open("/usr/share/dict/american-english", "r") as f:
		words = [w.rstrip() for w in f.readlines()]

	for NWORDS in [100, 1_000, 10_000, 100_000, 500_000]:

		sizes = []
		elapsed = []
		for _ in range(NTESTS):
			data = " ".join(random.choice(words) for _ in range(NWORDS)).encode()

			t0 = time.perf_counter()
			result = pylzf.compress(data)
			t0 = time.perf_counter() - t0

			try:
				dcmpr = pylzf.decompress(result)
				assert(data == dcmpr)
			except:
				print(len(data), "->", len(result), "->", len(dcmpr))

			sizes.append((len(data), len(result)))
			elapsed.append(t0)

		print("[PYLZF {:6d} words] Total time: {:9.3f} ms | Average time/test: {:7.3f} ms | Avg. space saved: {} %".format(
			NWORDS,
			round(sum(elapsed)*1000, 3),
			round(mean(elapsed)*1000, 3),
			round(mean([(1 - (d[1]/d[0]))*100 for d in sizes]), 3)
		))

		sizes = []
		elapsed = []
		for _ in range(NTESTS):
			data = " ".join(random.choice(words) for _ in range(NWORDS)).encode()

			t0 = time.perf_counter()
			result = zlib.compress(data, level=1)
			t0 = time.perf_counter() - t0

			assert(data == zlib.decompress(result))

			sizes.append((len(data), len(result)))
			elapsed.append(t0)

		print("[ZLIB {:7d} words] Total time: {:9.3f} ms | Average time/test: {:7.3f} ms | Avg. space saved: {} %".format(
			NWORDS,
			round(sum(elapsed)*1000, 3),
			round(mean(elapsed)*1000, 3),
			round(mean([(1 - (d[1]/d[0]))*100 for d in sizes]), 3)
		))
