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
#include <iostream>       // added by Dahi
using namespace std;     // added by Dahi
#include <vector>       //added by Dahi
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
  cimg_usage(std::string("stepper program of the Laboratory of Mechanics in Lille (LML) is intended to make translation displacement using a stepping motor, \
it uses different GNU libraries (see --info option)\n\n \
usage: ./scan -h -I\n \
       ./scan --grab-device-path off_line -o ~/dataseb/AFDAR/cameraMTF/focus/image_x%02d_y%02d_z%02d_i%03d.jpg -n 3 -nx 1 -ny 1 -nz 11\n \
version: "+std::string(VERSION)+"\t(other library versions: RS232."+std::string(RS232_VERSION)+", stepper."+std::string(STEPPER_VERSION)+", grab."+std::string(GRAB_VERSION)+")\n compilation date: " \
            ).c_str());//cimg_usage
 ///information and help
  const bool show_h   =cimg_option("-h",    false,NULL);//-h hidden option
        bool show_help=cimg_option("--help",show_h,"help (or -h option)");show_help=show_h|show_help;//same --help or -h option
  bool show_info=cimg_option("-I",false,NULL);//-I hidden option
  if( cimg_option("--info",show_info,"show compilation options (or -I option)") ) {show_info=true;cimg_library::cimg::info();}//same --info or -I option
 ///device camera
  const std::string CameraDeviceType=cimg_option("--grab-device-type","grab_WGet","type of grab device (e.g. ArduinoTTL or grab_WGet or Elphel_OpenCV or Elphel_rtsp or grab_image_file).");
  const std::string CameraDevicePath=cimg_option("--grab-device-path","192.168.0.9","path of grab device.");
  ///image
  const int ImageNumber=cimg_option("-n",10,"number of images to acquire.");
  const unsigned int mechanical_jitter=cimg_option("--jitter",10,"set mechanical jitter to go back to origin for scanning mode only (i.e. reset position with same mechanical direction, could not be negative).");
  const std::string ImagePath=cimg_option("-o","./image_x%02d_y%02d_z%02d_i%03d.jpg","path for image(s).");
  const std::string  DataPath=cimg_option("-O","./meanFlagNFail.cimg","path for extracted data file (i.e. mean images, flag and fail).");
 ///device stepper
//! \bug [copy] need to do again stepperNreader for device command line options.
  const std::string StepperDeviceType=cimg_option("--stepper-device-type","uControlXYZ","Type of stepper device");
  const std::string StepperDevicePath=cimg_option("--stepper-device-path","/dev/ttyUSB0","Path of stepper device");
  const std::string StepperDeviceSerialType=cimg_option("--stepper-device-serial-type","serial_system","Type of serial device for stepper (i.e. serial_termios or serial_system)");
  ///displacement
  cimg_library::CImg<int> step(3);step.fill(0);
  {
  const int step_x=cimg_option("-sx",10,"displacement step along X axis (e.g. -10 steps to go backward, i.e. displacement can be positive or negative).");
  step(0)=step_x;
const int step_y=cimg_option("-sy",10,"displacement step along Y axis.");
  step(1)=step_y;
const int step_z=cimg_option("-sz",1,"displacement step along Z axis.");
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
  cimg_library::CImg<int> number(3);number.fill(1);  // it was fill(1) for x and y
  {
  const int number_x=cimg_option("-nx",10,"number of displacement along X axis.");
  number(0)=number_x;
  const int number_y=cimg_option("-ny",10,"number of displacement along Y axis.");
  number(1)=number_y;
  const int number_z=cimg_option("-nz",1,"number of displacement along z axis.");
  number(2)=number_z;
  }
#if cimg_display>0
  const bool do_display=cimg_option("-X",false,"activate GUI (i.e. progress display during scan mode only; set --scan true option).");
  const unsigned int zoom=cimg_option("--GUI-progress-zoom",100,"GUI progress display zoom.");
#endif
  ///wait time between steps
  const int wait_time=cimg_option("--wait-time",1000,"wait time between steps in ms.");
  ///stop if help requested
  if(show_help) {/*print_help(std::cerr);*/return 0;}

//scan device object
  Cscan scan;

//grab device object
  Cgrab_factory grab_factory;
  Cgrab *pGrab=grab_factory.create(CameraDeviceType);
//open
  if(!pGrab->open(CameraDevicePath)) return 1;
//get
  cimg_library::CImg<int> image;
//! \todo [high] need to do again initialisation of image for its sizes and for maximum position in image.
{//[remove] static due to loss
std::string file;
scan.image_file_name(file,ImagePath,0,0,0,0);
if(!pGrab->grab(image,file)) return 1;
}//[remove] static due to loss

//stepper device object
//! \todo [high] need stepper factory
  Cstepper stepper;
// OPEN 
  if(!stepper.open(StepperDevicePath,StepperDeviceSerialType)) return 1;

//next scanning object
  Cdata4scan<float,int> data4scan;//mean,flag,fail (e.g. map)
#if version_cimg < 130
  data4scan.initialise(image.width  ,image.height  ,number(0),number(1),number(2));
#else
  data4scan.initialise(image.width(),image.height(),number(0),number(1),number(2));
#endif
  scan.scanning(stepper,number,step,velocity,wait_time,mechanical_jitter,
    *pGrab,image,ImagePath,ImageNumber,data4scan
#if cimg_display>0
    ,zoom,do_display
#endif //cimg_display
    );//scanning

  data4scan.save(DataPath);//save processed data (e.g. mean images), flag (i.e. satisfied (or not) map) and fail (i.e. number of failing map).
data4scan.print("mean");
data4scan.flag.print("flag");
data4scan.fail.print("fail");

//CLOSE
  stepper.close();
  pGrab->close();
  return 0;
}//main



