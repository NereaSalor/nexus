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

class G4GenericMessenger;


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
  };

} // end namespace nexus

#endif
