from setuptools import setup, Extension
from Cython.Build import cythonize

setup(
    name="dsp",
    version="0.0.1",
    description="SDR-related DSP components implementated for practice.",
    ext_modules=cythonize(["src/pll.pyx", "src/fir.pyx", "src/fm.pyx", "src/am.pyx"]),
    zip_safe=False
)
