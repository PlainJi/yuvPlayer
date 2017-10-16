#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <semaphore.h>
#include <pthread.h>

int fps = 0;
extern int yuvWidth;
extern int yuvHeight;
extern int yuvSize;
extern int pixFormat;
extern char *yuvdata;
extern int run;
extern sem_t picNum;
extern int read_frame(void);

void *event_loop(void *arg)
{
	SDL_Event event;

	while(run) {
		SDL_WaitEvent(&event);
		switch(event.key.keysym.sym)
		{
	                case SDLK_q:
        	                run = 0;
                	        break;
			default:
				break;
		}
	}
}

void *thread_show(void *arg) {
  // SDL init
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_RendererInfo info;
  SDL_Rect rect;
  SDL_Texture *texture;
  SDL_Event event;
  int cnt = 0;
  time_t cur = 0;
  time_t last = 0;
  char title[128];

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    return (void*)2;
  }

  window = SDL_CreateWindow("YUV speed test", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, yuvWidth, yuvHeight,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    fprintf(stderr, "Couldn't set create window: %s\n", SDL_GetError());
    return (void*)5;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "Couldn't set create renderer: %s\n", SDL_GetError());
    return (void*)6;
  }
  SDL_GetRendererInfo(renderer, &info);
  printf("Using %s rendering\n", info.name);

  texture = SDL_CreateTexture(renderer, pixFormat, SDL_TEXTUREACCESS_STREAMING, yuvWidth, yuvHeight);
  if (!texture) {
    fprintf(stderr, "Couldn't set create texture: %s\n", SDL_GetError());
    return (void*)7;
  }

  pthread_t a;
  pthread_create(&a, NULL, event_loop, NULL);

  while (run) {
    sem_wait(&picNum);
    
    fps++;
    cur = time(NULL);
    if (last != cur ) {
      last = cur;
      sprintf(title, "U3V Camera     FPS:%d\n", fps);
      fps = 0;
      SDL_SetWindowTitle(window, title);
    }
    SDL_UpdateTexture(texture, NULL, yuvdata, yuvWidth * 2);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return (void*)0;
}



