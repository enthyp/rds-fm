import numpy as np


def read_iq(filepath, dtype):
    data = read_real(filepath, dtype)
    data = np.reshape(data, (-1, 2))
    iq_samples = data[:, 0] + 1j * data[:, 1]
    return iq_samples
    

iq_samples = read_iq('./iq_speech', np.int16)

