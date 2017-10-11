#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>

int cam_fd;
void *z_sock;
sem_t picNum;
char yuvdata[1280*720*2];
int run=1;
extern void *thread_show(void *);

int main(int argc, char *argv[]) 
{
	pthread_t a;
	int ret = 0;
	int fd = 0;

	if (argc!=2) {
		printf("Usage: %s file.422\n", argv[0]);
		return -1;
	}
	fd = open(argv[1], O_RDONLY);
	assert(fd>0);

	sem_init(&picNum, 0, 0);
	pthread_create(&a, NULL, thread_show, NULL);
	while(run) {
		if (read(fd, yuvdata, 1280*720*2) <=0)
			break;
        	sem_post(&picNum);
		usleep(32*1000);
	}
	close(fd);
	return 0;
}
