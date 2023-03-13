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

#include "OTPCRunAction.hh"

#include "G4Run.hh"

#include "G4ios.hh"
#include "fstream"
#include "iomanip"
#include "Randomize.hh"
#include "time.h"

#include <format>

#include "G4SystemOfUnits.hh"
// #include "g4std/fstream"
// #include "g4std/iomanip"

using namespace std;


OTPCRunAction::OTPCRunAction()
{
	//choose the Random engine 
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
	//set random seed with system time (it must change from one run to another!!! otherwise we simulate all the time the same in the OTPC case!!)
	G4long seed = time(NULL);
	CLHEP::HepRandom::setTheSeed(seed);
	//////////////////////////////////////////////////////////////////////////////

	static const double     pi = 3.14159265358979323846;

	timer = new G4Timer;

	//////////Reading the input data for primary generator///////////

	ifstream evenInputInformation("../../../particles.data");

	evenInputInformation >> header1;
	evenInputInformation >> type1 >> type2 >> type3;
	evenInputInformation >> header2;
	evenInputInformation >> E1 >> E2 >> E3;
	evenInputInformation.close();
	G4cout << E1 << " " << E2 << " " << E3 << endl;

	//Dimensions OTPC:
	G4double RangeX = 322.56; //mm 
	G4double RangeY = 322.56; //mm 
	G4double RangeZ = 142; //mm 

	//Random initial point inside OTPC (70% of volume, excluding sides)

	//xi=-0.35*RangeX+G4UniformRand()*0.7*RangeX;
	//yi=-0.35*RangeY+G4UniformRand()*0.7*RangeY;
	//zi=-0.35*RangeZ+G4UniformRand()*0.7*RangeZ;

	xi = 0.0;
	yi = 0.0;
	zi = 0.0;

	G4double aperture = 2.; // in degrees

	theta1 = acos(1. + (cos(aperture * degree) - 1.) * G4UniformRand()) * 180 / pi;
	phi1 = 360 * G4UniformRand();

	theta2 = acos(1. + (cos(aperture * degree) - 1.) * G4UniformRand()) * 180 / pi;
	phi2 = 360 * G4UniformRand();

	theta3 = acos(1. + (cos(aperture * degree) - 1.) * G4UniformRand()) * 180 / pi;
	phi3 = 360 * G4UniformRand();


	FILE* metaFile = fopen("metadata.txt", "a");
	fprintf(metaFile, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", E1, E2, E3, xi, yi, zi, theta1, theta2, phi1, phi2, theta3, phi3);
	fclose(metaFile);

	eventFile.open("event.csv", std::ios_base::out);

	///////////////////////////////////////////////////////////////////////////////////	

}

OTPCRunAction::~OTPCRunAction()
{
	delete timer;
}

void OTPCRunAction::BeginOfRunAction(const G4Run*)
{
	//Start CPU timer
	timer->Start();

}

void OTPCRunAction::EndOfRunAction(const G4Run*)
{

	eventFile.close();

	//Stop timer and get CPU time
	timer->Stop();
	G4double cputime = timer->GetUserElapsed();
	cout << " CPU time = " << cputime << " s" << endl;


}

G4double OTPCRunAction::transferE1()
{
	return E1;
}
G4double OTPCRunAction::transferE2()
{
	return E2;
}
G4double OTPCRunAction::transferE3()
{
	return E3;
}
G4int OTPCRunAction::transferType1()
{
	return type1;
}
G4int OTPCRunAction::transferType2()
{
	return type2;
}
G4int OTPCRunAction::transferType3()
{
	return type3;
}
G4double OTPCRunAction::transferTheta1()
{
	return theta1;
}
G4double OTPCRunAction::transferTheta2()
{
	return theta2;
}
G4double OTPCRunAction::transferTheta3()
{
	return theta3;
}
G4double OTPCRunAction::transferPhi1()
{
	return phi1;
}
G4double OTPCRunAction::transferPhi2()
{
	return phi2;
}
G4double OTPCRunAction::transferPhi3()
{
	return phi3;
}
G4double OTPCRunAction::transferX()
{
	return xi;
}
G4double OTPCRunAction::transferY()
{
	return yi;
}
G4double OTPCRunAction::transferZ()
{
	return zi;
}

void OTPCRunAction::fillOut(std::vector<std::array<G4double, 4>> EnergyDeposit, std::array<G4double, 20> EnergyGammaCrystals)
{

	for (auto& EnergyDeposit_i : EnergyDeposit) {

		if (EnergyDeposit_i[3] > 0) {
			eventFile << EnergyDeposit_i[0] << ',' << EnergyDeposit_i[1] << ',' << EnergyDeposit_i[2] << ',' << EnergyDeposit_i[3] << '\n';
		}
	}

	for (auto& EnergyDepositOneCrystal : EnergyGammaCrystals) {
		eventFile << EnergyDepositOneCrystal << '\t';
	}
}
