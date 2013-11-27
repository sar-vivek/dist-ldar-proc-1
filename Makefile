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
OPTM=-O2
OPTS=-g
OBJ=DistLdarProcVar.o DistrubteMaster.o DistributeSlave.o LdarReader.o MergeMaster.o MergeSlave.o ProcessData.o 

DLP:
	$(COMPILER) $(OPTS) -c *.c -I . -pg 
	$(COMPILER) $(OPTS) $(LIBS) -I . -o DistLdarProc *.o $(OPTM) -pg

clean:
	rm -f *.o
