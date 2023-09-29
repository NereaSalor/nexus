// ----------------------------------------------------------------------------
// nexus | SiPMNerea.h
//
// Geometry of a Hamamatsu 1x1 mm2 SiPM.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_NEREA
#define SIPM_NEREA

#include "GeometryBase.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {

  class SiPMNerea: public GeometryBase
  {
  public:
    /// Constructor
    SiPMNerea();
    /// Destructor
    ~SiPMNerea();

    /// Return dimensions of the SiPM
    G4ThreeVector GetDimensions() const;

    /// Invoke this method to build the volumes of the geometry
    void Construct();

  private:
    G4ThreeVector dimensions_; ///< external dimensions of the SiPMNerea

    // Visibility of the tracking plane
    G4bool visibility_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };


} // end namespace nexus

#endif
