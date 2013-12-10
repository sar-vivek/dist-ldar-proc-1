/*JWHGPLHERE*/
/*
 * DistributeSlave.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "DistributeSlave.h"

void Receive(int sd, void *buffer, size_t len) {
    ssize_t ret;
    ret = recv(sd, buffer, len, 0);
    if (ret != len) perror("recv()");
}

void CreateMinMax() {
    uint32_t ix;
    uint32_t iy;
    int c;

    NodeMin.X_c = X_c;
    NodeMin.Y_c = Y_c;
    NodeMin.Z_c = 0;
    NodeMin.next = NULL;
    NodeMax.X_c = X_c + Xint;
    NodeMax.Y_c = Y_c + Yint;
    NodeMax.Z_c = 0;
    NodeMax.next = NULL;

    for (c = 0; c < NUM_CELLS; ++c) {
	ix = c % NUM_CELLS_X;
	iy = c / NUM_CELLS_X;

	CellMin[c].X_c = X_c + Xint_cell * ix;
	CellMin[c].Y_c = Y_c + Yint_cell * iy;
	CellMin[c].Z_c = 0;
	CellMin[c].next = NULL;

	CellMax[c].X_c = X_c + Xint_cell * (ix + 1);
	CellMax[c].Y_c = Y_c + Yint_cell * (iy + 1);
	CellMax[c].Z_c = 0;
	CellMax[c].next = NULL;
    }
}

void AddPoint() {
    uint32_t ix;
    uint32_t iy;
    int c;

    X_c = *((int32_t *) X_b) * Xscale + Xoffset;
    Y_c = *((int32_t *) Y_b) * Yscale + Yoffset;
    Z_c = *((int32_t *) Z_b) * Zscale + Zoffset;
    current->X_c = X_c;
    current->Y_c = Y_c;
    current->Z_c = Z_c;

    ix = lround(floor((X_c - NodeMin.X_c) / Xint_cell));
    iy = lround(floor((Y_c - NodeMin.Y_c) / Yint_cell));
    if (ix == NUM_CELLS_X) --ix;
    if (iy == NUM_CELLS_Y) --iy;
    c = NUM_CELLS_X * iy + ix;

    ix = lround(floor((X_c - CellMin[c].X_c) / Xint_bin));
    iy = lround(floor((Y_c - CellMin[c].Y_c) / Yint_bin));
#if DEBUG == 1
    if (ix < 0 || iy < 0 || ix > NUM_BINS_X || iy > NUM_BINS_Y) {
	printf("Error: ix = %u, iy = %u out of bounds\n");
	fflush(stdout);
    }
#endif
    if (ix == NUM_BINS_X) {
	--ix;
#if DEBUG == 1
	printf("Bin right edge hit.\n");
	fflush(stdout);
#endif
    }
    if (iy == NUM_BINS_Y) {
	--iy;
#if DEBUG == 1
	printf("Bin top edge hit.\n");
	fflush(stdout);
#endif
    }

    current->next = BinTbl[c][ix][iy];
    BinTbl[c][ix][iy] = current++;

    BinCnt[c][ix][iy]++;
    CellCnt[c]++;
    BinU1[c][ix][iy] += Z_c;
    *current2 = Z_c * Z_c;
    BinU2[c][ix][iy] += *current2++;
    ++mycount;

#if DEBUG == 1
    if (mycount > NODE_POINTS_MAX) {
	fprintf(stderr, "NODE_POINTS_MAX exceeded. Exiting.\n");
	fflush(stderr);
	exit(-1);
    }
#endif
}

void AddPoint2(int c, int32_t ix, int32_t iy) {
    LidarPointNode_t *node;
    int32_t ax;
    int32_t ay;
    int32_t bx;
    int32_t by;
    int cnt;
    int d;
    int i;
    int j;
    int k;
    int skip;

    Z_c = 0;
    cnt = 0;
    i = 1;

#if DEBUG == 1
    if (ix < 0 || bx >= NUM_BINS_X) {
	fprintf(stderr, "bx out of range: c = %d, ix = %d, iy = %d\n", c, ix, iy);
	fflush(stderr);
    }
    if (iy < 0 || by >= NUM_BINS_Y) {
	fprintf(stderr, "by out of range: c = %d, ix = %d, iy = %d\n", c, ix, iy);
	fflush(stderr);
    }
    if (c < 0 || c >= NUM_CELLS) {
	fprintf(stderr, "d out of range: c = %d, ix = %d, iy = %d\n", c, ix, iy);
	fflush(stderr);
    }
#endif

    node = BinTbl[c][ix][iy];
    while (node != NULL) {
#if DEBUG == 1
	printf("Found a point. i = 0, X_c = %lg, Y_c = %lg, Z_c = %lg\n", node->X_c, node->Y_c, node->Z_c);
	fflush(stdout);
#endif
	Z_c += node->Z_c;
	++cnt;
	node = node->next;
    }

    while (cnt == 0) {
	ax = ix - i;
	ay = iy - i;

	for (j = 0; j < 4; ++j) {
	    for (k = 0; k < 2 * i; ++k) {
		skip = 0;
		d = c;
		bx = ax;
		by = ay;

		/* Possible error here, theoretically, if ax < -NUM_BINS_X or
		 * ax >= 2 * NUM_BINS_X; similarly for ay. I am intentionally
		 * ignoring this possibility; it is not likely. */
		if (bx >= NUM_BINS_X) {
		    if (++d % NUM_CELLS_X == 0) skip = 1;
		    else bx -= NUM_BINS_X;
		} else if (bx < 0) {
		    if (d-- % NUM_CELLS_X == 0) skip = 1;
		    else bx += NUM_BINS_X;
		}
		if (by >= NUM_BINS_Y) {
		    d += NUM_CELLS_X;
		    if (d >= NUM_CELLS) skip = 1;
		    else by -= NUM_BINS_Y;
		} else if (by < 0) {
		    d -= NUM_CELLS_X;
		    if (d < 0) skip = 1;
		    else by += NUM_BINS_Y;
		}
		if (skip == 0) {
#if DEBUG == 1
		    if (bx < 0 || bx >= NUM_BINS_X) {
			fprintf(stderr, "bx out of range: c = %d, ax = %d, ay = %d, d = %d, bx = %d, by = %d\n",
				c, ax, ay, d, bx, by);
			fflush(stderr);
		    }
		    if (by < 0 || by >= NUM_BINS_Y) {
			fprintf(stderr, "by out of range: c = %d, ax = %d, ay = %d, d = %d, bx = %d, by = %d\n",
				c, ax, ay, d, bx, by);
			fflush(stderr);
		    }
		    if (d < 0 || d >= NUM_CELLS) {
			fprintf(stderr, "d out of range: c = %d, ax = %d, ay = %d, d = %d, bx = %d, by = %d\n",
				c, ax, ay, d, bx, by);
			fflush(stderr);
		    }
#endif
		    node = BinTbl[d][bx][by];
		    while (node != NULL) {
#if DEBUG == 1
			printf("Found a point. i = %d, X_c = %lg, Y_c = %lg, Z_c = %lg\n", i, node->X_c, node->Y_c, node->Z_c);
			fflush(stdout);
#endif
			Z_c += node->Z_c;
			++cnt;
			node = node->next;
		    }
		}
		switch (j) {
		    case 0: ++ax;
			    break;
		    case 1: ++ay;
			    break;
		    case 2: --ax;
			    break;
		    case 3: --ay;
			    break;
		    default: fprintf(stderr, "switch-case default\n");
			    fflush(stderr);
		}
	    }
	}

	++i;
    }

    Z_c /= cnt;

