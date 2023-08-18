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
#include "G4SystemOfUnits.hh"
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
	OTPCPrimaryGeneratorAction(OTPCRunAction*, bool loadDataFromFileArg);
	~OTPCPrimaryGeneratorAction() = default;

public:
	void GeneratePrimaries(G4Event* anEvent);
	void setRunPath(std::filesystem::path runPath);
	G4ParticleGun* GetParticleGun() { return particleGun.get(); };
	G4double getEnergy(int index = 0) const;
	void setEnergy(G4double energy, int index = 0);
	void setPosition(G4ThreeVector pos);
private:
	OTPCRunAction* runAction;
	std::unique_ptr<G4ParticleGun> particleGun;

	std::fstream metaFile;
	std::array<G4double, 3> E, theta, phi;
	std::array<G4int, 3> type = { 4, 0, 0 };
	G4ThreeVector position = { 0 * mm, 0 * mm, 0 * mm };
	std::array<G4ParticleDefinition*, 5> particleDefinitions;
	const bool loadDataFromFile;

	void loadData();
};

#endif
