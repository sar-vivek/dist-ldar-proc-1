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
#if DEBUG >=1 
    INT dbugi;
    int dbugj;
#endif

    if ((proc_file_out = fopen("processed.out", "wb")) == NULL) {
	fprintf(stderr, "Could not open file %s for writing. Exiting.\n", "processed.out");
	fflush(stderr);
	exit(-1);
    }

    if (NUM_NODES > 1) {
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

#if DEBUG >= 1
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
#if DEBUG >= 1
		    printf("Socket %d closed.\n", msock[i]);
		    fflush(stdout);
#endif
		    --socketcount;
		    continue;
		}
		fprintf(proc_file_out, "%2d %2d %2d | %4d %4d %4d | ", NodeSockIndex[readsock], c, t,
			*((uint32_t *) X_b), *((uint32_t *) Y_b), *((uint32_t *) Z_b));
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

	free(newevents);
    }

#if DEBUG >=1
    fprintf(stderr, "\n--------------TriVertex in the MERGE------------\n");
    for(c = 0; c < NUM_CELLS; ++c){
        for(dbugi = 0; dbugi <= NumTri[c]; dbugi++){
	   for(dbugj = 0; dbugj < 3; dbugj++)
		fprintf(stderr, "%4lg %4lg %4lg\t", TriVertex[c][dbugi][dbugj]->X_c, TriVertex[c][dbugi][dbugj]->Y_c, TriVertex[c][dbugi][dbugj]->Z_c);
	    fprintf(stderr,"\n");
	}
        fflush(stderr);
    }
#endif	
    /*
       for (c = 0; c < NUM_CELLS; ++c) {
       for (t = 0; t <= NumTri[c]; ++t) {
       fprintf(proc_file_out, "%2d %2d %2d | %4d %4d %4d | ", NodeID, c, t, lround((TriVertex[c][t][0]->X_c - Xoffset) / Xscale),
       lround((TriVertex[c][t][0]->Y_c - Yoffset) / Yscale), lround((TriVertex[c][t][0]->Z_c - Zoffset) / Zscale));
       fprintf(proc_file_out, "%4d %4d %4d | ", lround((TriVertex[c][t][1]->X_c - Xoffset) / Xscale),
       lround((TriVertex[c][t][1]->Y_c - Yoffset) / Yscale), lround((TriVertex[c][t][1]->Z_c - Zoffset) / Zscale));
       fprintf(proc_file_out, "%4d %4d %4d\n", lround((TriVertex[c][t][2]->X_c - Xoffset) / Xscale),
       lround((TriVertex[c][t][2]->Y_c - Yoffset) / Yscale), lround((TriVertex[c][t][2]->Z_c - Zoffset) / Zscale));
#if DEBUG >= 1
fflush(proc_file_out);
#endif
#if DEBUG >= 1 
fprintf(stderr, "%2d %2d %2d | %lg %lg %lg | ", NodeID, c, t, TriVertex[c][t][0]->X_c,
TriVertex[c][t][0]->Y_c, TriVertex[c][t][0]->Z_c);
fprintf(stderr, "%lg %lg %lg | ", TriVertex[c][t][1]->X_c,
TriVertex[c][t][1]->Y_c, TriVertex[c][t][1]->Z_c);
fprintf(stderr, "%lg %lg %lg\n", TriVertex[c][t][2]->X_c,
TriVertex[c][t][2]->Y_c, TriVertex[c][t][2]->Z_c);
fflush(stderr);
#endif
}
}*/

    if (fclose(proc_file_out)) perror("fclose()");

#if DEBUG >= 3
    for (i = 0; i < mycount; ++i) {
	fprintf(stderr, "(%4d, %4d, %4d)\n", lround((PntTbl[i].X_c - Xoffset) / Xscale),
		lround((PntTbl[i].Y_c - Yoffset) / Yscale), lround((PntTbl[i].Z_c - Zoffset) / Zscale));
    }
#endif

}
