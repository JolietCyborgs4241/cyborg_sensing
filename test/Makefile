#   Makefle for test data and tools

DATA_FILES = ball.data square.data stuff.data thing.data
CAMERA_DATA = camera_records.out

HOST_IP=127.0.0.1
HOST_PORT=12345


all:	$(CAMERA_DATA) gettimeofday

exec:
	chmod +x run_tests
	chmod +x prep_data

$(CAMERA_DATA): $(DATA_FILES)
	./prep_data

gettimeofday:   gettimeofday.c

test:	all
	./run_tests $(HOST_IP) $(HOST_PORT)

clean:
	-rm gettimeofday $(CAMERA_DATA)
