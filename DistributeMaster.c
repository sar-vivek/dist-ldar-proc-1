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
#include "DistributeSlave.h"
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

    for (i = 1; i < NUM_NODES; ++i) {
	memset(&svr_addr[i], 0, sizeof (struct sockaddr_in));

	msock[i] = Socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	svr_addr[i].sin_family = AF_INET;
	svr_addr[i].sin_port = htons(PORT_BASE);
	svr_addr[i].sin_addr.s_addr = inet_addr(NodeIPs[i]);

	Connect(msock[i], &svr_addr[i], sizeof (struct sockaddr_in));

	NodeSockIndex[msock[i]] = i;
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
    Xint_bin = Xint_cell / NUM_BINS_X;
    Yint_bin = Yint_cell / NUM_BINS_Y;

    X_c = MinX;
    Y_c = MinY;

    CreateMinMax();

    count = NumPointRec;

    while (count-- > 0) {
#if DEBUG >= 1
	if (count % 10000000 == 0) {
	    fprintf(stderr, "%u points remaining to be read and sent\n", count);
	    fflush(stderr);
	}
#endif
	fread(X_b, INT32_SIZE, 3, las_file_in);
	fseek(las_file_in, POINT_DATA_SKIP, SEEK_CUR);

	ix = lround(floor(*((int32_t *) X_b) * Xratio + Xdiff));
	iy = lround(floor(*((int32_t *) Y_b) * Yratio + Ydiff));

	if (ix >= NUM_NODES_X) ix = NUM_NODES_X - 1;
	if (ix < 0) ix = 0;
	if (iy >= NUM_NODES_Y) iy = NUM_NODES_Y - 1;
	if (iy < 0) iy = 0;

	i = NUM_NODES_X * iy + ix;

	if (i == 0) AddPoint();
	else Send(msock[i], X_b, XYZ_SIZE);
    }

    if (fclose(las_file_in)) perror("fclose()");

    *((int32_t *) X_b) = 0;
    *((int32_t *) Y_b) = 0;
    *((int32_t *) Z_b) = 0;

    for (i = 1; i < NUM_NODES; ++i) {
	Send(msock[i], X_b, XYZ_SIZE);
    }

    boundary_begin = current;

    BoundaryPointsAdd();

}
