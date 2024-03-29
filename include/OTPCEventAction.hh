// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: ExN02EventAction.hh,v 1.3 1999/12/15 14:49:20 gunter Exp $
// GEANT4 tag $Name: geant4-02-00 $
//
///////////////////////////////////////////////////////////////////
//
// V. Guadilla 2021
// Accumulated the energy deposited in sensitive volumes
//
//////////////////////////////////////////////////////////////////

#ifndef OTPCEventAction_h
#define OTPCEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <array>
#include <vector>
#include <tuple>

class G4Event;
class OTPCRunAction;

class OTPCEventAction : public G4UserEventAction
{
public:
	OTPCEventAction(OTPCRunAction*);
	~OTPCEventAction() = default;

public:
	void BeginOfEventAction(const G4Event*);
	void EndOfEventAction(const G4Event*);
	void addEdep(G4double Edep, G4double x, G4double y, G4double z);
	void addProcess(G4double x, G4double y, G4double z, G4String name);
	void depositEnergyOnCrystal(G4int nCrystal, G4double edep);
	void depositEnergyOnGas(G4double edep);
	void setFlag();

private:
	OTPCRunAction* runAction;
	G4int Range;

	std::vector<std::tuple<G4double, G4double, G4double, G4String>>
		ProcessStep;
	std::vector<std::array<G4double, 4>>
		EnergyDeposit;
	std::array<G4double, 20>
		TotalEnergyDepositCrystal = { 0 };
	G4double
		TotalEnergyDepositGas;
	const bool includeZeroEnergy = true;
	bool internalFlag;
};

#endif
