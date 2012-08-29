#ifndef DATA4SCAN
#define DATA4SCAN

//standard, NetCDF and CImg Library
#include "../convert/data.h"

//! data up to 5 dimensions (4D+1) mean image (plus flag and fail maps) for scaning positions
/**
 * data container up to 5D.
 * e.g. convert format .cimg to .nc .
**/
template<typename Tvalue, typename Tmap>
class Cdata4scan: public Cdata<Tvalue>
{
public:
  //! position flag
  cimg_library::CImg<Tmap> flag;//3D (X,Y,Z) 0 then red=0, green=1
  //! position fail counter
  cimg_library::CImg<Tmap> fail;//3D (X,Y,Z) 0 then increment++
  //! full image information
  cimg_library::CImg<int> full_image_size;//2C (dimx,dimy)
  //! region of interest of full image information
  // note: size of ROI is this->width and this->height.
  cimg_library::CImg<int> ROI_origin;//2C (x0,y0)
  //! first full image maximum value
  Tvalue full_image_maximum;
  //! first full image maximum position
  cimg_library::CImg<int> full_image_maximum_position;//2C (x0,y0)
  //! margin for ROI size (should be defined regarding to both spot size and full scanned area)
  cimg_library::CImg<int> margin;//2C (dx,dy)
#ifdef cimg_use_netcdf
  //! flag and fail information:
  std::string flag_name;
  std::string flag_unit_name;
  std::string fail_name;
  std::string fail_unit_name;
#endif //cimg_use_netcdf
  //! temporary image count for statistics
  unsigned int tmp_count;
  //! temporary image for mean
  cimg_library::CImg<Tvalue> tmp_mean;//2D (x,y)

  //! constructor
  /**
   *
  **/
  Cdata4scan()
  {
    ///set class name (debug only)
#if cimg_debug>1
    this->class_name="Cdata4scan";
std::cerr<<this->class_name<<"::"<<__func__<<"()\n"<<std::flush;
#endif
    ///set library version (information only)
    this->class_version=VERSION+std::string(" based on Cdata.")+DATA_VERSION+std::string(")");
    this->sample_type=true;
#ifdef cimg_use_netcdf
    ///set variable name
    this->component_name.clear();
    this->component_name.push_back("intensity");
    this->unit_name.clear();
    this->unit_name.push_back("gray level");
    ///set dimendion names
    this->dimension_name.clear();
    this->dimension_name.push_back("x");//image width
    this->dimension_name.push_back("y");//image height
    this->dimension_name.push_back("X");//scanning width
    this->dimension_name.push_back("Y");//scanning height
    this->dimension_name.push_back("Z");//scanning depth
    //! map ad fail information: 
    flag_name="flag";
    flag_unit_name="none; 0=outlier, 1=validated";
    fail_name="fail_counter";
    fail_unit_name="count";
#endif //cimg_use_netcdf
  }//constructor

  //! print member informations
  bool print_all(const char* title)
  {
    this->print(title);
    fail.print(".flag");
    fail.print(".fail");
    //full image
    std::cerr<<".full_image_size=("<<full_image_size(0)<<","<<full_image_size(1)<<")\n";
    std::cerr<<".ROI_origin=("<<ROI_origin(0)<<","<<ROI_origin(1)<<")\n";
    std::cerr<<".full_image_maximum="<<full_image_maximum<<" @";
    std::cerr<<".full_image_maximum_position=("<<full_image_maximum_position(0)<<","<<full_image_maximum_position(1)<<")\n";
    std::cerr<<".margin=("<<margin(0)<<","<<margin(1)<<")\n";
    //statitics
    tmp_mean.print(".tmp_mean");
    return true;
  }

  //! assign and fill temporary variables for statistics
  bool tmp_initialise(const int width,const int height,const int dimX,const int  dimY,const int dimZ)
  {
    tmp_count=0;
    tmp_mean.assign(width,height);
    tmp_mean=0.0;
cimg_library::CImg<Tmap> hop=cimg_library::CImg<Tmap>::vector(1,2,3);
hop.print("vector");
std::cerr<<"hop(0)="<<hop(0)<<" hop(1)="<<hop(1)<<" hop(2)="<<hop(2)<<"\n"<<std::flush;
    return true;
  }//tmp_initialise

