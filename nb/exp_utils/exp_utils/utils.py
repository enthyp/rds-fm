import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from scipy.fftpack import fftshift

def plot_some(arrays, start=0, plt_len=10000, style='-'):
    plt.figure(figsize=(20, 10))
    for i, a in enumerate(arrays):
        plt.plot(a[start:start + plt_len], style, label=f'arg_{i + 1}', alpha=1 / len(arrays))
    plt.legend()
    
def plot_several(arrays, style='-'):
    plt.figure(figsize=(20, 10))
    for i, a in enumerate(arrays):
        delim = a[-1]
        if len(a) == 3:
            x_s = a[0][delim[0]:delim[0] + delim[1]]
            y_s = a[1][delim[0]:delim[0] + delim[1]]
        elif len(a) == 2:
            x_s = [x for x in range(delim[0], min(delim[0] + delim[1], a[0].shape[0]))]
            y_s = a[0][delim[0]:delim[0] + delim[1]]
        plt.plot(x_s, y_s, style, label=f'arg_{i + 1}', alpha=1 / len(arrays))
    plt.legend()

def welch(samples, sample_rate, nper=1024, fsize=(20, 10)):
    f, Pxx = signal.welch(samples, sample_rate, nperseg=nper, detrend=lambda x: x)
    f, Pxx = fftshift(f), fftshift(Pxx)
    ind = np.argsort(f)
    f, Pxx = np.take_along_axis(f, ind, axis=0), np.take_along_axis(Pxx, ind, axis=0)
    
    plt.figure(figsize=fsize)
    plt.semilogy(f/1e3, Pxx)
    plt.xlabel('f [kHz]')
    plt.ylabel('PSD [Power/Hz]')
    plt.grid()

    plt.xticks(np.linspace(-sample_rate/2e3, sample_rate/2e3, 31))
    plt.xlim(-sample_rate/2e3, sample_rate/2e3)
    
def read_real(filepath, dtype):
    with open(filepath, 'rb') as input_file:
        data = np.fromfile(input_file, dtype=dtype)
    return data
    
def read_iq(filepath, dtype):
    data = read_real(filepath, dtype)
    data = np.reshape(data, (-1, 2))
    iq_samples = data[:, 0] + 1j * data[:, 1]
    return iq_samples
    
