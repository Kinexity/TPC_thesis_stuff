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
#include "G4PhysicalVolumeStore.hh"
#include "G4NistManager.hh"
#include <fstream>
#include <format>

#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include <G4UnionSolid.hh>
//#include "TMath.h"
#include "G4Polycone.hh"

#include "F02ElectricFieldSetup.hh"
#include <tuple>
#include <map>
#include <functional>

template <typename Ret_Type, typename... Func_Args>
class return_value_wrapper {
private:
	std::map<std::tuple<Func_Args...>, Ret_Type> ret_val_map;
	std::function<Ret_Type(Func_Args...)> func_ptr;
public:
	return_value_wrapper(std::function<Ret_Type(Func_Args...)>& f_ptr) : func_ptr(f_ptr) {};
	return_value_wrapper() = default;
	~return_value_wrapper() = default;
	decltype(func_ptr)& operator=(std::function < Ret_Type(Func_Args...)> f_ptr) {
		ret_val_map.clear();
		return (func_ptr = f_ptr);
	}
	Ret_Type operator()(Func_Args... args) {
		auto packed = std::make_tuple(args...);
		const auto it = ret_val_map.find(packed);
		return (it != ret_val_map.end() ? it->second : (ret_val_map[packed] = func_ptr(args...)));
	};
};


// Global file with the events
std::ifstream geoInputFile;

inline std::string filename_string(std::string path_str) {
	return path_str.substr(path_str.rfind("\\") + 1, path_str.size() - path_str.rfind("\\") - 1);
};

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << '\n'
#define checkpoint std::cout << "checkpoint" << _endl_

OTPCDetectorConstruction::OTPCDetectorConstruction(G4double crystD, std::string scintT) : crystalDepth(crystD), scintillatorType(scintT) {}

