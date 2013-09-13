/* adf version 0.2.3
   Copyright (c) 2001 by arj_
   arj_: icq#22638227 or irc.brasnet.org @ #linuxnews
   This is the new C version, it may work on linux only.
   I intend to port it for bsd later, I need some feedback. 
   Any bug, comment, sugestion or anything, contact me. 
   Compile: gcc -Wall adf.c -o adf  */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <libgen.h>
#include <getopt.h>
#include <ctype.h>
#include <stdlib.h>
#include <mntent.h>

#define DEF_OPTIONS "pdt"		/* default options  	*/
#define BARCOLOR "\033[0;34;44m"	/* bar foreground color	*/
#define BBARCOLOR "\033[0;37;47m"	/* bar background color	*/
#define COLOR1 "\033[1;34m"		/* separators color	*/
#define COLOR2 "\033[1;36m"		/* text and other stuff	*/
#define EMPTY "\033[0;0;0m"		/* white, don't change	*/
#define BARSIZE 30 			/* bar size 		*/

int help(int i, char *prognm) {
    if (i == 0) {
	printf ("Copyright (c) 2002 by arj_\n");
	printf ("version 0.3.0 release\n");
	printf ("%s [-admpts] [-b size] [--help] [--version]\n\n", prognm);
	printf ("  -a, --available\t show available space\n");
	printf ("  -d, --disk-usage\t show disk usage (used/total) \n");
	printf ("  -m, --mount-point\t show mount point\n");
	printf ("  -p, --percentage\t show used percentage\n");
	printf ("  -t, --system-type\t show fsystem type\n");
	printf ("  -s, --short-device\t short device names\n");
	printf ("  -b, --bar-size=size\t set bar size\n");
	printf ("  -h, --help\t\t this help screen\n");
	printf ("  -v, --version\t\t version and author informations\n\n");
	printf ("Bugs, sugestions: arj@arj.wox.org\n");
    }
    if (i == 1) printf("adf version 0.3.0 by arj_\n");
    if (i == 2) fprintf(stderr, "Try `%s --help' for more information.\n", prognm), exit(0);
    if (i == 3) fprintf(stderr, "%s: bar size must be a number\n", prognm);
    if (i == 4) fprintf(stderr, "%s: bar size must be grater or equal to 15\n", prognm);
    exit(0);
}

char *hu(float i) {
    char *sufix;
    char *retval; retval = (char *)malloc(1024);
    
    sufix = (char *)malloc(2); sufix = "K";
    if (i >= 1024) i = i/1024, sufix = "M";
    if (i >= 1024) i = i/1024, sufix = "G";
    
    if (!strncmp("G", sufix, 1)) sprintf (retval, "%.1f%s", i, sufix);
    else sprintf (retval, "%.0f%s", i, sufix);
    
    return retval;
}

char *graph(int full, int empty) {
	char *retval; int i; 
	retval = malloc(1024);

	sprintf(retval, BARCOLOR); 
	for (i=0;i<full;i++) sprintf(retval, "%s#", retval);
	sprintf(retval, "%s%s", retval, BBARCOLOR);
	for (i=0;i<empty;i++) sprintf(retval, "%s.", retval);
	sprintf(retval, "%s%s", retval, EMPTY);

	return retval;
}
	
static struct option const long_options[] =
{
  {"available", 0, NULL, 'a'},
  {"disk-usage", 0, NULL, 'd'},
  {"mount-point", 0, NULL, 'm'},
  {"percentage", 0, NULL, 'p'},
  {"system-type", 0, NULL, 't'},
  {"short-device", 0, NULL, 's'},
  {"bar-size", required_argument, NULL, 'b'},
  {"help", 0, NULL, 'H'},
  {"version", 0, NULL, 'V'},
  {NULL, 0, NULL, 0}
};
 
