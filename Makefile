PROGRAMS = scan  
DOCUMENTATIONS = doc

version = v0.1.3
version_stepper=`cat ../stepper/VERSION`
version_rs232=`cat ../rs232/VERSION`
version_grab=`cat ../grab/VERSION`
#OPT = -Dcimg_display=0 -Dcimg_debug=2 -Dcimg_use_vt100 -DVERSION=\"$(version)\"
OPT =                  -Dcimg_debug=2 -Dcimg_use_vt100 -DVERSION=\"$(version)\" -DSTEPPER_VERSION=\"$(version_stepper)\"  -DGRAB_VERSION=\"$(version_grab)\"  -DRS232_VERSION=\"$(version_rs232)\" -I/usr/X11R6/include -Dcimg_use_xshm -L/usr/X11R6/lib -lpthread -lX11 -lXext
CC = gcc
CPP = g++

all: $(PROGRAMS) $(DOCUMENTATIONS)

prog:$(PROGRAMS)

scan: scan.cpp Makefile ../stepper/stepper.h  ../grab/grab.h
	$(CPP) $(OPT) scan.cpp -o $@

doc: scan.Doxygen Makefile scan.cpp
	echo ${version} > VERSION
	./doxIt.sh

clean:
	rm -rf $(DOCUMENTATIONS)/*
	rm -f *.o
	rm -rf .libs
	@list='$(PROGRAMS)'; for p in $$list; do \
	  rm -f $$p ; \
	done
