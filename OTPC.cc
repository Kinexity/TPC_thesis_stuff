// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: exampleN01.cc,v 1.2 1999/12/15 14:49:18 gunter Exp $
// GEANT4 tag $Name: geant4-02-00 $
//
//
// --------------------------------------------------------------
//      GEANT 4 - exampleN01
//
//      For information related to this code contact:
//      CERN, IT Division, ASD Group
// --------------------------------------------------------------
//
/////////////////////////////////////////////////////////////////
//
//  Modified: 16-01-2013 A. Algora to run with the newer versions
//
//  To run simple OTPC simulations
//
////////////////////////////////////////////////////////////////

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"

#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif

#ifdef G4VIS_USE
// #include "OTPCVisManager.hh"
#include "G4VisExecutive.hh"
#endif

#include "OTPCDetectorConstruction.hh"
#include "OTPCPhysicsList.hh"
#include "OTPCPrimaryGeneratorAction3.hh"
#include "OTPCRunAction3.hh"
#include "OTPCEventAction.hh"
#include "OTPCSteppingAction.hh"

#include "Randomize.hh"
#include "globals.hh"


#include "G4ios.hh"
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <memory>
#include <chrono>
#include <format>

inline std::string filename_string(std::string path_str) {
	return path_str.substr(path_str.rfind("\\") + 1, path_str.size() - path_str.rfind("\\") - 1);
};

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << '\n'
#define checkpoint std::cout << "checkpoint" << _endl_

//using namespace std;

int main(int argc, char** argv) {
	auto start = std::chrono::high_resolution_clock::now();

	// Create results directory
	auto resultsDirectoryPath = std::filesystem::current_path() / "results_TPC";
	if (!std::filesystem::exists(resultsDirectoryPath)) {
		std::filesystem::create_directory(resultsDirectoryPath);
	}

	G4int numberOfEvent = 10000;

	if (argc > 1) {
		numberOfEvent = std::stoi(argv[1]);
	}

	// Choose the random engine and initialize
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanluxEngine);

	G4long Seed = 2193585;
	G4int Lux = 3;
	CLHEP::HepRandom::setTheSeed(Seed, Lux);

	checkpoint;
#ifdef  WIN32
	_putenv_s("G4ENSDFSTATEDATA", "C:\\Program Files (x86)\\Geant4 10.5\\data\\G4ENSDFSTATE2.2");
	system("set G4ENSDFSTATEDATA");
	_putenv_s("G4LEVELGAMMADATA", "C:\\Program Files(x86)\\Geant4 10.5\\data\\PhotonEvaporation5.3");
	system("set G4LEVELGAMMADATA");
	_putenv_s("G4RADIOACTIVEDATA", "C:\\Program Files (x86)\\Geant4 10.5\\data\\RadioactiveDecay5.3");
	system("set G4RADIOACTIVEDATA");
	_putenv_s("G4LEDATA", "C:\\Program Files (x86)\\Geant4 10.5\\data\\G4EMLOW7.7");
	system("set G4LEDATA");
#endif //  WIN32
	// construct the default run manager
	std::unique_ptr<G4RunManager> runManager = std::make_unique<G4RunManager>();

	checkpoint;
	// set mandatory initialization classes
	auto OTPCdetector = new OTPCDetectorConstruction;
	runManager->SetUserInitialization(OTPCdetector);
	auto OTPCphysList = new OTPCPhysicsList;
	runManager->SetUserInitialization(OTPCphysList);

	checkpoint;
	// set aditional user action classes
	OTPCRunAction* OTPCrun = new OTPCRunAction;
	runManager->SetUserAction(OTPCrun);
	OTPCEventAction* OTPCevent = new OTPCEventAction(OTPCrun);
	runManager->SetUserAction(OTPCevent);
	OTPCSteppingAction* OTPCstep = new OTPCSteppingAction(OTPCevent);
	runManager->SetUserAction(OTPCstep);

	checkpoint;
	// set mandatory user action class
	OTPCPrimaryGeneratorAction*
		OTPCgun = new OTPCPrimaryGeneratorAction(OTPCrun);
	runManager->SetUserAction(OTPCgun);

	//   // set mandatory user action class
	//   OTPCPrimaryGeneratorAction* OTPCgun =
	//     new OTPCPrimaryGeneratorAction(OTPCevent);
	//   runManager->SetUserAction(OTPCgun);

	checkpoint;
	// initialize G4 kernel
	runManager->Initialize();

	// Visualization, if you choose to have it!
#ifdef G4VIS_USE
//   G4VisManager* visManager = new OTPCVisManager;
	std::unique_ptr<G4VisManager> visManager = std::make_unique<G4VisExecutive>();
	visManager->Initialize();

	G4VVisManager::GetConcreteInstance()->SetDrawOverlapsFlag(true);
#endif

	const std::vector<G4double> energies = {
		100 * keV,
		200 * keV,
		300 * keV,
		400 * keV,
		500 * keV,
		600 * keV,
		700 * keV,
		800 * keV,
		900 * keV,
		1000 * keV,
		1250 * keV,
		1500 * keV,
		2000 * keV,
		3000 * keV,
		4000 * keV,
		5000 * keV };

	for (auto energy : energies) {
		G4String pname = "proton";
		auto eventFileName = std::format("event_{}keV_{}_{}cm_{}_{}.csv", 
			energy / keV, 
			OTPCdetector->getScintillatorType(), 
			OTPCdetector->getCrystalDepth() / cm, 
			OTPCphysList->getPhysicsListName(),
			OTPCphysList->GetCutValue(pname));
		auto eventFilePath = resultsDirectoryPath / eventFileName;
		OTPCrun->setEventFilePath(eventFilePath);
		// start a run
		checkpoint;
		OTPCgun->getEnergy()[0] = energy; //set energy for each run
		runManager->BeamOn(numberOfEvent);
	}
	auto stop = std::chrono::high_resolution_clock::now();
	std::cout << double((stop - start).count()) / 1e9 << '\n';
	// job termination
	return 0;
}


