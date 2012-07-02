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

//! set numbered image file name from file path format
/**
 * 
 * \param [out] file_name: image file name with numbers from i,j,k,l (e.g. ./img_x00_y00_z00_i000.png)
 * \param [in] file_path_format: format for the image file name (e.g. ./img_x%02d_y%02d_z%02d_i%03d.png)
 * \param [in] i,j,k,l: dimension index for current file name (e.g. 0,0,0,0)
**/
bool image_file_name(std::string &file_name,const std::string &file_path_format, int i,int j,int k,int l)
{
  char fileAsCA[512];
  std::sprintf((char*)fileAsCA/*.c_str()*/,file_path_format.c_str(),i,j,k,l);//e.g. file_path_format="./img_x%02d_y%02d_z%02d_i%03d.png"
  file_name=fileAsCA;
  return true;
}//image_file_name


//! grab a set of images and save them both to image file and its statistics to memory.
/**
 * 
 * \param [in] grab: camera device.
 * \param [out] image: last acquired image.
 * \param [in] ImagePath: format for the image file name (e.g. ./img_x%02d_y%02d_z%02d_i%03d.png)
 * \param [in] ImageNumber: number of images to record (e.g. size of set)
 * \param [in] i,j,k: dimension index for current image i.e. file name (e.g. 0,0,0)
 * \param [out] data4scan: statistics of recorded images and status map.
**/
int record_images(Cgrab &grab,cimg_library::CImg<int> &image,const std::string &ImagePath,const int ImageNumber,const int i,const int j,const int k,Cdata4scan<float,int> &data4scan)
{
//std::cerr<<__FILE__<<"/"<<__func__<<": grab type="<<grab.class_name<<".\n"<<std::flush;
  std::string file;
  for(int l=0;l<ImageNumber;++l)
  {//do
    image_file_name(file,ImagePath,i,j,k,l);
std::cerr<<"file=\""<<file<<"\"\n"<<std::flush;
    if(!grab.grab(image,file)) return 1;
    if(l==0&&i==0&&j==0&&k==0)
    {//set first full image information
      data4scan.set_first_full_image_information(image);
    }//first full image
std::cerr<<"warning: no crop (in "<<__FILE__<<"/"<<__func__<<"function )\n"<<std::flush;//! \todo set crop value from data4scan width and height (i.e. make a crop_sample function AND call it here)
    data4scan.add_sample(image,i,j,k);
 }//done      end of grab images
  //compute mean image
//! \todo [low] set data4scan type (factory) or add maximum and minimum variable within it
  data4scan.normalise(i,j,k);
  //set flag
  data4scan.flag(i,j,k)=1;//satisfied
  return 0;
}//record_images

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
  ///wait time between steps
  const int wait_time=cimg_option("--wait-time",1000,"wait time between steps in ms.");
  ///stop if help requested
  if(show_help) {/*print_help(std::cerr);*/return 0;}

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
image_file_name(file,ImagePath,0,0,0,0);
if(!pGrab->grab(image,file)) return 1;
}//[remove] static due to loss

//stepper device object
//! \todo [high] need stepper factory
  Cstepper stepper;
// OPEN 
  if(!stepper.open(StepperDevicePath,StepperDeviceSerialType)) return 1;
// MOVE 
  std::cerr << "displacement along (X,Y,Z)=("<<number(0)*step(0)<<","<<0<<","<<0<<") steps at (vX,vY,vZ)=("<<velocity(0)<<","<<0<<","<<0<<") step(s) per second speed.\n"<<std::flush;
/////////////////////////////////////////////////////////////

 const int mj = 10;
  cimg_library::CImg<int> map(number(0),number(1),number(2));   //added by Dahi
map.fill(0);//not_satisfied
//! \todo [high] need stepper that is not moving/reading (so factory or set specific)
  Cdata4scan<float,int> data4scan;//mean,flag,fail (e.g. map)
  data4scan.initialise(image.width/*()*/,image.height/*()*/,number(0),number(1),number(2));
