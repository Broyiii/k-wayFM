.PHONY: clean
CC = g++
WD = ./work/demo
FN = ./work/demo/out.hgr

all: 
	$(CC) kwayFM.cpp -O3 -o kwayFM

ibm1:
	./kwayFM -h ./work/ispd/ibm01/ibm01.hgr -k 4 -e 0.1

ibm2:
	./kwayFM -h ./work/ispd/ibm02/ibm02.hgr -k 2 -e 0.1

ibm3:
	./kwayFM -h ./work/ispd/ibm03/ibm03.hgr -k 6 -e 0.1

ibm4:
	./kwayFM -h ./work/ispd/ibm04/ibm04.hgr -k 6 -e 0.1

ibm5:
	./kwayFM -h ./work/ispd/ibm05/ibm05.hgr -k 6 -e 0.1

ibm6:
	./kwayFM -h ./work/ispd/ibm06/ibm06.hgr -k 6 -e 0.1

ibm7:
	./kwayFM -h ./work/ispd/ibm07/ibm07.hgr -k 6 -e 0.1

ibm8:
	./kwayFM -h ./work/ispd/ibm08/ibm08.hgr -k 6 -e 0.1

ibm9:
	./kwayFM -h ./work/ispd/ibm09/ibm09.hgr -k 6 -e 0.1

ibm10:
	./kwayFM -h ./work/ispd/ibm10/ibm10.hgr -k 6 -e 0.1

ibm11:
	./kwayFM -h ./work/ispd/ibm11/ibm11.hgr -k 6 -e 0.1

ibm12:
	./kwayFM -h ./work/ispd/ibm12/ibm12.hgr -k 6 -e 0.1

ibm13:
	./kwayFM -h ./work/ispd/ibm13/ibm13.hgr -k 6 -e 0.1

ibm14:
	./kwayFM -h ./work/ispd/ibm14/ibm14.hgr -k 6 -e 0.1

ibm15:
	./kwayFM -h ./work/ispd/ibm15/ibm15.hgr -k 6 -e 0.1

ibm16:
	./kwayFM -h ./work/ispd/ibm16/ibm16.hgr -k 6 -e 0.1

ibm17:
	./kwayFM -h ./work/ispd/ibm17/ibm17.hgr -k 6 -e 0.1

ibm18:
	./kwayFM -h ./work/ispd/ibm18/ibm18.hgr -k 6 -e 0.1

t1:
	./kwayFM_old -h ./work/ispd/ibm01/ibm01.hgr -k 4 -e 0.1

t4:
	./kwayFM_old -h ./work/ispd/ibm04/ibm04.hgr -k 6 -e 0.1

t10:
	./kwayFM_old -h ./work/ispd/ibm10/ibm10.hgr -k 6 -e 0.1

clean:
	rm kwayFM $(WD)/*.log