  //! assign and fill
  /**
   * \param [in] width:  size of croppped image along x axis, i.e. cropped image width  = ROI (e.g. 48 pixel).
   * \param [in] height: size of croppped image along y axis, i.e. cropped image height = ROI (e.g. 48 pixel).
   * \param [in] dim_X: number of position along X axis, i.e. scan size along X (e.g. 12 positions).
   * \param [in] dim_Y: number of position along Y axis, i.e. scan size along Y (e.g. 12 positions).
   * \param [in] dim_Z: number of position along Z axis, i.e. scan size along Z (e.g.  1 positions for part of a chip scan).
   * \param [in] margin_x: margin for x direction for ROI regarding to maximum in first image (e.g. 16 pixel).
   * \param [in] margin_y: margin for y direction for ROI regarding to maximum in first image (e.g. 16 pixel).
  **/
  bool initialise(const int width,const int height,const int dimX,const int dimY,const int dimZ, const int margin_x=32,const int margin_y=32,const int x0=-1,const int y0=-1)
  {
std::cerr<<__FILE__<<"/"<<__func__<<"-crop_size\n"<<std::flush;
    ///scan init.
    this->assign(dimZ,width,height,dimX,dimY);
    cimglist_for((*this),Z) (*this)(Z).fill(-1);
    flag.assign(dimX,dimY,dimZ);
    flag=0;
    fail.assign(dimX,dimY,dimZ);
    fail=0;
    ///full image init.
    full_image_size.assign(2);
    full_image_size=-1;
    ROI_origin.assign(2);
    ROI_origin(0)=x0;
    ROI_origin(1)=y0;
    margin.assign(2);
    margin(0)=margin_x;
    margin(1)=margin_y;
    full_image_maximum=-99;
    full_image_maximum_position.assign(2);
    full_image_maximum_position=-1;
    ///temporary init.
    return tmp_initialise(width,height, dimX,dimY,dimZ);
  }//initialise
  //! assign and fill using both margin and scan area in pixel
  /**
   * set data size (and filled with default values) for a rectangular area (of \c scan_area_pixel) serounded by a margin (i.e. scan area in the middle of a margin band; e.g. square area of 36x36 pixel)
   * \param [in] margin_pixel: margin for both x and y directions for ROI regarding to maximum in first image (e.g. (32,32) pixel).
   * \param [in] scan_area_pixel: effective scanned area in pixel (e.g. (4,4) pixel for a 3 step/pixel chip).
   * \param [in] dim_X: number of position along X axis, i.e. scan size along X (e.g. 12 positions).
   * \param [in] dim_Y: number of position along Y axis, i.e. scan size along Y (e.g. 12 positions).
   * \param [in] dim_Z: number of position along Z axis, i.e. scan size along Z (e.g. 61 positions for a focus search).
  **/
  bool initialise(const cimg_library::CImg<int> &margin_pixel,const cimg_library::CImg<int> &scan_area_pixel,const int dimX,const int dimY,const int dimZ)
  {//initialise
std::cerr<<__FILE__<<"/"<<__func__<<"-scan_area_pixel\n"<<std::flush;
    if( (margin_pixel.size()!=2) || (scan_area_pixel.size()!=2) ) return false;
    const int width= margin_pixel(0)*2+scan_area_pixel(0);
    const int height=margin_pixel(1)*2+scan_area_pixel(1);
    return initialise(width,height,dimX,dimY,dimZ,margin_pixel(0),margin_pixel(1));
  }//initialise
  //! assign and fill using both margin (in pixel) and pixel size (in meter/pixel)
  /**
   * set data size (and filled with default values) for a rectangular area (set with both pixel size and scan dimensions) serounded by a margin in pixel (i.e. scan area in the middle of a margin band; e.g. square area of 36x36 pixel)
   * \param [in] margin_pixel: margin for both x and y directions for ROI (e.g. (32,32) pixel).
   * \param [in] pixel_size: pixel size in step along both x and y axes (e.g. 3 steps/pixel).
   * \param [in] dim_X: number of position along X axis, i.e. scan size along X (e.g. 12 positions, i.e. 12 steps).
   * \param [in] dim_Y: number of position along Y axis, i.e. scan size along Y (e.g. 12 positions, i.e. 12 steps).
   * \param [in] dim_Z: number of position along Z axis, i.e. scan size along Z (e.g. 61 positions for a focus search).
  **/
  bool initialisef(const cimg_library::CImg<int> &margin_pixel,const cimg_library::CImg<float> &pixel_size, const int dimX,const int dimY,const int dimZ)
  {//initialise
std::cerr<<__FILE__<<"/"<<__func__<<"-pixel_size\n"<<std::flush;
    cimg_library::CImg<int> scan_area_pixel(2);
    scan_area_pixel(0)=dimX/pixel_size;
    scan_area_pixel(1)=dimY/pixel_size;
    return initialise(margin_pixel,scan_area_pixel,dimX,dimY,dimZ);
  }//initialise

