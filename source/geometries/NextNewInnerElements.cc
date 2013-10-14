// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 18 Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewInnerElements.h"
#include "NextNewEnergyPlane.h"
#include "NextNewFieldCage.h"
#include "NextNewTrackingPlane.h"
#include "MaterialsList.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Region.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>

namespace nexus {

  NextNewInnerElements::NextNewInnerElements():
    BaseGeometry()
  {
    // Build the internal objects that live there 
    _energy_plane = new NextNewEnergyPlane();
    _field_cage = new NextNewFieldCage();
    _tracking_plane = new NextNewTrackingPlane();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("vis", _visibility, "Grids Visibility");
    
    G4GenericMessenger::Command& step_cmd = _msg->DeclareProperty("max_step_size", _max_step_size, "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");

  }
   
  void NextNewInnerElements::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextNewInnerElements::Construct()
  {
    // Reading material
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();
    //INNER ELEMENTS
    _energy_plane->SetLogicalVolume(_mother_logic);
    _energy_plane->Construct();
    _field_cage->SetLogicalVolume(_mother_logic);
    _field_cage->Construct();
    _tracking_plane->SetLogicalVolume(_mother_logic);
    _tracking_plane->Construct();
  }

  NextNewInnerElements::~NextNewInnerElements()
  {
    delete _energy_plane;
    delete _field_cage;    
    delete _tracking_plane;
  }
  
  G4ThreeVector NextNewInnerElements::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);
    if (region == "CENTER") {
      return vertex;
    }
    // INNER ELEMENTS regions
    else if ( (region == "CARRIER_PLATE") || (region == "ENCLOSURE_BODY") ||
	      (region == "ENCLOSURE_WINDOW") || (region=="OPTICAL_PAD") || 
	      (region == "PMT_BODY") ) {
      vertex = _energy_plane->GenerateVertex(region);
    }
    else if ( (region == "FIELD_CAGE") || (region== "REFLECTOR") || 
	      (region == "ACTIVE") ) {
      vertex=_field_cage->GenerateVertex(region);
    }
    else if ( (region == "SUPPORT_PLATE") || (region == "DICE_BOARD") ) {
      vertex = _tracking_plane->GenerateVertex(region);
    }
    return vertex;
  }
}//end namespace nexus
