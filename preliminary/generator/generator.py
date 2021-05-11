# coding=utf-8

"""
@author: shenke
@project: ZTE-DB-2021
@file: judge.py
@date: 2021/4/23
@description:
"""

import random
import time
from tqdm import tqdm

char_arr = [chr(num) for num in range(48, 58)] + [chr(num) for num in range(65, 91)] + [chr(num) for num in range(97, 123)]


def current_timestamp():
    return int(round(time.time() * 1000))


def random_int(min_val, max_val):
    return random.randint(min_val, max_val)


def random_char():
    return random.choice(char_arr)


def random_str(min_len, max_len):
    str_len = random_int(min_len, max_len)
    val = ''.join(random_char() for _ in range(str_len))
    return '\"' + val + '\"'


class Generator:
    line_num_a_range = [80000, 100000]
    line_num_b_range = [70000, 90000]
    candidate_line_range = [(line_num_a_range[0] + line_num_b_range[0]) // 2, (line_num_a_range[1] + line_num_b_range[1]) // 2]
    max_value_num = 1000000

    def __init__(self):
        self.candidate_values = [random_str(10, 256) for _ in range(self.max_value_num)]
        self.candidate_lines = self.__generate_lines(random_int(self.candidate_line_range[0], self.candidate_line_range[1]))

    def __generate_line(self):
        return ','.join([random.choice(self.candidate_values) for _ in range(3)]) + '\n'

    def __generate_lines(self, line_num):
        return [self.__generate_line() for _ in range(line_num)]

    def __generate_file(self, file_path, max_line):
        with open(file_path, 'w') as f:
            line_num = random_int(1, max_line)
            for idx in range(1, line_num):
                line = self.__generate_line() if current_timestamp() % 2 == 0 else random.choice(self.candidate_lines)
                line = str(idx) + ',' + line
                f.write(line)

    def generate(self, dataset_a, dataset_b):
        line_num_a = random_int(self.line_num_a_range[0], self.line_num_a_range[1])
        line_num_b = random_int(self.line_num_b_range[0], self.line_num_b_range[1])
        self.__generate_file(dataset_a, line_num_a)
        self.__generate_file(dataset_b, line_num_b)


if __name__ == '__main__':
    datasets = [
        [
            '/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/test/input1.csv',
            '/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/test/input2.csv'
        ]
    ]

    for dataset in tqdm(datasets, ncols=60):
        Generator().generate(dataset[0], dataset[1])
