/////////////////////////////////////////////////////////////////////////
//
// V. Guadilla 2021
/////////////////////////////////////////////////////////////////////////

#include "OTPCPrimaryGeneratorAction3.hh"

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
#include <utility>

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

	ifstream evenInputInformation("../../../particles3.data");
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

	x = 0.0;
	y = 0.0;
	z = 0.0;

	static const double     pi = 3.14159265358979323846;

	G4double aperture = 180. * degree;

	static std::once_flag flag;

	std::call_once(flag, [&]() {
		std::vector<G4double> vec;
		for (int i = 0; i < 100000; i++) {
			vec.push_back(acos(1. + (cos(aperture) - 1.) * G4UniformRand()) * 180 / pi);
		}
		std::sort(vec.begin(), vec.end());
		std::fstream fi("gen.txt", std::ios_base::trunc | std::ios_base::out);
		for (auto elem : vec) {
			fi << elem << '\n';
		}
		fi.close();
		});

	for (G4int i = 0; i < 3; i++) {
		theta[i] = acos(1. + (cos(aperture) - 1.) * G4UniformRand()) * 180 / pi;
		phi[i] = 360 * G4UniformRand();
	}


	FILE* metaFile = fopen("metadata.txt", "a");
	fprintf(metaFile, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", E[0], E[1], E[2], x, y, z, theta[0], theta[1], theta[3], phi[0], phi[1], phi[2]);
	fclose(metaFile);

	// particle types
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

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

	std::array<G4ParticleDefinition*, 4> particleDefinitions = { proton,alpha,ion,gammaray };

	for (G4int i = 0; i < 3; i++) {
		if (type[i] > 0 & type[i] < 5) {

			//Momentum direction according to input angles
			G4ThreeVector momentumDirection;
			momentumDirection.setRThetaPhi(1., theta[i] * degree, phi[i] * degree);


			particleGun->SetParticleDefinition(particleDefinitions[type[i] - 1]);

			particleGun->SetParticlePosition(G4ThreeVector(x * mm, y * mm, z * mm));
			particleGun->SetParticleMomentumDirection(momentumDirection);
			particleGun->SetParticleEnergy(E[i] * keV);
			particleGun->GeneratePrimaryVertex(anEvent);

			//G4cout<<theta[i]<<" "<<phi[i]<<" "<<E[i]<<" lauched"<<G4endl;
		}
	}







}


