// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: ExN02RunAction.cc,v 1.3 1999/12/15 14:49:22 gunter Exp $
// GEANT4 tag $Name: geant4-02-00 $
//
////////////////////////////////////////////////////////////////
//
//  V. Guadilla 2021
///////////////////////////////////////////////////////////////

#include "OTPCRunAction3.hh"

#include "G4Run.hh"

#include "G4ios.hh"
#include "fstream"
#include "iomanip"
#include "Randomize.hh"
#include "time.h"


#include "G4SystemOfUnits.hh"
// #include "g4std/fstream"
// #include "g4std/iomanip"

using namespace std;


OTPCRunAction::OTPCRunAction()
{
	timer = std::make_unique<G4Timer>();

	eventFile.open("event.bin", std::ios_base::out | std::ios_base::binary);

	///////////////////////////////////////////////////////////////////////////////////	

}

void OTPCRunAction::BeginOfRunAction(const G4Run* rn)
{
	//Start CPU timer
	timer->Start();
	eventData.resize(rn->GetNumberOfEventToBeProcessed() * 20);
}

void OTPCRunAction::EndOfRunAction(const G4Run*)
{
	eventFile.write((const char*)eventData.data(), eventData.size() * sizeof(G4double));
	eventFile.close();

	//Stop timer and get CPU time
	timer->Stop();
	G4double cputime = timer->GetUserElapsed();
	cout << " CPU time = " << cputime << " s" << endl;


}

void OTPCRunAction::fillOut(std::vector<std::array<G4double, 4>>& EnergyDeposit, std::array<G4double, 20>& EnergyGammaCrystals)
{

	//for (auto& EnergyDeposit_i : EnergyDeposit) {
	//
	//	if (EnergyDeposit_i[3] > 0) {
	//		//eventFile << EnergyDeposit_i[0] << ',' << EnergyDeposit_i[1] << ',' << EnergyDeposit_i[2] << ',' << EnergyDeposit_i[3] << '\n';
	//	}
	//}
	//eventFile << "\n\n";
	std::copy(EnergyGammaCrystals.begin(), EnergyGammaCrystals.end(), eventData.begin() + eventIndex * 20);
	eventIndex++;
	if (eventIndex % 10000 == 0) {
		std::cout << eventIndex << '\n';
	}
	//std::stringstream ss;
	//for (auto& EnergyDepositOneCrystal : EnergyGammaCrystals) {
	//	ss << EnergyDepositOneCrystal << '\t';
	//}
	//ss << "\n";
	//eventFile << ss.str();
}