  //! set information for first  full image to store in NetCDF (or information file, not implemented yet !)
  bool set_first_full_image_information(cimg_library::CImg<int> &full_image,const int i=0,const int j=0,const int k=0)
  {
std::cerr<<__FILE__<<"/"<<__func__<<"\n"<<std::flush;
    if(i!=0||j!=0||k!=0) return false;
    ///full image size
#if version_cimg < 130
    full_image_size(0)=full_image.width;
    full_image_size(1)=full_image.height;
#else
    full_image_size(0)=full_image.width();
    full_image_size(1)=full_image.height();
#endif
    ///full image maximum
    cimg_library::CImg<> stat;
    stat=full_image.get_stats();
full_image.print("full_image");
stat.print("full_image stat");
    int x,y,z,v;
    //if( !full_image.contains(full_image.offset((int)stat(5)),x,y,z,v) )//set (x,y)
    if( !full_image.contains(full_image.data[(int)stat(5)],x,y,z,v) )//set (x,y)
    {
      std::cerr<<"Warning: maximum not in image !? -might be CImg library version error-\ndebug:\n"<<std::flush;
      std::cerr<<"@(x,y,z,v)=("<<x<<","<<y<<","<<z<<","<<v<<")\n"<<std::flush;
      full_image.print("full_image");
      full_image_maximum=cimg_library::cimg::type<Tvalue>::min();//absolute minimum value for the type
      full_image_maximum_position=-1;//dummy values
      ROI_origin=-1;
      return false;
    }
    full_image_maximum=stat(1);//max
    full_image_maximum_position(0)=x;
    full_image_maximum_position(1)=y;
    ///ROI rectangle
    if(ROI_origin(0)<0||ROI_origin(1)<0)
    {
      ROI_origin(0)=x-margin(0);
      ROI_origin(1)=y-margin(1);
    }
  }//set_first_full_image_information

  //! add sample data contribution to statistics
  // \see normalise
  bool add_sample(cimg_library::CImg<int> &image,const int i,const int j,const int k)
  {
//! \todo [high] do inplace add for mean
    tmp_mean+=image;
    tmp_count++;
//! \todo [very low] do also min, max, ...
    return true;
  }//add_sample

  //! normalise statistics for all added samples and set data at (i,j,k) postion.
  // \see add_sample
  bool normalise(const int i,const int j,const int k)
  {
    if(tmp_count<1) return false;
//! \todo [high] do inplace add for mean
    tmp_mean/=tmp_count;
#if version_cimg < 130
    ((*this)(k)).draw_image(tmp_mean,0,0,i,j);
#else
    ((*this)(k)).draw_image(0,0,i,j,tmp_mean);
#endif
    //reset mean for a next sum (e.g. setup following usage).
//! \todo [very high] put this reset to 0 back.
//    tmp_mean=0.0;tmp_count=0;
    return true;
  }//normalise

