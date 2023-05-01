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
//#include <conio.h>

inline std::string filename_string(std::string path_str) {
	return path_str.substr(path_str.rfind("\\") + 1, path_str.size() - path_str.rfind("\\") - 1);
};

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << '\n'
#define checkpoint std::cout << "checkpoint" << _endl_

//using namespace std;

std::string get_date_string() {
	auto now = std::chrono::system_clock::now();
	auto current_time = std::chrono::system_clock::to_time_t(now);

	std::tm date;
	localtime_s(&date, &current_time);

	char buffer[100];
	strftime(buffer, 100, "%Y-%m-%d-%H-%M-%S", &date);

	return std::format("[{}]", buffer);
}

std::vector<double> nums(double start, double end, size_t N) {
	if (N < 2 || start > end) {
		throw;
	}
	std::vector<double> v;
	auto mult = std::pow(end / start, 1. / (N - 1));
	for (int i = 0; i < N; i++) {
		v.push_back(start * std::pow(mult, i));
	}
	return v;
};

int main(int argc, char** argv) {

	bool 
		isDense = false,
		skipIfDataExists = false,
		dataOverwrite = false;
	G4double 
		crystalDepth = 10 * cm,
		cutValue = 1 * mm;
	std::string 
		scintillatorType = "CeBr3",
		physicsListName = "emlivermore";
	G4int 
		numberOfEvent = 1000000;

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

	if (argc > 1) {
		numberOfEvent = std::stoi(argv[1]);
		scintillatorType = argv[2];
		crystalDepth = std::stod(argv[3]) * cm;
		physicsListName = argv[4];
		cutValue = std::stod(argv[5]) * mm;
		skipIfDataExists = std::stoi(argv[6]);
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
	OTPCSteppingAction* OTPCstep = new OTPCSteppingAction(OTPCevent);
	runManager->SetUserAction(OTPCstep);

	checkpoint;
	// set mandatory user action class
	OTPCPrimaryGeneratorAction*
		OTPCgun = new OTPCPrimaryGeneratorAction(OTPCrun);
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

	G4String pname = "gamma";
	std::string runDirectoryName;
	std::filesystem::path runDirectoryPath;
	std::string paramString = std::format("{}_{}cm_{}_{}mm",
		OTPCdetector->getScintillatorType(),
		OTPCdetector->getCrystalDepth() / cm,
		OTPCphysList->getPhysicsListName(),
		OTPCphysList->GetCutValue(pname) / mm);
	for (int i = 0;; i++) {
		runDirectoryName = std::format("event_{}_{}",
			paramString,
			i);
		runDirectoryPath = resultsDirectoryPath / runDirectoryName;
		if (std::filesystem::exists(runDirectoryPath) && skipIfDataExists) {
			std::cout << "Data exists. Aborting simulation." << _endl_;
			return 0;
		}
		if (!std::filesystem::exists(runDirectoryPath) || dataOverwrite) {
			break;
		}
	}
	std::filesystem::create_directory(runDirectoryPath);
	OTPCdetector->saveDetails(runDirectoryPath);
	OTPCgun->setRunPath(runDirectoryPath);
	for (auto energy : energies) {
		auto partialFileName = std::format("event_{}keV_{}_",
			energy / keV,
			paramString);
		auto eventTotalDepositFileName = partialFileName + "totalDeposit";
		auto eventStepsDepositFileName = partialFileName + "stepsDeposit";
		auto eventTotalDepositFilePath = runDirectoryPath / eventTotalDepositFileName;
		auto eventStepsDepositFilePath = runDirectoryPath / eventStepsDepositFileName;
		OTPCrun->setEventFilePath(eventTotalDepositFilePath, eventStepsDepositFilePath);
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


