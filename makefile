.PHONY: clean
CC = g++
WD = ./work/demo
FN = ./work/demo/out.hgr

all: 
	$(CC) kwayFM.cpp -O3 -o kwayFM

ibm1:
	./kwayFM -h ./work/ispd/ibm01/ibm01.hgr -p ./input/ibm18/1.txt -k 2 -e 1.1

ibm2:
	./kwayFM -h ./work/ispd/ibm02/ibm02.hgr -p ./input/ibm18/2.txt -k 2 -e 1.1

ibm3:
	./kwayFM -h ./work/ispd/ibm03/ibm03.hgr -p ./input/ibm18/3.txt -k 5 -e 1.1

ibm4:
	./kwayFM -h ./work/ispd/ibm04/ibm04.hgr -p ./input/ibm18/4.txt -k 5 -e 1.1

ibm5:
	./kwayFM -h ./work/ispd/ibm05/ibm05.hgr -p ./input/ibm18/5.txt -k 5 -e 1.1

ibm6:
	./kwayFM -h ./work/ispd/ibm06/ibm06.hgr -p ./input/ibm18/6.txt -k 5 -e 1.1

ibm7:
	./kwayFM -h ./work/ispd/ibm07/ibm07.hgr -p ./input/ibm18/7.txt -k 8 -e 1.1

ibm8:
	./kwayFM -h ./work/ispd/ibm08/ibm08.hgr -p ./input/ibm18/8.txt -k 8 -e 1.1

ibm9:
	./kwayFM -h ./work/ispd/ibm09/ibm09.hgr -p ./input/ibm18/9.txt -k 8 -e 1.1

ibm10:
	./kwayFM -h ./work/ispd/ibm10/ibm10.hgr -p ./input/ibm18/10.txt -k 8 -e 1.1

ibm11:
	./kwayFM -h ./work/ispd/ibm11/ibm11.hgr -p ./input/ibm18/11.txt -k 13 -e 1.1

ibm12:
	./kwayFM -h ./work/ispd/ibm12/ibm12.hgr -p ./input/ibm18/12.txt -k 13 -e 1.1

ibm13:
	./kwayFM -h ./work/ispd/ibm13/ibm13.hgr -p ./input/ibm18/13.txt -k 13 -e 1.1

ibm14:
	./kwayFM -h ./work/ispd/ibm14/ibm14.hgr -p ./input/ibm18/14.txt -k 13 -e 1.1

ibm15:
	./kwayFM -h ./work/ispd/ibm15/ibm15.hgr -p ./input/ibm18/15.txt -k 64 -e 1.1

ibm16:
	./kwayFM -h ./work/ispd/ibm16/ibm16.hgr -p ./input/ibm18/16.txt -k 64 -e 1.1

ibm17:
	./kwayFM -h ./work/ispd/ibm17/ibm17.hgr -p ./input/ibm18/17.txt -k 128 -e 1.1

ibm18:
	./kwayFM -h ./work/ispd/ibm18/ibm18.hgr -p ./input/ibm18/18.txt -k 128 -e 1.1

clean:
	rm kwayFM $(WD)/*.log