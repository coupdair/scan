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
/*{base class* /
  //! class name for debug only
#if cimg_debug>1
  std::string class_name;
#endif
  //! class (or library) version for information only
  std::string class_version;
  //! sample type as last dimension (if false it is component type)
  bool sample_type;//!component type
#ifdef cimg_use_netcdf
  //! data information: 
  std::vector<std::string> component_name;
  std::vector<std::string> unit_name;
  //! data information
  std::vector<std::string> dimension_name;
#endif //cimg_use_netcdf
/*}base class*/

///scan (copy in scan)
  cimg_library::CImg<Tmap> flag;//3D (X,Y,Z) 0 then red=0, green=1
  cimg_library::CImg<Tmap> fail;//3D (X,Y,Z) 0 then increment++
#ifdef cimg_use_netcdf
  //! map ad fail information: 
  std::string flag_name;
  std::string flag_unit_name;
  std::string fail_name;
  std::string fail_unit_name;
#endif //cimg_use_netcdf

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

/*{base class* /
  //! sample type as last dimension
  //inline bool sample_type()   {return sample_type;}
  //! component type as last dimension
  inline bool component_type(){return !sample_type;}
/*}base class*/

  //! assign and fill
  bool initialise(const int width,const int height,const int dimX,const int  dimY,const int dimZ)
  {
    this->assign(dimZ,width,height,dimX,dimY);
    cimglist_for((*this),Z) (*this)(Z).fill(-1);
    flag.assign(dimX,dimY,dimZ);
    flag=0;
    fail.assign(dimX,dimY,dimZ);
    fail=0;
CImg<Tmap> hop=CImg<Tmap>::vector(1,2,3);
hop.print("vector");
std::cerr<<"hop(0)="<<hop(0)<<" hop(1)="<<hop(1)<<" hop(2)="<<hop(2)<<"\n"<<std::flush;
    return true;
  }//initialise

  //! maximum of the 5D data
  /**
   * return maximum value
  **/
  Tvalue maximum(int &x, int &y, int &X, int &Y, int &Z)
  {
    Tvalue max;
    //get stat for the 4D container
    CImgList<> stat(this->size);
    cimglist_for((*this),l)
    {
      stat[l]=(*this)[l].get_stats();
/*
max=(*this)[l].max();
std::cerr<<"Z="<<l<<"\nmax="<<max<<"\n"<<std::flush;
(*this)[l].print("data");
stat[l].print("stat");
*/
    }
    //get max for the 5D container
    ///get Z max position and its value
    int Zmax=-1;
    max=cimg::type<Tvalue>::min();
    cimglist_for(stat,l) if( stat[l](1)>max ) {max=stat[l](1);Zmax=l;}//max for each CImg from list
    ///set (x,y,X,Y,Z) max position
    Z=Zmax;//set (l) i.e. (Z)
    if( !((*this)[Zmax].contains((*this)[Zmax].data[(int)stat[Zmax](5)],x,y,X,Y)) )//set (x,y,z,v) i.e. (x,y,X,Y)
    {
      x=y=X=Y=-1;//dummy values
      return cimg::type<Tvalue>::min();//absolute minimum value for the type
    }
std::cerr<<"max="<<max<<"@(x,y,X,Y,Z)=("<<x<<","<<y<<","<<X<<","<<Y<<","<<Z<<")\n"<<std::flush;
    return max;
  }//maximum
  //! maximum of the 5D data
  /**
   * return maximum value
  **/
  Tvalue maximum(CImg<int> &pos)
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
    CImgNetCDF<float> cimgListTest4D;//for data: xyXY(Z)
    CImgList<int> imgList3D(flag.depth);//for flag: XY(Z), same size list
    CImgNetCDF<int> cimgTest2D;
    cimglist_for(imgList3D,Z) imgList3D(Z)=flag.get_shared_plane(Z);
    CImgList<int> imgList3Dx(fail.depth);//for fail: XY(Z), same size list
    CImgNetCDF<int> cimgTest2Dx;
    cimglist_for(imgList3Dx,Z) imgList3Dx(Z)=fail.get_shared_plane(Z);
(*this).print("5D");
imgList3D.print("3D");
imgList3Dx.print("3D");
/*move to Cdata*/
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
    {//maximum position as attribute
    CImg<int> max_position(5);max_position=-1;
    //maximum
    cimgListTest4D.pNCvar[0].add_att("maximum",this->maximum(max_position));
    //maximum position
    cimgListTest4D.pNCvar[0].add_att("maximum_position",max_position.size(),&max_position.data[0]);
    //maximum position order
    std::string axis_order="(";for(int i=0;i<this->dimension_name.size()-1;++i) axis_order+=this->dimension_name[i]+","; axis_order+=this->dimension_name[this->dimension_name.size()-1]+")";
    cimgListTest4D.pNCvar[0].add_att("maximum_position_order",axis_order.size(),axis_order.c_str());
    }//maximum position as attribute
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

