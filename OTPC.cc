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
#include "OTPCPrimaryGeneratorAction.hh"
#include "OTPCRunAction.hh"
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
#include <algorithm>
#include <numeric>
#include "utilities.h"

int main(int argc, char** argv) {

	bool
		isDense = false,
		skipIfDataExists = false,
		dataOverwrite = false,
		loadDataFromFile = false;
	G4double
		crystalDepth = 10 * cm,
		cutValue = 0.01 * mm;
	std::string
		scintillatorType = "CeBr3",
		physicsListName = "emlivermore",
		positionalArg,
		additionalInfo = "";
	uint64_t
		numberOfEvent = 1000000,
		eventsSliceSize = 1000000;
	G4ThreeVector
		particleInitialPosition;

	auto start = std::chrono::high_resolution_clock::now();

	std::filesystem::path homeDirectory = "C:\\Users\\26kub";
	if (!std::filesystem::exists(homeDirectory)) {
		std::cout << "Set correct home directory!" << _endl_;
		exit(1);
	}
	auto resultsDirectoryPath = homeDirectory / "results_TPC";
	if (!std::filesystem::exists(resultsDirectoryPath)) {
		std::filesystem::create_directory(resultsDirectoryPath);
	}

	switch (argc) {
	case 9:
		loadDataFromFile = argv[8];
	case 8:
		positionalArg = argv[7];
	case 7:
		skipIfDataExists = std::stoi(argv[6]);
	case 6:
		cutValue = std::stod(argv[5]) * mm;
	case 5:
		physicsListName = argv[4];
	case 4:
		crystalDepth = std::stod(argv[3]) * cm;
	case 3:
		scintillatorType = argv[2];
	case 2:
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
	auto OTPCdetector = new OTPCDetectorConstruction(crystalDepth, scintillatorType);
	runManager->SetUserInitialization(OTPCdetector);
	auto OTPCphysList = new OTPCPhysicsList(physicsListName);
	runManager->SetUserInitialization(OTPCphysList);

	checkpoint;
	// set aditional user action classes
	OTPCRunAction* OTPCrun = new OTPCRunAction;
	runManager->SetUserAction(OTPCrun);
	OTPCEventAction* OTPCevent = new OTPCEventAction(OTPCrun);
	runManager->SetUserAction(OTPCevent);
	OTPCSteppingAction* OTPCstep = new OTPCSteppingAction(OTPCevent, OTPCdetector->getScintillatorType());
	runManager->SetUserAction(OTPCstep);

	checkpoint;
	// set mandatory user action class
	OTPCPrimaryGeneratorAction*
		OTPCgun = new OTPCPrimaryGeneratorAction(OTPCrun, loadDataFromFile);
	runManager->SetUserAction(OTPCgun);

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
	OTPCphysList->SetDefaultCutValue(cutValue);

	std::vector<G4double> energies;
	if (isDense) {
		energies = nums(100 * keV, 5000 * keV, 250);
	}
	else if (loadDataFromFile) {
		energies.push_back(0); // add dummy value to do one run loop
	}
	else {
		energies = {
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
	}

	if (positionalArg.length() == 4) { // move particle source
		int
			xPositionSetting = std::stoi(positionalArg.substr(0, 2)),
			yPositionSetting = std::stoi(positionalArg.substr(2, 1)),
			zPositionSetting = std::stoi(positionalArg.substr(3, 1));
		auto corner = OTPCdetector->getChamberCorner();
		particleInitialPosition = {
			corner.getX() * xPositionSetting,
			corner.getY() * yPositionSetting,
			corner.getZ() * zPositionSetting
		};
		OTPCgun->setPosition(particleInitialPosition);
		additionalInfo += std::format("_{}", positionalArg);
	}
	else if (loadDataFromFile) {
		additionalInfo += "_dataFile";
	}


	G4String pname = "gamma";
	std::string runDirectoryName;
	std::filesystem::path runDirectoryPath;
	std::string paramString = std::format("{}_{}cm_{}_{}mm{}",
		OTPCdetector->getScintillatorType(),
		OTPCdetector->getCrystalDepth() / cm,
		OTPCphysList->getPhysicsListName(),
		OTPCphysList->GetCutValue(pname) / mm,
		additionalInfo);
	// find first available simulation index
	for (int i = 0;; i++) {
		runDirectoryName = std::format("event_{}_{}",
			paramString,
			i);
		runDirectoryPath = resultsDirectoryPath / runDirectoryName;
		if (std::filesystem::exists(runDirectoryPath) && skipIfDataExists) {
			std::cout << "Data exists. Aborting simulation." << _endl_;
			return 0; // data exists, exit program
		}
		if (!std::filesystem::exists(runDirectoryPath) || dataOverwrite) {
			break; // index found
		}
	}
	std::filesystem::create_directory(runDirectoryPath);
	OTPCdetector->saveDetails(runDirectoryPath);
	OTPCgun->setRunPath(runDirectoryPath);
	// iterate over energies
	for (auto energy : energies) {
		if (!loadDataFromFile) { // when loading from file dummy energy value is used once in the loop
			OTPCgun->setEnergy(energy); //set energy for each run
		}
		auto partialFileName = std::format("event_{}keV_{}_",
			OTPCgun->getEnergy() / keV,
			paramString);
		auto eventTotalDepositFileName = partialFileName + "totalDeposit";
		auto eventStepsDepositFileName = partialFileName + "stepsDeposit";
		auto eventTotalDepositFilePath = runDirectoryPath / eventTotalDepositFileName;
		auto eventStepsDepositFilePath = runDirectoryPath / eventStepsDepositFileName;
		OTPCrun->setEventFilePath(eventTotalDepositFilePath, eventStepsDepositFilePath);
		checkpoint;
		// start a run
		for (uint64_t eventCount = 0; eventCount < numberOfEvent; eventCount += eventsSliceSize) {
			auto runEventNumber = std::min(eventsSliceSize, numberOfEvent - eventCount);
			runManager->BeamOn(runEventNumber);
			std::cout << std::format("Events finished {}/{}\n", std::min(eventCount + eventsSliceSize, numberOfEvent), numberOfEvent);
		}
	}
	auto stop = std::chrono::high_resolution_clock::now();
	std::cout << double((stop - start).count()) / 1e9 << '\n';
	// job termination
	return 0;
}


