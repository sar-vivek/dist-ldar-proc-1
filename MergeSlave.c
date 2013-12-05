/*JWHGPLHERE*/
/*
 * MergeSlave.c
 *
 * Author: Vivek B Sardeshmukh 
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "MergeSlave.h"
#include "Triangulate.h"
#include "DistributeMaster.h" 

void MergeSend() {
    int i,c;
    INT t;
    double px,py,pz;
    Send(ssock, &NodeID, sizeof(nodeid));   
    for(c=0;c<NUM_CELLS;c++){
        Send(ssock, &c, sizeof(c));
        for(t=0;t<NumTri[c];t++){
            Send(ssock, &t, sizeof(t));
            for(i=0;i<3;i++){
                px=TriVertex[c][t][i]->X_c;
                py=TriVertex[c][t][i]->Y_c;
                pz=TriVertex[c][t][i]->Z_c;
                Send(ssock, &px, sizeof(px));
                Send(ssock, &py, sizeof(py));
                Send(ssock, &pz, sizeof(pz));
            }
        }
    }
}
