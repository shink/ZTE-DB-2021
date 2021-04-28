# coding=utf-8

"""
@author: shenke
@project: ZTE-DB-2021
@file: judge.py
@date: 2021/4/23
@description: 
"""

import os
from tqdm import tqdm
import time


def input(dataset):
    with open(dataset, mode='r', encoding='utf-8') as f:
        lines = f.readlines()

    data = []
    for line in lines:
        data.append(line[line.index('"'):].replace('\n', ''))
    return set(data)


def get_answer(dataset_a, dataset_b):
    data_a = input(dataset_a)
    data_b = input(dataset_b)
    return data_a.difference(data_b)


def judge(cmd, ans):
    start_time = time.perf_counter()
    res = os.popen(cmd)
    res = set(res.buffer.read().decode(encoding='utf-8').strip().split('\n'))
    end_time = time.perf_counter()
    elapsed_time = end_time - start_time

    judge_res = ''
    if res == ans:
        judge_res += "\nAccept!\n"
    else:
        judge_res += "\nWrong answer!\n"

        redundant_res = res - ans
        missing_res = ans - res
        if len(redundant_res) > 0:
            judge_res += "--------------\n"
            judge_res += "Redundant result:\n"
            for res in redundant_res:
                judge_res += (res + "\n")
        if len(missing_res) > 0:
            judge_res += "--------------\n"
            judge_res += "Missing result:\n"
            for res in missing_res:
                judge_res += (res + "\n")

    judge_res += ("Elapsed time: " + str(round(elapsed_time, 4)) + "s\n")
    return judge_res


if __name__ == '__main__':
    datasets = [
        ["/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/input1.csv",
         "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/input2.csv"],
        ["/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/test/input1.csv",
         "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/test/input2.csv"],
    ]
    exe = "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/bin/DB-2021"

    for dataset in tqdm(datasets, ncols=60):
        dataset_a = dataset[0]
        dataset_b = dataset[1]
        ans = get_answer(dataset_a, dataset_b)
        cmd = '\"{}\" \"{}\" \"{}\"'.format(exe, dataset_a, dataset_b)
        judge_res = judge(cmd, ans)
        print(judge_res)
