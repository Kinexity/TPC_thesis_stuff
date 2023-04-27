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
#include <filesystem>

#include "G4ThreeVector.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"

class G4VPhysicalVolume;
class F02ElectricFieldSetup;

class OTPCDetectorConstruction : public G4VUserDetectorConstruction
{
public:
	OTPCDetectorConstruction() = default;
	OTPCDetectorConstruction(G4double crystD, std::string scintT);
	~OTPCDetectorConstruction() = default;
	G4VPhysicalVolume* Construct();
	const G4double getCrystalDepth();
	const std::string getScintillatorType();
	void saveDetails(std::filesystem::path p);
private:
	F02ElectricFieldSetup* fEmFieldSetup;
	G4String              header1, header2, header3;
	G4int                 gas[3];
	G4double              fgas[3];
	G4double              T;
	G4double              P;
	G4double              E;
	G4double              d;


	G4double crystalDepth = 10 * cm;
	std::string scintillatorType = "CeBr3";
};

#endif

