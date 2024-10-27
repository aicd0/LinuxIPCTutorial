# visualize the data and save as image files
# last update: 2021/10/4 

import matplotlib.pylab as plt
import numpy as np
import os
import dependence.utils as utils

from _01_data2npz import output_path as input_path

output_path = 'outputs\\02_plot\\'

def main():
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    utils.check_01()
    for file_name in os.listdir(input_path):
        input_file_path = input_path + file_name
        output_file_path = output_path + utils.get_display_name(file_name) + '.png'
        data = np.load(input_file_path)['data']
    
        fig = plt.figure()
        ax = fig.add_subplot(111)
        plt.plot(data, color='r')
        ax.set_title('Denoised Signal')
        plt.ylabel('Amplitude')
        fig.tight_layout()
        plt.savefig(output_file_path)
        plt.close('all')
        print(output_file_path)

if __name__ == '__main__':
    main()