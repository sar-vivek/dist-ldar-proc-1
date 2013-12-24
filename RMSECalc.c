/*JWHGPLHERE*/
/*
 * RMSECalc.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "RMSECalc.h"

void RMSECalcMaster() {

}

void RMSECalcSlave() {

}

double FindZ(LidarPointNode_t *A, LidarPointNode_t *B, LidarPointNode_t *C, LidarPointNode_t *X) {

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
    if (b1 * c2 == b2 * c1) {
	perror("det = 0");
    }
    x3 = (x2 * (b1 * c3 - b3 * c1) - x1 * (b2 * c3 - b3 * c2)) / (b1 * c2 - b2 * c1);

    return x3;
}
