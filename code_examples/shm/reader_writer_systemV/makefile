# Makefile for the reader-writer program.
# Author: Naga Kandasamy
# Date: 7/10/2015
# -------------------------------

all: svshm_xfr_writer svshm_xfr_reader 

svshm_xfr_writer: svshm_xfr_writer.o binary_sems.o
	gcc svshm_xfr_writer.o binary_sems.o -o svshm_xfr_writer

svshm_xfr_reader: svshm_xfr_reader.o binary_sems.o
	gcc svshm_xfr_reader.o binary_sems.o -o svshm_xfr_reader

svshm_xfr_writer.o: svshm_xfr_writer.c
	gcc -c svshm_xfr_writer.c -std=c99 -Wall

svshm_xfr_reader.o: svshm_xfr_reader.c
	gcc -c svshm_xfr_reader.c -std=c99 -Wall

binary_sems.o: binary_sems.c
	gcc -c binary_sems.c -std=c99 -Wall

clean:
	rm svshm_xfr_writer svshm_xfr_writer.o binary_sems.o 
