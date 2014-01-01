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
INT *estack[NUM_CELLS];
INT NumTri[NUM_CELLS];
INT topstk[NUM_CELLS];
LidarPointNode_t BigTriangle[NUM_CELLS][4];
#if DEBUG >=2
INT pcount[NUM_CELLS];
#endif

INT triLoc(int cell, LidarPointNode_t *point, int *bfp, int *dfp) {
    double px;
    double py;
    double d1x;
    double d1y;
    double d2x;
    double d2y;
    double v1x;
    double v1y;
    double v2x;
    double v2y;
    double det;
    LidarPointNode_t *v1;
    LidarPointNode_t *v2;
    INT t;
    int found;
    int i;

    px = point->X_c;
    py = point->Y_c;
    t = NumTri[cell];
    found = 0;

    while (!found) {
        for (i = 0; i < 3; ++i) {
            v1 = TriVertex[cell][t][i];
            v2 = TriVertex[cell][t][(i + 1) % 3];
            v1x = v1->X_c;
            v1y = v1->Y_c;
            v2x = v2->X_c;
            v2y = v2->Y_c;
	    d1x = px - v1x;
	    d1y = py - v1y;
	    d2x = px - v2x;
	    d2y = py - v2y;
            det = d1x * (v2y - v1y) - (v2x - v1x) * d1y;
#if DEBUG >= 3 
	    fprintf(stderr, "Looking at triangle %u. i = %d, det = %lf\n", t, i, det);
	    fprintf(stderr, "p = (%lf,%lf), v1 = (%lf,%lf), v2 = (%lf,%lf)\n", px, py, v1x, v1y, v2x, v2y);
	    fflush(stderr);
#endif

            if (det == 0 || (det < DZERO && det > -DZERO)) {
                if (d1x < DZERO && d1x > -DZERO && d1y < DZERO && d1y > -DZERO) {
                    *bfp = i;
                    *dfp = i;

#if DEBUG >= 2
                    fprintf(stderr, "Duplicate points at (%lf,%lf) (%lf,%lf) \n", px, py, v1x, v1y);
                    fflush(stderr);
#endif

                    return t;
                }
                if (d2x < DZERO && d2x > -DZERO && d2y < DZERO && d2y > -DZERO) {
                    *bfp = (i + 1) % 3;
                    *dfp = *bfp;

#if DEBUG >= 2
                    fprintf(stderr, "Duplicate points at (%lf,%lf) (%lf,%lf) \n", px, py, v2x, v2y);
                    fflush(stderr);
#endif

                    return t;
                }
                if ((v1x < px && px < v2x) || (v1x > px && px > v2x)) {
                    *bfp = i;
                    return t;
                }
                if (v1x == v2x) {
                    if ((v1y < py && py < v2y) || (v1y > py && py > v2y)) {
                        *bfp = i;
                        return t;
                    }
                }

#if DEBUG >= 2
		fprintf(stderr, "Not returning.\n");
		fflush(stderr);
#endif

            } else if (det > 0) {

#if DEBUG >= 3 
                fprintf(stderr, "det > 0; t = %u\n", t);
                fflush(stderr);
#endif

                t = TriEdge[cell][t][i];
                found = 0;

#if DEBUG >= 3
		fprintf(stderr, "Breaking now. t = %u\n", t);
		fflush(stderr);
#endif

                break;
            } else found = 1;
        }
    }

    return t;
}

void push(int cell, INT e) {
    ++topstk[cell];

    if (topstk[cell] > CellCnt[cell]) {
#if DEBUG >= 1
        fprintf(stderr, "%s:%d:stack full\n", __FILE__, __LINE__);
        fflush(stderr);
#endif
        return;
    }

    estack[cell][topstk[cell]] = e;
}

INT pop(int cell) {

    if (topstk[cell] != BOUNDARY) {
        --topstk[cell];
        return estack[cell][topstk[cell] + 1];
    }
#if DEBUG >= 1
    fprintf(stderr, "%s:%d:stack empty\n", __FILE__, __LINE__);
    fflush(stderr);
#endif
    exit(-1);
}

/* ix and nt are triangle numbers 
 * and they are adjacent
 */
