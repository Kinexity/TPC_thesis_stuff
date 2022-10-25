// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: ExN02RunAction.hh,v 1.3 1999/12/15 14:49:20 gunter Exp $
// GEANT4 tag $Name: geant4-02-00 $
//
//
/////////////////////////////////////////////////////////////////
//
//  V Guadilla 2021
//
/////////////////////////////////////////////////////////////////

#ifndef OTPCRunAction_h
#define OTPCRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <fstream>
#include <iomanip>
#include <iostream>
#include "G4Timer.hh"

class G4Run;

class G4Timer;

class OTPCRunAction : public G4UserRunAction
{
  public:
    OTPCRunAction();
    ~OTPCRunAction();

  public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);


    void fillOut(G4double EnergyDepositX[5000], G4double EnergyDepositY[5000], G4double EnergyDepositZ[5000], G4double EnergyDeposit[5000]);    
    
    G4double transferE1();
    G4double transferE2();
    G4double transferE3();
    
    G4int transferType1();
    G4int transferType2();
    G4int transferType3();
    
    G4double transferTheta1();
    G4double transferTheta2();
    G4double transferTheta3();
    
    G4double transferPhi1();
    G4double transferPhi2();
    G4double transferPhi3();
    
    G4double transferX();
    G4double transferY();
    G4double transferZ();
    
    
private:
   
    G4Timer* timer;
    
    FILE * eventFile;
    G4double                  x[5000];
    G4double                  y[5000];
    G4double                  z[5000];
    G4double                  E[5000];
    G4String              header1, header2;
    G4int                 type1;
    G4int                 type2;
    G4int                 type3;
    G4double              E1;
    G4double              E2;
    G4double              E3;
    G4double              theta1;
    G4double              theta2;
    G4double              theta3;
    G4double              phi1;
    G4double              phi2;
    G4double              phi3;
    G4double              xi,yi,zi;
  
};

#endif






