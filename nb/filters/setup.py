from setuptools import setup, Extension
from Cython.Build import cythonize

extensions = [
	Extension("filters", ["src/*.pyx"])
]

setup(
    name="filters",
    version="0.0.1",
    description="Filter implementations for practice",
    ext_modules=cythonize(extensions),
	zip_safe=False
)
