import random
import string
import time

from pylzf import compress, decompress
import zlib

def mean(x):
	return sum(x)/len(x)

if __name__ == '__main__':

	NTESTS = 20

	with open("/usr/share/dict/american-english", "r") as f:
		words = [w.rstrip() for w in f.readlines()]

	for NWORDS in [1000, 10_000, 100_000]:

		print("-"*50)
		print("[*] Launching tests with {} words each".format(NWORDS))
		print("-"*50)

		res = []
		ratio = []
		saved = []
		for _ in range(NTESTS):
			data = "".join(random.choice(words) for _ in range(NWORDS)).encode()
			osize = len(data)

			t0 = time.perf_counter()
			data = compress(data)

			res.append(time.perf_counter() - t0)
			ratio.append(osize/len(data))
			saved.append(1 - (len(data)/osize))

		print(" [pylzf] total time ({} tests): {} ms".format(NTESTS, round(sum(res)*1000, 3)))
		print(" [pylzf] avg. time taken: {} ms".format(round(mean(res)*1000, 3)))
		print(" [pylzf] avg. compression ratio: {}".format(round(mean(ratio), 3)))
		print(" [pylzf] avg. space saving: {} %".format(round(mean(saved)*100, 3)))

		print("-"*50)

		res = []
		ratio = []
		saved = []
		for _ in range(NTESTS):
			data = "".join(random.choice(words) for _ in range(NWORDS)).encode()
			osize = len(data)

			t0 = time.perf_counter()
			data = zlib.compress(data, level=1)
			
			res.append(time.perf_counter() - t0)
			ratio.append(osize/len(data))
			saved.append(1 - (len(data)/osize))

		print(" [zlib] total time ({} tests): {} ms".format(NTESTS, round(sum(res)*1000, 3)))
		print(" [zlib] avg. time taken: {} ms".format(round(mean(res)*1000, 3)))
		print(" [zlib] avg. compression ratio: {}".format(round(mean(ratio), 3)))
		print(" [zlib] avg. space saving: {} %".format(round(mean(saved)*100, 3)))

		print("")