int edg(int cell, INT ix, INT nt) {
    int i;

    for (i = 0; i < 3; ++i) {
        if (TriEdge[cell][ix][i] == nt) return i;
    }

#if DEBUG >= 1
    fprintf(stderr, "%s:%d:triangles are not adjacent\n", __FILE__, __LINE__);
    fflush(stderr);
#endif

    return 0;
}

/*
 * circumcircle test - triangle v1 v2 v3. point p
 * returns 0 if swap is not required
 * else returns 1
 */
int swap(int cell, LidarPointNode_t *v1, LidarPointNode_t *v2, LidarPointNode_t *v3, LidarPointNode_t *p) {
    double x13;
    double y13;
    double x23;
    double y23;
    double x1p;
    double y1p;
    double x2p;
    double y2p;
    double cosa;
    double cosb;
    double sina;
    double sinb;

    x13 = v1->X_c - v3->X_c;
    y13 = v1->Y_c - v3->Y_c;
    x23 = v2->X_c - v3->X_c;
    y23 = v2->Y_c - v3->Y_c;
    x1p = v1->X_c - p->X_c;
    y1p = v1->Y_c - p->Y_c;
    x2p = v2->X_c - p->X_c;
    y2p = v2->Y_c - p->Y_c;
    cosa = x13 * x23 + y13 * y23;
    cosb = x2p * x1p + y1p * y2p;

    if ((cosa >= 0) && (cosb >= 0)) return 0;
    else if ((cosa < 0) && (cosb < 0)) return 1;
    else {
        sina = x13 * y23 - x23 * y13;
        sinb = x2p * y1p - x1p * y2p;

        if (sina * cosb + sinb * cosa < 0) return 1;
        else return 0;
    }
}

/*
 * process a bin 
 * insert points one by one 
 */
