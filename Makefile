PROGRAMS = version lavision scan  
DOCUMENTATIONS = doc

version = v0.2.3
version_cimg = 125
#version_cimg = 149
version_scan=${version}
version_stepper=`cat ../stepper/VERSION`
version_rs232=`cat ../rs232/VERSION`
version_grab=`cat ../grab/VERSION`
version_data=`cat ../convert/VERSION`
OPT_LIBRARY = -DVERSION=\"$(version)\" -DSCAN_VERSION=\"$(version_scan)\" -DDATA_VERSION=\"$(version_data)\" -DSTEPPER_VERSION=\"$(version_stepper)\"  -DGRAB_VERSION=\"$(version_grab)\"  -DRS232_VERSION=\"$(version_rs232)\" 
OPT_FORMAT = -Dcimg_use_jpeg -ljpeg -Dcimg_use_png -lpng -lz -Dcimg_use_tiff -ltiff -Dcimg_use_lavision -D_LINUX
OPT_NETCDF = -Dcimg_use_netcdf -I../NetCDF/include/ -lnetcdf_c++ -L../NetCDF/lib/ -lnetcdf -I/usr/include/netcdf-3/
OPT_XWINDOWS = -I/usr/X11R6/include -Dcimg_use_xshm -L/usr/X11R6/lib -lpthread -lX11 -lXext
OPT = -Dcimg_display=0 -Dcimg_debug=2 $(OPT_LIBRARY) $(OPT_FORMAT) -Dversion_cimg=$(version_cimg) $(OPT_NETCDF)
##CImg.v1.2.5
#OPT = $(OPT_XWINDOWS) -Dcimg_display=1 -Dcimg_debug=2 $(OPT_LIBRARY) $(OPT_FORMAT) -Dversion_cimg=$(version_cimg) $(OPT_NETCDF)
##CImg.v1.4.9
#OPT = $(OPT_XWINDOWS) -Dcimg_debug=2 -Dcimg_use_vt100 $(OPT_LIBRARY) -Dversion_cimg=$(version_cimg)

CC = gcc
CPP = g++

all: $(PROGRAMS) $(DOCUMENTATIONS)

prog:$(PROGRAMS)

version: Makefile
	echo ${version} > VERSION
	cd ..; rm CImg; ln -s CImg-1.2.5 CImg; cd ./scan/
#	cd ..; rm CImg; ln -s CImg-1.4.9 CImg; cd ./scan/

lavision: Makefile
	cd ../LaVision/
#	make
	cd ../scan/
	cp -p ../LaVision/*.o ../LaVision/linux/*.o ../scan/obj/
	rm ../scan/obj/Read_Examples.o

scan: scan.cpp Makefile scan.h data4scan.h ../rs232/serial.h ../stepper/stepper.h ../grab/grab.h ./obj/ReadIM7.o
	$(CPP) $(OPT) $@.cpp ./obj/*.o -o $@

doc: scan.Doxygen Makefile scan.h data4scan.h scan.cpp
	./doxIt.sh

clean:
	rm -rf $(DOCUMENTATIONS)/*
	rm -f *.o
	rm -rf .libs
	@list='$(PROGRAMS)'; for p in $$list; do \
	  rm -f $$p ; \
	done
