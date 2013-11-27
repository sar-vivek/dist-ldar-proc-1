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
OPTS=-march=i686 -g
OBJ=DistLdarProcVar.o DistrubteMaster.o DistributeSlave.o LdarReader.o MergeMaster.o MergeSlave.o ProcessData.o 

DLP:
	$(COMPILER) $(OPTS) -c *.c -I . 
	$(COMPILER) $(OPTS) $(LIBS) -I . -o DistLdarProc *.o $(OPTM) 	

clean:
	rm -f *.o
