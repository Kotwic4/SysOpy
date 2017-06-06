CFLAGS += -Wall -g
CFLAGS += $(OPT)
CC = gcc

LIBDIR = ../zad1
MAINDIR = ../zad2
MAIN = $(MAINDIR)/main.c

all: main_sta main_sha main_dyn

main_sta: $(MAIN) $(LIBDIR)/libbook.a
	$(CC) $(CFLAGS) -static -DSTALAB=1 -o $@ $(MAIN) -L$(LIBDIR) -lbook

main_sha: $(MAIN) $(LIBDIR)/libbook.so
	$(CC) $(CFLAGS) -DSHALAB=1 -o $@ $(MAIN) -L$(LIBDIR) -Wl,-rpath=$(LIBDIR) -lbook

main_dyn: $(MAIN) $(LIBDIR)/libbook.so
	$(CC) $(CFLAGS) -DDYNLAB=1  -o $@ $(MAIN) -ldl

$(LIBDIR)/libbook.a:
	cd $(LIBDIR) && $(MAKE) libbook.a CFLAGS=$(OPT)

$(LIBDIR)/libbook.so:
	cd $(LIBDIR) && $(MAKE) libbook.so CFLAGS=$(OPT)

clean:
	-rm -rf *.o main_sta main_sha main_dyn

test:
	echo $(OPT) >> result.txt && ./main_sta < $(MAINDIR)/dane.in >> result.txt && ./main_sha < $(MAINDIR)/dane.in >> result.txt && ./main_dyn < $(MAINDIR)/dane.in >> result.txt
 

run:
	$(MAKE) clean_all && $(MAKE) all OPT=$(OPT) && $(MAKE) test OPT=$(OPT)


clean_all:
	$(MAKE) clean
	cd $(LIBDIR) && $(MAKE) clean
