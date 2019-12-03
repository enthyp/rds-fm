import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from scipy.fftpack import fftshift

   

def welch(samples, sample_rate, nper=1024, fig_size=(20, 10)):
    f, Pxx = signal.welch(samples, sample_rate, nperseg=nper, detrend=lambda x: x)
    f, Pxx = fftshift(f), fftshift(Pxx)
    ind = np.argsort(f)
    f, Pxx = np.take_along_axis(f, ind, axis=0), np.take_along_axis(Pxx, ind, axis=0)
    
    plt.figure(figsize=std_fsize)
    plt.semilogy(f/1e3, Pxx)
    plt.xlabel('f [kHz]')
    plt.ylabel('PSD [Power/Hz]')
    plt.grid()

    plt.xticks(np.linspace(-sample_rate/2e3, sample_rate/2e3, 31))
    plt.xlim(-sample_rate/2e3, sample_rate/2e3)

