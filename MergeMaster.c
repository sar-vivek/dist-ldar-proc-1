/*JWHGPLHERE*/
/*
 * MergeMaster.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "Triangulate.h"
#include "DistributeSlave.h"
#include "MergeMaster.h"

void MergeReceive() {

    uint32_t t;
    int c;
    int i;
    int ret;
    int readsock;
    int socketcount = NUM_NODES - 1;

    if ((proc_file_out = fopen("processed.out", "wb")) == NULL) {
	fprintf(stderr, "Could not open file %s for writing. Exiting.\n", "processed.out");
	fflush(stderr);
	exit(-1);
    }

    newevents = (struct epoll_event *) Malloc(NUM_NODES * sizeof (struct epoll_event));

    polldesc = epoll_create(NUM_NODES - 1);
    if (polldesc == -1) perror("epoll_create()");

    for (i = 1; i < NUM_NODES; ++i) {
	msockevents[i].events = EPOLLIN;
	msockevents[i].data.fd = msock[i];
	if (epoll_ctl(polldesc, EPOLL_CTL_ADD, msock[i], &msockevents[i]) == -1) perror("epoll_ctl()");
    }

    while (1) {
	ret = epoll_wait(polldesc, newevents, NUM_NODES - 1, -1);
	if (ret == -1) perror("epoll_wait()");

#if DEBUG == 1
	printf("Received %d events\n", ret);
	fflush(stdout);
#endif

	for (i = 0; i < ret; ++i) {
	    readsock = newevents[i].data.fd;
	    Receive(readsock, X_b, XYZ_SIZE);
	    t = *((uint32_t *) X_b);
	    c = *((int *) Y_b);
	    Receive(readsock, X_b, XYZ_SIZE);
	    if (*((int32_t *) X_b) == 0 && *((int32_t *) Y_b) == 0 && *((int32_t *) Z_b) == 0) {
		if (epoll_ctl(polldesc, EPOLL_CTL_DEL, msock[i], &msockevents[i]) == -1) perror("epoll_ctl()");
		if (close(msock[i]) == -1) perror("close()");
#if DEBUG == 1
		printf("Socket %d closed.\n", msock[i]);
		fflush(stdout);
#endif
		--socketcount;
		continue;
	    }
	    fprintf(proc_file_out, "%2d %2d | %4d %4d %4d | ", c, t, *((uint32_t *) X_b),
		    *((uint32_t *) Y_b), *((uint32_t *) Z_b));
	    Receive(readsock, X_b, XYZ_SIZE);
	    fprintf(proc_file_out, "%4d %4d %4d | ", *((uint32_t *) X_b),
		    *((uint32_t *) Y_b), *((uint32_t *) Z_b));
	    Receive(readsock, X_b, XYZ_SIZE);
	    fprintf(proc_file_out, "%4d %4d %4d\n", *((uint32_t *) X_b),
		    *((uint32_t *) Y_b), *((uint32_t *) Z_b));
	}

	if (socketcount == 0) break;
    }

    if (close(polldesc) == -1) perror("close()");

    for (c = 0; c < NUM_CELLS; ++c) {
	for (t = 0; t < NumTri[c]; ++t) {
	    fprintf(proc_file_out, "%2d %2d | %4d %4d %4d | ", c, t, lround((TriVertex[c][t][0]->X_c - Xoffset) / Xscale)),
		    lround((TriVertex[c][t][0]->Y_c - Yoffset) / Yscale), lround((TriVertex[c][t][0]->Z_c - Zoffset) / Zscale));
	    fprintf(proc_file_out, "%4d %4d %4d | ", lround((TriVertex[c][t][1]->X_c - Xoffset) / Xscale),
		    lround((TriVertex[c][t][1]->Y_c - Yoffset) / Yscale), lround((TriVertex[c][t][1]->Z_c - Zoffset) / Zscale));
	    fprintf(proc_file_out, "%4d %4d %4d\n", lround((TriVertex[c][t][2]->X_c - Xoffset) / Xscale),
		    lround((TriVertex[c][t][2]->Y_c - Yoffset) / Yscale), lround((TriVertex[c][t][2]->Z_c - Zoffset) / Zscale));
	}
    }

    if (fclose(proc_file_out)) perror("fclose()");

    free(newevents);

}