G4VPhysicalVolume* OTPCDetectorConstruction::Construct() {

	G4NistManager* nistManager = G4NistManager::Instance();

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

	auto fn = std::function([&](std::string elementName)->G4Element* {
		return G4NistManager::Instance()->FindOrBuildElement(elementName);
	});
	return_value_wrapper<G4Element*, std::string> periodicTable(fn);

   // H
	G4Element* H = periodicTable("H");

	// He
	G4Element* He = periodicTable("He");

	// O
	G4Element* O = periodicTable("O");

	// B
	G4Element* B = periodicTable("B");

	// Al
	G4Element* Al = periodicTable("Al");

	// C
	G4Element* C = periodicTable("C");

	// Si
	G4Element* Si = periodicTable("Si");

	// K
	G4Element* K = periodicTable("K");

	// Cr
	G4Element* Cr = periodicTable("Cr");

	// Fe
	G4Element* Fe = periodicTable("Fe");

	// Ni
	G4Element* Ni = periodicTable("Ni");

	// Sb
	G4Element* Sb = periodicTable("Sb");

	// Ti
	G4Element* Ti = periodicTable("Ti");

	// Ar
	G4Element* Ar = periodicTable("Ar");

	// Cl
	G4Element* Cl = periodicTable("Cl");

	//F
	G4Element* F = periodicTable("F");

	// Na
	G4Element* Na = periodicTable("Na");

	// Mg
	G4Element* Mg = periodicTable("Mg");

	// Ca
	G4Element* Ca = periodicTable("Ca");

	// Ce
	G4Element* Ce = periodicTable("Ce");

	// La
	G4Element* La = periodicTable("La");

	// Cu
	G4Element* Cu = periodicTable("Cu");

	// Cs
	G4Element* Cs = periodicTable("Cs");

	// N
	G4Element* N = periodicTable("N");

	// Ge
	G4Element* Ge = periodicTable("Ge");

	// Br
	G4Element* Br = periodicTable("Br");

	//
	// define simple materials
	//

	   // Al
	density = 2.700 * g / cm3;
	a = 26.98 * g / mole;
	G4Material* Aluminium = new G4Material(name = "Aluminium", z = 13., a, density);

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


	// Define the SiO2 compound
	density = 2.65 * g / cm3;
	G4Material* SiO2 = new G4Material(name = "SiO2", density, ncomponents = 2);
	SiO2->AddElement(Si, 1);
	SiO2->AddElement(O, 2);

	// Define the Al2O3 compound
	density = 3.987 * g / cm3;
	G4Material* Al2O3 = new G4Material(name = "Al2O3", density, ncomponents = 2);
	Al2O3->AddElement(Al, 2);
	Al2O3->AddElement(O, 3);

	// Define the TiO2 compound
	density = 4.26 * g / cm3;
	G4Material* TiO2 = new G4Material(name = "TiO2", density, ncomponents = 2);
	TiO2->AddElement(Ti, 1);
	TiO2->AddElement(O, 2);

	// Define the B2O3 compound
	density = 2.55 * g / cm3;
	G4Material* B2O3 = new G4Material(name = "B2O3", density, ncomponents = 2);
	B2O3->AddElement(B, 2);
	B2O3->AddElement(O, 3);

	// Define the CaO compound
	density = 3.34 * g / cm3;
	G4Material* CaO = new G4Material(name = "CaO", density, ncomponents = 2);
	CaO->AddElement(Ca, 1);
	CaO->AddElement(O, 1);

	// Define the MgO compound
	density = 3.58 * g / cm3;
	G4Material* MgO = new G4Material(name = "MgO", density, ncomponents = 2);
	MgO->AddElement(Mg, 1);
	MgO->AddElement(O, 1);

	// Define the Na2O compound
	density = 2.27 * g / cm3;
	G4Material* Na2O = new G4Material(name = "Na2O", density, ncomponents = 2);
	Na2O->AddElement(Na, 2);
	Na2O->AddElement(O, 1);

	// Define the K2O compound
	density = 2.35 * g / cm3;
	G4Material* K2O = new G4Material(name = "K2O", density, ncomponents = 2);
	K2O->AddElement(K, 2);
	K2O->AddElement(O, 1);


	//fibreglass reinforced epoxy (Stesalit)
	density = 1.85 * g / cm3;
	G4Material* Stesalit = new G4Material(name = "Stesalit", density, ncomponents = 8);
	Stesalit->AddMaterial(SiO2, 55.0 * perCent);
	Stesalit->AddMaterial(Al2O3, 14.0 * perCent);
	Stesalit->AddMaterial(TiO2, 0.2 * perCent);
	Stesalit->AddMaterial(B2O3, 7.0 * perCent);
	Stesalit->AddMaterial(CaO, 22.0 * perCent);
	Stesalit->AddMaterial(MgO, 1.0 * perCent);
	Stesalit->AddMaterial(Na2O, 0.5 * perCent);
	Stesalit->AddMaterial(K2O, 0.3 * perCent);

	//-------------------correct Stesalit test-----------------------------


	G4Material* diglycidylEther = new G4Material("DiglycidylEther", 1.16 * g / cm3, 3);
	diglycidylEther->AddElement(C, 19); // C19H20O4
	diglycidylEther->AddElement(H, 20);
	diglycidylEther->AddElement(O, 4);

	// Define 1,4-Butanediol Diglycidyl Ether
	G4Material* butanediolEther = new G4Material("ButanediolEther", 1.10 * g / cm3, 3);
	butanediolEther->AddElement(C, 10); // C10H18O4
	butanediolEther->AddElement(H, 18);
	butanediolEther->AddElement(O, 4);

	// Define 1,6-Hexanediamine 2,2,4-trimethyl-
	G4Material* hexanediamine = new G4Material("Hexanediamine", 0.865 * g / cm3, 3);
	hexanediamine->AddElement(C, 9); // C9H22N2
	hexanediamine->AddElement(H, 22);
	hexanediamine->AddElement(N, 2);

	// Define Epotek 301-1
	G4Material* epotek301_1 = new G4Material("Epotek301_1", 1.19 * g / cm3, 3);
	epotek301_1->AddMaterial(diglycidylEther, 0.8 * 70.0 * perCent);
	epotek301_1->AddMaterial(butanediolEther, 0.8 * 30.0 * perCent);
	epotek301_1->AddMaterial(hexanediamine, 20.0 * perCent); // 20% of Part B


	 // Define Stesalit EP107-M950-40
	G4Material* stesalitEP107_M950_40 = new G4Material("StesalitEP107_M950_40", 1.45 * g / cm3, 2);

	stesalitEP107_M950_40->AddMaterial(Stesalit, 0.35);
	stesalitEP107_M950_40->AddMaterial(epotek301_1, 0.65);

	//-------------------correct Stesalit test end-------------------------




	// PTFE
	density = 2.0 * g / cm3;
	G4Material* PTFE = new G4Material(name = "PTFE", density, ncomponents = 2);
	PTFE->AddElement(C, natoms = 2);
	PTFE->AddElement(F, natoms = 4);

	// CeBr3
	density = 5.1 * g / cm3;
	G4Material* CeBr3 = new G4Material(name = "CeBr3", density, ncomponents = 2);
	CeBr3->AddElement(Ce, natoms = 1);
	CeBr3->AddElement(Br, natoms = 3);

	// LaBr3
	density = 5.06 * g / cm3;
	G4Material* LaBr3 = new G4Material(name = "LaBr3", density, ncomponents = 2);
	LaBr3->AddElement(La, natoms = 1);
	LaBr3->AddElement(Br, natoms = 3);

	// Ce doped LaBr3
	density = 5.1 * g / cm3;
	G4Material* LaBr3Ce = new G4Material(name = "LaBr3Ce", density, ncomponents = 2);
	LaBr3Ce->AddMaterial(LaBr3, 95.0 * perCent);
	LaBr3Ce->AddElement(Ce, 5.0 * perCent);

	// GAS OTPC 
	//////////Reading the input data for primary generator///////////

	std::ifstream geoInputFile("../../../geo.data");

	if (!geoInputFile.is_open()) {
		std::cout << "\n\nNO GEO INPUT INFORMATION FILE FOUND!!!" << _endl_;
		exit(1);
	}

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
	//apply units
	for (auto& fgas_ : fgas) {
		fgas_ *= perCent;
	}
	P *= 1e3 * pascal;
	T *= kelvin;
	E *= volt / cm;
	//Masses in g/mol:
	const G4double He_amu = 4.003 * g / mole;
	const G4double Ar_amu = 39.948 * g / mole;
	const G4double C_amu = 12.011 * g / mole;
	const G4double O_amu = 16. * g / mole;
	const G4double F_amu = 18.998 * g / mole;
	const G4double N_amu = 14.007 * g / mole;

	G4double CO2_amu = C_amu + 2 * O_amu;
	G4double CF4_amu = C_amu + 4 * F_amu;
	G4double N2_amu = 2 * N_amu;

	//Gas composition:
	G4double gas_amu = 0.0 * g / mole;

	//Individual gass constant calculation:
	G4double K_gas = 8314.4598 * 1e3 * pascal * cm3 / (mole * kelvin); //in kPa.cm3/(mol.K)

	std::vector<std::tuple<G4Element*, int>> gases;

	G4int Ngases = 0;
	for (G4int k = 0; k < 3; k++) {
		if (fgas[k] > 0) {
			switch (gas[k]) {
			case 1:
				Ngases += 2; // Two components!
				gas_amu += fgas[k] * CF4_amu;
				gases.emplace_back(C, int(1e5 * fgas[k])); // Add C to gases vector
				gases.emplace_back(F, int(1e5 * fgas[k] * 4)); // Add F to gases vector
				break;
			case 2:
				Ngases++;
				gas_amu += fgas[k] * Ar_amu;
				gases.emplace_back(Ar, int(1e5 * fgas[k])); // Add Ar to gases vector
				break;
			case 3:
				Ngases++;
				gas_amu += fgas[k] * He_amu;
				gases.emplace_back(He, int(1e5 * fgas[k])); // Add He to gases vector
				break;
			case 4:
				Ngases++;
				gas_amu += fgas[k] * N2_amu;
				gases.emplace_back(N, int(1e5 * fgas[k] * 2)); // Add N to gases vector
				break;
			default:
				break;
			}
		}
	}
	G4cout << "Gas atomic components " << Ngases << G4endl;

	//in kPa.cm3/gK 
	G4double K_gas_mixture = K_gas / gas_amu;

	//Densities for this T and P:
	G4double d = P / (K_gas_mixture * T); //in g/cm3

	G4cout << "Density: \n";
	G4cout << d / (g / cm3) << "g/cm3\n";
	G4cout << d / (mg / cm3) << "mg/cm3\n";


	G4Material* GasOTPC = new G4Material(name = "GasOTPC", d, ncomponents = Ngases, kStateGas, T, P);

	//In atoms!
	// Create G4Element and G4Material instances from gases vector
	for (auto& [elem, num] : gases) {
		if (num > 0) {
			GasOTPC->AddElement(elem, num);
		}
	}


	//Vacuum
	G4Material* Vacuum = nistManager->FindOrBuildMaterial("G4_Galactic");

	//Air
	G4Material* Air = nistManager->FindOrBuildMaterial("G4_AIR");

	//<--------------------------------------------------------------------------------------------------------------------------------->
	//<--------------------------------------------------------------World-------------------------------------------------------------->
	//<--------------------------------------------------------------------------------------------------------------------------------->

	G4double WorldSize = 100. * cm;

	G4Box* solidWorld = new G4Box("World", WorldSize / 2, WorldSize / 2, WorldSize / 2);
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, Vacuum, "World");  //VACUUM
	//G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,Air,"World");       //AIR
	G4VPhysicalVolume* physiWorld = new G4PVPlacement(0, G4ThreeVector(), "World", logicWorld, NULL, false, 0);


	G4ThreeVector OTPC_position = G4ThreeVector(0. * cm, 0. * cm, 0. * mm);


	// Define the dimensions of the mother volume
	G4double motherVolumeX = 50 * cm;
	G4double motherVolumeY = 50 * cm;
	G4double motherVolumeZ = 50 * cm;

	//Mother volumen
	G4Box* solidmother_OTPC = new G4Box("mother_OTPC", motherVolumeX / 2, motherVolumeY / 2, motherVolumeZ / 2);
	G4LogicalVolume* logicmother_OTPC = new G4LogicalVolume(solidmother_OTPC, Vacuum, "mother_OTPC");
	G4ThreeVector position_mother_OTPC = G4ThreeVector(0. * mm, 0. * cm, 0. * mm);
	G4VPhysicalVolume* physimother_OTPC = new G4PVPlacement(0, position_mother_OTPC, "mother_OTPC", logicmother_OTPC, physiWorld, false, 0);

	//<--------------------------------------------------------------------------------------------------------------------------------->
	//<--------------------------------------------------------Detector geometry-------------------------------------------------------->
	//<--------------------------------------------------------------------------------------------------------------------------------->

	// Define the dimensions of the active volume
	G4double activeVolumeX = 20 * cm;
	G4double activeVolumeY = 33 * cm;
	G4double activeVolumeZ = 21 * cm;

	// Define the dimensions of the external volume
	G4double externalVolumeX = 22.2 * cm;
	G4double externalVolumeY = 34.2 * cm;
	G4double externalVolumeZ = 22 * cm;

	// Create a solid volume for the active volume
	G4Box* activeVolumeSolid = new G4Box("activeVolumeSolid", activeVolumeX / 2, activeVolumeY / 2, activeVolumeZ / 2);

	// Create a solid volume for the external volume
	G4Box* externalVolumeSolid = new G4Box("externalVolumeSolid", externalVolumeX / 2, externalVolumeY / 2, externalVolumeZ / 2);

	// Subtract the active volume from the external volume to create detector walls
	G4SubtractionSolid* wallsSolid = new G4SubtractionSolid("wallsSolid", externalVolumeSolid, activeVolumeSolid /*, 0, activeVolumeShift*/);

	// Assign the Stesalit material to the walls of the detector
	G4LogicalVolume* wallsLogical = new G4LogicalVolume(wallsSolid, Stesalit, "wallsLogical");

	// Assign the GasOTPC to the active volume of the detector
	G4LogicalVolume* activeVolumeLogical = new G4LogicalVolume(activeVolumeSolid, GasOTPC, "activeVolumeLogical");

	G4VPhysicalVolume* physiOTPC = new G4PVPlacement(0, OTPC_position, "OTPC_activeVolume", activeVolumeLogical, physimother_OTPC, false, 0);
	G4VPhysicalVolume* physiWallsOTPC = new G4PVPlacement(0, OTPC_position, "OTPC_walls", wallsLogical, physimother_OTPC, false, 0);

	//<--------------------------------------------------------------------------------------------------------------------------------->
	//<-----------------------------------------------------------Electrodes------------------------------------------------------------>
	//<--------------------------------------------------------------------------------------------------------------------------------->

	//Electrode dimensions
	G4double electrodeThickness = 3 * mm; //thickness is counted in Z axis
	G4double electrodeSpacing = 7 * mm; //space between electrodes
	G4double electrodeWidth = 6 * mm; //from the walls

	// Create a solid volume for the one electrode plane volume
	G4Box* oneElectrodePlaneVolumeSolid = new G4Box("oneElectrodePlaneVolumeSolid", activeVolumeX / 2, activeVolumeY / 2, electrodeThickness / 2);

	// Create a solid volume for the one electrode inner volume
	G4Box* oneElectrodePlaneInternalVolumeSolid = new G4Box("oneElectrodePlaneInternalVolumeSolid", activeVolumeX / 2 - electrodeWidth, activeVolumeY / 2 - electrodeWidth, electrodeThickness);

	// Create a solid volume for the one electrode volume
	G4SubtractionSolid* oneElectrodeSolid = new G4SubtractionSolid("oneElectrodeSolid", oneElectrodePlaneVolumeSolid, oneElectrodePlaneInternalVolumeSolid /*, 0, activeVolumeShift*/);

	// Assign the Copper material to one electrode
	G4LogicalVolume* oneElectrodeLogical = new G4LogicalVolume(oneElectrodeSolid, Copper, "oneElectrodeLogical");

	//vector of electrode placements, idk why but I think it's needed for the placements to work
	std::vector<G4PVPlacement*> electrode_placements;

	// Position the electrodes and place them inside the active volume
	for (int i = 0; i < 20; i++) {
		G4ThreeVector position = G4ThreeVector(0, 0, (electrodeThickness + electrodeSpacing) * (i - 9.5));
		electrode_placements.push_back(new G4PVPlacement(0, position, oneElectrodeLogical, "electrodePhysical", activeVolumeLogical, false, i));
	}

	chamberCorner = { // add 1cm margin to not put particles in the walls
		oneElectrodePlaneInternalVolumeSolid->GetXHalfLength() - 1 * cm,
		oneElectrodePlaneInternalVolumeSolid->GetYHalfLength() - 1 * cm,
		activeVolumeSolid->GetZHalfLength() - 1 * cm };

	//<--------------------------------------------------------------------------------------------------------------------------------->
	//<---------------------------------------------------------Gamma detectors--------------------------------------------------------->
	//<--------------------------------------------------------------------------------------------------------------------------------->

	G4RotationMatrix* rotation = new G4RotationMatrix();
	rotation->rotateY(90 * deg);

	// detector dimensions
	G4double crystalSideLenght = 4.65 * cm;
	//crystal depth define as class constant
	G4double reflectiveCoatingThickness = 0.25 * mm;
	G4double externalCoverThickness = 1.5 * mm;

	G4double gammaDetectorSideHalfLenght = crystalSideLenght / 2 + reflectiveCoatingThickness + externalCoverThickness;
	G4double gammaDetectorDepthHalfLenght = crystalDepth / 2 + reflectiveCoatingThickness + externalCoverThickness;

	// gamma detector solid volume
	G4Box* gammaDetectorVolumeSolid = new G4Box("gammaDetectorVolumeSolid", gammaDetectorSideHalfLenght, gammaDetectorSideHalfLenght, gammaDetectorDepthHalfLenght);

	// gamma detector logical volume
	G4LogicalVolume* gammaDetectorVolumeLogical = new G4LogicalVolume(gammaDetectorVolumeSolid, Vacuum, "gammaDetectorVolumeLogical");

	// vector of gamma detector placements, idk why but I think it's needed for the placements to work
	std::vector<G4VPhysicalVolume*> gammaDetectorPlacements;

	// gamma detector placements
	G4double margin = 0.5 * cm;
	G4int gammaDetectorPlacementCounter = 0;
	for (auto detectorSideSign : { -1,1 }) { // side of the OTPC detector
		for (auto gammaDetectorRowSign : { -1,1 }) { // is it an upper or lower row of gamma detectors
			for (G4int gammaDetectorRowPosition = 0; gammaDetectorRowPosition < 6; gammaDetectorRowPosition++) { // position in the row of detectors
				if (detectorSideSign < 0 && (gammaDetectorRowPosition == 2 || gammaDetectorRowPosition == 3)) { //check if column is one of two central ones as detector are place asymmetrically
					continue;
				}
				if (removeGroup0 && detectorSideSign > 0 && (gammaDetectorRowPosition == 2 || gammaDetectorRowPosition == 3)) { //check if column is one of two central ones as detector are place asymmetrically
					gammaDetectorPlacementCounter++;
					continue;
				}
				G4double gammaDetectorOffsetX = externalVolumeX / 2 + gammaDetectorDepthHalfLenght;
				G4ThreeVector gammaDetectorPosition = {
					detectorSideSign * gammaDetectorOffsetX,
					(2 * gammaDetectorRowPosition - 5) * gammaDetectorSideHalfLenght + margin * (gammaDetectorRowPosition / 2 - 1),
					gammaDetectorRowSign * gammaDetectorSideHalfLenght };
				gammaDetectorPlacements.push_back(new G4PVPlacement(rotation, gammaDetectorPosition, "physiGammaDetector", gammaDetectorVolumeLogical, physimother_OTPC, false, gammaDetectorPlacementCounter++));
			}
		}
	}

	//<---------------------------------------------------------Crystal----------------------------------------------------------------->

	// crystal solid volume
	G4Box* crystalVolumeSolid = new G4Box("crystalVolumeSolid", crystalSideLenght / 2, crystalSideLenght / 2, crystalDepth / 2);

	G4Material* scintillatorMaterial;

	if (scintillatorType == "CeBr3") {
		scintillatorMaterial = CeBr3;
	}
	else if (scintillatorType == "LaBr3") {
		scintillatorMaterial = LaBr3Ce;
	}

	realScintillatorType = scintillatorMaterial->GetName();

	// crystal logical volume
	G4LogicalVolume* crystalVolumeLogical = new G4LogicalVolume(crystalVolumeSolid, scintillatorMaterial, "crystalVolumeLogical");

	// crystal placement in detector
	G4VPhysicalVolume* physiCrystal = new G4PVPlacement(0, G4ThreeVector(), crystalVolumeLogical, "physiCrystal", gammaDetectorVolumeLogical, false, 0);

	//<---------------------------------------------------------Reflective coating------------------------------------------------------>

	// reflective coating external solid volume
	G4Box* reflectiveCoatingExternalVolumeSolid = new G4Box("reflectiveCoatingExternalVolumeSolid", crystalSideLenght / 2 + reflectiveCoatingThickness, crystalSideLenght / 2 + reflectiveCoatingThickness, crystalDepth / 2 + reflectiveCoatingThickness);

	// reflective coating solid volume
	G4SubtractionSolid* reflectiveCoatingVolumeSolid = new G4SubtractionSolid("reflectiveCoatingVolumeSolid", reflectiveCoatingExternalVolumeSolid, crystalVolumeSolid);

	// reflective coating logical volume
	G4LogicalVolume* reflectiveCoatingVolumeLogical = new G4LogicalVolume(reflectiveCoatingVolumeSolid, PTFE, "reflectiveCoatingVolumeLogical");

	// reflective coating placement in detector
	G4VPhysicalVolume* physiReflectiveCoating = new G4PVPlacement(0, G4ThreeVector(), reflectiveCoatingVolumeLogical, "physiReflectiveCoating", gammaDetectorVolumeLogical, false, 0);

	//<---------------------------------------------------------External cover---------------------------------------------------------->

	// external cover external solid volume
	G4Box* externalCoverExternalVolumeSolid = new G4Box("externalCoverExternalVolumeSolid", gammaDetectorSideHalfLenght, gammaDetectorSideHalfLenght, gammaDetectorDepthHalfLenght);

	// external cover solid volume
	G4SubtractionSolid* externalCoverVolumeSolid = new G4SubtractionSolid("externalCoverVolumeSolid", gammaDetectorVolumeSolid, reflectiveCoatingExternalVolumeSolid);

	// external cover coating logical volume
	G4LogicalVolume* externalCoverVolumeLogical = new G4LogicalVolume(externalCoverVolumeSolid, Aluminium, "externalCoverVolumeLogical");

	// external cover placement in detector
	G4VPhysicalVolume* physiExternalCover = new G4PVPlacement(0, G4ThreeVector(), externalCoverVolumeLogical, "physiExternalCover", gammaDetectorVolumeLogical, false, 0);






	// Construct the field creator - this will register the field it creates
	//F02ElectricFieldSetup* fieldSetup = new F02ElectricFieldSetup();



	//********************************************************************************************************************/


	//
	/// visualization attributes
	//


	logicmother_OTPC->SetVisAttributes(G4VisAttributes::GetInvisible());
	logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());


	// red color for detector 
	G4VisAttributes* Att2 = new G4VisAttributes(G4Colour::Red());
	// blue for gas
	G4VisAttributes* Att3 = new G4VisAttributes(G4Colour::Blue());
	// grey for PMT
	G4VisAttributes* Att4 = new G4VisAttributes(G4Colour::Grey());
	//white
	G4VisAttributes* Att5 = new G4VisAttributes(G4Colour::White());
	//magenta
	G4VisAttributes* Att6 = new G4VisAttributes(G4Colour::Magenta());
	// black
	G4VisAttributes* Att_black = new G4VisAttributes(G4Colour::Black());
	// green
	G4VisAttributes* Att8 = new G4VisAttributes(G4Colour::Green());
	// orange
	G4VisAttributes* Att_orange = new G4VisAttributes(G4Colour(1, 0.5, 0, 0.8));
	// dark green
	G4VisAttributes* Att10 = new G4VisAttributes(G4Colour(0.5, 1.0, 0.0));


	G4VisAttributes* Att_pale_yellow = new G4VisAttributes(G4Colour(255. / 255, 204. / 255, 102. / 255));
	G4VisAttributes* Att_light_grey = new G4VisAttributes(G4Colour(224. / 255, 224. / 255, 224. / 255));
	G4VisAttributes* Att_light_blue = new G4VisAttributes(G4Colour(145. / 255, 202. / 255, 249. / 255));
	//G4VisAttributes* Att_pale_yellow = new G4VisAttributes(G4Colour(255./255, 204./255, 102./255));



	//OTPCLogicalVolume->SetVisAttributes(Att9);
	//OTPCLogicalVolume->SetVisAttributes(G4VisAttributes::GetInvisible());
	activeVolumeLogical->SetVisAttributes(G4VisAttributes::GetInvisible());
	oneElectrodeLogical->SetVisAttributes(Att_orange);
	wallsLogical->SetVisAttributes(Att_pale_yellow);
	externalCoverVolumeLogical->SetVisAttributes(Att_light_grey);
	crystalVolumeLogical->SetVisAttributes(Att_light_blue);
	reflectiveCoatingVolumeLogical->SetVisAttributes(Att_black);

	//
	// always return the physical World
	//
	isInitialized = true; //detector has been constructed
	return physiWorld;
}

