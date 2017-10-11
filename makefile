src := showyuv.c yuvPlayer.c
obj := $(src:.c=.o)
LIB := -lpthread -lSDL2

all: yuvPlayer

yuvPlayer: $(obj)
	gcc $^ -o $@ $(LIB)

$obj: $(src)
	gcc $< -o $@

.PHONY: clean
clean:
	rm -rf *.o yuvPlayer
