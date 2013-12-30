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

    INT sendTri;
    INT totalTri; 
    uint32_t t;
    int c;
    void *tb, *xb, *yb, *zb; 

    totalTri = 0;
    for(c = 0; c < NUM_CELLS; ++c){
	totalTri = totalTri + NumTri[c];
    }
    *((int *) NetworkBuffers[0]) = totalTri;
    Send(ssock, NetworkBuffers[0], INT32_SIZE);

    sendTri = 0;
    for (c = 0; c < NUM_CELLS; ++c) {
	for (t = 0; t <= NumTri[c]; ++t) {

	    cb = NetworkBuffers[0] + sendTri*TRI_SIZE; 
	    *((int *) NetworkBuffers[0]) = c;
	    tb = NetworkBuffers[0] + INT32_SIZE;  
	    *((uint32_t *) tb) = t;
	    
	    xb = tb + INT32_SIZE; 
	    *((int32_t *) xb) = lround((TriVertex[c][t][0]->X_c - Xoffset) / Xscale);
	    yb = xb + INT32_SIZE;
	    *((int32_t *) yb) = lround((TriVertex[c][t][0]->Y_c - Yoffset) / Yscale);
	    zb = yb + INT32_SIZE;
	    *((int32_t *) zb) = lround((TriVertex[c][t][0]->Z_c - Zoffset) / Zscale);

	    xb = zb + INT32_SIZE; 
	    *((int32_t *) xb) = lround((TriVertex[c][t][1]->X_c - Xoffset) / Xscale);
	    yb = xb + INT32_SIZE;
	    *((int32_t *) yb) = lround((TriVertex[c][t][1]->Y_c - Yoffset) / Yscale);
	    zb = yb + INT32_SIZE;
	    *((int32_t *) zb) = lround((TriVertex[c][t][1]->Z_c - Zoffset) / Zscale);

	    xb = zb + INT32_SIZE; 
	    *((int32_t *) xb) = lround((TriVertex[c][t][2]->X_c - Xoffset) / Xscale);
	    yb = xb + INT32_SIZE;
	    *((int32_t *) yb) = lround((TriVertex[c][t][2]->Y_c - Yoffset) / Yscale);
	    zb = yb + INT32_SIZE;
	    *((int32_t *) zb) = lround((TriVertex[c][t][2]->Z_c - Zoffset) / Zscale);
	    
	    ++sendTri;
	    if( sendTri == TRI_PER_PACKET ){
		Send(ssock, NetworkBuffers[0], TRI_PACKET_LEN);
		sendTri = 0;
	    }
	}
    }
}
