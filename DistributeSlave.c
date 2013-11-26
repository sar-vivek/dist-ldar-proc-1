/*JWHGPLHERE*/
/*
 * DistributeSlave.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "DistributeSlave.h"

void Receive(int sd, void *buffer, size_t len) {
    ssize_t ret;
    ret = recv(sd, buffer, len, 0);
    if (ret != len) perror("recv()");
}

void DistributeReceive() {

    struct sockaddr_in cli_addr;
    long int ix;
    long int iy;
    int lsock;
    int cli_addr_len;
    int i;
    int c;

    lsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
    printf("NumPointRec = %u\n", NumPointRec);
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
    Xint_bin = Xint_cell / (NUM_BINS_X - 1);
    Yint_bin = Yint_cell / (NUM_BINS_Y - 1);

    ix = NodeID % NUM_NODES_X;
    iy = NodeID / NUM_NODES_X;
    X_c = MinX + Xint * ix;
    Y_c = MinY + Yint * iy;

    NodeMin.X_c = X_c;
    NodeMin.Y_c = Y_c;
    NodeMin.Z_c = 0;
    NodeMin.next = NULL;
    NodeMax.X_c = X_c + Xint;
    NodeMax.Y_c = Y_c + Yint;
    NodeMax.Z_c = 0;
    NodeMax.next = NULL;

    for (i = 0; i < NUM_CELLS; ++i) {
	ix = i % NUM_CELLS_X;
	iy = i / NUM_CELLS_X;

	CellMin[i].X_c = X_c + Xint_cell * ix;
	CellMin[i].Y_c = Y_c + Yint_cell * iy;
	CellMin[i].Z_c = 0;
	CellMin[i].next = NULL;

	CellMax[i].X_c = X_c + Xint_cell * (ix + 1);
	CellMax[i].Y_c = Y_c + Yint_cell * (iy + 1);
	CellMax[i].Z_c = 0;
	CellMax[i].next = NULL;
    }

    while (1) {
	Receive(ssock, X_b, XYZ_SIZE);
	if (*X_b == 0 && *Y_b == 0 && *Z_b == 0) break;

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

	ix = lround(floor((X_c - CellMin[c]) / Xint_bin));
	iy = lround(floor((Y_c - CellMin[c]) / Yint_bin));

	current->next = BinTbl[c][ix][iy];
	BinTbl[c][ix][iy] = current++;

	BinCnt[c][ix][iy]++;
	CellCnt[c]++;
	BinU1[c][ix][iy] += Z_c;
	*current2 = Z_c * Z_c;
	BinU2[c][ix][iy] += *current2++;
    }
}
