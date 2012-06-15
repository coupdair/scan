//stepper
/*-----------------------------------------------------------------------
  File        : stepper.cpp
  Description : stepper motor for translation displacement program of the Laboratory of Mechanics in Lille (LML)
  Authors     : Sebastien COUDERT
-----------------------------------------------------------------------*/

/*Additional documentation for the generation of the reference page (using doxygen)*/
/** \mainpage
 *
 *  index:
 *  \li \ref sectionCommandLine
 *  \li \ref sectionStepperDocumentation
 *  \li \ref sectionDoxygenSyntax
 *
 *  \section sectionCommandLine command line options
 *
 *  \verbinclude "scan.help.output"
 *  
 *  \section sectionStepperDocumentation documentation outline
 *  This is the reference documentation of <a href="http://www.meol.cnrs.fr/">serial</a>, from the <a href="http://www.univ-lille1.fr/lml/">LML</a>.\n\n
 *  stepper software. The main function is in <a href="stepper_8cpp.html">stepper.cpp</a> source file.\n\n
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
///data
#include "data4scan.h"
//stepper library
#include "../stepper/stepper.h"
//grab library
#include "../grab/grab.h"


int record_images(Cgrab &grab,cimg_library::CImg<int> &image,const std::string &ImagePath, int ImageNumber,int i,int j,int k)
{
std::string file;//file.reserve(ImagePath.size()+64);file[0]='\0';
  cimg_library::CImg<int> data(ImageNumber);   //added by Dahi
  //for(int l=0;l<ImageNumber;++l)
  for(int l=0;l<data.width();++l)
  {//do

    {//image file name
std::cerr<<"ImagePath=\""<<ImagePath<<"\"\n"<<std::flush;
    char fileAsCA[512];
    std::sprintf((char*)fileAsCA/*.c_str()*/,ImagePath.c_str(),i,j,l);//e.g. ImagePath="./img_x%02d_y%02d_i%03d.jpg"
    file=fileAsCA;
    }//image file name
std::cerr<<"file=\""<<file<<"\"\n"<<std::flush;
    if(!grab.grab(image,file)) return 1;
////////////////////////////
   data(l)=l;                                   // added by Dahi
   cout << data(l) << " number of images " << endl; // added by Dahi
   
 }//done      end of grab images

//std::cout << file.c_str() << std::endl;
  data.print("data");
  return 0;
}//record_images

int main(int argc, char *argv[])
{ 
//commmand line options
 ///usage
  cimg_usage(std::string("stepper program of the Laboratory of Mechanics in Lille (LML) is intended to make translation displacement using a stepping motor, \
it uses different GNU libraries (see --info option)\n\n \
usage: ./stepper -h -I\n \
       ./stepper -nx 10 -sx 1 -vx 1000 --device-type uControlXYZ\n \
version: "+std::string(VERSION)+"\n compilation date: " \
            ).c_str());//cimg_usage
  ///information and help
  const bool show_h   =cimg_option("-h",    false,NULL);//-h hidden option
        bool show_help=cimg_option("--help",show_h,"help (or -h option)");show_help=show_h|show_help;//same --help or -h option
  bool show_info=cimg_option("-I",false,NULL);//-I hidden option
  if( cimg_option("--info",show_info,"show compilation options (or -I option)") ) {show_info=true;cimg_library::cimg::info();}//same --info or -I option
////////////////////////added by Dahi////////////////////// start//////
 ///device camera
//  const std::string DeviceType=cimg_option("--device-type","Elphel","type of grab device (e.g. ArduinoTTL or Elphel_wget or Elphel_OpenCV or Elphel_rtsp).");
  const std::string CameraDevicePath=cimg_option("--device-path","192.168.0.9","path of grab device.");
  ///image
  const int ImageNumber=cimg_option("-n",10,"number of images to acquire.");
  const std::string ImagePath=cimg_option("-o","./image_x%02d_y%02d_i%03d.jpg","path for image(s).");
  ///////const int         // this added by Dahi to see if the image name is changed or not
  ///stop if help requested
  if(show_help) {/*print_help(std::cerr);*/return 0;}
//grab device object
  Cgrab grab;
//open
  if(!grab.open(CameraDevicePath/*,DeviceType*/)) return 1;
//get
  cimg_library::CImg<int> image;
////////////////////////end of camera device/////////////////////////////////

///////////////////////////start stepper device/////////////////////////////
  ///device
  const std::string DeviceType=cimg_option("--device-type","uControlXYZ","Type of stepper device");
  const std::string StepperDevicePath=cimg_option("--device-path","/dev/ttyUSB0","Path of stepper device");
  const std::string SerialType=cimg_option("--serial-type","serial_system","Type of serial device for stepper (i.e. serial_termios or serial_system)");
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
//stepper device object
  Cstepper stepper;
// OPEN 
  if(!stepper.open(StepperDevicePath,SerialType)) return 1;
// MOVE 
  std::cerr << "displacement along (X,Y,Z)=("<<number(0)*step(0)<<","<<0<<","<<0<<") steps at (vX,vY,vZ)=("<<velocity(0)<<","<<0<<","<<0<<") step(s) per second speed.\n"<<std::flush;
/////////////////////////////////////////////////////////////

 const int mj = 10;
  cimg_library::CImg<int> map(number(0),number(1),number(2));   //added by Dahi
map.fill(0);//not_satisfied
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
//////////////////////////////////////////
//  grab images
   record_images(grab,image,ImagePath,ImageNumber,i,j,k);
   map(i,j,k)=1;//satisfied                                   // added by Dahi
map.display("map (0=not,1=satisfied)");
////////////////////////////////////////
 }//X move     (added)
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

//CLOSE
  stepper.close();
  grab.close();
  return 0;
}//main



