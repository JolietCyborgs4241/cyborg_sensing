#   Makefle for test data and tools

DATA_FILES = ball.data square.data stuff.data thing.data
SCRIPTS = prep_data run_tests
CAMERA_DATA = camera_records.out

HOST_IP=127.0.0.1
HOST_PORT=12345


all:	$(SCRIPTS) $(CAMERA_DATA) gettimeofday

prep_data:	prep_data.sh
	cp prep_data.sh  prep_data
	chmod +x prep_data

run_tests:	run_tests.sh
	cp run_tests.sh run_tests
	chmod +x run_tests

$(CAMERA_DATA): $(DATA_FILES)
	./prep_data

gettimeofday:   gettimeofday.c

test:	all
	./run_tests $(HOST_IP) $(HOST_PORT)

clean:
	-rm -f gettimeofday $(CAMERA_DATA) $(SCRIPTS)
