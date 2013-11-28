# Makefile

############################################################
# Authors:  James W Hegeman                                #
#           Vivek B Sardeshmukh                            #
#                                                          #
# Instructions:                                            #
#          make       -- Build DLP project                 #
#          make clean -- Delete object files               #
#                                                          #
############################################################

COMPILER=gcc
LIBS=-lm -lpthread
OPTM=-O1
OPTS=-m64 -fPIC -g
OBJ=DistLdarProcFVar.o DistributeMaster.o DistributeSlave.o LdarReader.o MergeMaster.o MergeSlave.o ProcessData.o Triangle.o

DLP:
	$(COMPILER) $(OPTS) -c *.c -I . -pg 
	$(COMPILER) $(OPTS) $(LIBS) -I . -o DistLdarProc *.o $(OPTM) -pg

clean:
	rm -f DistLdarProc
	rm -f *.o
