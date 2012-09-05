//scan
/*-----------------------------------------------------------------------
  File        : scan.cpp
  Description : scanning with stepper motor for translation displacement program of the Laboratory of Mechanics in Lille (LML)
  Authors     : Sebastien COUDERT
-----------------------------------------------------------------------*/

/*Additional documentation for the generation of the reference page (using doxygen)*/
/** \mainpage
 *
 *  index:
 *  \li \ref sectionCommandLine
 *  \li \ref sectionScanDocumentation
 *  \li \ref sectionDoxygenSyntax
 *
 *  \section sectionCommandLine command line options
 *
 *  \verbinclude "scan.help.output"
 *  
 *  \section sectionScanDocumentation documentation outline
 *  This is the reference documentation of <a href="http://www.meol.cnrs.fr/">serial</a>, from the <a href="http://www.univ-lille1.fr/lml/">LML</a>.\n\n
 *  stepper software. The main function is in <a href="scan_8cpp.html">scan.cpp</a> source file.\n\n
 *  This documentation has been automatically generated from the sources, 
 *  using the tool <a href="http://www.doxygen.org">doxygen</a>. It should be readed as HTML, LaTex and man page.\n
 *  It contains both
 *  \li a detailed description of all classes and functions
 *  \li TODO: a user guide (cf. \ref pages.html "related pages")
 *
 *  that as been documented in the sources.
 *
 *  \par Additional needed libraries:
 *
 *  \li <a href="http://cimg.sourceforge.net">the CImg Library</a> (v1.?.?) for command line options only.
 *
 *  \section sectionDoxygenSyntax make documentation using Doxygen syntax
 *  Each function in the source code should be commented using \b doxygen \b syntax in the same file.
 *  The documentation need to be written before the function.
 *  The basic syntax is presented in this part.
 *  \verbinclude "doxygen.example1.txt"
 *
 *  Two kind of comments are needed for \b declaration and \b explanation \b parts of the function:
 *  Standart documentation should the following (\b sample of code documentation):
 *  \verbinclude "doxygen.example2.txt"
 *
 *  In both declaration and explanation part, \b writting and \b highlithing syntax can be the following:\n\n
 *  \li \c \\code to get\n
 *
 *  \li \c \\n    a new line
 *  \li \c \\li   a list (dot list)
 *
 *  \li \c \\b    bold style
 *  \li \c \\c    code style
 *  \li \c \\e    enhanced style (italic)
 *
 *  For making \b shortcut please use:\n
 *  \li \c \\see to make a shortcut to a related function or variable
 *  \li \c \\link to make a shortcut to a file or a function
 *  \note this keyword needs to be closed using \c \\end*
 *
 *  \li \c \\todo to add a thing to do in the list of <a href="todo.html">ToDo</a> for the whole program
 *
 *  In explanation part, \b paragraph style can be the following:\n
 *  \li \c \\code for an example of the function use
 *  \li \c \\note to add a few notes
 *  \li \c \\attention for SOMETHING NOT FULLY DEFINED YET
 *  \li \c \\warning to give a few warning on the function
 *  \note these keywords need to be closed using \c \\end*
 *
 *  \verbinclude "doxygen.example3.txt"
 *
 *  Many other keywords are defined, so please read the documentation of <a href="http://www.doxygen.org/commands.html">doxygen</a>.
 *
**/

//! \todo serial_put doxygen user page

//standard library
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
///////////////////////////////////////////  
//CImg Library
///data, stepper, grab
#include "scan.h"

