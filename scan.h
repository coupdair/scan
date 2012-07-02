#ifndef SCAN
#define SCAN

#include <sstream>
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
class Cscan
{
public:
  //! class name for debug only
#if cimg_debug>1
  std::string class_name;
#endif
  //! class (or library) version for information only
  std::string class_version;
private:
  //! grab device

  //! stepper device

public:
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

//attributes:
//grab
//stepper
//data

//functions:
///image_file_name
///record_images
///scanning
////scanning_force
////scanning_lazy

};//Cscan class

#endif //SCAN

