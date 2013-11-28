/*JWHGPLHERE*/
/*
 * DistributeMaster.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "DistributeMaster.h"

int Socket(int domain, int type, int protocol) {
    int ret;
    ret = socket(domain, type, protocol);
    if (ret == -1) perror("socket()");
    return ret;
}

void Connect(int sock, struct sockaddr_in *addr, socklen_t addr_len) {
    int ret;
    ret = connect(sock, (struct sockaddr *) addr, addr_len);
    if (ret == -1) perror("connect()");
}

void Send(int sock, const void *buffer, size_t len) {
    ssize_t ret;
    size_t pos = 0;
    while (pos < len) {
	ret = send(sock, buffer + pos, len - pos, 0);
	if (ret == -1) {
	    perror("send()");
	    return;
	}
	pos += ret;
    }
}

void DistributeSend() {

    uint32_t ix;
    uint32_t iy;
    int i;
    int c;

    for (i = 1; i < NUM_NODES; ++i) {
	memset(&svr_addr[i], 0, sizeof (struct sockaddr_in));

	msock[i] = Socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	svr_addr[i].sin_family = AF_INET;
	svr_addr[i].sin_port = htons(PORT_BASE);
	svr_addr[i].sin_addr.s_addr = inet_addr(NodeIPs[i]);

	Connect(msock[i], &svr_addr[i], sizeof (struct sockaddr_in));
    }

    for (i = 1; i < NUM_NODES; ++i) {
	Send(msock[i], &Xscale, DOUBLE_SIZE);
	Send(msock[i], &Yscale, DOUBLE_SIZE);
	Send(msock[i], &Zscale, DOUBLE_SIZE);
	Send(msock[i], &Xoffset, DOUBLE_SIZE);
	Send(msock[i], &Yoffset, DOUBLE_SIZE);
	Send(msock[i], &Zoffset, DOUBLE_SIZE);
	Send(msock[i], &MaxX, DOUBLE_SIZE);
	Send(msock[i], &MinX, DOUBLE_SIZE);
	Send(msock[i], &MaxY, DOUBLE_SIZE);
	Send(msock[i], &MinY, DOUBLE_SIZE);
	Send(msock[i], &MaxZ, DOUBLE_SIZE);
	Send(msock[i], &MinZ, DOUBLE_SIZE);
    }

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

    X_c = MinX;
    Y_c = MinY;

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

    while (count-- > 0) {
	fread(X_b, INT32_SIZE, 3, las_file_in);
	fseek(las_file_in, POINT_DATA_SKIP, SEEK_CUR);

	ix = lround(floor(*((int32_t *) X_b) * Xratio + Xdiff));
	iy = lround(floor(*((int32_t *) Y_b) * Yratio + Ydiff));

	if (ix == NUM_NODES_X) --ix;
	if (iy == NUM_NODES_Y) --iy;

	i = NUM_NODES_X * iy + ix;

#if DEBUG == 1
	if (count % 100000 = 17) {
	    printf("i=%d ", i);
	}
#endif

	if (i == 0) {
	    X_c = *((int32_t *) X_b) * Xscale + Xoffset;
	    Y_c = *((int32_t *) Y_b) * Yscale + Yoffset;
	    Z_c = *((int32_t *) Z_b) * Zscale + Zoffset;
	    current->X_c = X_c;
	    current->Y_c = Y_c;
	    current->Z_c = Z_c;
	    current->next = NULL;

	    ix = lround(floor((X_c - MinX) / Xint_cell));
	    iy = lround(floor((Y_c - MinY) / Yint_cell));
	    if (ix == NUM_CELLS_X) --ix;
	    if (iy == NUM_CELLS_Y) --iy;
	    c = NUM_CELLS_X * iy + ix;

	    ix = lround(floor((X_c - CellMin[c].X_c) / Xint_bin));
	    iy = lround(floor((Y_c - CellMin[c].Y_c) / Yint_bin));

	    current->next = BinTbl[c][ix][iy];
	    BinTbl[c][ix][iy] = current++;

	    BinCnt[c][ix][iy]++;
	    CellCnt[c]++;
	    BinU1[c][ix][iy] += Z_c;
	    *current2 = Z_c * Z_c;
	    BinU2[c][ix][iy] += *current2++;
	} else {
	    Send(msock[i], X_b, XYZ_SIZE);
	}
    }

    *((int32_t *) X_b) = 0;
    *((int32_t *) Y_b) = 0;
    *((int32_t *) Z_b) = 0;

    for (i = 1; i < NUM_NODES; ++i) {
	Send(msock[i], X_b, XYZ_SIZE);
    }
}
