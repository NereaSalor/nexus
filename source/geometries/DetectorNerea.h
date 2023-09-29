// ----------------------------------------------------------------------------
// nexus | DetectorNerea.h
//
// General-purpose cylindric chamber.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef DETECTOR_NEREA_H
#define DETECTOR_NEREA_H

#include "GeometryBase.h"
#include <G4RotationMatrix.hh>

class G4GenericMessenger;

namespace nexus {class CylinderPointSampler2020;}


namespace nexus {

  class DetectorNerea: public GeometryBase
  {
  public:
    /// Constructor
    DetectorNerea();
    /// Destructor
    ~DetectorNerea();

    /// Return vertex within region <region> of the chamber
    virtual G4ThreeVector GenerateVertex(const G4String& region) const;

    virtual void Construct();

  private:
    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Vertex Generator
    CylinderPointSampler2020* lab_gen_;

    // Visibility of the geometry
    G4bool visibility_;

    //SiPM
    G4RotationMatrix* sipm_rot_;
    G4double rot_angle_;

  };

} // end namespace nexus

#endif
