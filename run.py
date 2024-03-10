import os
import sys


def test0():
    for i in range(9):
        os.system(f'./kwayFM -h ./work/ISPD_benchmark/ibm0{i+1}.hgr -e 0.1')
    for i in range(9,18):
        os.system(f'./kwayFM -h ./work/ISPD_benchmark/ibm{i+1}.hgr -e 0.1')


def test1():
    segments: list[int] = [2,2,5,5,5,5,8,8,8,8,13,13,13,13,64,64,128,128]
    for i in range(9):
        os.system(f'./kwayFM -h ./work/ISPD_benchmark/ibm0{i+1}.hgr -p ./input/ibm18/{i+1}.txt -k {segments[i]} -e 0.1')
    for i in range(9,18):
        os.system(f'./kwayFM -h ./work/ISPD_benchmark/ibm{i+1}.hgr -p ./input/ibm18/{i+1}.txt -k {segments[i]} -e 0.1')


def test2():
    segments: list[int] = [2,2,2,5,5,5,5,9,13,13,13,13,13,16,10,16,32,64]
    for i in range(9):
        os.system(f'./kwayFM -h ./work/IBM_weight/ibm0{i+1}.weight.hgr -p ./input/IBM18_weight_0.2/IBM0{i+1}.txt -k {segments[i]} -e 0.2')
    for i in range(9,18):
        os.system(f'./kwayFM -h ./work/IBM_weight/ibm{i+1}.weight.hgr -p ./input/IBM18_weight_0.2/IBM{i+1}.txt -k {segments[i]} -e 0.2')


def test3():
    segments: list[int] = [2,2,2,5,5,5,5,9,13,13,13,13,13,16,10,16,32,64]
    for i in range(9):
        os.system(f'./kwayFM -h ./work/IBM_weight/ibm0{i+1}.weight.hgr -p ./input/IBM18_weight_0.02/IBM0{i+1}.txt -k {segments[i]} -e 0.02')
    for i in range(9,18):
        os.system(f'./kwayFM -h ./work/IBM_weight/ibm{i+1}.weight.hgr -p ./input/IBM18_weight_0.02/IBM{i+1}.txt -k {segments[i]} -e 0.02')


def k2_noweight_0_1():
    for i in range(9):
        os.system(f'./kwayFM -h ./work/ISPD_benchmark/ibm0{i+1}.hgr -p ./input/k2_weight/k2_weight_0.1/IBM0{i+1}.txt -k 2 -e 0.1')
    for i in range(9,18):
        os.system(f'./kwayFM -h ./work/ISPD_benchmark/ibm{i+1}.hgr -p ./input/k2_weight/k2_weight_0.1/IBM{i+1}.txt -k 2 -e 0.1')


def k2_noweight_0_02():
    for i in range(9):
        os.system(f'./kwayFM -h ./work/ISPD_benchmark/ibm0{i+1}.hgr -p ./input/k2_weight/k2_weight_0.02/IBM0{i+1}.txt -k 2 -e 0.02')
    for i in range(9,18):
        os.system(f'./kwayFM -h ./work/ISPD_benchmark/ibm{i+1}.hgr -p ./input/k2_weight/k2_weight_0.02/IBM{i+1}.txt -k 2 -e 0.02')


if __name__ == '__main__':
    if (len(sys.argv) == 2):
        if (sys.argv[1] == '0'):
            test0()
        elif (sys.argv[1] == '1'):
            test1()
        elif (sys.argv[1] == '2'):
            test2()
        elif (sys.argv[1] == '3'):
            test3()
        elif (sys.argv[1] == '4'):
            k2_noweight_0_1()
        elif (sys.argv[1] == '5'):
            k2_noweight_0_02()
