from setuptools import setup, Extension
from Cython.Build import cythonize


extensions = [
    Extension('dsp.agc', ['src/agc.pyx']),
    Extension('dsp.decoding', ['src/decoding.pyx']),
    Extension('dsp.pll', ['src/pll.pyx']), 
    Extension('dsp.fir', ['src/fir.pyx']), 
    Extension('dsp.am', ['src/am.pyx']), 
    Extension('dsp.fm', ['src/fm.pyx']),
]

setup(
    name="dsp",
    version="0.0.1",
    description="SDR-related DSP components implementated for practice.",
    ext_modules=cythonize(extensions),
    zip_safe=False

)