  //! maximum of the 5D data
  /**
   * return maximum value
  **/
  Tvalue maximum(int &x, int &y, int &X, int &Y, int &Z)
  {
    Tvalue max;
    //get stat for the 4D container
#if version_cimg < 130
    cimg_library::CImgList<> stat(this->size);
#else
    cimg_library::CImgList<> stat(this->size());
#endif
    cimglist_for((*this),l)
    {
      stat[l]=(*this)[l].get_stats();
//max=(*this)[l].max();
//std::cerr<<"Z="<<l<<"\nmax="<<max<<"\n"<<std::flush;
//(*this)[l].print("data");
//stat[l].print("stat");
    }
    //get max for the 5D container
    ///get Z max position and its value
    int Zmax=-1;
    max=cimg_library::cimg::type<Tvalue>::min();
    cimglist_for(stat,l) if( stat[l](1)>max ) {max=stat[l](1);Zmax=l;}//max for each CImg from list
    ///set (x,y,X,Y,Z) max position
    Z=Zmax;//set (l) i.e. (Z)
#if version_cimg < 130
    if( !((*this)[Zmax].contains((*this)[Zmax].data[(int)stat[Zmax](5)],x,y,X,Y)) )//set (x,y,z,v) i.e. (x,y,X,Y)
    {
      x=y=X=Y=-1;//dummy values
      return cimg_library::cimg::type<Tvalue>::min();//absolute minimum value for the type
    }
#else
    x=(int)stat[Zmax][8];y=(int)stat[Zmax][9];X=(int)stat[Zmax][10];Y=(int)stat[Zmax][11];//set (x,y,z,v) i.e. (x,y,X,Y)
#endif
std::cerr<<"max="<<max<<"@(x,y,X,Y,Z)=("<<x<<","<<y<<","<<X<<","<<Y<<","<<Z<<")\n"<<std::flush;
    return max;
  }//maximum
  //! maximum of the 5D data
  /**
   * return maximum value
  **/
  Tvalue maximum(cimg_library::CImg<int> &pos)
  {
    return maximum(pos(0),pos(1),pos(2),pos(3),pos(4));
  }//maximum

#ifdef cimg_use_netcdf
  //! save data in NetCDF format
  /** 
   *
   * @param[in] file_path: path of file to write
   *
   * @return 
   */
  virtual bool save_NetCDF(const std::string& file_path)
  {
#if cimg_debug>1
std::cerr<<this->class_name<<"::"<<__func__<<"("<<file_path<<")\n"<<std::flush;
#endif
    //dimension names
    vector<string> dim_names;
    dim_names.push_back(this->dimension_name[0]);//x
    dim_names.push_back(this->dimension_name[1]);//y
    dim_names.push_back(this->dimension_name[2]);//z
    dim_names.push_back(this->dimension_name[3]);//v
    string     dim_time=this->dimension_name[4]; //l
    //NetCDF for CImg
    ///for data: xyXY(Z)
    CImgNetCDF<Tvalue> cimgListTest4D;
    ///for flag: XY(Z), same size list
    cimg_library::CImgList<Tmap> imgList3D(flag.depth);
    CImgNetCDF<Tmap> cimgTest2D;
    cimglist_for(imgList3D,Z) imgList3D(Z)=flag.get_shared_plane(Z);
    ///for fail: XY(Z), same size list
    cimg_library::CImgList<Tmap> imgList3Dx(fail.depth);
    CImgNetCDF<Tmap> cimgTest2Dx;
    cimglist_for(imgList3Dx,Z) imgList3Dx(Z)=fail.get_shared_plane(Z);
//(*this).print("5D");
//imgList3D.print("3D");
//imgList3Dx.print("3D");
//! \todo [low] move to Cdata in saveNetCDFFile, addNetCDFheader, addNetCDFData for data only.
//! \todo [low] move to Cdata4scan in setNetCDFFile, setNetCDFheader, addNetCDFData for flag and fail only.
    ////file
    cout << "CImgNetCDF::saveNetCDFFile(" << file_path << ",...) return " 	<< cimgListTest4D.saveNetCDFFile((char*)file_path.c_str()) << endl;
    cout << "CImgNetCDF::setNetCDFFile(" << file_path << ",...) return " 	<< cimgTest2D.setNetCDFFile(cimgListTest4D.pNCFile) << endl;
    cout << "CImgNetCDF::setNetCDFFile(" << file_path << ",...) return " 	<< cimgTest2Dx.setNetCDFFile(cimgListTest4D.pNCFile) << endl;
    ////dim
    cout << "CImgNetCDF::addNetCDFDims(" << file_path << ",...) return " 	<< cimgListTest4D.addNetCDFDims((*this)[0],dim_names,dim_time) << endl;
    std::vector<NcDim*>vpNCDim;
    vpNCDim.push_back(cimgListTest4D.vpNCDim[2]);//X
    vpNCDim.push_back(cimgListTest4D.vpNCDim[3]);//Y
    cout << "CImgNetCDF::setNetCDFDims(" << file_path << ",...) return " 	<< cimgTest2D.setNetCDFDims(vpNCDim,cimgListTest4D.pNCDimt) << endl;
    cout << "CImgNetCDF::setNetCDFDims(" << file_path << ",...) return " 	<< cimgTest2Dx.setNetCDFDims(vpNCDim,cimgListTest4D.pNCDimt) << endl;
    ////var
    cout << "CImgNetCDF::addNetCDFVar(" << file_path << ",...) return " 	<< cimgListTest4D.addNetCDFVar((*this)[0],this->component_name[0],this->unit_name[0]) << endl;
////intensity attribute{
//! \todo put intensity attribute in a function (so it can be use either here (data+attribute) or for .cimg) in PURE NetCDF.
    {//maximum position as attribute
    CImg<int> max_position(5);max_position=-1;
    //maximum
    cimgListTest4D.pNCvar[0].add_att("maximum",this->maximum(max_position));
    cimgListTest4D.pNCvar[0].add_att("maximum_unit",this->unit_name[0].size(),this->unit_name[0].c_str());
    //maximum position
    cimgListTest4D.pNCvar[0].add_att("maximum_position",max_position.size(),&max_position.data[0]);
    cimgListTest4D.pNCvar[0].add_att("maximum_position_unit",5,"pixel");
    //maximum position order
    std::string axis_order="(";for(int i=0;i<this->dimension_name.size()-1;++i) axis_order+=this->dimension_name[i]+","; axis_order+=this->dimension_name[this->dimension_name.size()-1]+")";
    cimgListTest4D.pNCvar[0].add_att("maximum_position_order",axis_order.size(),axis_order.c_str());
    }//maximum position as attribute
    if( (full_image_size(0)!=(*this)[0].width)||(full_image_size(1)!=(*this)[0].height) )
    {//full image informations as attribute
    //image size
    cimgListTest4D.pNCvar[0].add_att("full_image_size",this->full_image_size.width,&(this->full_image_size.data[0]));
    //image size order
    std::string axis_order="("+this->dimension_name[0]+","+this->dimension_name[1]+")";
    cimgListTest4D.pNCvar[0].add_att("full_image_dimension_order",axis_order.size(),axis_order.c_str());
    //maximum
    cimgListTest4D.pNCvar[0].add_att("first_full_image_maximum",full_image_maximum);
    cimgListTest4D.pNCvar[0].add_att("first_full_image_maximum_unit",this->unit_name[0].size(),this->unit_name[0].c_str());
    cimgListTest4D.pNCvar[0].add_att("first_full_image_maximum_position",this->full_image_maximum_position.width,&(this->full_image_maximum_position.data[0]));
    cimgListTest4D.pNCvar[0].add_att("first_full_image_maximum_position_unit",5,"pixel");
    //ROI
    cimg_library::CImg<int> ROI_rectangle;//4C (x0,y0,x1,y1)
    ROI_rectangle=CImg<int>::vector(ROI_origin(0),ROI_origin(1),(*this)[0].width+ROI_origin(0)-1,(*this)[0].height+ROI_origin(1)-1);
    cimgListTest4D.pNCvar[0].add_att("region_of_interest_rectangle",ROI_rectangle.height,&(ROI_rectangle.data[0]));
    cimgListTest4D.pNCvar[0].add_att("region_of_interest_rectangle_unit",5,"pixel");
    //ROI coordinate order
    std::string rectangle_order="("+this->dimension_name[0]+"0,"+this->dimension_name[1]+"0, "+this->dimension_name[0]+"1,"+this->dimension_name[1]+"1)";
    cimgListTest4D.pNCvar[0].add_att("region_of_interest_rectangle_order",rectangle_order.size(),rectangle_order.c_str());
    }//full image informations as attribute
////}intensity attribute
    cout << "CImgNetCDF::addNetCDFVar(" << file_path << ",...) return " 	<< cimgTest2D.addNetCDFVar(imgList3D[0],flag_name,flag_unit_name) << endl;
    cout << "CImgNetCDF::addNetCDFVar(" << file_path << ",...) return " 	<< cimgTest2Dx.addNetCDFVar(imgList3Dx[0],fail_name,fail_unit_name) << endl;
    ////data
    cimglist_for((*this),l)
    {
      cout << "CImgNetCDF::addNetCDFData(" << file_path << ",...) return " 	<< cimgListTest4D.addNetCDFData((*this)[l]) << endl;
      cout << "CImgNetCDF::addNetCDFData(" << file_path << ",...) return " 	<< cimgTest2D.addNetCDFData(imgList3D[l],cimgListTest4D.loadDimTime()-1) << endl;
      cout << "CImgNetCDF::addNetCDFData(" << file_path << ",...) return " 	<< cimgTest2Dx.addNetCDFData(imgList3Dx[l],cimgListTest4D.loadDimTime()-1) << endl;
    }
    return true;
  }//save_NetCDF
#endif //cimg_use_netcdf

};//Cdata4scan class

#endif //DATA4SCAN

