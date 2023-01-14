/////////////////////////////////////////////////////////////////////////
//
// V. Guadilla 2021
/////////////////////////////////////////////////////////////////////////

#include "OTPCPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GeneralParticleSource.hh" 
#include "G4IonTable.hh"

#include "G4ThreeVector.hh"
#include "globals.hh"
#include "Randomize.hh"

#include "G4ios.hh"
#include "fstream"
#include "iomanip"

#include "G4SystemOfUnits.hh"

#include "OTPCRunAction.hh"

using namespace std;


OTPCPrimaryGeneratorAction::OTPCPrimaryGeneratorAction(OTPCRunAction* RunAct)
	:runAction(RunAct)
{

	// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	//Geantino for geometry checking
	  G4int n_particle = 1;
	  particleGun = new G4ParticleGun(n_particle);

	  // particle type
	  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	  G4String particleName;

	  // particle type
	  particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="geantino"));
	  particleGun->SetParticleEnergy(1.0*GeV);
	  */

	  /// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// particles per event
	G4int n_particle = 1;
	particleGun = new G4ParticleGun(n_particle);

	// particle type
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

	/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



}

OTPCPrimaryGeneratorAction::~OTPCPrimaryGeneratorAction()
{
	delete particleGun;
}



void OTPCPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

	// particle types
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

	G4ParticleDefinition* proton
		= particleTable->FindParticle(particleName = "proton");
	G4ParticleDefinition* alpha
		= particleTable->FindParticle(particleName = "alpha");

	// information transfered from the RunAction about the source.data file or generated in the Run Action itself

	E[0] = runAction->transferE1();
	E[1] = runAction->transferE2();
	E[2] = runAction->transferE3();
	type[0] = runAction->transferType1();
	type[1] = runAction->transferType2();
	type[2] = runAction->transferType3();
	theta[0] = runAction->transferTheta1();
	theta[1] = runAction->transferTheta2();
	theta[2] = runAction->transferTheta3();
	phi[0] = runAction->transferPhi1();
	phi[1] = runAction->transferPhi2();
	phi[2] = runAction->transferPhi3();

	x = runAction->transferX();
	y = runAction->transferY();
	z = runAction->transferZ();

	G4int Z, A;
	//10Be
	Z = 4;
	A = 10;
	G4double excitEnergy = 0. * keV;
	G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z, A, excitEnergy);


	G4double ux, uy, uz;

	for (G4int i = 0; i < 3; i++) {
		if (type[i] > 0 & type[i] < 4) {

			//Momentum direction according to input angles
			ux = sin(theta[i] * degree) * cos(phi[i] * degree);
			uy = sin(theta[i] * degree) * sin(phi[i] * degree);
			uz = cos(theta[i] * degree);

			//G4cout<<ux<<" "<<uy<<" "<<uz<<" "<<x<<" "<<y<<" "<<z<<G4endl;

			if (type[i] == 1) { particleGun->SetParticleDefinition(proton); }
			else if (type[i] == 2) { particleGun->SetParticleDefinition(alpha); }
			else if (type[i] == 3) { particleGun->SetParticleDefinition(ion); }

			particleGun->SetParticlePosition(G4ThreeVector(x * mm, y * mm, z * mm));
			particleGun->SetParticleMomentumDirection(G4ThreeVector(ux, uy, uz));
			particleGun->SetParticleEnergy(E[i] * keV);
			particleGun->GeneratePrimaryVertex(anEvent);

			//G4cout<<theta[i]<<" "<<phi[i]<<" "<<E[i]<<" lauched"<<G4endl;
		}
	}







}


