/*	$NetBSD: virtif_user.c,v 1.3 2014/03/14 10:06:22 pooka Exp $	*/

/*
 * Copyright (c) 2013 Antti Kantee.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _KERNEL
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/uio.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#include <rump/rumpuser_component.h>

#include <bmk-rumpuser/core_types.h>
#include <bmk-rumpuser/rumpuser.h>

#include <bmk-core/errno.h>
#include <bmk-core/memalloc.h>
#include <bmk-core/string.h>
#include <bmk-core/sched.h>

#include <net/if_ether.h>

#include "if_virt.h"
#include "virtif_user.h"
#include <bmk-core/solo5.h>

#if VIFHYPER_REVISION != 20140313
#error VIFHYPER_REVISION mismatch
#endif

struct virtif_user {
	struct virtif_sc *viu_virtifsc;
	int viu_devnum;

	int viu_fd;
	int viu_pipe[2];
	pthread_t viu_rcvthr;

	int viu_dying;

	char viu_rcvbuf[9018]; /* jumbo frame max len */
};

static void *
rcvthread(void *aaargh)
{
	struct virtif_user *viu = aaargh;
	struct pollfd pfd[2];
	struct iovec iov;
	ssize_t nn = 0;
	int prv;

	//rumpuser_component_kthread();

	/* give us a rump kernel context */
	rumpuser__hyp.hyp_schedule();
	rumpuser__hyp.hyp_lwproc_newlwp(0);
	rumpuser__hyp.hyp_unschedule();

	pfd[0].fd = viu->viu_fd;
	pfd[0].events = POLLIN;
	pfd[1].fd = viu->viu_pipe[0];
	pfd[1].events = POLLIN;

	while (!viu->viu_dying) {
		prv = poll(pfd, 2, -1);
		if (prv == 0)
			continue;
		if (prv == -1) {
			/* XXX */
			fprintf(stderr, "virt%d: poll error: %d\n",
			    viu->viu_devnum, errno);
			sleep(1);
			continue;
		}
		if (pfd[1].revents & POLLIN)
			continue;

		//nn = read(viu->viu_fd,
		//    viu->viu_rcvbuf, sizeof(viu->viu_rcvbuf));
		// int solo5_net_read_sync(uint8_t *data, int *n);
		if (nn == -1 && errno == EAGAIN)
			continue;

		if (nn < 1) {
			/* XXX */
			fprintf(stderr, "virt%d: receive failed\n",
			    viu->viu_devnum);
			sleep(1);
			continue;
		}
		iov.iov_base = viu->viu_rcvbuf;
		iov.iov_len = nn;

		//rumpuser_component_schedule(NULL);
		rumpuser__hyp.hyp_schedule();
		VIF_DELIVERPKT(viu->viu_virtifsc, &iov, 1);
		rumpuser__hyp.hyp_unschedule();
		//rumpuser_component_unschedule();
	}

	assert(viu->viu_dying);

	//rumpuser_component_kthread_release();
	return NULL;
}

int
VIFHYPER_MAC(uint8_t **enaddr)
{
	char *enaddrstr;
	struct ether_addr *e_addr;

	enaddrstr = solo5_net_mac_str();
	e_addr = ether_aton(enaddrstr);
	if (e_addr == NULL)
		return 1;
	*enaddr = e_addr->ether_addr_octet;
	return 0;
}

int
VIFHYPER_CREATE(const char *devstr, struct virtif_sc *vif_sc, uint8_t *enaddr,
	struct virtif_user **viup)
{
	struct virtif_user *viu = NULL;
	int devnum;
	int nlocks;
	int rv;

	rumpkern_unsched(&nlocks, NULL);
	//cookie = rumpuser_component_unschedule();

	/*
	 * Since this interface doesn't do LINKSTR, we know devstr to be
	 * well-formatted.
	 */
	devnum = atoi(devstr);

	viu = calloc(1, sizeof(*viu));
	if (viu == NULL) {
		rv = errno;
		goto oerr1;
	}
	viu->viu_virtifsc = vif_sc;

	viu->viu_devnum = devnum;

	//if (pipe(viu->viu_pipe) == -1) {
	//	rv = errno;
	//	goto oerr3;
	//}

if (0)
	if ((rv = pthread_create(&viu->viu_rcvthr, NULL, rcvthread, viu)) != 0)
		goto oerr1;

	//rumpuser_component_schedule(cookie);
	rumpkern_sched(nlocks, NULL);
	*viup = viu;
	return 0;

 oerr1:
	close(viu->viu_pipe[0]);
	close(viu->viu_pipe[1]);
	free(viu);
	//rumpuser_component_schedule(cookie);
	//return rumpuser_component_errtrans(rv);
	return rv;
}

void
VIFHYPER_SEND(struct virtif_user *viu,
	struct iovec *iov, size_t iovlen)
{
	size_t tlen, i;
	int nlocks;
	void *d;
	char *d0;

	rumpkern_unsched(&nlocks, NULL);
	/*
	 * solo5 doesn't do scatter-gather, so just simply
	 * copy the data into one lump here.  drop packet if we
	 * can't allocate temp memory space.
	 */
	if (iovlen == 1) {
		d = iov->iov_base;
		tlen = iov->iov_len;
	} else {
		for (i = 0, tlen = 0; i < iovlen; i++) {
			tlen += iov[i].iov_len;
		}

		/*
		 * allocate the temp space from RUMPKERN instead of BMK
		 * since there are no huge repercussions if we fail or
		 * succeed.
		 */
		d = d0 = bmk_memalloc(tlen, 0, BMK_MEMWHO_RUMPKERN);
		if (d == NULL)
			goto out;

		for (i = 0; i < iovlen; i++) {
			bmk_memcpy(d0, iov[i].iov_base, iov[i].iov_len);
			d0 += iov[i].iov_len;
		}
	}

	solo5_net_write_sync(d, tlen);

	if (iovlen != 1)
		bmk_memfree(d, BMK_MEMWHO_RUMPKERN);

	solo5_console_write("send\n", 5);
        for (i=0; i<iovlen; i++)
		solo5_console_write("\tsend\n", 6);

out:
	rumpkern_sched(nlocks, NULL);
}

int
VIFHYPER_DYING(struct virtif_user *viu)
{
	//void *cookie = rumpuser_component_unschedule();

	viu->viu_dying = 1;
#if 0
	if (write(viu->viu_pipe[1],
	    &viu->viu_dying, sizeof(viu->viu_dying)) == -1) {
		/*
		 * this is here mostly to avoid a compiler warning
		 * about ignoring the return value of write()
		 */
		fprintf(stderr, "%s: failed to signal thread\n",
		    VIF_STRING(VIFHYPER_DYING));
	}
#endif
	//rumpuser_component_schedule(cookie);

	return 0;
}

void
VIFHYPER_DESTROY(struct virtif_user *viu)
{
	//void *cookie = rumpuser_component_unschedule();

	pthread_join(viu->viu_rcvthr, NULL);
	close(viu->viu_pipe[0]);
	close(viu->viu_pipe[1]);
	free(viu);

	//rumpuser_component_schedule(cookie);
}
#endif
