RM := rm -f

GCC := gcc
GCCFLAGS := -Wall -std=c99 -g -lrt -pthread
SOURCES_MAIN := slave.c utils.c
SOURCES_VIEW := utils.c

all:app view

app: main.c
	$(GCC) main.c $(GCCFLAGS) $(SOURCES_MAIN) -o md5

view: view.c
	$(GCC) view.c $(GCCFLAGS) $(SOURCES_VIEW) -o view

clean:
	$(RM)	md5
	$(RM)	view
	$(RM)	resultado.txt