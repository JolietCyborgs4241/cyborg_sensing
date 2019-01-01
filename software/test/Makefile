#   Makefle for test data and tools

DATA_FILES  = cam_ball.data cam_square.data cam_stuff.data \
			  cam_thing.data magnetic.data gforce.data ranger.data
SCRIPTS     = prep_data run_posts
SENSOR_DATA = sensor_records.out

HOST_IP=127.0.0.1
HOST_PORT=12345


all:	$(SCRIPTS) $(SENSOR_DATA) gettimeofday
	make $(SCRIPTS)
	make $(SENSOR_DATA)
	make gettimeofday
	cd status_bar ; make all

prep_data:	prep_data.sh
	cp prep_data.sh  prep_data
	chmod +x prep_data

run_posts:	run_posts.sh
	cp run_posts.sh run_posts
	chmod +x run_posts

$(SENSOR_DATA): $(DATA_FILES)
	./prep_data

gettimeofday:   gettimeofday.c

test:	all
	./run_tests $(HOST_IP) $(HOST_PORT)

clean:
	-rm -f gettimeofday $(SENSOR_DATA) $(SCRIPTS)
	cd status_bar ; make clean