#if DEBUG == 1
    printf("Adding a point in cell %d. ix = %d, iy = %d, X_c = %lg, Y_c = %lg, Z_c = %lg\n",
	   c, ix, iy, X_c, Y_c, Z_c);
    fflush(stdout);
#endif

    current->X_c = X_c;
    current->Y_c = Y_c;
    current->Z_c = Z_c;
    current->next = BinTbl[c][ix][iy];
    BinTbl[c][ix][iy] = current++;

    BinCnt[c][ix][iy]++;
    CellCnt[c]++;
    BinU1[c][ix][iy] += Z_c;
    *current2 = Z_c * Z_c;
    BinU2[c][ix][iy] += *current2++;
    ++mycount;

#if DEBUG == 1
    if (mycount > NODE_POINTS_MAX) {
	fprintf(stderr, "NODE_POINTS_MAX exceeded. Exiting.\n");
	fflush(stderr);
	exit(-1);
    }
#endif
}

void BoundaryPointsAdd() {
    int32_t i;
    int c;

    for (c = 0; c < NUM_CELLS; ++c) {
	X_c = CellMin[c].X_c;
	Y_c = CellMin[c].Y_c;
	AddPoint2(c, 0, 0);
	X_c += 2 * Xint_bin;
	Zinit[c][0] = Z_c;
	for (i = 1; i < NUM_BINS_X; i += 2) {
	    AddPoint2(c, i, 0);
	    X_c += 2 * Xint_bin;
	}

	X_c = CellMax[c].X_c;
	AddPoint2(c, NUM_BINS_X - 1, 0);
	Y_c += 2 * Yint_bin;
	Zinit[c][1] = Z_c;
	for (i = 1; i < NUM_BINS_Y; i += 2) {
	    AddPoint2(c, NUM_BINS_X - 1, i);
	    Y_c += 2 * Yint_bin;
	}

	X_c = CellMax[c].X_c;
	Y_c = CellMax[c].Y_c;
	AddPoint2(c, NUM_BINS_X - 1, NUM_BINS_Y - 1);
	X_c -= 2 * Xint_bin;
	Zinit[c][3] = Z_c;
	for (i = NUM_BINS_X - 2; i > 0; i -= 2) {
	    AddPoint2(c, i, NUM_BINS_Y - 1);
	    X_c -= 2 * Xint_bin;
	}
	
	X_c = CellMin[c].X_c;
	AddPoint2(c, 0, NUM_BINS_Y - 1);
	Y_c -= 2 * Yint_bin;
	Zinit[c][2] = Z_c;
	for (i = NUM_BINS_Y - 2; i > 0; i -= 2) {
	    AddPoint2(c, 0, i);
	    Y_c -= 2 * Yint_bin;
	}
    }    
}