const G4double OTPCDetectorConstruction::getCrystalDepth() {
	if (isInitialized) {
		return crystalDepth;
	}
	else {
		std::cout << "Error: detector not constructed" << _endl_;
		exit(1);
	}
}

const std::string& OTPCDetectorConstruction::getScintillatorType() {
	//if (isInitialized) {
	return realScintillatorType;
	//}
	//else {
	//	std::cout << "Error: detector not constructed" << _endl_;
	//	exit(1);
	//}
}

void OTPCDetectorConstruction::saveDetails(std::filesystem::path p) {

	std::cout << p << _endl_;
	// Open file for writing physical volumes
	std::ofstream outfile;
	outfile.open(p / "volumes.csv");
	outfile << "Physical Volume,Material,Shape\n";

	checkpoint;
	// Loop over all the physical volumes
	auto& physicalVolumeStore = *G4PhysicalVolumeStore::GetInstance();
	for (const auto& physicalVolume : physicalVolumeStore) {
		const auto& material = *physicalVolume->GetLogicalVolume()->GetMaterial();
		const auto& shape = *physicalVolume->GetLogicalVolume()->GetSolid();
		outfile << std::format("{},{},{}\n",
			physicalVolume->GetName(), material.GetName(), shape.GetName());
	}
	outfile.close();

	checkpoint;
	// Open file for writing materials
	outfile.open(p / "materials.csv");
	outfile << "Material,Density (g/cm3)\n";

	// Loop over all the materials
	auto& materialTable = *G4Material::GetMaterialTable();
	for (const auto& material : materialTable) {
		outfile << std::format("{},{:.3f}\n",
			material->GetName(), material->GetDensity() / (g / cm3));
	}
	outfile.close();

	checkpoint;
	// Open file for writing elements
	outfile.open(p / "elements.csv");
	outfile << "Element,Atomic Number\n";
	std::ofstream outfile_isotopes(p / "isotopes.csv");
	outfile_isotopes << "Isotope,Atomic Number,Atomic Mass (g/mol),Natural Abundance\n";
	// Loop over all the elements
	auto& elementTable = *G4Element::GetElementTable();
	for (const auto& element : elementTable) {
		outfile << std::format("{},{}\n",
			element->GetName(), element->GetZ());
		auto& isotopeVector = *element->GetIsotopeVector();
		auto relativeAbundanceVector = element->GetRelativeAbundanceVector();
		// Loop over all the isotopes
		for (const auto& isotope : isotopeVector) {
			outfile_isotopes << std::format("{},{},{:.3f},{:.3f}\n",
				isotope->GetName(), isotope->GetZ(), isotope->GetA() / (g / mole),
				relativeAbundanceVector[isotope->GetIndex()]);
		}
	}
	outfile.close();
	outfile_isotopes.close();

}

G4ThreeVector OTPCDetectorConstruction::getChamberCorner() {
	if (isInitialized) {
		return chamberCorner;
	}
	else {
		std::cout << "Error: detector not constructed" << _endl_;
		exit(1);
	}
}
