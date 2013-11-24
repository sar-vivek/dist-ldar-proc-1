/*JWHGPLHERE*/a
/*
 * DistributeMaster.c
 *
 * Author: James W Hegeman
 *
 */

#include <"DLPstd.h">
#include <"DistLdarProcFVar.h">
#include <"DistributeMaster.h">

int Socket(int domain, int type, int protocol) {
    int ret;
    ret = socket(domain, type, protocol);
    if (ret == -1) perror("socket()");
    return ret;
}

void Connect(int sock, const struct sock_addr *addr, socklen_t addr_len) {
    int ret;
    ret = connect(sock, addr, addr_len);
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

    LidarPointNode_t *itr;
    LidarPointNode_t *prev;
    int i;
    int ix;
    int iy;
    int c;

    for (i = 1; i <= NUM_NODES; ++i) {
	memset(&svr_addr[i], 0, sizeof (struct sockaddr_in));

	msock[i] = Socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	svr_addr[i].sin_family = AF_INET;
	svr_addr[i].sin_port = htons(PORT_BASE);
	svr_addr[i].sin_addr.s_addr = inet_addr(NodeIPs[i]);

	Connect(msock[i], svr_addr[i], sizeof (struct sockaddr_in));
    }

    for (i = 1; i < = NUM_NODES; ++i) {
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

	ix = *((int32_t *) X_b) * Xratio + Xdiff;
	iy = *((int32_t *) Y_b) * Yratio + Ydiff;

	if (ix == NUM_NODES_X) --ix;
	if (iy == NUM_NODES_Y) --iy;

	i = NUM_NODES_X * iy + ix;

	if (i == 0) {
	    X_c = *((int32_t *) X_b) * Xscale + Xoffset;
	    Y_c = *((int32_t *) Y_b) * Yscale + Yoffset;
	    Z_c = *((int32_t *) Z_b) * Zscale + Zoffset;
	    current->X_c = X_c;
	    current->Y_c = Y_c;
	    current->Z_c = Z_c;
	    current->next = NULL;

	    ix = (X_c - MinX) / Xint_cell;
	    iy = (Y_c - MinY) / Yint_cell;
	    if (ix == NUM_CELLS_X) --ix;
	    if (iy == NUM_CELLS_Y) --iy;
	    c = NUM_CELLS_X * iy + ix;

	    ix = (X_c - CellMin[c]) / Xint_bin;
	    iy = (Y_c - CellMin[c]) / Yint_bin;
	    if (ix == NUM_BINS_X) --ix;
	    if (iy == NUM_BINS_Y) --iy;

	    itr = BinTbl[c][ix][iy];
	    if (BinTbl[c][ix][iy] == NULL) {
		BinTbl[c][ix][iy] = current++;
	    } else {
		while (itr != NULL) {
		    prev = itr;
		    itr = itr->next;
		}
		prev->next = current++;
	    }

	    BinCnt[c][ix][iy]++;
	    CellCnt[c]++;
	    BinU1[c][ix][iy] += Z_c;
	    *current2 = Z_c * Z_c;
	    BinU2[c][ix][iy] += *current2++;
	} else {
	    Send(msock[i], X_b, XYZ_SIZE);
	}
    }
}
