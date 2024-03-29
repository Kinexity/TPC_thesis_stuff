// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: ExN02EventAction.cc,v 1.4 2000/01/24 14:45:51 sOTPCing Exp $
// GEANT4 tag $Name: geant4-02-00 $
//
//////////////////////////////////////////////////////////////////////
//
// V. Guadilla 2021
/////////////////////////////////////////////////////////////////////

#include "OTPCEventAction.hh"

#include "OTPCRunAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"


#include "globals.hh"
#include "G4ios.hh"
#include "fstream"
#include "iomanip"
#include <numeric>


OTPCEventAction::OTPCEventAction(OTPCRunAction* RunAct) : runAction(RunAct) {}

void OTPCEventAction::BeginOfEventAction(const G4Event*) {
	EnergyDeposit.clear();
	ProcessStep.clear();
	TotalEnergyDepositCrystal.fill(0);
	TotalEnergyDepositGas = 0;
	internalFlag = false;
}

void OTPCEventAction::EndOfEventAction(const G4Event* evt) {

	//runAction->fillOut(EnergyDeposit, TotalEnergyDepositCrystal);
	G4double totalEnergy = std::reduce(TotalEnergyDepositCrystal.begin(), TotalEnergyDepositCrystal.end());
	runAction->fillOutSteps(ProcessStep, totalEnergy);
	runAction->fillOutGasIonization(TotalEnergyDepositGas);
	if (includeZeroEnergy || totalEnergy > 0) {
		runAction->fillOutScintillation(TotalEnergyDepositCrystal);
	}
	runAction->updateEventCounter(internalFlag);

}

void OTPCEventAction::depositEnergyOnCrystal(G4int nCrystal, G4double edep) {
	TotalEnergyDepositCrystal[nCrystal] += edep; //we have to initialize this variable at the beginning of the EventAction
}

void OTPCEventAction::depositEnergyOnGas(G4double edep) {
	TotalEnergyDepositGas += edep;
}

void OTPCEventAction::setFlag() {
	internalFlag = true;
}

void OTPCEventAction::addEdep(G4double Edep, G4double x, G4double y, G4double z) {
	EnergyDeposit.push_back({ Edep, x, y, z });
}

void OTPCEventAction::addProcess(G4double x, G4double y, G4double z, G4String name) {
	ProcessStep.push_back({ x,y,z,name });
}


