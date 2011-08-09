/*
 *  FL-COW by Davide Libenzi ( File Links Copy On Write )
 *  Copyright (C) 2003  Davide Libenzi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>



#define DUMMY_DATA1 "FLCOW Test Data Before"
#define DUMMY_DATA2 "FLCOW Test Data After"


static int create_file(int (*ofunc)(char const *, int, ...),
		       char const *name, char const *data) {
	int fd;

	if ((fd = (*ofunc)(name, O_WRONLY | O_CREAT, 0666)) == -1) {
		perror(name);
		return -1;
	}
	write(fd, data, strlen(data));
	close(fd);

	return 0;
}


static int test_flcow(int (*ofunc)(char const *, int, ...), char const *ext) {
	struct stat stb1, stb2;
	char buf[512], name1[512], name2[512];

	getcwd(buf, sizeof(buf) - 1);
	setenv("FLCOW_PATH", buf, 1);

	sprintf(name1, ",,flcow-test1++.%u", getpid());
	if (create_file(ofunc, name1, DUMMY_DATA1) < 0) {
		fprintf(stdout, "[%s] Test Result\t\t[ FAILED ]\n", ext);
		return 1;
	}
	fprintf(stdout, "[%s] File Creation\t\t[ OK ]\n", ext);

	sprintf(name2, ",,flcow-test2++.%u", getpid());
	if (link(name1, name2) < 0) {
		unlink(name1);
		fprintf(stdout, "[%s] Test Result\t\t[ FAILED ]\n", ext);
		return 2;
	}
	fprintf(stdout, "[%s] Link Creation\t\t[ OK ]\n", ext);

	stat(name1, &stb1);
	stat(name2, &stb2);
	if (stb1.st_nlink < 2 || stb2.st_nlink < 2) {
		unlink(name2);
		unlink(name1);
		fprintf(stdout, "[%s] Link Check\t\t[ FAILED ]\n", ext);
		return 3;
	}
	fprintf(stdout, "[%s] Link Check\t\t[ OK ]\n", ext);

	if (create_file(ofunc, name1, DUMMY_DATA2) < 0) {
		unlink(name2);
		unlink(name1);
		fprintf(stdout, "[%s] File Rewrite\t\t[ FAILED ]\n", ext);
		return 4;
	}
	fprintf(stdout, "[%s] File Rewrite\t\t[ OK ]\n", ext);

	stat(name1, &stb1);
	stat(name2, &stb2);
	if (stb1.st_nlink > 1 || stb2.st_nlink > 1) {
		unlink(name2);
		unlink(name1);
		fprintf(stdout, "[%s] COW Check\t\t[ FAILED ]\n", ext);
		return 5;
	}
	fprintf(stdout, "[%s] COW Check\t\t[ OK ]\n", ext);

	unlink(name2);
	unlink(name1);

	fprintf(stdout, "[%s] Test Result\t\t[ OK ]\n\n", ext);

	return 0;
}


int main(int argc, char **argv) {
	int res;

	if ((res = test_flcow(open, "open")) != 0)
		return res;
#ifdef HAVE_OPEN64
	if ((res = test_flcow(open64, "open64")) != 0)
		return res;
#endif

	return 0;
}

