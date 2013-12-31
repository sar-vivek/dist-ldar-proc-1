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
    uint32_t done;
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
    ReadP = LasPoints;

    while (count-- > 0) {
#if DEBUG >= 2
	if (count % 10000000 == 0) {
	    fprintf(stderr, "%u points remaining to be 'read' and 'sent'\n", count);
	    fflush(stderr);
	}
#endif
	ix = lround(floor(*((int32_t *) ReadP) * Xratio + Xdiff));
	iy = lround(floor(*((int32_t *) (ReadP + INT32_SIZE)) * Yratio + Ydiff));

	if (ix >= NUM_NODES_X) ix = NUM_NODES_X - 1;
	if (ix < 0) ix = 0;
	if (iy >= NUM_NODES_Y) iy = NUM_NODES_Y - 1;
	if (iy < 0) iy = 0;

	i = NUM_NODES_X * iy + ix;

	memcpy(NetBufCurrent[i], ReadP, XYZ_SIZE);
	NetBufCurrent[i] += XYZ_SIZE;
	NetBufCounter[i]++;
	ReadP += PointDataRecLen;
    }

#if DEBUG >= 1
    for (i = 0; i < NUM_NODES; ++i) {
	assert((size_t) (NetBufCurrent[i] - NetworkBuffers[i]) == ((size_t) NetBufCounter[i]) * ((size_t) XYZ_SIZE));
    }
    assert((size_t) (ReadP - LasPoints) == ((size_t) NumPointRec) * ((size_t) PointDataRecLen));
#endif

    count = NetBufCounter[0];
    for (i = 0; i < NUM_NODES; ++i) {
	if (i > 0) Send(msock[i], &NetBufCounter[i], UINT32_SIZE);
	PacketCounter[i] = NetBufCounter[i] / XYZ_PER_PACKET;
	PacketCounter[i]++;

	NetBufCurrent[i] = NetworkBuffers[i];
	if (NetBufCounter[i] + XYZ_PER_PACKET > NET_BUF_LEN) {
	    fprintf(stderr, "Uh oh. Exceeded network buffer for node %d.\n", i);
	    fflush(stderr);
	    exit(-1);
	}
    }

#if DEBUG >= 1
    for (i = 0; i < NUM_NODES; ++i) {
	fprintf(stderr, "Node %d is to get %u points\n", i, NetBufCounter[i]);
    }
    fflush(stderr);
#endif

    done = 0;
    while (1) {
	for (i = 1; i < NUM_NODES; ++i) {
	    if (PacketCounter[i] == 0) {
		done |= (1 << i);
		continue;
	    }
	    Send(msock[i], NetBufCurrent[i], PACKET_LEN);
	    PacketCounter[i]--;
#if DEBUG >= 2
	    if (PacketCounter[i] % 100000 == 0) {
		fprintf(stderr, "%u packets remaining to be sent to node %d\n", PacketCounter[i], i);
		fflush(stderr);
	    }
#endif
	    NetBufCurrent[i] += PACKET_LEN;
	}
	if (done == (1 << NUM_NODES) - 2) break;
    }

    NetBufCurrent[0] = NetworkBuffers[0];

}
