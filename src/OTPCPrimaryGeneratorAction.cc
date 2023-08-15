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
#include <random>


#include "OTPCRunAction.hh"

inline std::string filename_string(std::string path_str) {
	return path_str.substr(path_str.rfind("\\") + 1, path_str.size() - path_str.rfind("\\") - 1);
};

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << '\n'
#define checkpoint std::cout << "checkpoint" << _endl_

G4double generateRandomEnergy() {
	// Define the energies and probabilities
	static const std::array<G4double, 5> energies = { 204.0 * keV, 275.0 * keV, 583.0 * keV, 595.0 * keV, 866.0 * keV };
	static const std::array<G4double, 5> probabilities = { 0.030943, 0.141454, 0.168222, 0.168222, 0.491159 };

	// Set up the discrete distribution
	static const std::discrete_distribution<int> distribution(probabilities.begin(), probabilities.end());

	// Generate a random index based on the probabilities
	static std::random_device rd;
	static std::mt19937 generator(rd());
	int index = distribution(generator);

	// Return the energy corresponding to the generated index
	return energies[index];
}

G4ThreeVector generateRandomPosition() {
	//Dimensions OTPC:
	G4double RangeX = 20 * cm;
	G4double RangeY = 33 * cm;
	G4double RangeZ = 21 * cm;

	//Random initial point inside OTPC (70% of volume, excluding sides)

	auto volumePercentage = 70 * perCent;

	auto position = volumePercentage / 2 * G4ThreeVector(
		(2 * G4UniformRand() - 1) * RangeX,
		(2 * G4UniformRand() - 1) * RangeY,
		(2 * G4UniformRand() - 1) * RangeZ
	);
	// Return the energy corresponding to the generated index
	return position;
}


OTPCPrimaryGeneratorAction::OTPCPrimaryGeneratorAction(OTPCRunAction* RunAct, bool loadDataFromFileArg)
	:runAction(RunAct), loadDataFromFile(loadDataFromFileArg)
{

	//choose the Random engine 
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
	//set random seed with system time (it must change from one run to another!!! otherwise we simulate all the time the same in the OTPC case!!)
	G4long seed = time(NULL);
	CLHEP::HepRandom::setTheSeed(seed);

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
	G4ParticleDefinition* geantino
		= particleTable->FindParticle(particleName = "geantino");

	G4int Z, A;
	//example: 10Be
	Z = 4;
	A = 10;
	G4double excitEnergy = 0. * keV;
	G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z, A, excitEnergy);


	particleDefinitions = { proton,alpha,ion,gammaray, geantino };

	if (loadDataFromFile) {
		loadData();
	}

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

	// initial position is randomized if data is loaded from file
	if (loadDataFromFile) {
		position = generateRandomPosition();
	}

	G4double aperture = 180. * degree;

	for (int i = 0; i < 3; i++) {
		theta[i] = std::acos(1. + (std::cos(aperture) - 1.) * G4UniformRand());
		phi[i] = CLHEP::twopi * G4UniformRand();
	}

	//if (loadDataFromFile) {
	//	E[1] = generateRandomEnergy();
	//}

	std::array tpl = { E[0] / keV, E[1] / keV, E[2] / keV, position.x() / mm, position.y() / mm, position.z() / mm, theta[0] / degree, theta[1] / degree, theta[3] / degree, phi[0] / degree, phi[1] / degree, phi[2] / degree };
	metaFile.write((char*)tpl.data(), sizeof(tpl));

	for (G4int i = 0; i < 3; i++) {
		if (loadDataFromFile && type[i] == 4 && E[1] != 595.0 * keV) {
			continue;
		}
		if (type[i] > 0 && type[i] < 6) {
			//Momentum direction according to input angles
			G4ThreeVector momentumDirection;
			momentumDirection.setRThetaPhi(1., theta[i], phi[i]);
			particleGun->SetParticleDefinition(particleDefinitions[type[i] - 1]);

			particleGun->SetParticlePosition(position);
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

G4double OTPCPrimaryGeneratorAction::getEnergy(int index) const {
	return E[index];
}

void OTPCPrimaryGeneratorAction::setEnergy(G4double energy, int index) {
	E[index] = energy;
}

void OTPCPrimaryGeneratorAction::setPosition(G4ThreeVector pos) {
	position = pos;
}

void OTPCPrimaryGeneratorAction::loadData() {
	//////////////////////////////////////////////////////////////////////////////

	//////////Reading the input data for primary generator///////////

	std::ifstream evenInputInformation;
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
	for (auto& energy : E) {
		energy *= keV;
	}
	G4cout << E[0] / keV << " " << E[1] / keV << " " << E[2] / keV << '\n';
}


