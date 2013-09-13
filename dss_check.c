/* check if dss is running ok, if not, restart services */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#define TIME_CHECK 20

void control_dss (char ask[]);
void check_dss (int cams);
void get_cfg (void);
void daemonize (int cam_n);

int main (void)
{
	/* the simple main, only call get_cfg and cry */
	get_cfg();
	exit (EXIT_SUCCESS);
}

void control_dss (char ask[])
{
	/* this function call rc.dss3 with argument restart */
	const char command[] = "/etc/rc.d/rc.dss3 start";

	if (ask == "restart"){
		printf("DSS sera reiniciado\n");
		if (system (command) == -1)
			exit (EXIT_FAILURE);
	}
}

void check_dss (int cams)
{
	/* this function get current date from cam%d.jpg file and check 
	 * if it`s a recent created jpeg */
	unsigned int i, dev, erru;
	char file[40]; 
	struct stat buf;
	struct timeval tv;

	while (1) {

	erru = 0;

	if (gettimeofday(&tv, NULL) == -1)
		exit (EXIT_FAILURE);

	for (i=1; i<=cams; i+=4) {
		memset (&file, 0, sizeof (file));
		snprintf (file, sizeof (file), "/usr/local/dss/imagens/cam%d.jpg", i);

		if (! stat (file,&buf)) {

			if (i == 1) dev = 0;
			else if (i == 5) dev = 1;
			else if (i == 9) dev = 2;
			else dev = 3;
			
			if ((tv.tv_sec - buf.st_mtime) > 5){
				printf ("DSS[%d] ERROR. ", dev);
				erru = 1; 
			} else printf ("DSS[%d] OK. ", dev);

			printf ("\n");
		}
	}

	/* if erru is set to 1, restart all DSS processes */
	if (erru == 1)
		control_dss ("restart");

	/* time to sleep */
	sleep (TIME_CHECK); 
	}
}

void get_cfg (void)
{
	const char conf[] = "/usr/local/dss/etc/cam.conf";
	char buffer[4];
	int number;
	FILE *fp;

	if ((fp = fopen (conf, "r")) == NULL)
		exit (EXIT_FAILURE);

	/* get the value from conf file */
	while (fgets (buffer, sizeof (number), fp) != NULL);

	fclose (fp);

	/* call daemonize with the number of cameras to check */	
	daemonize (number = atoi (buffer));
}

void daemonize (int cam_n)
{
	int d;

	/* fork */
	if ((d = daemon (0, 0)) != 0)
		exit (EXIT_FAILURE);

	/* now call check_dss with the number off cameras */
	check_dss (cam_n);
}
