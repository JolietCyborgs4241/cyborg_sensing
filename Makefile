#	top level Makefile for cyborg_vision

all:
	cd cv_db ; make all
	cd doc ; make all
	cd test ; make all

clean:
	cd cv_db ; make clean
	cd doc ; make clean
	cd test ; make clean