void processBin(int cell, INT ix, INT iy) {
    LidarPointNode_t *p;
    LidarPointNode_t *v1;
    LidarPointNode_t *v2;
    LidarPointNode_t *v3;
    INT t;
    INT t2;
    INT a;
    INT b;
    INT c;
    INT l;
    INT r;
    int bflag;
    int dflag;
    int erl;
    int era;
    int erb;
    int i1;
    int i2;

    p = BinTbl[cell][ix][iy];

    while (p != NULL) {
#if DEBUG >=2 
	++pcount[cell];
	if (pcount[cell] == 5000000) {
	    pcount[cell] = 0;
	    fprintf(stderr, "Cell %d : Beep.. beep.. I'm alive\n", cell);
	    fflush(stderr);
	}
#endif
	/*locate index of triangle containing p*/
	bflag = -1;
	dflag = -1;
	t = triLoc(cell, p, &bflag, &dflag);

	if (dflag > -1) {
	    p = p->next;
	    continue;
	}

	if (bflag == -1) {
	    /*add 2 new and update 1 triangle*/
	    a = TriEdge[cell][t][0];
	    b = TriEdge[cell][t][1];
	    c = TriEdge[cell][t][2];
	    v1 = TriVertex[cell][t][0];
	    v2 = TriVertex[cell][t][1];
	    v3 = TriVertex[cell][t][2];
	    TriVertex[cell][t][0] = p;
	    TriVertex[cell][t][1] = v1;
	    TriVertex[cell][t][2] = v2;
	    TriEdge[cell][t][0] = NumTri[cell] + 2;
	    TriEdge[cell][t][1] = a;
	    TriEdge[cell][t][2] = NumTri[cell] + 1;

	    t2 = ++NumTri[cell];
	    TriVertex[cell][t2][0] = p;
	    TriVertex[cell][t2][1] = v2;
	    TriVertex[cell][t2][2] = v3;
	    TriEdge[cell][t2][0] = t;
	    TriEdge[cell][t2][1] = b;
	    TriEdge[cell][t2][2] = NumTri[cell] + 1;

	    t2 = ++NumTri[cell];
	    TriVertex[cell][t2][0] = p;
	    TriVertex[cell][t2][1] = v3;
	    TriVertex[cell][t2][2] = v1;
	    TriEdge[cell][t2][0] = NumTri[cell] - 1;
	    TriEdge[cell][t2][1] = c;
	    TriEdge[cell][t2][2] = t;

	    /*update adjacency lists*/
	    if (a != BOUNDARY) push(cell, t);

	    if (b != BOUNDARY) {
		TriEdge[cell][b][edg(cell, b, t)] = NumTri[cell] - 1;
		push(cell, NumTri[cell] - 1);
	    }

	    if (c != BOUNDARY) {
		TriEdge[cell][c][edg(cell, c, t)] = NumTri[cell];
		push(cell, NumTri[cell]);
	    }

	} else {
	    /* new point was on an edge - add 2 new and update 2 triangles */
	    i1 = (bflag + 1) % 3;
	    i2 = (bflag + 2) % 3;
	    t2 = ++NumTri[cell];


	    a = TriEdge[cell][t][bflag];
	    b = TriEdge[cell][t][i1];
	    c = TriEdge[cell][t][i2];
	    v1 = TriVertex[cell][t][bflag];
	    v2 = TriVertex[cell][t][i1];
	    v3 = TriVertex[cell][t][i2];
	    TriVertex[cell][t][0] = p;
	    TriVertex[cell][t][1] = v3;
	    TriVertex[cell][t][2] = v1;
	    TriEdge[cell][t][0] = t2;
	    TriEdge[cell][t][1] = c;
	    TriEdge[cell][t][2] = a;

	    TriVertex[cell][t2][0] = p;
	    TriVertex[cell][t2][1] = v2;
	    TriVertex[cell][t2][2] = v3;
	    TriEdge[cell][t2][1] = b;
	    TriEdge[cell][t2][2] = t;

	    if (b != BOUNDARY) {
		TriEdge[cell][b][edg(cell, b, t)] = t2;
		push(cell, t2);
	    }

	    if (c != BOUNDARY) push(cell, t);

	    if (a == BOUNDARY) {
		TriEdge[cell][t2][0] = BOUNDARY;
	    } else {
		TriEdge[cell][t2][0] = t2 + 1;

		bflag = edg(cell, a, t);
		i1 = (bflag + 1) % 3;
		i2 = (bflag + 2) % 3;
		t2 = ++NumTri[cell];

		b = TriEdge[cell][a][i1];
		c = TriEdge[cell][a][i2];
		v1 = TriVertex[cell][a][bflag];
		v2 = TriVertex[cell][a][i1];
		v3 = TriVertex[cell][a][i2];
		TriVertex[cell][a][0] = p;
		TriVertex[cell][a][1] = v2;
		TriVertex[cell][a][2] = v3;
		TriEdge[cell][a][0] = t;
		TriEdge[cell][a][1] = b;
		TriEdge[cell][a][2] = t2;

		TriVertex[cell][t2][0] = p;
		TriVertex[cell][t2][1] = v3;
		TriVertex[cell][t2][2] = v1;
		TriEdge[cell][t2][0] = a;
		TriEdge[cell][t2][1] = c;
		TriEdge[cell][t2][2] = t2 - 1;

		if (b != BOUNDARY) push(cell, a);

		if (c != BOUNDARY) {
		    TriEdge[cell][c][edg(cell, c, a)] = t2;
		    push(cell, t2);
		}
	    }
	}

	while (topstk[cell] != BOUNDARY) {   /*simply saying >=0 */
	    l = pop(cell);
	    r = TriEdge[cell][l][1];
	    /*if (r == BOUNDARY) continue;*/

	    /*circumcircle test*/
	    erl = edg(cell, r, l);
	    era = (erl + 1) % 3;
	    erb = (era + 1) % 3;
	    v1 = TriVertex[cell][r][erl];
	    v2 = TriVertex[cell][r][era];
	    v3 = TriVertex[cell][r][erb];

	    if (swap(cell, v1, v2, v3, p)) {
		/*p is in circle of triangle r*/
		a = TriEdge[cell][r][era];
		b = TriEdge[cell][r][erb];
		c = TriEdge[cell][l][2];
		TriVertex[cell][l][2] = v3;
		TriEdge[cell][l][1] = a;
		TriEdge[cell][l][2] = r;
		TriVertex[cell][r][0] = p;
		TriVertex[cell][r][1] = v3;
		TriVertex[cell][r][2] = v1;
		TriEdge[cell][r][0] = l;
		TriEdge[cell][r][1] = b;
		TriEdge[cell][r][2] = c;

		if (a != BOUNDARY) {
		    TriEdge[cell][a][edg(cell, a, r)] = l;
		    push(cell, l);
		}
		if (b != BOUNDARY) push(cell, r);
		if (c != BOUNDARY) TriEdge[cell][c][edg(cell, c, l)] = r;
	    }
	}

	p = p->next;
    }
}

