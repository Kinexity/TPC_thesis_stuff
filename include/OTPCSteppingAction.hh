// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: ExN02SteppingAction.hh,v 1.3 1999/12/15 14:49:20 gunter Exp $
// GEANT4 tag $Name: geant4-02-00 $
//
//////////////////////////////////////////////////////////////////////
//
//  V. Guadilla 2021 
//  Get energy deposited in sensitive volume
//


#ifndef OTPCSteppingAction_h
#define OTPCSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include <string>

class OTPCEventAction;

class OTPCSteppingAction : public G4UserSteppingAction
{
public:
	OTPCSteppingAction(OTPCEventAction*, const std::string& scintName);
	~OTPCSteppingAction() = default;

	void UserSteppingAction(const G4Step*);

private:
	const std::string& scintilatorType;
	OTPCEventAction* eventAction;
};

#endif
