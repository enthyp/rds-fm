import numpy as np


def read_real(filepath, dtype):
    with open(filepath, 'rb') as input_file:
        data = np.fromfile(input_file, dtype=dtype)
    return data
    
def read_iq(filepath, dtype):
    data = read_real(filepath, dtype)
    data = np.reshape(data, (-1, 2))
    iq_samples = data[:, 0] + 1j * data[:, 1]
    return iq_samples
 
