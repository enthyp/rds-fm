import os
import numpy as np
from dsp.decoding import SymbolDecoder


def read_real(filepath, dtype):
    with open(filepath, 'rb') as input_file:
        data = np.fromfile(input_file, dtype=dtype)
    return data

def read_iq(filepath, dtype):
    data = read_real(filepath, dtype)
    data = np.reshape(data, (-1, 2))
    iq_samples = data[:, 0] + 1j * data[:, 1]
    return iq_samples

dir_path = os.path.dirname(os.path.realpath(__file__))
iq_samples = read_iq(os.path.join(dir_path, 'iq_speech'), np.int16)

# Try to decode this BS.   
symbol_rate = 1187.5
sample_rate = 120000
samples_per_symbol = sample_rate / symbol_rate
output_samples_per_symbol = 1

sd = SymbolDecoder(
    sample_rate, 
    samples_per_symbol,
    output_samples_per_symbol
)

output = sd.run(iq_samples.real)
print(output[:10])
