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
    INT pktCount;
    uint32_t t;
    int c;
    int32_t *xb; 

    totalTri = 0;
    for(c = 0; c < NUM_CELLS; ++c){
	totalTri = totalTri + NumTri[c] + 1;
    }
    Send(ssock, &totalTri, UINT32_SIZE);

    pktCount = 0;
    sendTri = 0;
    xb = NetworkBuffers[0];
    for (c = 0; c < NUM_CELLS; ++c) {
	for (t = 0; t <= NumTri[c]; ++t) {

	    *xb = lround((TriVertex[c][t][0]->X_c - Xoffset) / Xscale);
	    xb = xb + 1;
	    *xb = lround((TriVertex[c][t][0]->Y_c - Yoffset) / Yscale);
	    xb = xb + 1;
	    *xb = lround((TriVertex[c][t][0]->Z_c - Zoffset) / Zscale);
	    xb = xb + 1; 

	    *xb = lround((TriVertex[c][t][1]->X_c - Xoffset) / Xscale);
	    xb = xb + 1;
	    *xb = lround((TriVertex[c][t][1]->Y_c - Yoffset) / Yscale);
	    xb = xb + 1;
	    *xb = lround((TriVertex[c][t][1]->Z_c - Zoffset) / Zscale);
	    xb = xb + 1; 

	    *xb = lround((TriVertex[c][t][2]->X_c - Xoffset) / Xscale);
	    xb = xb + 1;
	    *xb = lround((TriVertex[c][t][2]->Y_c - Yoffset) / Yscale);
	    xb = xb + 1;
	    *xb = lround((TriVertex[c][t][2]->Z_c - Zoffset) / Zscale);
	    xb = xb + 1; 

	    ++sendTri;
	    if( sendTri == TRI_PER_PACKET ){
		Send(ssock, NetworkBuffers[0], TRI_PACKET_LEN);
		sendTri = 0;
		xb = NetworkBuffers[0];
		++pktCount;
	    }
	}
    }

    if(sendTri != 0)
        Send(ssock, NetworkBuffers[0], TRI_PACKET_LEN);

#if DEBUG >=1 
    if(pktCount * TRI_PER_PACKET < totalTri){
	fprintf(stderr, "sent= %u sendTri = %u\n", totalTri - pktCount * TRI_PER_PACKET, sendTri);
    }
    fprintf(stderr, "Number of triangles to send : %u\n", totalTri);
    fprintf(stderr, "Number of packets sent : %u\n", pktCount);
    fflush(stderr);
#endif
}
