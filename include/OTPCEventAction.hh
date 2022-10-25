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

class G4Event;
class OTPCRunAction;

class OTPCEventAction : public G4UserEventAction
{
  public:
    OTPCEventAction(OTPCRunAction*);
    ~OTPCEventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void addEdep(G4double Edep, G4double x, G4double y, G4double z);
    

  private:
    OTPCRunAction*            runAction;
    G4int                     Range;
    G4int                     nstep;
    
    G4double                  EnergyDepositX[5000];
    G4double                  EnergyDepositY[5000];
    G4double                  EnergyDepositZ[5000];
    G4double                  EnergyDeposit[5000];
};

#endif