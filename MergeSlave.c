/*JWHGPLHERE*/
/*
 * MergeSlave.c
 *
 * Authors: Vivek B Sardeshmukh, James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "Triangulate.h"
#include "DistributeMaster.h"
#include "MergeSlave.h"

void MergeSend() {

    ssize_t ret;
    uint32_t t;
    int c;
    int i;

    for (c = 0; c < NUM_CELLS; ++c) {
	for (t = 0; t <= NumTri[c]; ++t) {
	    *((uint32_t *) X_b) = t;
	    *((int *) Y_b) = c;
	    Send(ssock, X_b, XYZ_SIZE);
	    *((int32_t *) X_b) = lround((TriVertex[c][t][0]->X_c - Xoffset) / Xscale);
	    *((int32_t *) Y_b) = lround((TriVertex[c][t][0]->Y_c - Yoffset) / Yscale);
	    *((int32_t *) Z_b) = lround((TriVertex[c][t][0]->Z_c - Zoffset) / Zscale);
	    Send(ssock, X_b, XYZ_SIZE);
	    *((int32_t *) X_b) = lround((TriVertex[c][t][1]->X_c - Xoffset) / Xscale);
	    *((int32_t *) Y_b) = lround((TriVertex[c][t][1]->Y_c - Yoffset) / Yscale);
	    *((int32_t *) Z_b) = lround((TriVertex[c][t][1]->Z_c - Zoffset) / Zscale);
	    Send(ssock, X_b, XYZ_SIZE);
	    *((int32_t *) X_b) = lround((TriVertex[c][t][2]->X_c - Xoffset) / Xscale);
	    *((int32_t *) Y_b) = lround((TriVertex[c][t][2]->Y_c - Yoffset) / Yscale);
	    *((int32_t *) Z_b) = lround((TriVertex[c][t][2]->Z_c - Zoffset) / Zscale);
	    Send(ssock, X_b, XYZ_SIZE);
	}
    }

    *((int32_t *) X_b) = 0;
    *((int *) Y_b) = 0;
    Send(ssock, X_b, XYZ_SIZE);
    *((int32_t *) X_b) = 0;
    *((int32_t *) Y_b) = 0;
    *((int32_t *) Z_b) = 0;
    Send(ssock, X_b, XYZ_SIZE);

}
