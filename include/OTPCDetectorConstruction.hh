// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: ExN01DetectorConstruction.hh,v 1.2 1999/12/15 14:49:18 gunter Exp $
// GEANT4 tag $Name: geant4-02-00 $
//
//////////////////////////////////////////////////////////////////////////
//
// V. Guadilla 2021
// Simple OTPC detector geometry

#ifndef OTPCDetectorConstruction_h
#define OTPCDetectorConstruction_h 1

// STL //
#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "G4ThreeVector.hh"
#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class F02ElectricFieldSetup;

class OTPCDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    OTPCDetectorConstruction();
    ~OTPCDetectorConstruction();

  public:
     G4VPhysicalVolume* Construct();
     
  private:
      F02ElectricFieldSetup* fEmFieldSetup;
      G4String              header1, header2, header3;
      G4int                 gas[3];
      G4double              fgas[3];
      G4double              T;
      G4double              P;
      G4double              E;
      G4double              d;


};

#endif

