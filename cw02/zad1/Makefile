CFLAGS += -Wall -g
CC = gcc
DATA_FILE = datafile
TEST_FILE = testfile
RESULT_FILE = wyniki.txt
TIME_S = time -f "real %e user %U sys %S" -a -o $(RESULT_FILE)

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

clean:
	-rm -rf *.o main $(DATA_FILE) $(TEST_FILE)

generate:
	./main generate $(DATA_FILE) $(WIDTH) $(NUMBER)

test_sort_sys:
	cp $(DATA_FILE) $(TEST_FILE)
	echo "\nsort sys" >> $(RESULT_FILE)
	$(TIME_S) ./main sort sys $(TEST_FILE) $(WIDTH) $(NUMBER) 

test_sort_lib:
	cp $(DATA_FILE) $(TEST_FILE)
	echo "\nsort lib" >> $(RESULT_FILE)
	$(TIME_S) ./main sort lib $(TEST_FILE) $(WIDTH) $(NUMBER)

test_shuffle_sys:
	echo "\nshuffle sys" >> $(RESULT_FILE)
	$(TIME_S) ./main shuffle sys $(TEST_FILE) $(WIDTH) $(NUMBER)

test_shuffle_lib:
	echo "\nshuffle lib" >> $(RESULT_FILE)
	$(TIME_S) ./main shuffle lib $(TEST_FILE) $(WIDTH) $(NUMBER)

test: main
	echo "\nwidth="$(WIDTH), "number="$(NUMBER) >> $(RESULT_FILE)	
	$(MAKE) generate WIDTH=$(WIDTH) NUMBER=$(NUMBER)
	$(MAKE) test_sort_sys WIDTH=$(WIDTH) NUMBER=$(NUMBER)
	$(MAKE) test_sort_lib WIDTH=$(WIDTH) NUMBER=$(NUMBER)
	$(MAKE) test_shuffle_sys WIDTH=$(WIDTH) NUMBER=$(NUMBER)
	$(MAKE) test_shuffle_lib WIDTH=$(WIDTH) NUMBER=$(NUMBER)

test_all:
	echo "" > $(RESULT_FILE)
	$(MAKE) test WIDTH=4 NUMBER=4000
	$(MAKE) test WIDTH=4 NUMBER=8000
	$(MAKE) test WIDTH=512 NUMBER=4000
	$(MAKE) test WIDTH=512 NUMBER=8000
	$(MAKE) test WIDTH=4096 NUMBER=4000
	$(MAKE) test WIDTH=4096 NUMBER=8000
	$(MAKE) test WIDTH=8192 NUMBER=4000
	$(MAKE) test WIDTH=8192 NUMBER=8000

