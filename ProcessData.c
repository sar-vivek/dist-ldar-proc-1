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

    VarWeight_t WeightMatrix[37] = {
	{-3, -1, 1},
	{-3, 0, 2},
	{-3, 1, 1},
	{-2, -2, 1},
	{-2, -1, 2},
	{-2, 0, 3},
	{-2, 1, 2},
	{-2, 2, 1},
	{-1, -3, 1},
	{-1, -2, 2},
	{-1, -1, 3},
	{-1, 0, 4},
	{-1, 1, 3},
	{-1, 2, 2},
	{-1, 3, 1},
	{0, -3, 2},
	{0, -2, 3},
	{0, -1, 4},
	{0, 0, 5},
	{0, 1, 4},
	{0, 2, 3},
	{0, 3, 2},
	{1, -3, 1},
	{1, -2, 2},
	{1, -1, 3},
	{1, 0, 4},
	{1, 1, 3},
	{1, 2, 2},
	{1, 3, 1},
	{2, -2, 1},
	{2, -1, 2},
	{2, 0, 3},
	{2, 1, 2},
	{2, 2, 1},
	{3, -1, 1},
	{3, 0, 2},
	{3, 1, 1},
    };
    double t_diff;
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

    c = *((int *) workerID);

#if DEBUG >= 3
    mycount2 = 0;
    for (ix = 0; ix < NUM_BINS_X; ++ix) {
	for (iy = 0; iy < NUM_BINS_Y; ++iy) {
	    fprintf(stderr, "Bin [%u,%u] has %u points.\n", ix, iy, BinCnt[c][ix][iy]);
	    mycount2 += BinCnt[c][ix][iy];
	}
    }
#endif

    for (ix = 3; ix < NUM_BINS_X - 3; ++ix) {
	for (iy = 3; iy < NUM_BINS_Y - 3; ++iy) {
	    i = 1;
	    n = 0;
	    u1 = 0;
	    u2 = 0;
	    vw = WeightMatrix;
	    if (BinCnt[c][ix][iy] == 0) continue;
	    for (i = 0; i < 37; ++i) {
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

	    if (n > 3 && u2 < u1 * u1 + VarThreshold) {
		node = BinTbl[c][ix][iy];
		while (node != NULL) {
		    *(FiltTbl + (node - PntTbl)) = 1;
		    node = node->next;
		}
	    }
	}
    }

    for (ix = 3; ix < NUM_BINS_X - 3; ++ix) {
	for (iy = 3; iy < NUM_BINS_Y - 3; ++iy) {
	    if (BinTbl[c][ix][iy] == NULL) continue;
	    if (FiltTbl[BinTbl[c][ix][iy] - PntTbl] == 1) BinTbl[c][ix][iy] = NULL;
	}
    }

    gettimeofday(&t_filt[c], NULL);
    t_diff = 1000000 * (t_filt[c].tv_sec - t_bin.tv_sec) + t_filt[c].tv_usec - t_bin.tv_usec;
    t_diff /= 1000000;
    printf("Time taken for Node %d, Cell %d Filter Phase: %lf seconds\n", NodeID, c, t_diff);
    fflush(stdout);

    Delaunay(c);

    gettimeofday(&t_tri[c], NULL);
    t_diff = 1000000 * (t_tri[c].tv_sec - t_filt[c].tv_sec) + t_tri[c].tv_usec - t_filt[c].tv_usec;
    t_diff /= 1000000;
    printf("Time taken for Node %d, Cell %d Triangulate Phase: %lf seconds\n", NodeID, c, t_diff);
    fflush(stdout);

    if (c == 0) return NULL;
    pthread_exit(NULL);
}