int main(int argc, char *argv[]) {
    FILE *fp;
    struct statfs st; struct mntent *mnt;
    char *device, *mountpoint, *type, *options, ch;
    double total=0, used=0, avail=0, percent=0; 
    int bsize, full, empty, c;
    float blsize, blocks, free;
    char *aoptions = "admtpsb:";

    device = (char *)malloc(1024);
    mountpoint = (char *)malloc(1024);
    type = (char *)malloc(1024);
    options = (char *)malloc(1024);

    argv[0] = basename(argv[0]); bsize = BARSIZE;
    if (argc < 2) sprintf(options, "%s", DEF_OPTIONS);

    while((c = getopt_long(argc, argv, aoptions, long_options, NULL)) != -1) {
	switch (c) {
            case 0:
                break;
            case 'a':
                sprintf(options, "%sa", options);
                break;
            case 'd':
                sprintf(options, "%sd", options);
                break;
            case 'm':
                sprintf(options, "%sm", options);
                break;
            case 'p':
                sprintf(options, "%sp", options);
                break;
            case 't':
                sprintf(options, "%st", options);
                break;
            case 's':
                sprintf(options, "%ss", options);
                break;
	    case 'b':
		ch = optarg[0];
		if (isdigit(ch)) {
			if (atoi(optarg) < 15) help(4, argv[0]);
			else bsize = atoi(optarg);
		} else help(3, argv[0]); break;
	    case 'H':
		help(0, argv[0]);
		break;
	    case 'V':
		help(1, argv[0]);
		break;
	    default:
		help(2, argv[0]);
		break;
	}
    }

    if (optind != argc) help(2, argv[0]);

    if (!options[0]) sprintf(options, "%s", DEF_OPTIONS);
    if (strlen(options) == 1 && !strncmp("s", options, 1)) sprintf(options, "s%s", DEF_OPTIONS);

    if (bsize > 999) bsize = 999; /* We don't want a SEGV */

    if((fp = setmntent("/etc/mtab", "r"))==NULL)
    {perror("open /etc/mtab"); exit(0);}

    while((mnt = getmntent(fp))) {
        statfs(mnt->mnt_dir, &st);
        if (st.f_blocks > 0) {
	    device = mnt->mnt_fsname;
	    mountpoint = mnt->mnt_dir;
	    type = mnt->mnt_type;

            blocks = st.f_blocks;
            blsize = st.f_bsize;
	    avail = st.f_bavail;
	    free = st.f_bfree;
		
	    percent = (blocks-free);
	    percent = (percent * 100)/(percent+avail);

            total = (blocks*blsize)/1024;
	    used = total-(free*blsize)/1024;
	    avail = (avail*blsize)/1024;

    	    full = (percent * bsize)/100;
    	    empty = bsize-full;

	    if (strstr(options, "s")) device = basename(device);
	    printf("%s%s%s:%s\t%s ", COLOR2, device, COLOR1, EMPTY, graph(full, empty)); 
	    if (strstr(options, "p")) {
	        if (percent < 10) printf("%s(%s%.0f%s  %s)%s ", COLOR1, COLOR2, percent, "%", COLOR1, EMPTY);
		if (percent < 100) printf("%s(%s%.0f%s %s)%s ", COLOR1, COLOR2, percent, "%", COLOR1, EMPTY);
		if (percent == 100) printf("%s(%s%.0f%s%s)%s ", COLOR1, COLOR2, percent, "%", COLOR1, EMPTY);
	    }
	    if (strstr(options, "d")) printf("%s(%s%s%s/%s%s%s)%s ", COLOR1, COLOR2, hu(used), COLOR1, COLOR2, hu(total), COLOR1, EMPTY);
	    if (strstr(options, "a")) printf("%s(%s%s%s)%s ", COLOR1, COLOR2, hu(avail), COLOR1, EMPTY);
	    if (strstr(options, "m")) printf("%s(%s%s%s)%s ", COLOR1, COLOR2, mountpoint, COLOR1, EMPTY);
	    if (strstr(options, "t")) printf("%s(%s%s%s)%s", COLOR1, COLOR2, type, COLOR1, EMPTY);
	    printf("\n");

	}
    }

    return 1;
    
}