//! main function
/**
 *
**/
int main(int argc, char *argv[])
{ 
//commmand line options
 ///usage
  cimg_usage(std::string("scan program of the Laboratory of Mechanics in Lille (LML) is intended to make translation displacement using a stepping motor and grab images for each position, \
it uses different GNU libraries (see --info option)\n\n \
usage: ./scan -h -I\n \
       ./scan --stepper-device-type fake --grab-device-type grab_image_file -o image_z%02d_y%02d_x%02d_i%03d.cimg -n 3 -nx 1 -ny 1 -nz 11\n \
       ./scan --stepper-device-type fake --grab-device-type grab_image_file -o image_x%02d_y%02d_z%02d_i%03d.cimg --output-xyzi true -n 3 -nx 1 -ny 1 -nz 11\n \
       ./scan -o ./scan/image_z%02d_y%02d_x%02d_i%04d.cimg -t ./image/B%05d.im7 --grab-device-type grab_AandDEE_serial --grab-device-path /dev/ttyACM0 -X false -n 100 -nx 50 -ny 50 -O ./mean/image_z%02d_y%02d_x%02d.mean.cimg --stepper-device-type uControlXYZnReader --stepper-device-path=/dev/ttyUSB0 -sx 1 -sy 1\n \
version: "+std::string(VERSION)+"\t(other library versions: RS232."+std::string(RS232_VERSION)+", stepper."+std::string(STEPPER_VERSION)+", grab."+std::string(GRAB_VERSION)+", data."+std::string(DATA_VERSION)+")\n compilation date: " \
            ).c_str());//cimg_usage
 ///information and help
  cimg_help("  * general options");
  const bool show_h   =cimg_option("-h",    false,NULL);//-h hidden option
        bool show_help=cimg_option("--help",show_h,"help (or -h option)");show_help=show_h|show_help;//same --help or -h option
  bool show_info=cimg_option("-I",false,NULL);//-I hidden option
  if( cimg_option("--info",show_info,"show compilation options (or -I option)") ) {show_info=true;cimg_library::cimg::info();}//same --info or -I option
 ///device camera
  cimg_help("  * grab device (e.g. camera) and image (see also Region Of Interest -ROI-) options");
  const std::string CameraDeviceType=cimg_option("--grab-device-type","grab_WGet","type of grab device (e.g. ArduinoTTL or grab_WGet or Elphel_OpenCV or Elphel_rtsp or grab_image_file).");
  const std::string CameraDevicePath=cimg_option("--grab-device-path","192.168.0.9","path of grab device.");
  ///image
  const int ImageNumber=cimg_option("-n",10,"number of images to acquire.");
  const std::string ImagePath=cimg_option("-o","./image_x%02d_y%02d_z%02d_i%03d.jpg","path for image(s).");
  const bool outputOrder=cimg_option("--xyzi-order",false,"output file name order could be xyzi (default is zyxi ordered).");
  const std::string TemporaryImagePath=cimg_option("-t","image_%05d.imx","temporary path for image(s) (i.e. image_%05d.imx e.g. image_000001.imx).");
  const std::string  DataPath=cimg_option("-O","./meanFlagNFail.cimg","path for extracted data file (i.e. mean images, flag and fail).");
 ///device stepper
  cimg_help("  * stepper device (e.g. translation stage) options");
  const std::string StepperDeviceType=cimg_option("--stepper-device-type","uControlXYZ","Type of stepper device");
  const std::string StepperDevicePath=cimg_option("--stepper-device-path","/dev/ttyUSB0","Path of stepper device");
  const std::string StepperDeviceSerialType=cimg_option("--stepper-device-serial-type","serial_system","Type of serial device for stepper (i.e. serial_termios or serial_system)");
  const std::string StepperReaderDevicePath=cimg_option("--position-reader-device-path","/dev/ttyUSB1","path of position reader device"); 
  const std::string StepperReaderDeviceSerialType=cimg_option("--position-reader-serial-type","serial_system","type of serial device for position reader (i.e. serial_termios or serial_system)"); 
  cimg_help("  * scanning options");
  ///displacement jitter
  const unsigned int jitter=cimg_option("--jitter",10,"set mechanical jitter to go back to origin for scanning mode only (i.e. reset position with same mechanical direction, could not be negative).");
  cimg_library::CImg<unsigned int> mechanical_jitter(3);mechanical_jitter=jitter;
  ///displacement
  cimg_library::CImg<int> step(3);step.fill(0);
  {
  const int step_x=cimg_option("-sx",10,"displacement step along X axis in step (e.g. -10 steps to go backward, i.e. displacement can be positive or negative).");
  step(0)=step_x;
  const int step_y=cimg_option("-sy",10,"displacement step along Y axis in step.");
  step(1)=step_y;
  const int step_z=cimg_option("-sz", 1,"displacement step along Z axis in step.");
  step(2)=step_z;
  }
  ///velocity
  cimg_library::CImg<int> velocity(3);velocity.fill(0);
  {
  const unsigned int velocity_x=cimg_option("-vx",1000,"displacement velocity along X axis (note: absolute  velocity, i.e. could not be negative).");
  velocity(0)=velocity_x;
  const unsigned int velocity_y=cimg_option("-vy",1000,"displacement velocity along Y axis (note: absolute  velocity, i.e. could not be negative).");
  velocity(1)=velocity_y;
  const unsigned int velocity_z=cimg_option("-vz",1000,"displacement velocity along X axis (note: absolute  velocity, i.e. could not be negative).");
  velocity(2)=velocity_z;
  }
  ///number of steps
  cimg_library::CImg<int> number(4);number.fill(1);  // it was fill(1) for x and y
  {
  const int number_x=cimg_option("-nx",10,"number of displacement along X axis.");
  number(0)=number_x;
  const int number_y=cimg_option("-ny",10,"number of displacement along Y axis.");
  number(1)=number_y;
  const int number_z=cimg_option("-nz",1,"number of displacement along z axis.");
  number(2)=number_z;
  }
  number(3)=ImageNumber;
  cimg_help("  ** ROI option 1: automatic setup from full image maximum position and using pixel size and margin");
  ///camera pixel size
  cimg_library::CImg<float> pixel_size(2);pixel_size.fill(6.45);//e-6);//PCO
  {
  const float px=cimg_option("-px",pixel_size(0),"pixel size along x direction in step (pixel size used only if crop coordinates are negative, default).");
  pixel_size(0)=px;
  const float py=cimg_option("-py",pixel_size(1),"pixel size along y direction in step (e.g. if step is 1 um, 2.2 for Elphel or 6.45 for ImageIntense.");
  pixel_size(1)=py;
  }
  ///ROI (crop)
  ////ROI margin
  cimg_library::CImg<int> margin(2);margin.fill(32);
  {
  const int mx=cimg_option("-mx",margin(0),"margin width (margin used only if crop coordinates are negative, default).");
  margin(0)=mx;
  const int my=cimg_option("-my",margin(1),"margin height.");
  margin(1)=my;
  }
  cimg_help("  ** ROI option 2: fixed rectangle");
  ////ROI rectangle
  const int x0=cimg_option("-x0",-1,"crop x0 coordinate in pixel: left   (e.g. -x0 12 or default maximum centering).");
  const int y0=cimg_option("-y0",-1,"crop y0 coordinate in pixel: top    (e.g. -y0 34) range should be [0..height-1].");
  const int x1=cimg_option("-x1",-1,"crop x1 coordinate in pixel: right  (e.g. -x1 56).");
  const int y1=cimg_option("-y1",-1,"crop y1 coordinate in pixel: bottom (e.g. -y1 78).");
  cimg_help("  * others");
  ///wait time between steps
  const int wait_time=cimg_option("--wait-time",1000,"wait time between steps in ms.");
#if cimg_display>0
  const bool do_display=cimg_option("-X",false,"activate GUI (i.e. progress display during scan mode only; set --scan true option).");
  const unsigned int zoom=cimg_option("--GUI-progress-zoom",100,"GUI progress display zoom.");
#endif
  ///stop if help requested
  if(show_help) {/*print_help(std::cerr);*/return 0;}

//scan device object
  Cscan<float,int> scan;
  //init
  scan.initialise(StepperDeviceType,StepperDevicePath,StepperDeviceSerialType,
    StepperReaderDevicePath,StepperReaderDeviceSerialType,mechanical_jitter,
    CameraDeviceType,CameraDevicePath,ImagePath,TemporaryImagePath,
    number,margin,pixel_size,x0,y0,x1,y1,outputOrder);
  //scan
  scan.scanning(number,step,velocity,wait_time,mechanical_jitter,
    ImagePath,ImageNumber,DataPath
#if cimg_display>0
    ,zoom,do_display
#endif //cimg_display
    );//scanning
  //save
  scan.data4scan.save(DataPath);//save processed data (e.g. mean images), flag (i.e. satisfied (or not) map) and fail (i.e. number of failing map).
{//force NetCDF
std::string tmp=DataPath+".nc";
scan.data4scan.save(tmp);
}//force NetCDF
scan.data4scan.print("mean");
scan.data4scan.flag.print("flag");
scan.data4scan.fail.print("fail");
  //close devices
  scan.close();
  return 0;
}//main

