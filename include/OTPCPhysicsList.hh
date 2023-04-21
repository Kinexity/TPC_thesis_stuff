//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file electromagnetic/TestEm11/include/OTPCPhysicsList.hh
/// \brief Definition of the OTPCPhysicsList class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
// 14.10.02 (V.Ivanchenko) provide modular list on base of old OTPCPhysicsList
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OTPCPhysicsList_h
#define OTPCPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class StepMax;
class OTPCPhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class OTPCPhysicsList : public G4VModularPhysicsList
{
public:
	OTPCPhysicsList();
	~OTPCPhysicsList();

	virtual void ConstructParticle();

	void AddOTPCPhysicsList();
	virtual void ConstructProcess();
	void AddDecay();
	void AddRadioactiveDecay();
	void AddStepMax();

	void AddIonGasModels();

	StepMax* GetStepMaxProcess() { return fStepMaxProcess; };

	std::string getPhysicsListName();
private:
	G4String               fEmName;
	G4VPhysicsConstructor* fEmOTPCPhysicsList;
	static G4ThreadLocal StepMax* fStepMaxProcess;

	OTPCPhysicsListMessenger* fMessenger;

	G4String name="emlivermore";
	//G4String name = "local";
	//G4String name="emstandard_opt3";
	//G4String name="empenelope";
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

