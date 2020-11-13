import os
import pathlib
from setuptools import setup
from distutils.core import Extension

if __name__ == '__main__':
	
	current_directory = os.path.dirname(os.path.abspath(__file__))
	module = Extension(
		'pylzf',
		sources = [
			os.path.join(current_directory, "lib", "liblzf-3.6", "lzf_c.c"),
			os.path.join(current_directory, "lib", "liblzf-3.6", "lzf_d.c"),
			os.path.join(current_directory, "src", "pylzf.c")
		],
		extra_compile_args = ["-O3", "-Wdisabled-optimization", "-I./lib"],
		language = 'c'
	)

	setup(
		name="pylzf",
		version="1.0.0",
		description="A simple compression library.",
		author="https://github.com/00xc",
		author_email="00xc@protonmail.com",
		setup_requires=['wheel'],
		ext_modules=[module],
	)