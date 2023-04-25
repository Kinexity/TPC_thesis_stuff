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
#include "G4RandomDirection.hh"

#include "G4ThreeVector.hh"
#include "globals.hh"
#include "Randomize.hh"

#include "G4ios.hh"
#include "fstream"
#include "iomanip"
#include <cstdlib>

#include "G4SystemOfUnits.hh"

#include "OTPCRunAction.hh"

using namespace std;

inline std::string filename_string(std::string path_str) {
	return path_str.substr(path_str.rfind("\\") + 1, path_str.size() - path_str.rfind("\\") - 1);
};

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << '\n'
#define checkpoint std::cout << "checkpoint" << _endl_

OTPCPrimaryGeneratorAction::OTPCPrimaryGeneratorAction(OTPCRunAction* RunAct)
	:runAction(RunAct)
{

	//choose the Random engine 
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
	//set random seed with system time (it must change from one run to another!!! otherwise we simulate all the time the same in the OTPC case!!)
	G4long seed = time(NULL);
	CLHEP::HepRandom::setTheSeed(seed);
	//////////////////////////////////////////////////////////////////////////////

	//////////Reading the input data for primary generator///////////

	ifstream evenInputInformation;
	evenInputInformation.open("../../../particles3.data");
	if (!evenInputInformation.is_open()) {
		std::cout << "\n\nNO EVENT INPUT INFORMATION FILE FOUND!!!" << _endl_;
		exit(1);
	}
	std::string header1, header2;

	evenInputInformation >> header1;
	evenInputInformation >> type[0] >> type[1] >> type[2];
	evenInputInformation >> header2;
	evenInputInformation >> E[0] >> E[1] >> E[2];
	evenInputInformation.close();
	G4cout << E[0] << " " << E[1] << " " << E[2] << endl;

	// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	//Geantino for geometry checking
	G4int n_particle = 1;
	particleGun = new G4ParticleGun(n_particle);

	// particle type
	G4ParticleTable particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

	// particle type
	particleGun->SetParticleDefinition(particleTable->FindParticle(particleName = "geantino"));
	particleGun->SetParticleEnergy(1.0  GeV);
	particleGun->SetParticleMomentumDirection(G4RandomDirection());
	*/

	/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Simulation of physical particles

	// particles per event
	G4int n_particle = 1;
	particleGun = std::make_unique<G4ParticleGun>(n_particle);

	// particle type
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

	/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	G4ParticleDefinition* proton
		= particleTable->FindParticle(particleName = "proton");
	G4ParticleDefinition* alpha
		= particleTable->FindParticle(particleName = "alpha");
	G4ParticleDefinition* gammaray
		= particleTable->FindParticle(particleName = "gamma");

	G4int Z, A;
	//example: 10Be
	Z = 4;
	A = 10;
	G4double excitEnergy = 0. * keV;
	G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z, A, excitEnergy);


	particleDefinitions = { proton,alpha,ion,gammaray };

}



void OTPCPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

	/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	//Geantino for geometry checking

	// particle definitions
	G4ParticleTable particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

	particleGun->SetParticleDefinition(particleTable->FindParticle(particleName = "geantino"));
	particleGun->SetParticleEnergy(1.0  GeV);
	particleGun->SetParticleMomentumDirection(G4RandomDirection());

	particleGun->GeneratePrimaryVertex(anEvent);
	*/
	/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Simulation of physical particles

	//Dimensions OTPC:
	G4double RangeX = 322.56; //mm 
	G4double RangeY = 322.56; //mm 
	G4double RangeZ = 142; //mm 

	//Random initial point inside OTPC (70% of volume, excluding sides)

	//x=-0.35*RangeX+G4UniformRand()*0.7*RangeX;
	//y=-0.35*RangeY+G4UniformRand()*0.7*RangeY;
	//z=-0.35*RangeZ+G4UniformRand()*0.7*RangeZ;

	x = 0.0 * mm;
	y = 0.0 * mm;
	z = 0.0 * mm;

	G4double aperture = 180. * degree;

	for (int i = 0; i < 3; i++) {
		theta[i] = acos(1. + (cos(aperture) - 1.) * G4UniformRand());
		phi[i] = CLHEP::twopi * G4UniformRand();
	}

	//metaFile << std::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n", E[0] / keV, E[1] / keV, E[2] / keV, x / mm, y / mm, z / mm, theta[0] / degree, theta[1] / degree, theta[3] / degree, phi[0] / degree, phi[1] / degree, phi[2] / degree);
	std::array<G4double,12> tpl = { E[0] / keV, E[1] / keV, E[2] / keV, x / mm, y / mm, z / mm, theta[0] / degree, theta[1] / degree, theta[3] / degree, phi[0] / degree, phi[1] / degree, phi[2] / degree };
	metaFile.write((char*)tpl.data(), sizeof(tpl));

	for (G4int i = 0; i < 3; i++) {
		if (type[i] > 0 & type[i] < 5) {
			//Momentum direction according to input angles
			G4ThreeVector momentumDirection;
			momentumDirection.setRThetaPhi(1., theta[i], phi[i]);
			particleGun->SetParticleDefinition(particleDefinitions[type[i] - 1]);

			particleGun->SetParticlePosition(G4ThreeVector(x, y, z));
			particleGun->SetParticleMomentumDirection(momentumDirection);
			particleGun->SetParticleEnergy(E[i]);
			particleGun->GeneratePrimaryVertex(anEvent);

			//G4cout<<theta[i]<<" "<<phi[i]<<" "<<E[i]<<" lauched"<<G4endl;
		}
	}
}

void OTPCPrimaryGeneratorAction::setRunPath(std::filesystem::path runPath) {
	metaFile.open(runPath / "metadata.bin", std::ios_base::binary | std::ios_base::out);
}

std::array<G4double, 3>& OTPCPrimaryGeneratorAction::getEnergy() {
	return E;
}