void Delaunay(int cell) {
    INT ix;
    INT iy;
#if DEBUG >=3
    INT dbugi;
    int dbugj;
#endif
#if DEBUG >=2
    int i;
#endif

#if DEBUG >=2
    pcount[cell] = 0;
#endif
    topstk[cell] = BOUNDARY;

    BigTriangle[cell][0].X_c = CellMin[cell].X_c; 
    BigTriangle[cell][0].Y_c = CellMin[cell].Y_c; 
    BigTriangle[cell][0].Z_c = Zinit[cell][0];
    BigTriangle[cell][1].X_c = CellMax[cell].X_c; 
    BigTriangle[cell][1].Y_c = CellMin[cell].Y_c; 
    BigTriangle[cell][1].Z_c = Zinit[cell][1];
    BigTriangle[cell][2].X_c = CellMin[cell].X_c; 
    BigTriangle[cell][2].Y_c = CellMax[cell].Y_c; 
    BigTriangle[cell][2].Z_c = Zinit[cell][2];
    BigTriangle[cell][3].X_c = CellMax[cell].X_c; 
    BigTriangle[cell][3].Y_c = CellMax[cell].Y_c; 
    BigTriangle[cell][3].Z_c = Zinit[cell][3];
#if DEBUG >= 2
    fprintf(stderr, "------------BigSquare------------------- \n"); 
    for (i = 0; i < 4; ++i) {
	fprintf(stderr, "%lf %lf %lf\n", BigTriangle[cell][i].X_c,
		BigTriangle[cell][i].Y_c, BigTriangle[cell][i].Z_c);
    }
    fprintf(stderr, "-----------------------------------------\n");
    fflush(stderr);
#endif
    TriVertex[cell][0][0] = &BigTriangle[cell][0];
    TriVertex[cell][0][1] = &BigTriangle[cell][1];
    TriVertex[cell][0][2] = &BigTriangle[cell][2];
    TriVertex[cell][1][0] = &BigTriangle[cell][2];
    TriVertex[cell][1][1] = &BigTriangle[cell][1];
    TriVertex[cell][1][2] = &BigTriangle[cell][3];
    TriEdge[cell][0][0] = BOUNDARY; 
    TriEdge[cell][0][1] = 1;
    TriEdge[cell][0][2] = BOUNDARY; 
    TriEdge[cell][1][0] = 0; 
    TriEdge[cell][1][1] = BOUNDARY;
    TriEdge[cell][1][2] = BOUNDARY; 
    /*triangles are numbered from 0*/
    NumTri[cell] = 1 ; 

    /*insert points one by one*/
    /*for optimization we insert from bins in a specific order - details in the paper*/
    for (iy = 0; iy < NUM_BINS_Y; ++iy) {
	if (iy % 2 == 1) {
	    for (ix = 0; ix < NUM_BINS_X; ++ix) {
		processBin(cell, NUM_BINS_X - ix - 1, iy);
	    }
	} else {
	    for (ix = 0; ix < NUM_BINS_X; ++ix) {
		processBin(cell, ix, iy);
	    }
	}
    }

#if DEBUG >= 1
    fprintf(stdout,"Success %u %u\n", NumTri[cell], (2 * CellCnt[cell] + 1));
    fflush(stdout);
#endif

#if DEBUG >=3
    fprintf(stderr, "\n--------------TriVertex in the END------------\n");
    for (dbugi = 0; dbugi <= NumTri[cell]; dbugi++) {
	for (dbugj = 0; dbugj < 3; dbugj++) {
	    fprintf(stderr, "%4lf %4lf %4lf\t", TriVertex[cell][dbugi][dbugj]->X_c,
		    TriVertex[cell][dbugi][dbugj]->Y_c, TriVertex[cell][dbugi][dbugj]->Z_c);
	}
	fprintf(stderr,"\n");
    }
    fflush(stderr);
#endif

}
