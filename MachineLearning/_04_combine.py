import numpy as np
import os
import dependence.utils as utils

from _01_data2npz import output_path as input_path

output_path = 'outputs\\04_combine\\'
output_file = 'combine.npz'
output_file_path = output_path + output_file

def main():
    if not os.path.exists(output_path):
        os.makedirs(output_path)
    
    combine = np.empty((0))

    utils.check_01()
    for file_name in os.listdir(input_path):
        input_file_path = input_path + file_name
        data = np.load(input_file_path)['data']
        combine = np.concatenate((combine, data))

    size = combine.shape[0]
    new_size = 150
    if size % new_size:
        raise ValueError()
     
    combine = np.reshape(combine, (size // new_size, new_size))

    np.savez(output_file_path, data=combine)
    print(output_file_path + ', shape=' + str(combine.shape))

if __name__ == '__main__':
    main()