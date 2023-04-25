/////////////////////////////////////////////////////////////////////////
//
//     04-2002  A. Algora--> OTPCDetectorConstruction.cc
//
//     Test of Geometries                                              //
/////////////////////////////////////////////////////////////////////////

#ifndef OTPCPrimaryGeneratorAction_h
#define OTPCPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include <array>
#include <filesystem>
#include <fstream>

class G4Event;
class OTPCRunAction;

extern std::ifstream eventInputFile;

class OTPCPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
	OTPCPrimaryGeneratorAction(OTPCRunAction*);
	~OTPCPrimaryGeneratorAction() = default;

public:
	void GeneratePrimaries(G4Event* anEvent);
	void setRunPath(std::filesystem::path runPath);
	G4ParticleGun* GetParticleGun() { return particleGun.get(); };
	std::array<G4double, 3>& getEnergy();
private:
	OTPCRunAction* runAction;
	std::unique_ptr<G4ParticleGun> particleGun;

	std::fstream metaFile;
	std::array<G4double, 3> E, theta, phi;
	G4int type[3];
	G4double x, y, z;

	std::array<G4ParticleDefinition*, 4> particleDefinitions;
};

#endif
