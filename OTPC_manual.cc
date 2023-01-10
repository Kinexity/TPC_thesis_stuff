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
//  2022 V. Guadilla
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



//using namespace std;

int main(int argc, char** argv)
{
	// Choose the random engine and initialize
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanluxEngine);

	G4long Seed = 2193585;
	G4int Lux = 3;
	CLHEP::HepRandom::setTheSeed(Seed, Lux);

	_putenv_s("G4ENSDFSTATEDATA", "C:\\Program Files (x86)\\Geant4 10.5\\data\\G4ENSDFSTATE2.2");
	system("set G4ENSDFSTATEDATA");
	_putenv_s("G4LEVELGAMMADATA", "C:\\Program Files(x86)\\Geant4 10.5\\data\\PhotonEvaporation5.3");
	system("set G4LEVELGAMMADATA");
	_putenv_s("G4RADIOACTIVEDATA", "C:\\Program Files (x86)\\Geant4 10.5\\data\\RadioactiveDecay5.3");
	system("set G4RADIOACTIVEDATA");
	_putenv_s("G4LEDATA", "C:\\Program Files (x86)\\Geant4 10.5\\data\\G4EMLOW7.7");
	system("set G4LEDATA");

	//   // open efficiency file
	//   ofstream efficiencyOutputFile("OTPC.eff");
	//   efficiencyOutputFile << " Efficiency for OTPC runs" << G4endl;

	  // construct the default run manager
	G4RunManager* runManager = new G4RunManager;

	// set mandatory initialization classes
	runManager->SetUserInitialization(new OTPCDetectorConstruction);
	runManager->SetUserInitialization(new OTPCPhysicsList);

	// set aditional user action classes
	OTPCRunAction* OTPCrun = new OTPCRunAction;
	runManager->SetUserAction(OTPCrun);
	OTPCEventAction* OTPCevent = new OTPCEventAction(OTPCrun);
	runManager->SetUserAction(OTPCevent);
	OTPCSteppingAction* OTPCstep =
		new OTPCSteppingAction(OTPCevent);
	runManager->SetUserAction(OTPCstep);

	// set mandatory user action class
	OTPCPrimaryGeneratorAction*
		OTPCgun = new OTPCPrimaryGeneratorAction(OTPCrun);
	runManager->SetUserAction(OTPCgun);

	//   // set mandatory user action class
	//   OTPCPrimaryGeneratorAction* OTPCgun =
	//     new OTPCPrimaryGeneratorAction(OTPCevent);
	//   runManager->SetUserAction(OTPCgun);

	  // initialize G4 kernel
	runManager->Initialize();

	// Visualization, if you choose to have it!
#ifdef G4VIS_USE
//   G4VisManager* visManager = new OTPCVisManager;
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
#endif

	//get the pointer to the User Interface manager
	G4UImanager* UI = G4UImanager::GetUIpointer();

	G4UIsession* session = 0;

	if (argc == 1)
	{
		// define UI terminal for interactive mode
#ifdef G4UI_USE_TCSH
		session = new G4UIterminal(new G4UItcsh);
#else
		session = new G4UIterminal();
#endif

		session->SessionStart();
		delete session;
	}
	else
		// Batch mode
	{
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UI->ApplyCommand(command + fileName);
	}

	// job termination
#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;
	return 0;
}


