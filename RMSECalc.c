/*JWHGPLHERE*/
/*
 * RMSECalc.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "DistributeMaster.h"
#include "DistributeSlave.h"
#include "Triangulate.h"
#include "RMSECalc.h"

double RMSEFindZ(LidarPointNode_t *A, LidarPointNode_t *B, LidarPointNode_t *C, LidarPointNode_t *X) {

    double b1;
    double b2;
    double b3;
    double c1;
    double c2;
    double c3;
    double x1;
    double x2;
    double x3;

    b1 = B->X_c - A->X_c;
    b2 = B->Y_c - A->Y_c;
    b3 = B->Z_c - A->Z_c;
    c1 = C->X_c - A->X_c;
    c2 = C->Y_c - A->Y_c;
    c3 = C->Z_c - A->Z_c;
    x1 = X->X_c - A->X_c;
    x2 = X->Y_c - A->Y_c;

    /* Solve for dx3 in the determinant equation: */
    /* dx1*(db2*dc3-db3*dc2) - dx2*(db1*dc3-db3*dc1) + dx3*(db1*dc2-db2*dc1) = 0 */
    if (b1 * c2 == b2 * c1) perror("det = 0");

    x3 = (x2 * (b1 * c3 - b3 * c1) - x1 * (b2 * c3 - b3 * c2)) / (b1 * c2 - b2 * c1);

    return (x3 + A->Z_c);
}

double ComputeLocalMSE() {

    double mse;
    double Z_inter;
    double diff;
    time_t t_r;
    uint32_t ix;
    uint32_t iy;
    INT t;
    int bflag;
    int c;
    int dflag;
    int r;
    int ri;
    int rmax;
    int vcnt;

    time(&t_r);
    srand((unsigned int) t_r);

    mse = 0;
    vcnt = 0;

    if (mycount == 0) return mse;

    assert(mycount <= (uint32_t) INT_MAX);
    assert(mycount < (uint32_t) RAND_MAX);

    ri = (int) ((RAND_MAX - 1) / mycount);
    rmax = ri * mycount;

    while (vcnt < VERIFY_POINTS_PER_NODE) {
	r = rand();
	if (r < rmax) {
	    ri = r % mycount;
	    X_c = PntTbl[ri].X_c;
	    Y_c = PntTbl[ri].Y_c;
	    Z_c = PntTbl[ri].Z_c;

	    if (FiltTbl[ri] == 1) {
		ix = lround(floor((X_c - NodeMin.X_c) / Xint_cell));
		iy = lround(floor((Y_c - NodeMin.Y_c) / Yint_cell));
		if (ix == NUM_CELLS_X) --ix;
		if (iy == NUM_CELLS_Y) --iy;
		c = NUM_CELLS_X * iy + ix;
		bflag = -1;
		dflag = -1;
		t = triLoc(c, PntTbl + ri, &bflag, &dflag);

#if DEBUG >= 1
		if (dflag > -1) {
		    fprintf(stderr, "\nThat's interesting. Point (%lf,%lf,%lf) was filtered out ",
			    X_c, Y_c, Z_c);
		    fprintf(stderr, "but is within the error tolerance of point (%lf,%lf,%lf).\n\n",
			    TriVertex[c][t][dflag]->X_c, TriVertex[c][t][dflag]->Y_c,
			    TriVertex[c][t][dflag]->Z_c);
		    fflush(stderr);
		}
#endif

		Z_inter = RMSEFindZ(TriVertex[c][t][0], TriVertex[c][t][1], TriVertex[c][t][2], PntTbl + ri);
		diff = Z_c - Z_inter;
		mse += diff * diff;

#if DEBUG >= 2
		printf("Point = (%lf,%lf,%lf) : Triangle = (%lf,%lf,%lf) , (%lf,%lf,%lf) , (%lf,%lf,%lf)\n",
		       X_c, Y_c, Z_c, TriVertex[c][t][0]->X_c, TriVertex[c][t][0]->Y_c, TriVertex[c][t][0]->Z_c,
		       TriVertex[c][t][1]->X_c, TriVertex[c][t][1]->Y_c, TriVertex[c][t][1]->Z_c,
		       TriVertex[c][t][2]->X_c, TriVertex[c][t][2]->Y_c, TriVertex[c][t][2]->Z_c);
		printf("Z_inter = %lf, diff = %lf, mse (sum) = %lf\n\n", Z_inter, diff, mse);
#endif
	    }

	    ++vcnt;
	}
    }

    mse /= VERIFY_POINTS_PER_NODE;

#if DEBUG >= 1
    printf("\nmse = %lf\n\n", mse);
#endif

    return mse;
}

void RMSECalcMaster() {

    double mse[NUM_NODES];
    double rmse;
    uint32_t sync;
    uint32_t testcount;
    uint32_t testcount2;
    int i;

    for (i = 1; i < NUM_NODES; ++i) {
	Receive(msock[i], &sync, UINT32_SIZE);
	fprintf(stderr, "Received sync number %u from Node %d\n", sync, i);
    }
    fflush(stderr);

    printf("Node %d kept %u original points and had %u left after filtering\n", NodeID, mycount, mycount2);
    for (i = 1; i < NUM_NODES; ++i) {
	Receive(msock[i], &testcount, UINT32_SIZE);
	Receive(msock[i], &testcount2, UINT32_SIZE);
	printf("Node %d got %u original points and had %u left after filtering\n", i, testcount, testcount2);
    }
    printf("In total, for all nodes, %d boundary points were added\n", (NUM_NODES * NUM_CELLS * (NUM_BINS_X + NUM_BINS_Y)));
    fflush(stdout);

    mse[0] = ComputeLocalMSE();

    for (i = 1; i < NUM_NODES; ++i) {
	Receive(msock[i], &mse[i], DOUBLE_SIZE);
    }

    for (i = 1; i < NUM_NODES; ++i) {
	Receive(msock[i], X_b, XYZ_SIZE);
	if (*((int32_t *) X_b) != i || *((int32_t *) Y_b) != i || *((int32_t *) Z_b) != i) {
	    fprintf(stderr, "Error terminating TCP connection %d.\n", i);
	    fflush(stderr);
	}
    }

    for (i = 1; i < NUM_NODES; ++i) {
	if (close(msock[i]) == -1) perror("close()");
    }

    for (i = 1; i < NUM_NODES; ++i) {    
	mse[0] += mse[i];
    }

    mse[0] /= NUM_NODES;
    rmse = sqrt(mse[0]);

    printf("\nRMSE = %lf\n\n", rmse);
    fflush(stdout);
}

void RMSECalcSlave() {

    double mse;
    ssize_t ret;
    uint32_t sync;

    sync = NodeID * NodeID * NodeID;
    Send(ssock, &sync, UINT32_SIZE);
    Send(ssock, &mycount, UINT32_SIZE);
    Send(ssock, &mycount2, UINT32_SIZE);

    mse = ComputeLocalMSE();

    Send(ssock, &mse, DOUBLE_SIZE);

    *((int32_t *) X_b) = (int32_t) NodeID;
    *((int32_t *) Y_b) = (int32_t) NodeID;
    *((int32_t *) Z_b) = (int32_t) NodeID;
    Send(ssock, X_b, XYZ_SIZE);

    while ((ret = recv(ssock, X_b, 1, 0)) != 0) {
	if (ret == 1) {
	    fprintf(stderr, "Received byte %X after socket was supposed to close.\n", (int) *((char *) X_b));
	    fflush(stderr);
	} else perror("recv()");
    }

    if (close(ssock) == -1) perror("close()");
}
