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
    this->component_name.clear();
    this->component_name.push_back("intensity");
    this->unit_name.push_back("none");
    ///set dimendion names
    this->dimension_name.clear();
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
max=(*this)[l].max();
std::cerr<<"Z="<<l<<"\nmax="<<max<<"\n"<<std::flush;
(*this)[l].print("data");
stat[l].print("stat");
    }
    //get max for the 5D container
    ///get Z max position and its value
    int Zmax=-1;
    max=stat[0](1);
    cimglist_for(stat,l) if( stat[l](1)>max ) {max=stat[l](1);Zmax=l;}
    const int dv=(*this)[0].dim;
    const int dz=(*this)[0].depth;
    const int dy=(*this)[0].height;
    const int dx=(*this)[0].width;
    const int pos=stat[Zmax](5);//position as offset
    ///set (x,y,X,Y,Z) max position
    Z=Zmax;//l Z
    Y=pos/(dz*dy*dx);//v Y
    X=-1;//(pos-(Y*dz*dy*dx))/dy*dx;//z X
    y=-1;//(int)(stat(5))-X*this->dimy();//y y
    x=-1;//(int)(stat(5))-(y+X)//x x
std::cerr<<"max="<<75<<"@(x,y,X,Y,Z)=("<<35<<","<<32<<","<<2<<","<<3<<","<<4<<")\n"<<std::flush;//current example
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
//! \todo . add maximum position as attribute
    {//maximum position as attribute
    CImg<int> max_position(5);max_position=-1;
this->maximum(max_position);
    cimgListTest4D.pNCvar[0].add_att("maximum",123);//NcVar *pNCvar;
    }//maximum position as attribute
    //cout << "CImgNetCDF::addNetCDFVar(" << file_path << ",...) return " 	<< cimgTest2D.addNetCDFVar(imgList3D[0],var_name,unit_name) << endl;
    ////data
    cimglist_for((*this),l)
    {
      cout << "CImgNetCDF::addNetCDFData(" << file_path << ",...) return " 	<< cimgListTest4D.addNetCDFData((*this)[l]) << endl;
    //  cout << "CImgNetCDF::addNetCDFData(" << file_path << ",...) return " 	<< cimgTest2D.addNetCDFData(imgList3D[l],cimgListTest4D.loadDimTime()-1) << endl;
    }
    return true;
  }//save_NetCDF
#endif //cimg_use_netcdf

};//Cdata4scan class

#endif //DATA4SCAN

