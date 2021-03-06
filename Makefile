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
LIBS=-lpthread -lm
OPTM=-O1
OPTS=-Wall -m64 -fPIC
OBJ=DistLdarProcFVar.o DistributeMaster.o DistributeSlave.o LdarReader.o MergeMaster.o MergeSlave.o ProcessData.o RMSECalc.o Triangulate.o

DLP:
	rm -f core*
	$(COMPILER) $(OPTS) -c *.c -I .
	$(COMPILER) $(OPTS) -I . -o DistLdarProc *.o $(OPTM) $(LIBS)  

clean:
	rm -f DistLdarProc
	rm -f *.o
