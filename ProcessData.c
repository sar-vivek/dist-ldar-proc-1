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

    int c;

    c = *((int *) workerID);

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

    if (c == 0) return NULL;
    pthread_exit(NULL);
}
