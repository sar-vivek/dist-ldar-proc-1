/*JWHGPLHERE*/
/*
 * ProcessData.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "Triangulate.h"
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
    uint32_t ix;
    uint32_t iy;
    uint32_t n;
    int c;
    int i;
    int dx;
    int dy;
    int weight;

#if DEBUG >= 1
    uint32_t mycount2;
#endif

    c = *((int *) workerID);

#if DEBUG >= 1
    printf("Hello from thread 0x%08X\n", pthread_self());
    fflush(stdout);
#endif

#if DEBUG >= 3
    mycount2 = 0;
    for (ix = 0; ix < NUM_BINS_X; ++ix) {
	for (iy = 0; iy < NUM_BINS_Y; ++iy) {
	    fprintf(stderr, "Bin [%u,%u] has %u points.\n", ix, iy, BinCnt[c][ix][iy]);
	    mycount2 += BinCnt[c][ix][iy];
	}
    }
#endif

#if DEBUG >= 1
    fprintf(stderr, "Node count is %u (not including boundary points).\n", mycount);
    fflush(stderr);
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

	    if (n > 1 && u2 < u1 * u1 + VAR_THRESHOLD) {
		node = BinTbl[c][ix][iy];
		while (node != NULL) {
		    *(FiltTbl + (node - PntTbl)) = 1;
		    node = node->next;
		}
	    }
	}
    }

    for (ix = 2; ix < NUM_BINS_X - 2; ++ix) {
	for (iy = 2; iy < NUM_BINS_Y - 2; ++iy) {
	    if (BinTbl[c][ix][iy] == NULL) continue;
	    if (*(FiltTbl + (BinTbl[c][ix][iy] - PntTbl)) == 1) BinTbl[c][ix][iy] = NULL;
	}
    }

#if DEBUG >= 1
    mycount2 = 0;
    for (ix = 0; ix < mycount; ++ix) {
	if (*(FiltTbl + ix) == 0) ++mycount2;
    }
    printf("There are %u (original) points remaining after filtering.\n", mycount2);
    fflush(stdout);
#endif

    Delaunay(c);

    if (c == 0) return NULL;
    pthread_exit(NULL);
}
