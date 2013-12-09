/*
 * Triangulate.c
 *
 * Author: Vivek B Sardeshmukh   
 *
 * Derived from 'A fast algorithm for constructing Delaunay triangulations in
 * the plane' by S. W. Sloan, Adv. Eng. Software, 1987, Vol. 9, No. 1.
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "Triangulate.h"

LidarPointNode_t ***TriVertex[NUM_CELLS]; 
INT **TriEdge[NUM_CELLS];
INT NumTri[NUM_CELLS]; 
INT *estack[NUM_CELLS];
INT topstk[NUM_CELLS];

INT triLoc(int cell, LidarPointNode_t *point){
    double xp, yp;
    INT t;
    int i, found;
    LidarPointNode_t *v1, *v2;
    xp=point->X_c;
    yp=point->Y_c;
    t=NumTri[cell];
    found=0;
    while(!found){
        for(i=0; i<3; i++){
            v1=TriVertex[cell][t][i];
            v2=TriVertex[cell][t][(i+1)%3];
            if((v1->Y_c - yp)*(v2->X_c - xp) > (v1->X_c - xp)*(v2->Y_c - yp)){
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

void push(int cell, INT e){
    topstk[cell]++;
    if(topstk[cell] > CellCnt[cell]){
        printf("%s:%d:stack full\n",__FILE__, __LINE__);
        return;
    }
    estack[cell][topstk[cell]]=e;
    return;
}

INT pop(int cell){
    if(topstk[cell] != (INT)-1 ){
        topstk[cell]--;
        return estack[cell][topstk[cell]+1];
    }
    else{
        printf("%s:%d:stack empty\n",__FILE__, __LINE__);
        exit(-1);
    }
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
    return 0;
}
/*
 * circumcircle test - triangle v1 v2 v3. point p
 * returns 0 if swap is not required
 * else returns 1
 */
int swap(int cell, LidarPointNode_t *v1, LidarPointNode_t *v2, LidarPointNode_t *v3, LidarPointNode_t *p){
    double x13, y13, x23, y23, x1p, y1p, x2p, y2p, cosa, cosb,sina,sinb;
    x13=v1->X_c - v3->X_c;
    y13=v1->Y_c - v3->Y_c;
    x23=v2->X_c - v3->X_c;
    y23=v2->Y_c - v3->Y_c;
    x1p=v1->X_c - p->X_c;
    y1p=v1->Y_c - p->Y_c;
    x2p=v2->X_c - p->X_c;
    y2p=v2->Y_c - p->Y_c;
    cosa=x13*x23 + y13*y23;
    cosb=x2p*x1p + y1p*y2p;
    if((cosa >= 0) && (cosb >= 0)) 
        return 0;
    else if((cosa < 0) && (cosb < 0))
        return 1;
    else{
        sina=x13*y23-x23*y13;
        sinb=x2p*y1p-x1p*y2p;
        if(sina*cosb+sinb*cosa < 0)
            return 1;
        else
            return 0;
    }
}

/*
 * process a bin 
 * insert points one by one 
 */
