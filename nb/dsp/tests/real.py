import os
import numpy as np
from dsp.decoding import SymbolDecoder

dir_path = os.path.dirname(os.path.realpath(__file__))
samples = np.load(os.path.join(dir_path, 'rds.npy'))

symbol_rate = 1187.5
sample_rate = 120000
samples_per_symbol = sample_rate / symbol_rate
output_samples_per_symbol = 1
symbol_delay = 3
filterbank_size = 32
matched_filter_rolloff = 0.8

sd = SymbolDecoder(
    sample_rate,
    samples_per_symbol,
    output_samples_per_symbol,
    symbol_delay,
    filterbank_size, 
    matched_filter_rolloff
)

symbols = sd.run(samples)
print(symbols[:10])
