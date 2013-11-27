/*JWHGPLHERE*/
/*
 * ProcessData.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "ProcessData.h"

void *ProcessData(void *workerID) {

    double u1;
    double u2;
    uint32_t count2;
    uint32_t ix;
    uint32_t iy;
    uint32_t n;
    int c;

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

    for (ix = 0; ix < NUM_BINS_X; ++ix) {
	for (iy = 0; iy < NUM_BINS_Y; ++iy) {
	    u1 = 0;
	    u2 = 0;
	    n = 0;
	    if (BinCnt[c][ix][iy] == 0) continue;
	    if (ix > 0) {
		if (ix > 




		if (iy > 0) {
		    u1 += BinU1[c][ix - 1][iy - 1];
		    u2 += BinU2[c][ix - 1][iy - 1];
		    n += BinCnt[c][ix - 1][iy - 1];
		}
		u1 += 2 * BinU1[c][ix - 1][iy];
		u2 += 2 * BinU2[c][ix - 1][iy];
		n += 2 * BinCnt[c][ix - 1][iy];
		if (iy < NUM_BINS_Y - 1) {
		    u1 += BinU1[c][ix - 1][iy + 1];
		    u2 += BinU2[c][ix - 1][iy + 1];
		    n += BinCnt[c][ix - 1][iy + 1];
		}
	    }
	    if (iy > 0) {
		u1 += 2 * BinU1[c][ix][iy - 1];
		u2 += 2 * BinU2[c][ix][iy - 1];
		n += 2 * BinCnt[c][ix][iy - 1];
	    }
	    u1 += 4 * BinU1[c][ix][iy];
	    u2 += 4 * BinU2[c][ix][iy];
	    n += 4 * BinCnt[c][ix][iy];
	    if (iy < NUM_BINS_Y - 1) {
		u1 += 2 * BinU1[c][ix][iy + 1];
		u2 += 2 * BinU2[c][ix][iy + 1];
		n += 2 * BinCnt[c][ix][iy + 1];
	    }
	    if (ix < NUM_BINS_X - 1) {
		if (iy > 0) {
		    u1 += BinU1[c][ix + 1][iy - 1];
		    u2 += BinU2[c][ix + 1][iy - 1];
		    n += BinCnt[c][ix + 1][iy - 1];
		}
		u1 += 2 * BinU1[c][ix + 1][iy];
		u2 += 2 * BinU2[c][ix + 1][iy];
		n += 2 * BinCnt[c][ix + 1][iy];
		if (iy < NUM_BINS_Y - 1) {
		    u1 += BinU1[c][ix + 1][iy + 1];
		    u2 += BinU2[c][ix + 1][iy + 1];
		    n += BinCnt[c][ix + 1][iy + 1];
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
		current = BinTbl[c][ix][iy];
		while (current != NULL) {
		    *(FiltTbl + (current - PntTbl)) = 1;
		    current = current->next;
		}
	    }
	}
    }

    count2 = 0;
    for (ix = 0; ix < NumPointRec; ++ix) {
	if (*(FiltTbl + ix) == 0) ++count2;
    }
    printf("There are %u points remaining after filtering.\n", count2);
    fflush(stdout);

    if (c == 0) return NULL;
    pthread_exit(NULL);
}
