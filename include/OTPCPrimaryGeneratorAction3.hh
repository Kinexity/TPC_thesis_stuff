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


class G4Event;
class OTPCRunAction;

extern std::ifstream eventInputFile;

class OTPCPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    OTPCPrimaryGeneratorAction(OTPCRunAction*);
    ~OTPCPrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event* anEvent);
    G4ParticleGun* GetParticleGun() { return particleGun;} ;
          
  private:
    OTPCRunAction*            runAction;
    G4ParticleGun*  particleGun;
    
        
    G4double E[3], theta[3], phi[3];
    G4int type[3];
    G4double x,y,z;

};

#endif
