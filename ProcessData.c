/*JWHGPLHERE*/
/*
 * ProcessData.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "ProcessData.h"

typedef struct VarWeight {
    int dx;
    int dy;
    int weight;
} VarWeight_t;

void *ProcessData(void *workerID) {

    VarWeight_t WeightMatrix[21] = {
	{-2, -1, 1},
	{-2, 0, 2},
	{-2, 1, 1},
	{-1, -2, 1},
	{-1, -1, 2},
	{-1, 0, 3},
	{-1, 1, 2},
	{-1, 2, 1},
	{0, -2, 2},
	{0, -1, 3},
	{0, 0, 4},
	{0, 1, 3},
	{0, 2, 2},
	{1, -2, 1},
	{1, -1, 2},
	{1, 0, 3},
	{1, 1, 2},
	{1, 2, 1},
	{2, -1, 1},
	{2, 0, 2},
	{2, 1, 1},
    };
    double u1;
    double u2;
    LidarPointNode_t *node;
    VarWeight_t *vw;
#if DEBUG == 1
    uint32_t count2;
#endif
    uint32_t ix;
    uint32_t iy;
    uint32_t n;
    int c;
    int i;
    int dx;
    int dy;
    int weight;

    c = *((int *) workerID);

#if DEBUG == 1
    count2 = 0;
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
		printf("Bin [%ld,%ld] has %u points.\n", ix, iy, BinCnt[c][ix][iy]);
	    }
	    count2 += BinCnt[c][ix][iy];
	}
    }
    printf("Count was at first %u\n", NumPointRec);
    printf("Count is now %u\n", count2);
    fflush(stdout);
#endif

    for (ix = 2; ix < NUM_BINS_X - 2; ++ix) {
	for (iy = 2; iy < NUM_BINS_Y - 2; ++iy) {
	    i = 1;
	    n = 0;
	    u1 = 0;
	    u2 = 0;
	    vw = WeightMatrix;
	    if (BinCnt[c][ix][iy] == 0) continue;
	    for (i = 0; i < 21; ++i) {
		dx = vw->dx;
		dy = vw->dy;
		weight = vw->weight;
		u1 += weight * BinU1[c][ix + dx][iy + dy];
		u2 += weight * BinU2[c][ix + dx][iy + dy];
		n += weight * BinCnt[c][ix + dx][iy + dy];
		++vw;
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
		printf("Bin [%u,%u] has variance %lg.\n", ix, iy, u2 - (u1 * u1));
		fflush(stdout);
	    }
#endif

	    if (u2 < u1 * u1 + VAR_THRESHOLD) {
		node = BinTbl[c][ix][iy];
		while (node != NULL) {
		    *(FiltTbl + (node - PntTbl)) = 1;
		    node = node->next;
		}
	    }
	}
    }

#if DEBUG == 1
    count2 = 0;
    for (ix = 0; ix < NumPointRec; ++ix) {
	if (*(FiltTbl + ix) == 0) ++count2;
    }
    printf("There are %u points remaining after filtering.\n", count2);
    fflush(stdout);
#endif

    if (c == 0) return NULL;
    pthread_exit(NULL);
}
