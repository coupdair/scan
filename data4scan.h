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
    this->component_name.push_back("intensity");
    this->unit_name.push_back("none");
    ///set dimendion names
    this->dimension_name.push_back("x");//image width
    this->dimension_name.push_back("y");//image height
    this->dimension_name.push_back("X");//scanning width
    this->dimension_name.push_back("Y");//scanning height
    this->dimension_name.push_back("Z");//scanning depth
#endif //cimg_use_netcdf
  }//constructor

/*{base class* /
  //! sample type as last dimension
  //inline bool sample_type()   {return sample_type;}
  //! component type as last dimension
  inline bool component_type(){return !sample_type;}
/*}base class* /

  //! load data
  /** 
   *
   * @param[in] file_path: path of file to read
   *
   * @return 
   */
  bool load(const std::string& file_path)
  {
#if cimg_debug>1
std::cerr<<this->class_name<<"::"<<__func__<<"("<<file_path<<")\n"<<std::flush;
#endif
    cimg_library::CImgList<Tvalue>load(file_path.c_str());
    return true;
  }//load

  //! load data
  /** 
   *
   * @param[in] file_path: path of file to write
   *
   * @return 
   */
//! \todo put this only in Cdata as virtual and add new save_NetCDF as virtual too.
  bool save(const std::string& file_path)
  {
#if cimg_debug>1
std::cerr<<this->class_name<<"::"<<__func__<<"("<<file_path<<")\n"<<std::flush;
#endif
    //get file name extention
#if version_cimg < 130
    const char *ext = cimg::filename_split(file_path.c_str());
#else
    const char *ext = cimg::split_filename(file_path.c_str());
#endif //version_cimg
    if (!cimg::strncasecmp(ext,"nc",2))
    {//NetCDF
#ifdef cimg_use_netcdf
      //dimension names
      vector<string> dim_names;
      dim_names.push_back(this->dimension_name[0]);//x
      dim_names.push_back(this->dimension_name[1]);//y
      dim_names.push_back(this->dimension_name[2]);//z
      dim_names.push_back(this->dimension_name[3]);//v
      string     dim_time=this->dimension_name[4]; //l
      //variable names
/** /
      ///single
      string var_name="flag";
      ///list
      vector<string> var_names;
      var_names.push_back("gray_level");
      //unit names
      ///single
      string unit_name="none; 0=outlier, 1=validated";
      ///list
      vector<string> unit_names;
      unit_names.push_back("none; range=[0-255]");
*/
      CImgNetCDF<float> cimgListTest4D;
/*
      //CImgList<int> imgList3D(nZ,nz,nv);//i.e. XYZ, same size list
      //CImgNetCDF<int> cimgTest2D;
(*this).print("5D");
//imgList3D.print("3D");
/*move to Cdata*/
      ////file
      cout << "CImgNetCDF::saveNetCDFFile(" << file_path << ",...) return " 	<< cimgListTest4D.saveNetCDFFile((char*)file_path.c_str()) << endl;
      //cout << "CImgNetCDF::setNetCDFFile(" << file_path << ",...) return " 	<< cimgTest2D.setNetCDFFile(cimgListTest4D.pNCFile) << endl;
      ////dim
      cout << "CImgNetCDF::addNetCDFDims(" << file_path << ",...) return " 	<< cimgListTest4D.addNetCDFDims((*this)[0],dim_names,dim_time) << endl;
      //std::vector<NcDim*>vpNCDim;
      //vpNCDim.push_back(cimgListTest4D.vpNCDim[2]);//X
      //vpNCDim.push_back(cimgListTest4D.vpNCDim[3]);//Y
      //cout << "CImgNetCDF::setNetCDFDims(" << file_path << ",...) return " 	<< cimgTest2D.setNetCDFDims(vpNCDim,cimgListTest4D.pNCDimt) << endl;
      ////var
      cout << "CImgNetCDF::addNetCDFVar(" << file_path << ",...) return " 	<< cimgListTest4D.addNetCDFVar((*this)[0],this->component_name[0],this->unit_name[0]) << endl;
      //cout << "CImgNetCDF::addNetCDFVar(" << file_path << ",...) return " 	<< cimgTest2D.addNetCDFVar(imgList3D[0],var_name,unit_name) << endl;
      ////data
      cimglist_for((*this),l)
      {
        cout << "CImgNetCDF::addNetCDFData(" << file_path << ",...) return " 	<< cimgListTest4D.addNetCDFData((*this)[l]) << endl;
      //  cout << "CImgNetCDF::addNetCDFData(" << file_path << ",...) return " 	<< cimgTest2D.addNetCDFData(imgList3D[l],cimgListTest4D.loadDimTime()-1) << endl;
      }
#else //cimg_use_netcdf
      std::cerr<<"error: current compiled option do not handle NetCDF (see -I option: cimg_use_netcdf not defined) in "<<this->class_name<<"::"<<__func__<<"("<<file_path<<") function using CImg.v"<<cimg_version<<" version.\n"<<std::flush;
      return false;
#endif //!cimg_use_netcdf
    }//NetCDF
    else
    {//other format
      cimg_library::CImgList<Tvalue>::save(file_path.c_str());
    }//other format
    return true;
  }//save

};//Cdata4scan class

#endif //DATA4SCAN

