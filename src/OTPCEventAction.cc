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


OTPCEventAction::OTPCEventAction(OTPCRunAction* RunAct)
:runAction(RunAct)
{}

OTPCEventAction::~OTPCEventAction()
{}

void OTPCEventAction::BeginOfEventAction(const G4Event*)
{
  
  Range=5000; //max number of steps saved
  
  for(G4int i=0; i<Range; i++){
      EnergyDepositX[i]=0.0;
  }
  for(G4int i=0; i<Range; i++){
      EnergyDepositY[i]=0.0;
  }
  for(G4int i=0; i<Range; i++){
      EnergyDepositZ[i]=0.0;
  }
  for(G4int i=0; i<Range; i++){
      EnergyDeposit[i]=0.0;
  }
  
  nstep=0;

}

void OTPCEventAction::EndOfEventAction(const G4Event* evt)
{
  /*  
  for(G4int i=0; i<RangeX; i++){
    if(EnergyDepositX[i]>0){G4cout<<"From event: "<<i<<" "<<EnergyDepositX[i]<<G4endl;}
  }*/
  
  runAction->fillOut(EnergyDepositX,EnergyDepositY,EnergyDepositZ,EnergyDeposit);
  
}

void OTPCEventAction::addEdep(G4double Edep, G4double x, G4double y, G4double z)
{
  EnergyDepositX[nstep] = x;
  EnergyDepositY[nstep] = y;
  EnergyDepositZ[nstep] = z;
  EnergyDeposit[nstep] = Edep;
  nstep++;
  //G4cout<<nstep<<G4endl;
  
}


