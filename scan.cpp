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

//! scan a full volume, i.e. all positions of the volume (note: reset to original position with mechanical jitter)
/**
 * scan a volume, i.e. make all displacement along the 3 axes to do each position in the volume.
 * \note reset to original position, especially in order to make 3D position scanning in each direction loop.
 * \note could be used as 1D line, column or depth scanning for any axis scan, or even 2D plane or slice scanning.
 *
 * \param[in] stepper: displacement stage device (should be already open)
 * \param[in] number: number of iteration in each of the scanning loops (i.e. number of positions for the 3 axes; size should be 3)
 * \param[in] step: 3D displacement in step (i.e. size should be 3)
 * \param[in] velocity: 3D velocity in step per second (i.e. size should be 3)
 * \param[in] wait_time: minimum delay between each loop displacement
 * \param[in] mechanical_jitter: mechanical jitter to perform a good reset for any axes
**/
int scanning(Cstepper &stepper,const cimg_library::CImg<int> &number,const cimg_library::CImg<int> &step,const cimg_library::CImg<int> &velocity,const int wait_time, const unsigned int mechanical_jitter
#if cimg_display>0
,const unsigned int zoom=100,const bool do_display=false
#endif //cimg_display
)
{
///set signed mechanical jitter
  //set mechanical jitter for all axes with same sign as corresponding displacement.
  cimg_library::CImg<int>  mj(3);
  cimg_forX(mj,a) mj(a)=((step(a)>0)?1:-1)*mechanical_jitter;

///reset to (0,0,0): NOT for this function

///scanning message
  std::cerr<<"full scanning volume of "<<
 #ifdef cimg_use_vt100
  cimg_library::cimg::t_purple<<
 #endif
  "size (sX,sY,sZ)=("<<number(0)*step(0)<<","<<number(1)*step(1)<<","<<number(2)*step(2)<<") steps"<<
 #ifdef cimg_use_vt100
  cimg_library::cimg::t_normal<<
 #endif
  " by "<<
 #ifdef cimg_use_vt100
  cimg_library::cimg::t_purple<<
 #endif
  "displacement (dX,dY,dZ)=("<<step(0)<<","<<step(1)<<","<<step(2)<<") steps"<<
 #ifdef cimg_use_vt100
  cimg_library::cimg::t_normal<<
 #endif
  " at (vX,vY,vZ)=("<<velocity(0)<<","<<velocity(1)<<","<<velocity(2)<<") step(s) per second speed.\n"<<std::flush;

#if cimg_display>0
  //GUI to display scanning progress
  cimg_library::CImg<char> volume(number(0),number(1),number(2));volume.fill(2);//0=fail(red), 1=done(green), 2=to_do(blue)
  //color
//! \todo use \c volume for setting colors to \c colume (e.g. \c red color in case of position failure; need position check stepper)
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
  cimg_library::CImg<unsigned char> colume;
  //display
  cimg_library::CImgDisplay progress;
  if(do_display)
  {
//! \todo assign both \c colume and \c progress for displaying at best an image (i.e. 2D)
#if version_cimg < 130
    colume.assign(volume.width  ,volume.height,1,3);
    progress.assign(volume.width  *zoom,volume.height  *zoom);//,volume.depth()*zoom);
#else
    colume.assign(volume.width(),volume.height(),1,3);
    progress.assign(volume.width()*zoom,volume.height()*zoom);//,volume.depth()*zoom);
#endif
    progress.set_title("scan progress");
  }
#endif //cimg_display

///* Z axis loop
  //set 1D displacement for Z axis
  cimg_library::CImg<int> stepz(3);stepz.fill(0);stepz(2)=step(2);//e.g. (0,0,10)
  //Z axis loop
  for(int k=0;k<number(2);++k)
  {

#if cimg_display>0
    if(do_display)
    {
    //current slice
      cimg_forXY(colume,x,y) colume.draw_point(x,y,blue);
//! \todo remove slice in title if number(2)==1
      progress.set_title("#%d/%d: scan progress (slice#%d/%d)",k,number(2),k,number(2));
    }//do_display
#endif //cimg_display

///** Y axis loop
    //set 1D displacement for Y axis
    cimg_library::CImg<int> stepy(3);stepy.fill(0);stepy(1)=step(1);//e.g. (0,10,0)
    //Y axis loop
    for(int j=0;j<number(1);++j)
    {
///*** X axis loop
      //set 1D displacement for X axis
      cimg_library::CImg<int> stepx(3);stepx.fill(0);stepx(0)=step(0);//e.g. (10,0,0)
      //X axis loop
      for(int i=0;i<number(0);++i)
      {
///**** position message
        std::cerr << "actual displacement to "<<
       #ifdef cimg_use_vt100
        cimg_library::cimg::t_green<<
       #endif
        "(X,Y,Z)=("<<i*step(0)<<","<<j*step(1)<<","<<k*step(2)<<") "<<
       #ifdef cimg_use_vt100
        cimg_library::cimg::t_normal<<
       #endif
        "step position over entire scanning of ("<<number(0)*step(0)<<","<<number(1)*step(1)<<","<<number(2)*step(2)<<") steps.\n"<<std::flush;
#if cimg_display>0
        //set status
        volume(i,j,k)=1;
        if(do_display)
        {
          //GUI to display scanning progress
          colume.draw_point(i,j,green);
          progress.display(colume.get_resize(zoom));
        }//do_display
#endif //cimg_display
///**** move along X axis
        //move along X axis
        if(number(0)>1)
        {//X move only if more than one line to do
          if(!stepper.move(stepx,velocity)) return 1;
        }//X move
///**** wait a while for user
        cimg_library::cimg::wait(wait_time);
      }//X axis loop
///*** reset X axis
      //go back to zero on X axis (i.e. move backward along X axis)
      if(number(0)>1)
      {//X reset (with mechanical jitter)
        // 0. reset message
        std::cerr<<
       #ifdef cimg_use_vt100
        cimg_library::cimg::t_purple<<
       #endif
        "reset X axis to (X,Y,Z)=(0,"<<j*step(1)<<","<<k*step(2)<<")"<<
       #ifdef cimg_use_vt100
        cimg_library::cimg::t_normal<<
       #endif
        ".\n"<<std::flush;
        // 1. move backward with mechanical jitter in X step // mechanical jitter = mj
        stepx(0)=-(step(0)*number(0)+mj(0));
        if(!stepper.move(stepx,velocity)) return 1;
        cimg_library::cimg::wait(wait_time);
        // 2. move forward mechanical jitter in X step
        stepx(0)=mj(0);
        if(!stepper.move(stepx,velocity)) return 1;
        cimg_library::cimg::wait(wait_time);
      }//X reset
///*** move along Y axis
      //move along Y axis
      if(number(1)>1)
      {//Y move only if more than one column to do
        if(!stepper.move(stepy,velocity)) return 1;
      }//Y move
///*** wait a while for user
      cimg_library::cimg::wait(wait_time);
    }//Y axis loop
///** reset Y axis
    //go back to zero on Y axis (i.e. move backward along Y axis)
    if(number(1)>1)
    {//Y reset (with mechanical jitter)
      // 0. reset message
      std::cerr<<
     #ifdef cimg_use_vt100
      cimg_library::cimg::t_purple<<
     #endif
      "reset Y axis to (X,Y,Z)=(0,0,"<<k*step(2)<<")"<<
     #ifdef cimg_use_vt100
      cimg_library::cimg::t_normal<<
     #endif
      ".\n"<<std::flush;
      // 1. move backward with mechanical jitter in Y step // mechanical jitter = mj
      stepy(1)=-(step(1)*number(1)+mj(1));
      if(!stepper.move(stepy,velocity)) return 1;
      cimg_library::cimg::wait(wait_time);
      // 2. move forward mechanical jitter in Y step
      stepy(1)=mj(1);
      if(!stepper.move(stepy,velocity)) return 1;
      cimg_library::cimg::wait(wait_time);
    }//Y reset
///** move along Z axis
    //move along Z axis
    if(number(2)>1)
    {//Z move only if more than one slice to do
      if(!stepper.move(stepz,velocity)) return 1;
    }//Z move
///** wait a while for user
    cimg_library::cimg::wait(wait_time);
  }//Z axis loop
///* reset Z axis
  //go back to zero on Z axis (i.e. move backward along Z axis)
  if(number(2)>1)
  {//Z reset (with mechanical jitter)
    // 0. reset message
    std::cerr<<
   #ifdef cimg_use_vt100
    cimg_library::cimg::t_purple<<
   #endif
    "reset Z axis to (X,Y,Z)=(0,0,0)"<<
   #ifdef cimg_use_vt100
    cimg_library::cimg::t_normal<<
   #endif
    ".\n"<<std::flush;
    // 1. move backward with mechanical jitter in Z step // mechanical jitter = mj
    stepz(2)=-(step(2)*number(2)+mj(2));
    if(!stepper.move(stepz,velocity)) return 1;
    cimg_library::cimg::wait(wait_time);
    // 2. move forward mechanical jitter in Z step
    stepz(2)=mj(2);
    if(!stepper.move(stepz,velocity)) return 1;
    cimg_library::cimg::wait(wait_time);
  }//Z reset

#if cimg_display>0
  //close GUI
  progress.close();
#endif //cimg_display

  return 0;
}//scanning

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

//next scanning object
  Cdata4scan<float,int> data4scan;//mean,flag,fail (e.g. map)
#if version_cimg < 130
  data4scan.initialise(image.width  ,image.height  ,number(0),number(1),number(2));
#else
  data4scan.initialise(image.width(),image.height(),number(0),number(1),number(2));
#endif
scanning(stepper,number,step,velocity,wait_time,
  mechanical_jitter
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



