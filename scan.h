#ifndef SCAN
#define SCAN

#include <sstream>
#include "../scan/data4scan.h"
#include "../stepper/stepper_factory.h"
#include "../grab/grab_factory.h"

//! make a full scanning
/**
 * scanning types:
 * - scanning : scan but no check (especially use full for all ready recorded scan)
 * - scanning_force : scan with position reader, so it could try several times to make the position ordered one after the other (might be long; internal try loop)
 * - scanning_lazy  : scan with position reader, make a displacement and record it if not done yet (might be faster; external try loop)
 * make displacements and record images.
 * note: embendding GUI for progress
**/
template<typename Tvalue, typename Tmap>
class Cscan
{
public:
  //! class name for debug only
#if cimg_debug>1
  std::string class_name;
#endif
  //! class (or library) version for information only
  std::string class_version;
//private:
  //! grab device
  Cgrab *pGrab;
  bool file_path_order_xyzi;
  //! stepper device
  Cstepper *pStepper;
public:
  //! image data
  Cdata4scan<Tvalue,Tmap> data4scan;//mean,flag,fail (e.g. map)

  //! constructor
  /**
   *
  **/
  Cscan()
  {
#if cimg_debug>1
    class_name="Cscan";
#endif
    class_version=SCAN_VERSION;
  }//constructor

//functions:
///image_file_name
///record_images
///scanning
////scanning_force
////scanning_lazy

//! assign and fill
/**
 * \param [in] width:  size of croppped image along x axis, i.e. cropped image width  = ROI (e.g. 48 pixel).
 * \param [in] dim_X: number of position along X axis, i.e. scan size along X (e.g. 12 positions).
 * \param [in] margin_x: margin for x direction for ROI regarding to maximum in first image (e.g. 16 pixel).
**/
bool initialise(const std::string &StepperDeviceType,const std::string &StepperDevicePath,const std::string &StepperDeviceSerialType,
  const std::string &StepperReaderDevicePath,const std::string &StepperReaderDeviceSerialType,const cimg_library::CImg<unsigned int> &mechanical_jitter,
  const std::string &CameraDeviceType,const std::string &CameraDevicePath,const std::string &ImagePath,const std::string &TemporaryImagePath,
  const cimg_library::CImg<int> &number,
  const cimg_library::CImg<int> &margin_pixel,
  const cimg_library::CImg<float> &pixel_size,
  const int x0,const int y0,const int x1,const int y1,bool is_file_path_order_xyzi=false)
{
  ///grab device object
  Cgrab_factory grab_factory;
  pGrab=grab_factory.create(CameraDeviceType);
  pGrab->temporary_image_path=TemporaryImagePath;
  file_path_order_xyzi=is_file_path_order_xyzi;
  //open
  if(!pGrab->open(CameraDevicePath)) return false;
  //get full image size
  int width=-1,height=-1;
//! \todo [low] remove get first image that is no need as Cdata4scan::set_first_full_image_information will be called for first image only (warning: for default grab/load image flag: need to create specific load_from_file class that is not base class)
  {//grab a first image (presently a sequence)
  cimg_library::CImg<int> image;
  std::string file;
  this->image_file_name(file,ImagePath,0,0,0,0);
//! \todo v record full sequence (to have compatibility with AandDEE reset)
  pGrab->sequence_initialisation(number(3));
  for(int i=0;i<number(3);++i)
  {
    if(!pGrab->grab(image,file)) return false;
image.print("initialise/image for size");
  }
#if version_cimg < 130
  width=image.width;  height=image.height;
#else
  width=image.width();height=image.height();
#endif
  }//grab a first image

  ///stepper device object
  //! \todo [high] . need stepper factory
  Cstepper_factory stepper_factory;
  pStepper=stepper_factory.create(StepperDeviceType);
  //open 
  if(!pStepper->open(StepperDevicePath,StepperDeviceSerialType,StepperReaderDevicePath,StepperReaderDeviceSerialType,mechanical_jitter)) return false;
cimg_library::cimg::wait(1234);

  ///data object (cropped image)
  if(x0<0||y0<0||x1<0||y1<0)
  {
    if(pGrab->grabed_image)
    {
std::cerr<<"Information: cropped image (i.e. ROI) size is set from margin and pixel size (and scan parameters too).\n"<<std::flush;
      data4scan.initialisef(margin_pixel,pixel_size,number(0),number(1),number(2));
    }
    else
    {
std::cerr<<"Information: cropped image (i.e. ROI) size is set by loaded image size (i.e. no crop).\n"<<std::flush;
      data4scan.initialise(width,height,number(0),number(1),number(2),0,0,0,0);
    }
  }
  else
  {
std::cerr<<"Information: cropped image (i.e. ROI) size is set by ROI rectangle coordinates, only.\n"<<std::flush;
    const int width=x1-x0+1;
    const int height=y1-y0+1;
    data4scan.initialise(width,height,number(0),number(1),number(2),margin_pixel(0),margin_pixel(1),x0,y0);
  }
  return true;
}//initialise

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
  if(file_path_order_xyzi)
    std::sprintf((char*)fileAsCA/*.c_str()*/,file_path_format.c_str(),i,j,k,l);//e.g. file_path_format="./img_x%02d_y%02d_z%02d_i%03d.png"
  else
    std::sprintf((char*)fileAsCA/*.c_str()*/,file_path_format.c_str(),k,j,i,l);//e.g. file_path_format="./img_z%02d_y%02d_x%02d_i%03d.png"
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
int record_images(Cgrab &grab,cimg_library::CImg<int> &image,const std::string &ImagePath,const int ImageNumber,const int i,const int j,const int k,Cdata4scan<float,int> &data4scan,const std::string &DataPath)
{
//std::cerr<<__FILE__<<"/"<<__func__<<": grab type="<<grab.class_name<<".\n"<<std::flush;
  std::string file;
std::string file_list;
//! \todo . add AandDEE reset and DaVis jump
  grab.sequence_initialisation(ImageNumber);//DaVis and AandDEE reset
  for(int l=0;l<ImageNumber;++l)
  {//do
    image_file_name(file,ImagePath,i,j,k,l);
std::cerr<<"file=\""<<file<<"\"\n"<<std::flush;
    if(!grab.grab(image,file)) return 1;

//! \todo [libGrab] _ use camera class with bad pixel taken into account
file_list+=" "+grab.temporary_image_name;
if(grab.grabed_image)
{
std::cerr<<"WARNING: set ImagerIntense dead pixel to 0 !!\n"<<std::flush;
image(493,763)=0;
}

//image.print("image");
    if(l==0&&i==0&&j==0&&k==0)
    {//set first full image information (size, maximum position, ROI origin, ...)
      data4scan.set_first_full_image_information(image);
    }//first full image
//data4scan.print_all("data4scan");
//! \todo [high] _ set crop value from data4scan width and height (i.e. make a crop_sample function AND call it here)
    {//crop
    const int x0=data4scan.ROI_origin(0);
    const int y0=data4scan.ROI_origin(1);
    const int x1=x0+data4scan[0].width-1;
    const int y1=y0+data4scan[0].height-1;
    image.crop(x0,y0,x1,y1);
//image.print("cropped image");
    }//crop
    //save
    grab.save(image,file);
    //statistics
    data4scan.add_sample(image,i,j,k);
 }//done      end of grab images
  //compute mean image
//! \todo [low] set data4scan type (factory) or add maximum and minimum variable within it
  data4scan.normalise(i,j,k);

//! \todo [low] _ vector for saving many formats e.g. cimg, NetCDF.
//REMOVE FORCE SAVE MEAN and REMOVE TEMPORARY{
if(grab.grabed_image)
{//save mean image
//save mean
image_file_name(file,DataPath,i,j,k,-1);
data4scan.tmp_mean.save(file.c_str());
//save int mean
cimg_library::CImg<unsigned short> meanAsInt(data4scan.tmp_mean);
file+=".16bit.cimg";
meanAsInt.save(file.c_str());
}//save mean image
//reset to zero (put it back in .normalise) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
data4scan.tmp_mean=0.0;data4scan.tmp_count=0;
if(grab.grabed_image)
//! \todo [libGrab] _ add function to remove temporary image file in grab class for sequence
{//remove temporary image files
image_file_name(file,grab.temporary_image_path,-1,-1,grab.temporary_image_index,-1);
file+=".remove.txt";
std::ofstream fo(file.c_str(),ofstream::binary);
fo<<file_list<<std::flush;
fo.close();
std::string cmd="rm `cat "+file+"`";
std::system(cmd.c_str());
}//remove temporary image files
//}REMOVE FORCE SAVE MEAN and REMOVE TEMPORARY

  //set flag
  data4scan.flag(i,j,k)=1;//satisfied
  return 0;
}//record_images

//! scan a full volume, i.e. all positions of the volume (note: reset to original position with mechanical jitter)
/**
 * scan a volume, i.e. make all displacement along the 3 axes to do each position in the volume.
 * \note intended to be used for without reader (as it is relative displacement: step)
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
int scanning_raw(Cstepper &stepper,const cimg_library::CImg<int> &number,const cimg_library::CImg<int> &step,const cimg_library::CImg<int> &velocity,const int wait_time, const unsigned int mechanical_jitter,
  Cgrab &grab,cimg_library::CImg<int> &image,const std::string &ImagePath,const int ImageNumber,Cdata4scan<Tvalue,Tmap> &data4scan,const std::string  DataPath
#if cimg_display>0
  ,const unsigned int zoom=100,const bool do_display=false
#endif //cimg_display
)
{
std::cerr<<__FILE__<<"/"<<__func__<<"\n"<<std::flush;
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
///**** grab
        record_images(grab,image,ImagePath,ImageNumber,i,j,k,data4scan,DataPath);
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
}//scanning_raw

//! scan a full volume, i.e. all positions of the volume (note: reset to original position with mechanical jitter)
/**
 * scan a volume, i.e. make all displacement along the 3 axes to do each position in the volume.
 * \note intended to be used for with reader (as it is absolute displacement: position)
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
int scanning_force(Cstepper &stepper,const cimg_library::CImg<int> &number,const cimg_library::CImg<int> &step,const cimg_library::CImg<int> &velocity,const int wait_time, const unsigned int mechanical_jitter,
  Cgrab &grab,cimg_library::CImg<int> &image,const std::string &ImagePath,const int ImageNumber,Cdata4scan<Tvalue,Tmap> &data4scan,const std::string &DataPath
#if cimg_display>0
  ,const unsigned int zoom=100,const bool do_display=false
#endif //cimg_display
)
{
///set signed mechanical jitter
  //set mechanical jitter for all axes with same sign as corresponding displacement.
  cimg_library::CImg<int>  mj(3);
  cimg_forX(mj,a) mj(a)=((step(a)>0)?1:-1)*mechanical_jitter;
  stepper.mechanical_jitter=mechanical_jitter;

///reset to (0,0,0)
//! \todo [high] add both reset and check absolute position in (0,0,0)

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
//! \todo [low] volume could be removed as both content is not used and content is in Cscan class as data. Could be cleaned making a progress class.
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
  //absolute position (3D)
  cimg_library::CImg<int> pos(3);pos.fill(0);//e.g. (1,2,3)
  //Z axis loop
  int kz;//absolute position along Z axis (e.g. k=kz if step=1 in Z direction)
  for(int k=0,kz=0;k<number(2);++k,kz+=step(2))
  {
    //set current Z for absolute position
    pos(2)=kz;
#if cimg_display>0
    if(do_display)
    {
    //current slice
      cimg_forXY(colume,x,y) colume.draw_point(x,y,blue);
//! \todo remove slice in title if number(2)==1
      progress.set_title("#%d/%d: scan progress (slice#%d/%d)",k,number(2),k,number(2));
    }//do_display
#endif //cimg_display
///** move along Z axis
    //force move along Z axis
    if(number(2)>1)
    {//Z move only if more than one slice to do
      if(!stepper.move(pos,velocity)) return 1;
      data4scan.fail(0,0,k)+=stepper.fail;
    }//Z move
///** wait a while for user
    cimg_library::cimg::wait(wait_time);

///** Y axis loop
    //set 1D displacement for Y axis
    int jy;//absolute position along Y axis (e.g. j=jy if step=1 in Y direction)
    //Y axis loop
    for(int j=0,jy=0;j<number(1);++j,jy+=step(1))
    {
      //set current Y for absolute position
      pos(1)=jy;
///*** move along Y axis
      //force move along Y axis
      if(number(1)>1)
      {//Y move only if more than one column to do
        if(!stepper.move(pos,velocity)) return 1;
        data4scan.fail(0,j,k)+=stepper.fail;
//        if(!stepper.move(1,jy,velocity)) return 1;
      }//Y move
///*** wait a while for user
      cimg_library::cimg::wait(wait_time);
///*** X axis loop
      //X axis loop
      int ix;//absolute position along X axis (e.g. i=ix if step=1 in X direction)
      for(int i=0,ix=0;i<number(0);++i,ix+=step(0))
      {
        //set current Y for absolute position
        pos(0)=ix;
///**** position message
        std::cerr << "actual displacement to "<<
       #ifdef cimg_use_vt100
        cimg_library::cimg::t_green<<
       #endif
        "(X,Y,Z)=("<<ix<<","<<jy<<","<<kz<<") "<<
       #ifdef cimg_use_vt100
        cimg_library::cimg::t_normal<<
       #endif
        "step position over entire scanning of ("<<number(0)*step(0)<<","<<number(1)*step(1)<<","<<number(2)*step(2)<<") steps.\n"<<std::flush;
///**** move along X axis
        //force move along X axis
        if(number(0)>1)
        {//X move only if more than one line to do
          if(!stepper.move(pos,velocity)) return 1;
          data4scan.fail(i,j,k)+=stepper.fail;
//        if(!stepper.move(0,ix,velocity)) return 1;
        }//X move
///**** wait a while for user
        cimg_library::cimg::wait(wait_time);
///**** grab
        record_images(grab,image,ImagePath,ImageNumber,i,j,k,data4scan,DataPath);
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
        "reset X axis to (X,Y,Z)=(0,"<<jy<<","<<kz<<")"<<
       #ifdef cimg_use_vt100
        cimg_library::cimg::t_normal<<
       #endif
        ".\n"<<std::flush;
        // 1n2. move with mechanical jitter
        cimg_library::CImg<int> reset_x(3);reset_x(0)=0;reset_x(1)=jy;reset_x(2)=kz;//e.g. (0,2,1)
        if(!stepper.move(reset_x,velocity)) return 1;
        data4scan.fail(0,j,k)+=stepper.fail;
        cimg_library::cimg::wait(wait_time);
      }//X reset
    }//Y axis loop
///** reset Y axis
    //go back to zero on Y axis (i.e. move backward along Y axis)
    if(number(1)>1)
    {//Y reset (with mechanical jitter)
std::cerr<<"WARNING: reset in Y direction is limited for safety in physical space.\n"<<std::flush;
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
      // 1n2. move with mechanical jitter
      cimg_library::CImg<int> reset_y(3);reset_y(0)=0;reset_y(1)=0;reset_y(2)=kz;//e.g. (0,0,1)
      if(!stepper.move(reset_y,velocity)) return 1;
      data4scan.fail(0,0,k)+=stepper.fail;
      cimg_library::cimg::wait(wait_time);
    }//Y reset
  }//Z axis loop
///* reset Z axis
  //go back to zero on Z axis (i.e. move backward along Z axis)
  if(number(2)>1)
  {//Z reset (with mechanical jitter)
std::cerr<<"WARNING: reset in Z direction is limited for safety in physical space.\n"<<std::flush;
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
    // 1n2. move with mechanical jitter
    cimg_library::CImg<int> reset_z(3);reset_z=0;//e.g. (0,0,0)
    if(!stepper.move(reset_z,velocity)) return 1;
    cimg_library::cimg::wait(wait_time);
  }//Z reset

#if cimg_display>0
  //close GUI
  progress.close();
#endif //cimg_display

  return 0;
}//scanning_force

//! scan a full volume, i.e. all positions of the volume (note: reset to original position with mechanical jitter)
/**
 * scan a volume, i.e. make all displacement along the 3 axes to do each position in the volume.
 * \note reset to original position, especially in order to make 3D position scanning in each direction loop.
 * \note could be used as 1D line, column or depth scanning for any axis scan, or even 2D plane or slice scanning.
 *
 * \param[in] number: number of iteration in each of the scanning loops (i.e. number of positions for the 3 axes; size should be 3)
 * \param[in] step: 3D displacement in step (i.e. size should be 3)
 * \param[in] velocity: 3D velocity in step per second (i.e. size should be 3)
 * \param[in] wait_time: minimum delay between each loop displacement
 * \param[in] mechanical_jitter: mechanical jitter to perform a good reset for any axes
**/
int scanning(const cimg_library::CImg<int> &number,const cimg_library::CImg<int> &step,const cimg_library::CImg<int> &velocity,const int wait_time, const unsigned int mechanical_jitter,
  const std::string &ImagePath,const int ImageNumber,const std::string  DataPath
#if cimg_display>0
  ,const unsigned int zoom=100,const bool do_display=false
#endif //cimg_display
)
{
std::cerr<<__FILE__<<"/"<<__func__<<"\n"<<std::flush;
data4scan.print("data4scan");
  //init image size
#if version_cimg < 130
  cimg_library::CImg<int> image(data4scan[0].width  ,data4scan[0].height  );
#else
  cimg_library::CImg<int> image(data4scan[0].width(),data4scan[0].height());
#endif
  //raw scan
//  scanning_raw(*pStepper,number,step,velocity,wait_time,mechanical_jitter,
  scanning_force(*pStepper,number,step,velocity,wait_time,mechanical_jitter,
    *pGrab,image,ImagePath,ImageNumber,data4scan,DataPath
#if cimg_display>0
    ,zoom,do_display
#endif //cimg_display
    );
  return 0;
}//scanning

//! close
/**
 * close both grab and stepper devices
**/
bool close()
{
  pStepper->close();
  pGrab->close();
}//close

};//Cscan class

#endif //SCAN

