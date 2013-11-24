/*JWHGPLHERE*/
/*
 * DistLdarProcFVar.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistributeSlave.c"
#include "DistributeMaster.c"
#include "MergeSlave.h"
#include "MergeMaster.h"
#include "DistLdarProcFVar.h"

LidarPointNode_t *BinTbl[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
double BinU1[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
double BinU2[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
uint32_t BinCnt[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
uint32_t CellCnt[NUM_CELLS];
LidarPointNode_t CellMin[NUM_CELLS];
LidarPointNode_t CellMax[NUM_CELLS];

char NodeIPs[3][16] = {"128.255.101.181", "128.255.101.133", "128.255.101.11"};

struct sockaddr_in svr_addr[NUM_NODES];
int msock[NUM_NODES];

LidarPointNode_t NodeMin;
LidarPointNode_t NodeMax;

struct timeval t_start;
struct timeval t_end;

double t_diff;
double Xint;
double Yint;
double Xdiff;
double Ydiff;
double Xint_cell;
double Yint_cell;
double Xint_bin;
double Yint_bin;
double X_c;
double Y_c;
double Z_c;
double u1;
double u2;

LidarPointNode_t *PntTbl;
LidarPointNode_t *current;
double *Z2;
double *current2;
int8_t *FiltTbl;
void *X_b;
void *Y_b;
void *Z_b;

int NodeID;
int ssock;
int32_t X;
int32_t Y;
int32_t Z;
uint32_t count;
uint32_t n;

void *Malloc(size_t len) {
    void *ret;
    ret = malloc(len);
    if (ret == NULL) perror("malloc()");
    return ret;
}

int main(int argc, char *argv[]) {

    long int ix;
    long int iy;
    int i;

    if (argc < 3 || argc > 4) {
	fprintf(stderr, "Usage: %s NODE_ID INFILE [ADDRFILE]\n", argv[0]);
	fflush(stderr);
	exit(-1);
    }

    assert(sizeof (char) == CHAR_SIZE);
    assert(sizeof (unsigned char) == UCHAR_SIZE);
    assert(sizeof (uint16_t) == UINT16_SIZE);
    assert(sizeof (int32_t) == INT32_SIZE);
    assert(sizeof (uint32_t) == UINT32_SIZE);
    assert(sizeof (double) == DOUBLE_SIZE);
    assert(NUM_BINS_X >= 2);
    assert(NUM_BINS_Y >= 2);
    assert(NUM_CELLS_X * NUM_CELLS_Y == NUM_CELLS);

    gettimeofday(&t_start, NULL);

    NodeID = (int) strtol(argv[1], NULL, 10);

    LasFileInit(argv[2]);
    count = NumPointRec;

    PntTbl = (LidarPointNode_t *) Malloc(NumPointRec * sizeof (LidarPointNode_t));
    current = PntTbl;

    Z2 = (double *) Malloc(NumPointRec * DOUBLE_SIZE);
    current2 = Z2;

    FiltTbl = (int8_t *) Malloc(NumPointRec * INT8_SIZE);

    X_b = Malloc(XYZ_SIZE);
    Y_b = X_b + INT32_SIZE;
    Z_b = X_b + 2 * INT32_SIZE;

    for (ix = 0; ix < NumPointRec; ++ix) {
	*(FiltTbl + ix) = 0;
    }

    for (i = 0; i < NUM_CELLS; ++i) {
	for (ix = 0; ix < NUM_BINS_X; ++ix) {
	    for (iy = 0; iy < NUM_BINS_Y; ++iy) {
		BinTbl[i][ix][iy] = NULL;
		BinU1[i][ix][iy] = 0;
		BinU2[i][ix][iy] = 0;
		BinCnt[i][ix][iy] = 0;
	    }
	}
    }

    Xint = (MaxX - MinX) / NUM_NODES_X;
    Yint = (MaxY - MinY) / NUM_NODES_Y;
    Xratio = Xscale / Xint;
    Yratio = Yscale / Yint;
    Xdiff = (Xoffset - MinX) / Xint;
    Ydiff = (Yoffset - MinY) / Yint;

    Xint_local = ;
    Yint_local = ;
    X



    if (NodeID == 0) DistributeSend();
    else DistributeReceive();

    WorkerIDs[0] = 0;
    for (i = 1; i <= NUM_WORKERS; ++i) {
	WorkerIDs[i] = i;
	if (pthread_create(&Workers[i], NULL, &ProcessData, &WorkerIDs[i])) perror("pthread_create()");
    }
    ProcessData(&WorkerIDs[0]);

    if (NodeID == 0) MergeSend();
    else MergeReceive();

    for (i = 1; i <= NUM_WORKERS; ++i) {
	pthread_join(Workers[i], NULL);
    }

    gettimeofday(&t_end, NULL);
    t_diff = 1000000 * (t_end.tv_sec - t_start.tv_sec) + t_end.tv_usec - t_start.tv_usec;
    t_diff /= 1000000;
    printf("\nTime taken: %lg seconds\n\n", t_diff);
    fflush(stdout);







    free(PntTbl);
    free(Z2);
    free(FiltTbl);
    free(X_b);

    return 0;
}





	if (BinTbl[ix][iy] == NULL) {
	    BinTbl[ix][iy] = current;
	} else {
	    CurTbl[ix][iy]->next = current;
	}
	CurTbl[ix][iy] = current++;
	BinU1[ix][iy] += Z_c;
	*current2 = Z_c * Z_c;
	BinU2[ix][iy] += *current2++;
	++BinCnt[ix][iy];
    }

#if DEBUG == 1
    count = 0;
    for (ix = 0; ix < NUM_BINS_X; ++ix) {
	for (iy = 0; iy < NUM_BINS_Y; ++iy) {
	    if ((ix == 134 && iy == 2) ||
		(ix == 570 && iy == 1776) ||
		(ix == 23 && iy == 2699) ||
		(ix == 1046 && iy == 1809) ||
		(ix == 1798 && iy == 2143) ||
		(ix == 1309 && iy == 98) ||
		(ix == 2094 && iy == 579) ||
		(ix == 2890 && iy == 1576) ||
		(ix == 2615 && iy == 2910))
	    {
		printf("Bin [%ld,%ld] has %u points.\n", ix, iy, BinCnt[ix][iy]);
	    }
	    count += BinCnt[ix][iy];
	}
    }
    printf("Count was at first %u\n", NumPointRec);
    printf("Count is now %u\n", count);
    fflush(stdout);
#endif

    for (ix = 0; ix < NUM_BINS_X; ++ix) {
	for (iy = 0; iy < NUM_BINS_Y; ++iy) {
	    u1 = 0;
	    u2 = 0;
	    n = 0;
	    if (BinCnt[ix][iy] == 0) continue;
	    if (ix > 0) {
		if (iy > 0) {
		    u1 += BinU1[ix - 1][iy - 1];
		    u2 += BinU2[ix - 1][iy - 1];
		    n += BinCnt[ix - 1][iy - 1];
		}
		u1 += 2 * BinU1[ix - 1][iy];
		u2 += 2 * BinU2[ix - 1][iy];
		n += 2 * BinCnt[ix - 1][iy];
		if (iy < NUM_BINS_Y - 1) {
		    u1 += BinU1[ix - 1][iy + 1];
		    u2 += BinU2[ix - 1][iy + 1];
		    n += BinCnt[ix - 1][iy + 1];
		}
	    }
	    if (iy > 0) {
		u1 += 2 * BinU1[ix][iy - 1];
		u2 += 2 * BinU2[ix][iy - 1];
		n += 2 * BinCnt[ix][iy - 1];
	    }
	    u1 += 3 * BinU1[ix][iy];
	    u2 += 3 * BinU2[ix][iy];
	    n += 3 * BinCnt[ix][iy];
	    if (iy < NUM_BINS_Y - 1) {
		u1 += 2 * BinU1[ix][iy + 1];
		u2 += 2 * BinU2[ix][iy + 1];
		n += 2 * BinCnt[ix][iy + 1];
	    }
	    if (ix < NUM_BINS_X - 1) {
		if (iy > 0) {
		    u1 += BinU1[ix + 1][iy - 1];
		    u2 += BinU2[ix + 1][iy - 1];
		    n += BinCnt[ix + 1][iy - 1];
		}
		u1 += 2 * BinU1[ix + 1][iy];
		u2 += 2 * BinU2[ix + 1][iy];
		n += 2 * BinCnt[ix + 1][iy];
		if (iy < NUM_BINS_Y - 1) {
		    u1 += BinU1[ix + 1][iy + 1];
		    u2 += BinU2[ix + 1][iy + 1];
		    n += BinCnt[ix + 1][iy + 1];
		}
	    }
	    u1 /= n;
	    u2 /= n;

#if DEBUG == 1
	    if ((ix == 134 && iy == 2) ||
		(ix == 570 && iy == 1776) ||
		(ix == 23 && iy == 2699) ||
		(ix == 1046 && iy == 1809) ||
		(ix == 1798 && iy == 2143) ||
		(ix == 1309 && iy == 98) ||
		(ix == 2094 && iy == 579) ||
		(ix == 2890 && iy == 1576) ||
		(ix == 2615 && iy == 2910))
	    {
		printf("Bin [%ld,%ld] has variance %lg.\n", ix, iy, u2 - (u1 * u1));
		fflush(stdout);
	    }
#endif

	    if (u2 < u1 * u1 + VAR_THRESHOLD) {
		current = BinTbl[ix][iy];
		while (current != NULL) {
		    *(FiltTbl + (current - PntTbl)) = 1;
		    current = current->next;
		}
	    }
	}
    }

    count = 0;
    for (ix = 0; ix < NumPointRec; ++ix) {
	if (*(FiltTbl + ix) == 0) ++count;
    }
    printf("There are %u points remaining after filtering.\n", count);
    fflush(stdout);
