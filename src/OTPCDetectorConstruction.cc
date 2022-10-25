// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
/////////////////////////////////////////////////////////////////////////
//
// V.Guadilla 2019
//
////////////////////////////////////////////////////////////////////////

#include "OTPCDetectorConstruction.hh"

// GEANT4 //
#include "globals.hh"
#include "G4Transform3D.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include <G4UnionSolid.hh>
//#include "TMath.h"
#include "G4Polycone.hh"

#include "F02ElectricFieldSetup.hh"
#include "fstream"

using namespace std;

// Global file with the events
ifstream geoInputFile;

OTPCDetectorConstruction::OTPCDetectorConstruction()
{ }

OTPCDetectorConstruction::~OTPCDetectorConstruction()
{ }

G4VPhysicalVolume* OTPCDetectorConstruction::Construct()
{

	//----------------------------------------------------
	// Materials definitions
	//----------------------------------------------------

	G4String name, symbol;             //a=mass of a mole;
	G4double a, z, density;            //z=mean number of protons;  
	G4int ncomponents, natoms;
	G4double fractionmass;
	G4double abundance;
	G4State state;
	G4double pressure;
	G4double temperature;

	G4int iz, n;                       //iz=number of protons  in an isotope; 
 //                                      // n=number of nucleons in an isotope;

 //
 // define Elements
 //

   // H
	G4Element* H = new G4Element("Hydrogen", "H", z = 1., a = 1.01 * g / mole);

	// He
	G4Isotope* He3 = new G4Isotope("3He", 2, 3, 3.016029 * g / mole);
	G4Isotope* He4 = new G4Isotope("4He", 2, 4, 4.002603 * g / mole);
	G4Element* He = new G4Element("Helium", "He", 2);
	He->AddIsotope(He3, 0.000137 * perCent);
	He->AddIsotope(He4, 99.999863 * perCent);

	// O
	G4Element* O = new G4Element("Oxygen", "O", z = 8., a = 16.00 * g / mole);

	// B
	a = 10.811 * g / mole;
	G4Element* B = new G4Element(name = "Boron", symbol = "B", z = 5., a);

	// Al
	a = 26.982 * g / mole;
	G4Element* Al = new G4Element(name = "Aluminium", symbol = "Al", z = 13., a);

	// C
	G4Isotope* C12 = new G4Isotope("12C", 6, 12, 12 * g / mole);
	G4Isotope* C13 = new G4Isotope("13C", 6, 13, 13.003355 * g / mole);
	G4Element* C = new G4Element("Carbon", "C", 2);
	C->AddIsotope(C12, 98.93 * perCent);
	C->AddIsotope(C13, 1.07 * perCent);

	// Si
	a = 28.086 * g / mole;
	G4Element* Si = new G4Element(name = "Silicium", symbol = "Si", z = 14., a);

	// K
	a = 39.098 * g / mole;
	G4Element* K = new G4Element(name = "Potasium", symbol = "K", z = 19., a);

	// Cr
	a = 51.996 * g / mole;
	G4Element* Cr = new G4Element(name = "Chromium", symbol = "Cr", z = 24., a);

	// Fe
	a = 55.845 * g / mole;
	G4Element* Fe = new G4Element(name = "Iron", symbol = "Fe", z = 26., a);

	// Ni
	a = 58.693 * g / mole;
	G4Element* Ni = new G4Element(name = "Nickel", symbol = "Ni", z = 28., a);

	// Sb
	a = 121.76 * g / mole;
	G4Element* Sb = new G4Element(name = "Antimony", symbol = "Sb", z = 51., a);

	// Ti
	a = 47.867 * g / mole;
	G4Element* Ti = new G4Element(name = "Titanium", symbol = "Ti", z = 22., a);

	// Ar
	a = 39.95 * g / mole;
	G4Element* Ar = new G4Element(name = "Argon", symbol = "Ar", z = 18., a);

	// Cl
	a = 35.453 * g / mole;
	G4Element* Cl = new G4Element(name = "Chlorine", symbol = "Cl", z = 17., a);

	//F
	a = 18.998 * g / mole;
	G4Element* F = new G4Element("Fluorine", "F", z = 9., a);


	// N
	G4Isotope* N14 = new G4Isotope("14N", 7, 14, 14.003 * g / mole);
	G4Isotope* N15 = new G4Isotope("15N", 7, 15, 15.000 * g / mole);
	G4Element* N = new G4Element("Nitrogen", "N", 2);
	N->AddIsotope(N14, 99.632 * perCent);
	N->AddIsotope(N15, 0.368 * perCent);


	// Cs
	G4Isotope* Cs133 = new G4Isotope(name = "Cs133", iz = 55, n = 133, a = 132.905 * g / mole);
	G4Element* Cs = new G4Element(name = "Cesium", symbol = "Cs", ncomponents = 1);
	Cs->AddIsotope(Cs133, 100 * perCent);

	// Cu
	G4Isotope* Cu63 = new G4Isotope("63Cu", 29, 63, 62.929601 * g / mole);
	G4Isotope* Cu65 = new G4Isotope("65Cu", 29, 65, 64.927794 * g / mole);
	G4Element* Cu = new G4Element("Copper", "Cu", 2);
	Cu->AddIsotope(Cu63, 69.17 * perCent);
	Cu->AddIsotope(Cu65, 30.83 * perCent);

	// Ge
	G4Isotope* Ge70 = new G4Isotope(name = "Ge70", iz = 32, n = 70, a = 69.924250 * g / mole);
	G4Isotope* Ge72 = new G4Isotope(name = "Ge72", iz = 32, n = 72, a = 71.922076 * g / mole);
	G4Isotope* Ge73 = new G4Isotope(name = "Ge73", iz = 32, n = 73, a = 72.923459 * g / mole);
	G4Isotope* Ge74 = new G4Isotope(name = "Ge74", iz = 32, n = 74, a = 73.921178 * g / mole);
	G4Isotope* Ge76 = new G4Isotope(name = "Ge76", iz = 32, n = 76, a = 75.921403 * g / mole);

	G4Element* Ge = new G4Element(name = "Germanium", symbol = "Ge", ncomponents = 5);
	Ge->AddIsotope(Ge70, abundance = 20.84 * perCent);
	Ge->AddIsotope(Ge72, abundance = 27.54 * perCent);
	Ge->AddIsotope(Ge73, abundance = 7.73 * perCent);
	Ge->AddIsotope(Ge74, abundance = 36.28 * perCent);
	Ge->AddIsotope(Ge76, abundance = 7.61 * perCent);

	//
	// define simple materials
	//

	   // Al
	density = 2.700 * g / cm3;
	a = 26.98 * g / mole;
	G4Material* Aluminum = new G4Material(name = "Aluminum", z = 13., a, density);

	// Cu
	density = 8.960 * g / cm3;
	G4Material* Copper = new G4Material(name = "Copper", density, ncomponents = 1);
	Copper->AddElement(Cu, natoms = 1);


	// Stainless Steel
	density = 8.00 * g / cm3;
	G4Material* SST = new G4Material(name = "SST", density, ncomponents = 3);
	SST->AddElement(Cr, fractionmass = 8.0 * perCent);
	SST->AddElement(Fe, fractionmass = 74.0 * perCent);
	SST->AddElement(Ni, fractionmass = 18.0 * perCent);

	//Plastik  

	G4Material* Plastik = new G4Material(name = "Plastik", density = 0.900 * g / cm3, ncomponents = 2);
	Plastik->AddElement(C, natoms = 3);
	Plastik->AddElement(H, natoms = 6);

	//BC404 

	G4Material* BC404 = new G4Material(name = "BC404", density = 1.032 * g / cm3, ncomponents = 2);
	BC404->AddElement(C, natoms = 10);
	BC404->AddElement(H, natoms = 11);

	// PCB                  //Bakelit definition*****
	density = 1.45 * g / cm3;
	G4Material* PCB = new G4Material(name = "PCB", density, ncomponents = 3);
	PCB->AddElement(C, natoms = 9);
	PCB->AddElement(H, natoms = 9);
	PCB->AddElement(O, natoms = 1);

	// Peek
	density = 1.26 * g / cm3;
	G4Material* Peek = new G4Material(name = "Peek", density, ncomponents = 3);
	Peek->AddElement(C, natoms = 19);
	Peek->AddElement(H, natoms = 12);
	Peek->AddElement(O, natoms = 3);

	// Borosilicated glass
	density = 2.30 * g / cm3;
	G4Material* BGL = new G4Material(name = "BGL", density, ncomponents = 5);
	BGL->AddElement(B, fractionmass = 5.3 * perCent);
	BGL->AddElement(O, fractionmass = 54.9 * perCent);
	BGL->AddElement(Al, fractionmass = 1.7 * perCent);
	BGL->AddElement(Si, fractionmass = 32.7 * perCent);
	BGL->AddElement(K, fractionmass = 5.4 * perCent);

	// Mu-metal
	density = 8.58 * g / cm3;
	G4Material* MuMetal = new G4Material(name = "MuMetal", density, ncomponents = 4);
	MuMetal->AddElement(Cr, fractionmass = 2.0 * perCent);
	MuMetal->AddElement(Fe, fractionmass = 18.0 * perCent);
	MuMetal->AddElement(Ni, fractionmass = 75.0 * perCent);
	MuMetal->AddElement(Cu, fractionmass = 5.0 * perCent);

	// Photocathode Material 
	// (Bialkali K2CsSb)
	density = 2.00 * g / cm3;
	G4Material* K2CsSb = new G4Material(name = "K2CsSb", density, ncomponents = 3);
	K2CsSb->AddElement(K, natoms = 2);
	K2CsSb->AddElement(Cs, natoms = 1);
	K2CsSb->AddElement(Sb, natoms = 1);

	// Mylar
	density = 1.39 * g / cm3;
	G4Material* Mylar = new G4Material(name = "Mylar", density, ncomponents = 3);
	Mylar->AddElement(C, natoms = 10);
	Mylar->AddElement(H, natoms = 8);
	Mylar->AddElement(O, natoms = 4);

	//Fe2O3
	density = 5.24 * g / cm3;
	G4Material* Fe2O3 = new G4Material(name = "Fe2O3", density, ncomponents = 2);
	Fe2O3->AddElement(Fe, natoms = 2);
	Fe2O3->AddElement(O, natoms = 3);

	//Magnetic media
	density = 2.93 * g / cm3;
	G4Material* MagneticMedia = new G4Material(name = "MagneticMedia", density, ncomponents = 2);
	MagneticMedia->AddMaterial(Fe2O3, fractionmass = 40.0 * perCent);
	MagneticMedia->AddMaterial(Mylar, fractionmass = 60.0 * perCent);

	//EJ-200  for the beta plastic detector 
	density = 1.023 * g / cm3;
	G4Material* EJ200 = new G4Material(name = "EJ200", density, ncomponents = 2);
	EJ200->AddElement(H, fractionmass = 0.0847);
	EJ200->AddElement(C, fractionmass = 0.9153);

	//EJ-510  for the reflective paint 
	density = 0.013 * g / cm3;
	G4Material* EJ510 = new G4Material(name = "EJ510", density, ncomponents = 4);
	EJ510->AddElement(H, fractionmass = 0.0290);
	EJ510->AddElement(C, fractionmass = 0.1719);
	EJ510->AddElement(O, fractionmass = 0.3886);
	EJ510->AddElement(Ti, fractionmass = 0.4105);

	// ABS (Acrylonitrile Butadiene Styrene)
	density = 1.04 * g / cm3;
	G4Material* ABS = new G4Material(name = "ABS", density, ncomponents = 3);
	ABS->AddElement(C, natoms = 15);
	ABS->AddElement(H, natoms = 17);
	ABS->AddElement(N, natoms = 1);

	//POLYETHYLENE
	//density = 0.94*g/cm3;
	density = 0.4 * g / cm3;
	G4Material* PE = new G4Material(name = "Polyethylene  ", density, ncomponents = 2);
	PE->AddElement(H, fractionmass = 14.3711 * perCent);
	PE->AddElement(C, fractionmass = 85.6289 * perCent);

	//POLYVINYL CHLORIDE: PVC
	density = 1.3 * g / cm3;
	G4Material* PVC = new G4Material(name = "Polyvinylchloride  ", density, ncomponents = 3);
	PVC->AddElement(H, fractionmass = 4.8380 * perCent);
	PVC->AddElement(C, fractionmass = 38.4360 * perCent);
	PVC->AddElement(Cl, fractionmass = 56.7260 * perCent);


	G4Material* Silicon =
		new G4Material("Silicon", z = 14., a = 28.09 * g / mole, density = 2.330 * g / cm3);


	// Ge
	density = 5.323 * g / cm3;
	G4Material* Germanium = new G4Material(name = "Germanium", density, ncomponents = 1);
	Germanium->AddElement(Ge, natoms = 1);

	//polyethylene source 2017Bi
	density = 1.05 * g / cm3;
	G4Material* plastic_Bi = new G4Material(name = "plastic_Bi", density, ncomponents = 2);
	plastic_Bi->AddElement(H, fractionmass = 0.498);
	plastic_Bi->AddElement(C, fractionmass = 0.502);

	////////////////////////////////////////////////////////////////////////////////////////////  
	// Nitrogen
	density = 1.25053 * mg / cm3;
	G4Material* Nitrogen = new G4Material(name = "N2", density, ncomponents = 1, kStateGas, 296.15 * kelvin, 1 * atmosphere);
	Nitrogen->AddElement(N, 2);

	// Argon 
	density = 1.7836 * mg / cm3;
	G4Material* Argon = new G4Material(name = "Argon", density, ncomponents = 1, kStateGas, 296.15 * kelvin, 1 * atmosphere);
	Argon->AddElement(Ar, 1);

	// Helium 
	density = 0.1786 * mg / cm3;
	G4Material* Helium = new G4Material(name = "Helium", density, ncomponents = 1, kStateGas, 296.15 * kelvin, 1 * atmosphere);
	Helium->AddElement(He, 1);

	// CF4 
	density = 3.72 * mg / cm3;
	G4Material* CF4 = new G4Material(name = "CF4", density, ncomponents = 2, kStateGas, 296.15 * kelvin, 1 * atmosphere);
	CF4->AddElement(C, 1);
	CF4->AddElement(F, 4);


	// GAS OTPC 
	//////////Reading the input data for primary generator///////////

	ifstream geoInputFile("../../../geo.data");

	geoInputFile >> header1;
	geoInputFile >> gas[0] >> gas[1] >> gas[2];
	geoInputFile >> header2;
	geoInputFile >> fgas[0] >> fgas[1] >> fgas[2];
	geoInputFile >> header3;
	geoInputFile >> T >> P >> E;
	geoInputFile.close();
	G4cout << gas[0] << " " << gas[1] << " " << gas[2] << G4endl;
	G4cout << fgas[0] << " " << fgas[1] << " " << fgas[2] << G4endl;
	G4cout << T << " " << P << " " << E << G4endl;
	//////////////////////////////////////////////////////////////////

	//Masses in g/mol:
	G4double He_amu = 4.003;
	G4double Ar_amu = 39.948;
	G4double C_amu = 12.011;
	G4double O_amu = 16.;
	G4double F_amu = 18.998;
	G4double N_amu = 14.007;

	G4double CO2_amu = C_amu + 2 * O_amu;
	G4double CF4_amu = C_amu + 4 * F_amu;
	G4double N2_amu = 2 * N_amu;

	//Gas composition:
	G4double gas_amu = 0.0;

	//Individual gass constant calculation:
	G4double K_gas = 8314.4598; //in kPa.cm3/(mol.K)

	G4int Ngases = 0;
	for (G4int k = 0; k < 3; k++) {
		if (fgas[k] > 0) {
			if (gas[k] == 1) {
				Ngases = Ngases + 2; //Two components!
				gas_amu = gas_amu + 1e-2 * fgas[k] * CF4_amu; //fgas are in %!
			}
			else if (gas[k] == 2) {
				Ngases++;
				gas_amu = gas_amu + 1e-2 * fgas[k] * Ar_amu; //fgas are in %!
			}
			else if (gas[k] == 3) {
				Ngases++;
				gas_amu = gas_amu + 1e-2 * fgas[k] * He_amu; //fgas are in %!
			}
			else if (gas[k] == 4) {
				Ngases++;
				gas_amu = gas_amu + 1e-2 * fgas[k] * N2_amu; //fgas are in %!
			}
		}
	}
	G4cout << "Gas atomic components " << Ngases << G4endl;

	//in kPa.cm3/gK 
	G4double K_gas_mixture = K_gas / gas_amu;

	//Densities for this T and P:
	G4double d = P / (K_gas_mixture * T); //in g/cm3

	G4cout << "Density: " << endl;
	G4cout << d << "g/cm3" << endl;
	G4cout << 1e3 * d << "mg/cm3" << endl;


	G4Material* GasOTPC = new G4Material(name = "GasOTPC", 1e3 * d * mg / cm3, ncomponents = Ngases, kStateGas, T * kelvin, P * 1e3 * pascal);

	//In atoms!
	for (G4int k = 0; k < 3; k++) {
		if (fgas[k] > 0) {
			if (gas[k] == 1) {
				GasOTPC->AddElement(C, int(1e3 * fgas[k]));  //CF4
				GasOTPC->AddElement(F, int(1e3 * fgas[k] * 4));
			}
			else if (gas[k] == 2) {
				GasOTPC->AddElement(Ar, int(1e3 * fgas[k]));  //Ar
			}
			else if (gas[k] == 3) {
				GasOTPC->AddElement(He, int(1e3 * fgas[k])); //He
			}
			else if (gas[k] == 4) {
				GasOTPC->AddElement(N, int(1e3 * fgas[k] * 2));  //N2
			}
		}
	}


	//
	//  define Vacuum
	//

	pressure = 3.e-18 * pascal;
	temperature = 2.73 * kelvin;
	density = 1.e-25 * g / cm3;

	G4Material* Vacuum = new G4Material(name = "Galactic", z = 1., a = 1.01 * g / mole,
		density, kStateGas, temperature, pressure);

	//Air
	density = 1.29 * mg / cm3;
	G4Material* Air = new G4Material(name = "Air  ", density, ncomponents = 4);
	Air->AddElement(N, fractionmass = 75.51 * perCent);
	Air->AddElement(O, fractionmass = 23.18 * perCent);
	Air->AddElement(C, fractionmass = 0.02 * perCent);
	Air->AddElement(Ar, fractionmass = 1.29 * perCent);

	//------------------------------------------------------
   // Detector geometry
   //------------------------------------------------------

	//     
   /// World
   //

	G4double WorldSize = 100. * cm;

	G4Box* solidWorld = new G4Box("World", WorldSize / 2, WorldSize / 2, WorldSize / 2);
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, Vacuum, "World");  //VACUUM
	//G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,Air,"World");       //AIR
	G4VPhysicalVolume* physiWorld = new G4PVPlacement(0, G4ThreeVector(), "World", logicWorld, NULL, false, 0);

	// OTPC active volume:

	G4double half_x_OTPC = 16.128 * cm;
	G4double half_y_OTPC = 16.128 * cm;
	G4double half_z_OTPC = 7.1 * cm;

	G4Box* solidOTPC = new G4Box("OTPC", half_x_OTPC, half_y_OTPC, half_z_OTPC);
	G4LogicalVolume* OTPCLogicalVolume = new G4LogicalVolume(solidOTPC, GasOTPC, "OTPCLogicalVolume");
	G4ThreeVector OTPC_position = G4ThreeVector(0. * cm, 0. * cm, 0. * mm);
	G4VPhysicalVolume* physiOTPC = new G4PVPlacement(0, OTPC_position, "OTPC", OTPCLogicalVolume, physiWorld, false, 0);

	// Construct the field creator - this will register the field it creates
	F02ElectricFieldSetup* fieldSetup = new F02ElectricFieldSetup();



	//********************************************************************************************************************/


	//
	/// visualization attributes
	//

	logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());


	// red color for detector 
	G4VisAttributes* Att2 = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
	// blue for gas
	G4VisAttributes* Att3 = new G4VisAttributes(G4Colour(0.0, 0.0, 0.5));
	// grey for PMT
	G4VisAttributes* Att4 = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
	//white
	G4VisAttributes* Att5 = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
	//magenta
	G4VisAttributes* Att6 = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
	// black
	G4VisAttributes* Att7 = new G4VisAttributes(G4Colour(0.0, 0.0, 0.0));
	// green
	G4VisAttributes* Att8 = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
	// orange
	G4VisAttributes* Att9 = new G4VisAttributes(G4Colour(1, 0.5, 0, 0.8));
	// dark green
	G4VisAttributes* Att10 = new G4VisAttributes(G4Colour(0.5, 1.0, 0.0));

	//OTPCLogicalVolume->SetVisAttributes(Att9);
	OTPCLogicalVolume->SetVisAttributes(G4VisAttributes::GetInvisible());

	//
	// always return the physical World
	//

	return physiWorld;
}
