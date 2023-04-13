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

#include "OTPCRunAction3.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"


#include "globals.hh"
#include "G4ios.hh"
#include "fstream"
#include "iomanip"

inline std::string filename_string(std::string path_str) {
	return path_str.substr(path_str.rfind("\\") + 1, path_str.size() - path_str.rfind("\\") - 1);
};

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << '\n'
#define checkpoint std::cout << "checkpoint" << _endl_

OTPCEventAction::OTPCEventAction(OTPCRunAction* RunAct)
	:runAction(RunAct)
{}

OTPCEventAction::~OTPCEventAction()
{}

void OTPCEventAction::BeginOfEventAction(const G4Event*) {
	//EnergyDeposit.clear();
	//for (auto& elem : TotalEnergyDepositCrystal) {
	//	elem = 0;
	//}
}

void OTPCEventAction::EndOfEventAction(const G4Event* evt)
{
	/*
	for(G4int i=0; i<RangeX; i++){
	  if(EnergyDepositX[i]>0){G4cout<<"From event: "<<i<<" "<<EnergyDepositX[i]<<G4endl;}
	}*/

	runAction->fillOut(EnergyDeposit, TotalEnergyDepositCrystal);

}

void OTPCEventAction::add_E_i(G4int nCrystal, G4double edep)
{
	TotalEnergyDepositCrystal[nCrystal] += edep; //we have to initialize this variable at the beginning of the EventAction
}

void OTPCEventAction::addEdep(G4double Edep, G4double x, G4double y, G4double z) {
	EnergyDeposit.push_back({ x,y,z,Edep });
	//G4cout<<nstep<<G4endl;

}


