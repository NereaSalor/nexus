// ----------------------------------------------------------------------------
// nexus | DetectorNerea.cc
//
// General-purpose cylindric chamber.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DetectorNerea.h"

#include "SiPMNerea.h"
#include "NextNewKDB.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "CylinderPointSampler2020.h"
#include "IonizationSD.h"
#include "SensorSD.h"
#include "FactoryBase.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>


namespace nexus {

  REGISTER_CLASS(DetectorNerea, GeometryBase)

  using namespace CLHEP;

  DetectorNerea::DetectorNerea():
    GeometryBase(), msg_(0), visibility_(true)
  {
  }

  //Create a vertex generator for a cilinder


  DetectorNerea::~DetectorNerea()
  {
    delete msg_;
  }



  void DetectorNerea::Construct()
  {
    // CHAMBER ///////////////////////////////////////////////////////

    const G4double chamber_diam   =  10. * cm;
    const G4double chamber_length = 50. * cm;
    const G4double chamber_thickn =   1. * cm;

    G4Tubs* chamber_solid =
      new G4Tubs("CHAMBER", 0., (chamber_diam/2. + chamber_thickn),
        (chamber_length/2. + chamber_thickn), 0., twopi);

    G4LogicalVolume* chamber_logic =
      new G4LogicalVolume(chamber_solid, materials::Steel(), "CHAMBER");

    this->SetLogicalVolume(chamber_logic);

    // Chamber visibility
    if (visibility_) {
      G4VisAttributes active_col_cham = nexus::Red();
      //To paint all the structure, not edges only
      //active_col_cham.SetForceSolid(true);
      chamber_logic->SetVisAttributes(active_col_cham);
    } else {
      chamber_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }


    // GAS ///////////////////////////////////////////////////////////
    const G4double gas_diam = chamber_diam - 2*chamber_thickn;
    const G4double gas_length = chamber_length - 2*chamber_thickn;
    const G4double sipm_depth   =  0.8 * mm;
    G4Tubs* gas_solid =
      new G4Tubs("GAS", 0., gas_diam/2., gas_length/2., 0., twopi);


    // Krypton as detector gas

    //density = 3.708*mg/cm3;
    //a = 83.*g/mole;
    //G4Material* gKr  = new G4Material(name="Kr",z=38., a, density );

    G4Material* gxe = materials::GXe(10.*bar);
    gxe->SetMaterialPropertiesTable(opticalprops::GXe(10.*bar, 303));

    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic, "GAS",
		      chamber_logic, false, 0, true);

    // Gas Visibility
    if (visibility_) {
      G4VisAttributes active_col_gas = nexus::LightBlue();
      //To paint all the structure, not edges only
      active_col_gas.SetForceSolid(true);
      gas_logic->SetVisAttributes(active_col_gas);
    } else {
      gas_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    //SENSITIVE DETECTORS /////////////////////////////////////////////

    G4Tubs* active_solid =
      new G4Tubs("ACTIVE", 0., gas_diam/2., gas_length/2., 0, twopi);

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, gxe, "ACTIVE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), active_logic, "ACTIVE",
                      gas_logic, false, 0, true);