void DistributeReceive() {

    struct sockaddr_in cli_addr;
    uint32_t ix;
    uint32_t iy;
    int lsock;
    int cli_addr_len;

    lsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (lsock == -1) perror("socket()");

    memset(svr_addr, 0, sizeof (struct sockaddr_in));
    svr_addr->sin_family = AF_INET;
    svr_addr->sin_port = htons(PORT_BASE);
    svr_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(lsock, (struct sockaddr *) &svr_addr[0], sizeof (struct sockaddr_in)) == -1) perror("bind()");

    if (listen(lsock, 1) == -1) perror("listen()");
    ssock = accept(lsock, (struct sockaddr *) &cli_addr, (socklen_t *) &cli_addr_len);
    if (ssock == -1) perror("accept()");

    Receive(ssock, &Xscale, DOUBLE_SIZE);
    Receive(ssock, &Yscale, DOUBLE_SIZE);
    Receive(ssock, &Zscale, DOUBLE_SIZE);
    Receive(ssock, &Xoffset, DOUBLE_SIZE);
    Receive(ssock, &Yoffset, DOUBLE_SIZE);
    Receive(ssock, &Zoffset, DOUBLE_SIZE);
    Receive(ssock, &MaxX, DOUBLE_SIZE);
    Receive(ssock, &MinX, DOUBLE_SIZE);
    Receive(ssock, &MaxY, DOUBLE_SIZE);
    Receive(ssock, &MinY, DOUBLE_SIZE);
    Receive(ssock, &MaxZ, DOUBLE_SIZE);
    Receive(ssock, &MinZ, DOUBLE_SIZE);

#if DEBUG == 1
    printf("-------------------------------------\n");
    printf("Xscale = %lg\n", Xscale);
    printf("Yscale = %lg\n", Yscale);
    printf("Zscale = %lg\n", Zscale);
    printf("Xoffset = %lg\n", Xoffset);
    printf("Yoffset = %lg\n", Yoffset);
    printf("Zoffset = %lg\n", Zoffset);
    printf("MaxX = %lg\n", MaxX);
    printf("MinX = %lg\n", MinX);
    printf("MaxY = %lg\n", MaxY);
    printf("MinY = %lg\n", MinY);
    printf("MaxZ = %lg\n", MaxZ);
    printf("MinZ = %lg\n", MinZ);
    printf("-------------------------------------\n\n");
    fflush(stdout);
#endif

    Xint = (MaxX - MinX) / NUM_NODES_X;
    Yint = (MaxY - MinY) / NUM_NODES_Y;
    Xratio = Xscale / Xint;
    Yratio = Yscale / Yint;
    Xdiff = (Xoffset - MinX) / Xint;
    Ydiff = (Yoffset - MinY) / Yint;
    Xint_cell = Xint / NUM_CELLS_X;
    Yint_cell = Yint / NUM_CELLS_Y;
    Xint_bin = Xint_cell / NUM_BINS_X;
    Yint_bin = Yint_cell / NUM_BINS_Y;

    ix = NodeID % NUM_NODES_X;
    iy = NodeID / NUM_NODES_X;
    X_c = MinX + Xint * ix;
    Y_c = MinY + Yint * iy;

    CreateMinMax();

    while (1) {
	Receive(ssock, X_b, XYZ_SIZE);

	if (*((int32_t *) X_b) == 0 && *((int32_t *) Y_b) == 0 && *((int32_t *) Z_b) == 0) break;

	AddPoint();
    }

    BoundaryPointsAdd();

}
