#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <SDL2/SDL_pixels.h>

int cam_fd;
void *z_sock;
sem_t picNum;
char *fileName = NULL;
int yuvWidth = 0;
int yuvHeight = 0;
int yuvSize = 0;
char *yuvFormat = NULL;
int pixFormat = 0;
char *yuvdata = NULL;
int run=1;
extern void *thread_show(void *);

int main(int argc, char *argv[]) 
{
	pthread_t a;
	int ret = 0;
	int fd = 0;

	if(argc != 5) {
	    printf("Usage: \n"
		   "\t%s file yuvWidth yuvHeight format[yuy2|uyvy]\n"
		   "\te.g.: %s inputfile.422 1280 720 yuy2\n", argv[0], argv[0]);
	    return -1;
	  }

	// init
	fileName = argv[1];
	yuvWidth = atoi(argv[2]);
	yuvHeight = atoi(argv[3]);
	yuvFormat = argv[4];
	printf("FILENAME:\t%s\n"
		"WIDTH:\t%d\n"
		"HEIGHT:\t%d\n"
		"FORMAT:\t%s\n",
		fileName, yuvWidth, yuvHeight, yuvFormat);
	if (!strcmp(yuvFormat, "yuy2")) {
		yuvSize = yuvWidth * yuvHeight * 2;
		pixFormat = SDL_PIXELFORMAT_YUY2;
	} else if(!strcmp(yuvFormat, "uyvy")) {
		yuvSize = yuvWidth * yuvHeight * 2;
		pixFormat = SDL_PIXELFORMAT_UYVY;
	} else {
		printf("pix format %s not support currently!\n", yuvFormat);
		return -1;
	}
	assert(NULL != (yuvdata=(char*)malloc(yuvSize)));

	fd = open(fileName, O_RDONLY);
	assert(fd>0);

	sem_init(&picNum, 0, 0);
	pthread_create(&a, NULL, thread_show, NULL);
	while(run) {
		if (read(fd, yuvdata, yuvSize) <=0)
			break;
        	sem_post(&picNum);
		usleep(32*1000);
	}
	close(fd);
	free(yuvdata);

	return 0;
}


