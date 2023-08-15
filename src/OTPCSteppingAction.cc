// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: ExN02SteppingAction.cc,v 1.3 1999/12/15 14:49:22 gunter Exp $
// GEANT4 tag $Name: geant4-02-00 $
//
/////////////////////////////////////////////////////////////////////
//
//  V.Guadilla 2019
//
//  Collect the energy deposited in the sensitive OTPC volume
///////////////////////////////////////////////////////////////////

#include "OTPCSteppingAction.hh"
#include "OTPCEventAction.hh"
#include "G4SteppingManager.hh"
#include "G4RadioactiveDecay.hh"
#include "G4DynamicParticle.hh"

#include "G4SystemOfUnits.hh"
#include <format>
#include <set>

OTPCSteppingAction::OTPCSteppingAction(OTPCEventAction* EvAct, const std::string& scintName) :eventAction(EvAct), scintilatorType(scintName) {}

inline std::string filename_string(std::string path_str) {
	return path_str.substr(path_str.rfind("\\") + 1, path_str.size() - path_str.rfind("\\") - 1);
};

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << '\n'
#define checkpoint std::cout << "checkpoint" << _endl_

std::vector<std::string>
scintillatorProcesses = { "compt", "eBrem", "msc", "phot", "eIoni", "UserMaxStep" },
gasProcesses = { "eIoni", "hIoni", "UserMaxStep" };

void OTPCSteppingAction::UserSteppingAction(const G4Step* aStep)
{

	G4double edep = aStep->GetTotalEnergyDeposit();

	G4StepPoint* prePoint = aStep->GetPreStepPoint();
	G4StepPoint* postPoint = aStep->GetPostStepPoint();
	G4TouchableHandle touch = prePoint->GetTouchableHandle();
	const std::string currentMaterialName = touch->GetVolume()->GetLogicalVolume()->GetMaterial()->GetName();
	const G4VProcess* process = postPoint->GetProcessDefinedStep();
	std::string processName = process->GetProcessName();
	auto track = aStep->GetTrack();
	G4int begin;

	G4String nameP = track->GetDefinition()->GetParticleName();

	if (false && process) {
		//if (edep>0.0 & currentMaterialName == "GasOTPC" & nameP=="alpha"){
		//if (edep > 0.0 & currentMaterialName == "GasOTPC") {
			//

		auto prePos = prePoint->GetPosition();
		auto postPos = postPoint->GetPosition();
		auto deltaPos = postPos - prePos;
		auto pos = prePos + G4UniformRand() * deltaPos; // position randomization simplified


		//eventAction->addProcess(x / mm, y / mm, z / mm, processName + '\t' + nameP);
		eventAction->addEdep(edep / keV, pos.x() / mm, pos.y() / mm, pos.z() / mm);
		//G4cout<<edep/keV<<"    "<<x/mm<<"    "<<y/mm<<"    "<<z/mm<<G4endl;
	}

	if (edep > 0.0) {
		if (currentMaterialName == scintilatorType && std::find(scintillatorProcesses.begin(), scintillatorProcesses.end(), processName) != scintillatorProcesses.end()) {
			G4int nCrystal = touch->GetCopyNumber(1); //N will be the number of levels up, we have to check it to pickup the index of CeBr3 crystal
			eventAction->depositEnergyOnCrystal(nCrystal, edep / keV);
		}
		else if (currentMaterialName == "GasOTPC" && std::find(gasProcesses.begin(), gasProcesses.end(), processName) != gasProcesses.end()) {
			eventAction->depositEnergyOnGas(edep / keV);
		}
	}

	if (currentMaterialName == scintilatorType) {
		eventAction->setFlag();
	}
}