    IonizationSD* sensdet = new IonizationSD("/DETECTOR_NEREA/ACTIVE");
    active_logic->SetSensitiveDetector(sensdet);
    G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);


    //PHOTOMULTIPLIER //////////////////////////////////////////////

    // Added photomultiplier from other file and inserted 4 inside the detector

    SiPMNerea sipm_geom;
    sipm_geom.Construct();
    G4LogicalVolume * sipm_logic = sipm_geom.GetLogicalVolume();

    const G4double sipm_z_pos = (-gas_length + 2*sipm_depth + 2*chamber_thickn)/2.;
    //Sin rotar la geometría
    //new G4PVPlacement(0, G4ThreeVector(chamber_diam/4., 0., sipm_z_pos),
      //                sipm_logic, "SIPM", chamber_logic, false, 0, true);

    //////PARA ROTAR EL SIPM PARA QUE LA VENTANA ACTIVA ESTÉ HACIA EL GAS
    sipm_rot_ = new G4RotationMatrix();
    rot_angle_ = pi;
    sipm_rot_->rotateY(rot_angle_);
    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(chamber_diam/4., 0., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 0, false);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(-chamber_diam/4., 0., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 1, false);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(-chamber_diam/6., 0., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 2, false);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(chamber_diam/6., 0., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 3, false);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(chamber_diam/10., 0., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 4, false);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(-chamber_diam/10., 0., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 5, false);


    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(0., chamber_diam/4., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 6, true);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(0., -chamber_diam/4., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 7, true);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(0., chamber_diam/6., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 8, true);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(0., -chamber_diam/6., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 9, true);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(0., chamber_diam/10., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 10, true);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(0., -chamber_diam/10., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 11, true);

    new G4PVPlacement(G4Transform3D(*sipm_rot_, G4ThreeVector(0., 0., sipm_z_pos)),
                      sipm_logic, "SIPM", active_logic, false, 12, true);


    // Photomultiplier visibility ///
    if (visibility_) {
      G4VisAttributes active_col_sipm = nexus::Yellow();
      sipm_logic->SetVisAttributes(active_col_sipm);
    } else {
      sipm_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }


    /// VERTEX GENERATORS

    lab_gen_ =
      new CylinderPointSampler2020(0, gas_diam/2., gas_length/2., 0, twopi, 0,
                                G4ThreeVector(0., 0., 0.));
  }

  G4ThreeVector DetectorNerea::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    if (region == "GAS") {
      vertex = lab_gen_->GenerateVertex("VOLUME");
    }
    if (region == "FRONT_SIPM"){
      const G4double chamber_diam   =  10. * cm;
      G4ThreeVector front_sipm_point = G4ThreeVector(0, 0., -18);
      vertex = front_sipm_point;
    }

    return vertex;
  }

} // end namespace nexus

// MY PHOTOMULTIPLIER ///////////////////////////////////////////////

//const G4double sipm_x = 2.4 * mm;
//const G4double sipm_y = 2.4 * mm;
//const G4double sipm_z = 0.8 * mm;

//G4Box * sipm_solid = new G4Box("SiPM", sipm_x/2., sipm_y/2., sipm_z/2);

//G4Material * epoxy = materials::Epoxy();
//epoxy->SetMaterialPropertiesTable(opticalprops::GlassEpoxy());

//G4LogicalVolume * sipm_logic = new G4LogicalVolume(sipm_solid, epoxy, "SiPM");

//new G4PVPlacement(0, G4ThreeVector(sipm_x + 0.2, 0., -chamber_length/2. + sipm_z/2),
  //                sipm_logic, "SiPM", chamber_logic, false, 0, true);

//new G4PVPlacement(0, G4ThreeVector(-(sipm_x + 0.2), 0., -chamber_length/2. + sipm_z/2),
  //                sipm_logic, "SiPM", chamber_logic, false, 1, true);

//new G4PVPlacement(0, G4ThreeVector(0., sipm_y + 0.2, -chamber_length/2. + sipm_z/2),
  //                sipm_logic, "SiPM", chamber_logic, false, 2, true);

//new G4PVPlacement(0, G4ThreeVector(0., -(sipm_y + 0.2), -chamber_length/2. + sipm_z/2),
  //                sipm_logic, "SiPM", chamber_logic, false, 3, true);


// Active windows

//const G4double active_x = 1.   * mm;
//const G4double active_y = 1.   * mm;
//const G4double active_z = 0.01 * mm;

//G4Box * Sipm_active_solid =
//new G4Box("PHOTODIODES", active_x/2., active_y/2., active_z/2.);

//G4Material * silicon = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

//G4LogicalVolume * Sipm_active_logic =
//new G4LogicalVolume(Sipm_active_solid, silicon, "PHOTODIODES");

//new G4PVPlacement(0, G4ThreeVector(sipm_x + 0.2, 0., -chamber_length/2. + sipm_z/2),
//                                 Sipm_active_logic, "PHOTODIODES", chamber_logic, false, 0, false);

//new G4PVPlacement(0, G4ThreeVector(-(sipm_x + 0.2), 0., -chamber_length/2. + sipm_z/2),
//                                 Sipm_active_logic, "PHOTODIODES", chamber_logic, false, 1, false);

//new G4PVPlacement(0, G4ThreeVector(0., sipm_y + 0.2, -chamber_length/2. + sipm_z/2),
//                                 Sipm_active_logic, "PHOTODIODES", chamber_logic, false, 2, false);

//new G4PVPlacement(0, G4ThreeVector(0., -(sipm_y + 0.2), -chamber_length/2. + sipm_z/2),
//                                 Sipm_active_logic, "PHOTODIODES", chamber_logic, false, 3, false);