/////////////////////////////////////////////////////////
cimg_library::CImg<int> stepz(3);stepz.fill(0);stepz(2)=step(2);//e.g. (0,0,10) added stepz

for(int k=0;k<number(2);++k)
{
cimg_library::CImg<int> stepy(3);stepy.fill(0);stepy(1)=step(1);//stepz(2)=step(2)//e.g. (0,-10,0) added stepz

//////////////////////////////////////////////////////////
for(int j=0;j<number(1);++j)
{
  cimg_library::CImg<int> stepx(3);stepx.fill(0);stepx(0)=step(0);//e.g. (10,0,0)
  for(int i=0;i<number(0);++i)
  {
    std::cerr << "actual displacement along (X,Y,Z)=("<<i*step(0)<<","<<0<<","<<0<<") steps over entire displacement of ("<<number(0)*step(0)<<","<<0<<","<<0<<") steps.\n"<<std::flush;
    
  if(number(0)>1)  //(added)
  {//X move          (added)
   if(!stepper.move(stepx,velocity)) return 1;
    cimg_library::cimg::wait(wait_time);
  }//X move     (added)
//////////////////////////////////////////
//  grab images
   //record_images(grab,image,ImagePath,ImageNumber,i,j,k);
   record_images(*pGrab,image,ImagePath,ImageNumber,i,j,k,data4scan);//update data4scan: mean and .flag
   map(i,j,k)=1;//satisfied                                   // added by Dahi
//map.display("map (0=not,1=satisfied)");
////////////////////////////////////////
  }//X step loop
//////////////////////////////////////////////////////////
  //go back to zero on X axis //move backward in X step
  if(number(0)>1)  //(added)
{//X reset (with mechanical jitter)  (added)
  // 1. move backward+mechaniocal jitter in X step // mechanical jitter = mj
 
  stepx(0)= - ((step(0) * number(0)) + mj);
  if(!stepper.move(stepx,velocity)) return 1;

  // 2. move forward mechanical jitter in X step
  stepx(0)=  mj;
  if(!stepper.move(stepx,velocity)) return 1;
  cimg_library::cimg::wait(wait_time);

}//X reset   (added)

/////////////////////////////////////////////
cimg_library::cimg::wait(wait_time);
if(number(1)>1)  //(added)
{//Y move          (added)
  //Y move
  if(!stepper.move(stepy,velocity)) return 1;
}//Y move     (added)
}//Y step loop


 if(number(1)>1)  //(added)
{//Y reset (with mechanical jitter)  (added)
 // 1. move backward+mechaniocal jitter in Y step // mechanical jitter = mj

  stepy(1)= - ((step(1) * number(1)) + mj);
  if(!stepper.move(stepy,velocity)) return 1;

 // 2. move forward mechanical jitter in X step
  stepy(1)=  mj;
  if(!stepper.move(stepy,velocity)) return 1;
}//Y reset   (added)

//////////////////////////////////////////////////////////////////
  if(number(2)>1)  //(added)
  {//Z move          (added)
    if(!stepper.move(stepz,velocity)) return 1;
    cimg_library::cimg::wait(wait_time);
  }//Z move     (added)
}//Z step loop
  if(number(2)>1)
  {//Z reset (with mechanical jitter)  (added)
   // 1. move backward+mechaniocal jitter in Z step // mechanical jitter = mj
    stepz(2)= - ((step(2) * number(2)) + mj);
    if(!stepper.move(stepz,velocity)) return 1;
   // 2. move forward mechanical jitter in X step
    stepz(2)=  mj;
    if(!stepper.move(stepz,velocity)) return 1;
  }//Z reset   (added)

  //////////////////////////////////////////////////////////////////
  data4scan.save(DataPath);//save processed data (e.g. mean images), flag (i.e. satisfied (or not) map) and fail (i.e. number of failing map).
data4scan.print("mean");
data4scan.flag.print("flag");
data4scan.fail.print("fail");
//CLOSE
  stepper.close();
  pGrab->close();
  return 0;
}//main