void processBin(int cell, INT ix, INT iy){
    LidarPointNode_t *p, *v1, *v2, *v3;
    INT t,a,b,c,l,r;
    int erl,era,erb;
    p=BinTbl[cell][ix][iy];
    if(p==NULL)
        return;
    while(p!=NULL){
        /*locate t in p is*/
        t=triLoc(cell, p);
        /*add 2 new and update 1 triangle*/
        a=TriEdge[cell][t][0];
        b=TriEdge[cell][t][1];
        c=TriEdge[cell][t][2];
        v1=TriVertex[cell][t][0];
        v2=TriVertex[cell][t][1];
        v3=TriVertex[cell][t][2];
        TriVertex[cell][t][0]=p;
        TriVertex[cell][t][1]=v1;
        TriVertex[cell][t][2]=v2;
        TriEdge[cell][t][0]=NumTri[cell]+2;
        TriEdge[cell][t][1]=a;
        TriEdge[cell][t][2]=NumTri[cell]+1;
        NumTri[cell]++;
        TriVertex[cell][NumTri[cell]][0]=p;
        TriVertex[cell][NumTri[cell]][1]=v2;
        TriVertex[cell][NumTri[cell]][2]=v3;
        TriEdge[cell][NumTri[cell]][0]=t;
        TriEdge[cell][NumTri[cell]][1]=b;
        TriEdge[cell][NumTri[cell]][2]=NumTri[cell]+1;
        NumTri[cell]++;
        TriVertex[cell][NumTri[cell]][0]=p;
        TriVertex[cell][NumTri[cell]][1]=v3;
        TriVertex[cell][NumTri[cell]][2]=v1;
        TriEdge[cell][NumTri[cell]][0]=NumTri[cell]-1;
        TriEdge[cell][NumTri[cell]][1]=c;
        TriEdge[cell][NumTri[cell]][2]=t;
        /*update adjacency lists*/
        if(a!=BOUNDARY)
            push(cell, t);
        if(b!=BOUNDARY){
            TriEdge[cell][b][edg(cell,b,t)]=NumTri[cell]-1;
            push(cell, NumTri[cell]-1);
        }
        if(c!=BOUNDARY){
            TriEdge[cell][c][edg(cell,c,t)]=NumTri[cell];
            push(cell, NumTri[cell]);
        }
        while(topstk[cell]!=BOUNDARY) { /*simply saying >=0 */
            l=pop(cell);
            r=TriEdge[cell][l][1];
            /*if(r==BOUNDARY)
                continue;*/
            /*circumcircle test*/
            erl=edg(cell,r,l);
            era=(erl+1)%3;
            erb=(era+1)%3;
            v1=TriVertex[cell][r][erl];
            v2=TriVertex[cell][r][era];
            v3=TriVertex[cell][r][erb];
            if(swap(cell, v1,v2,v3,p)){
                /*p is in circle of triangle r*/
                a=TriEdge[cell][r][era];
                b=TriEdge[cell][r][erb];
                c=TriEdge[cell][l][2];
                TriVertex[cell][l][2]=v3;
                TriEdge[cell][l][1]=a;
                TriEdge[cell][l][2]=r;
                TriVertex[cell][r][0]=p;
                TriVertex[cell][r][1]=v3;
                TriVertex[cell][r][2]=v1;
                TriEdge[cell][r][0]=l;
                TriEdge[cell][r][1]=b;
                TriEdge[cell][r][2]=c;

                if(a!=BOUNDARY){
                    TriEdge[cell][a][edg(cell,a,r)]=l;
                    push(cell,l);
                }
                if(b!=BOUNDARY){
                    push(cell,r);
                }
                if(c!=BOUNDARY){
                    TriEdge[cell][c][edg(cell,c,l)]=r;
                }
            }
        }
        p=p->next;
    }
}


void Delaunay(int cell){
    INT numt,nt;
    INT ix,iy,tstart,tstop;
    int i,j,remove;
    double DMAX; /*pseudo-diagonal distance of a cell*/
    LidarPointNode_t BigTriangle[3];
    double xcen, ycen;

    DMAX=CellMax[cell].X_c - CellMin[cell].X_c;
    DMAX=DMAX > CellMax[cell].Y_c - CellMin[cell].Y_c ? DMAX : CellMax[cell].Y_c - CellMin[cell].Y_c;

    numt=2*CellCnt[cell] + 1;
    topstk[cell]=BOUNDARY;

    /*calculate pseudo-triangle*/
    xcen=0.5*(CellMax[cell].X_c + CellMin[cell].X_c);
    ycen=0.5*(CellMax[cell].Y_c + CellMin[cell].Y_c);
    BigTriangle[0].X_c = xcen-0.866*DMAX;
    BigTriangle[0].Y_c = ycen-0.65*DMAX;
    BigTriangle[1].X_c = xcen+0.866*DMAX;
    BigTriangle[1].Y_c = ycen-0.65*DMAX;
    BigTriangle[2].X_c = xcen;
    BigTriangle[2].Y_c = ycen + 1.2*DMAX;
    TriVertex[cell][0][0] = &BigTriangle[0];
    TriVertex[cell][0][1] = &BigTriangle[1];
    TriVertex[cell][0][2] = &BigTriangle[2];
    TriEdge[cell][0][0] = BOUNDARY; 
    TriEdge[cell][0][1] = BOUNDARY;
    TriEdge[cell][0][2] = BOUNDARY; 
    NumTri[cell]=0; /*triangles are numbered from 0*/ 
    /*insert points one by one*/
    /*for optimization we insert from bins in a specific order - details in the paper*/
    for(iy=0;iy<NUM_BINS_Y;iy++){
        if(iy%2){
            for(ix=0;ix<NUM_BINS_X;ix++)
                processBin(cell, NUM_BINS_X-ix-1,iy);
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
            if(ix!=BOUNDARY){
                TriEdge[cell][ix][edg(cell, ix,nt)]=BOUNDARY;  
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
                if(ix!=BOUNDARY){
                    TriEdge[cell][ix][edg(cell, ix,nt)]=BOUNDARY;  
                }
            }
        }
        else{
            numt = numt+1;
            for(i=0;i<3;i++){
                ix=TriEdge[cell][nt][i];
                TriEdge[cell][numt][i] = ix;
                TriVertex[cell][numt][i] = TriVertex[cell][nt][i];
                if(ix!=BOUNDARY){
                    TriEdge[cell][ix][edg(cell, ix,nt)]=numt;
                }
            }
        }
    }
    /*-----------------------end remove pseudo----------------------------*/
}

