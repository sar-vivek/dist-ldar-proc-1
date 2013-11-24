/*
 * Author:  Vivek Sardeshmukh   
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


LidarPointNode_t* **TriVertex[NUM_CELLS]; 
LidarPointNode_t* **TriEdge[NUM_CELLS];
INT NumTri[NUM_CELLS]; 



INT triLoc(int cellnum, LidarPointNode_t *point){
    double xp,yp;
    INT t;
    int i,found;
    LidarPointNode_t *v1, *v2;
    t=NumTri[cellnum];
    found=0;
    while(!found){
        for(i=0; i<3; i++){
            v1=TriVertex[cellnum][t][i];
            v2=TriVertex[cellnum][t][(i+1)%3];
            if((v1->Y_c-yp)*(v2->X_c - xp) > (v1->X_c -xp)*(v2->Y_c - yp)){
                t=TriEdge[cellnum][t][i];
                found=0;
                break;
            }
            else{ 
                found=1;
            }
        }    
    }
    return t;
}

INT triangulate(int cellnum){
    double DMAX; /*pseudo-diagonal distance of a cell*/

    DMAX=CellMax[cellnum]->X_c - CellMin[cellnum]->X_c;
    DMAX=DMAX > CellMax[cellnum]->Y_c - CellMin[cellnum]->Y_c ? DMAX : CellMax[cellnum]->Y_c - CellMin[cellnum]->Y_c;

    return Delaunay(cellnum);
}

INT Delaunay(int cellnum){
    INT numt,nt;
    int i,j,k;
    numt=2*CellCnt[NUM_CELLS] + 1;
    for(nt=0;nt<numt;nt++){
        TriVertex[cellnum][nt]=malloc(3*sizeof(LidarPointNode_t*));
        if(TriVertex[cellnum][nt]==NULL){
            perror("TriVertex[cellnum][nt]");
        }
    }
    for(nt=0;nt<numt;nt++){
        TriEdge[cellnum][nt]=malloc(3*sizeof(INT));
        if(TriEdge[cellnum][nt] == NULL){
            perror("TriEdge[cellnum][nt])");
        }
    }



