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



INT triLoc(int cell, LidarPointNode_t *point){
    double xp,yp;
    INT t;
    int i,found;
    LidarPointNode_t *v1, *v2;
    t=NumTri[cell];
    found=0;
    while(!found){
        for(i=0; i<3; i++){
            v1=TriVertex[cell][t][i];
            v2=TriVertex[cell][t][(i+1)%3];
            if((v1->Y_c-yp)*(v2->X_c - xp) > (v1->X_c -xp)*(v2->Y_c - yp)){
                t=TriEdge[cell][t][i];
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

INT triangulate(int cell){
    double DMAX; /*pseudo-diagonal distance of a cell*/

    DMAX=CellMax[cell]->X_c - CellMin[cell]->X_c;
    DMAX=DMAX > CellMax[cell]->Y_c - CellMin[cell]->Y_c ? DMAX : CellMax[cell]->Y_c - CellMin[cell]->Y_c;

    return Delaunay(cell);
}

INT Delaunay(int cell){
    INT numt,nt;
    INT ix,iy;
    double DMAX; /*pseudo-diagonal distance of a cell*/
    LidarPointNode_t BigTriangle[3];
    double xcen, ycen;

    DMAX=CellMax[cell]->X_c - CellMin[cell]->X_c;
    DMAX=DMAX > CellMax[cell]->Y_c - CellMin[cell]->Y_c ? DMAX : CellMax[cell]->Y_c - CellMin[cell]->Y_c;

    numt=2*CellCnt[cell] + 1;
    for(nt=0;nt<numt;nt++){
        TriVertex[cell][nt]=malloc(3*sizeof(LidarPointNode_t*));
        if(TriVertex[cell][nt]==NULL){
            perror("TriVertex[cell][nt]");
        }
    }
    for(nt=0;nt<numt;nt++){
        TriEdge[cell][nt]=malloc(3*sizeof(INT));
        if(TriEdge[cell][nt] == NULL){
            perror("TriEdge[cell][nt])");
        }
    }


    /*calculate pseudo-triangle*/
    xcen=0.5*(CellMax[cell]->X_c - CellMin[cell]->X_c);
    ycen=0.5*(CellMax[cell]->Y_c - CellMin[cell]->Y_c);
    BigTriangle[0].X_c = xcen-0.866*DMAX;
    BigTriangle[0].Y_c = ycen-0.5*DMAX;
    BigTriangle[1].X_c = xcen+0.866*DMAX;
    BigTriangle[1].Y_c = ycen-0.5*DMAX;
    BigTriangle[2].X_c = xcen;
    BigTriangle[2].Y_c = ycen + DMAX;
    TriVertex[cell][0][0] = &(BigTriangle[0]);
    TriVertex[cell][0][1] = &(BigTriangle[1]);
    TriVertex[cell][0][2] = &(BigTriangle[2]);
    TriEdge[cell][0][0] = BOUNDRY; 
    TriEdge[cell][0][1] = BOUNDRY;
    TriEdge[cell][0][2] = BOUNDRY; 

    /*insert points one by one*/
    /*for optimization we insert from bins in a specific order - details in the paper*/
    for(iy=0;iy<NUM_BINS_Y;iy++){
        if(iy%2){
            for(ix=NUM_BINS_X-1;ix>=0;ix--)
                processBin(cell, ix,iy);
        }
        else{
            for(ix=0;ix<NUM_BINS_X;ix++)
                processBin(cell, ix,iy);
        }
    }

    /*------------------remove pseudos-----------------------*/
    /*remove all triangles containing pseudo-triangle points*/
    /*first find triangle that is to be removed*/
    nt=0;
    remove=0;
    while(!remove && nt<numt){
        i=0;
        while(!remove && i < 3){
            for(j=0;j<3;j++){
                if(TriVertex[cell][nt][i] == &BigTriangle[j]){
                    remove=1;
                    break;
                }   
            }
            i++;
        }
        nt++;
    }
    /*nt is the first triangle that to be removed*/
    if(remove){
        for(i=0;i<3;i++){
            ix=TriEdge[cell][nt][i];
            if(ix!=BOUNDRY){
                TriEdge[cell][ix][edg(cell, ix,nt)]=BOUNDRY;  
            }
        }
    }
    /*starting from nt now remove rest triangles*/
    tstart=nt+1;
    tstop=numt;
    numt=nt-1;
    for(nt=tstart; nt<tstop; nt++){
        i=0;
        while(!remove && i < 3){
            for(j=0;j<3;j++){
                if(TriVertex[cell][nt][i] == &BigTriangle[j]){
                    remove=1;
                    break;
                }   
            }
            i++;
        }
        if(remove){
            for(i=0;i<3;i++){
                ix=TriEdge[cell][nt][i];
                if(ix!=BOUNDRY){
                    TriEdge[cell][ix][edg(cell, ix,nt)]=BOUNDRY;  
                }
            }
        }
        else{
            numt = numt+1;
            for(i=0;i<3;i++){
                ix=TriEdge[cell][nt][i];
                TriEdge[cell][numt][i] = ix;
                TriVertex[cell][numt][i] = TriVertex[cell][nt][i];
                if(ix!=BOUNDRY){
                    TriEdge[cell][ix][edg(cell, ix,nt)]=numt;
                }
            }
        }
    }
    /*-----------------------end remove pseudo----------------------------*/
}

/* ix and nt are triangle numbers 
 * and they are adjacent
 * return the edge number between 0 to 2 which is shared between these two triangles
 */
int edg(int cell, INT ix, INT nt){
    int i;
    for(i=0;i<3;i++){
        if(TriEdge[cell][ix][i]==nt)
            return i;
    }
#if DEBUG
    printf("%s:%d:triangles are not adjacent\n",__FILE__, __LINE__);
#endif
}

/*
 * process a bin 
 * insert points one by one 
 */
RETURNTYPE processBin(int cell, INT x, INT y){






