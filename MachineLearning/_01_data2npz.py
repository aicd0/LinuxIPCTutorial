# convert source data into npz files
# last update: 2021/10/4

import numpy as np
import os

from scipy import signal

input_path = 'data\\'
output_path = 'outputs\\01_data2npz\\'

def main():
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    for file_name in os.listdir(input_path):
        input_file_path = input_path + file_name
        output_file_path = output_path + file_name + '.npz'

        with open(input_file_path) as f:
            data = f.read()
            data = data.split( )
            data = [float(r) for r in data]
            data = data[0 : 45000 * 4 : 4]

            fs = 3000
            lowcut = 1
            highcut = 30
            order = 2
            nyq = 0.5 * fs
            low = lowcut / nyq
            high = highcut / nyq
            b,a = signal.butter(order, [low, high], btype='band')
            data_denoised = signal.lfilter(b, a, data)

            np.savez(output_file_path, data=data_denoised)
            print(output_file_path)

if __name__ == '__main__':
    main()