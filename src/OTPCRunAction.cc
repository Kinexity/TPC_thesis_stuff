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
#include "OTPCPrimaryGeneratorAction.hh"

#include "G4Run.hh"

#include "G4ios.hh"
#include "fstream"
#include "iomanip"
#include "Randomize.hh"
#include "time.h"
#include <numeric>
#include <format>
#include <execution>


#include "G4SystemOfUnits.hh"
// #include "g4std/fstream"
// #include "g4std/iomanip"

using namespace std;


OTPCRunAction::OTPCRunAction()
{
	timer = std::make_unique<G4Timer>();

	runManager = G4RunManager::GetRunManager();

	///////////////////////////////////////////////////////////////////////////////////	

}

void OTPCRunAction::BeginOfRunAction(const G4Run*)
{
	//eventTotalDepositFile.open(eventTotalDepositFilePath.string() + ".csv", std::ios_base::out | std::ios_base::trunc);
	//eventStepsDepositFile.open(eventStepsDepositFilePath.string() + ".csv", std::ios_base::out | std::ios_base::trunc);
	eventTotalDepositFileBinary.open(eventTotalDepositFilePath.string() + ".bin", std::ios_base::out | std::ios_base::binary | std::ios_base::app);
	eventTotalGasDepositFileBinary.open(eventTotalDepositFilePath.string() + "_gas.bin", std::ios_base::out | std::ios_base::binary | std::ios_base::app);
	//eventStepsDepositFileBinary.open(eventStepsDepositFilePath.string() + ".bin", std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	currentEnergy = reinterpret_cast<const OTPCPrimaryGeneratorAction*>(runManager->GetUserPrimaryGeneratorAction())->getEnergy();
	//Start CPU timer
	timer->Start();
	eventIndex = 0;
}

void OTPCRunAction::EndOfRunAction(const G4Run*)
{

	//eventTotalDepositFile.close();
	//eventStepsDepositFile.close();
	eventTotalDepositFileBinary.close();
	eventTotalGasDepositFileBinary.close();
	//eventStepsDepositFileBinary.close();
	//Stop timer and get CPU time
	timer->Stop();
	G4double cputime = timer->GetRealElapsed();
	std::cout << std::format("Flags set = {} \n CPU time = {} s\n", eventFlagCounter, cputime);

}

void OTPCRunAction::fillOut(std::vector<std::array<G4double, 4>>& EnergyDeposit) {
	if (EnergyDeposit.size() > 0) {
		eventStepsDepositFile << std::format("{}\t{}\n", eventIndex, EnergyDeposit.size());
		for (auto& EnergyDeposit_i : EnergyDeposit) {
			eventStepsDepositFile << std::format("{},{},{},{}\n", EnergyDeposit_i[0], EnergyDeposit_i[1], EnergyDeposit_i[2], EnergyDeposit_i[3]);
		}
	}
}

void OTPCRunAction::fillOutScintillation(std::array<G4double, 20>& EnergyGammaCrystals) {
	eventTotalDepositFileBinary.write((char*)EnergyGammaCrystals.data(), EnergyGammaCrystals.size() * sizeof(G4double));

	//for (auto& EnergyDepositOneCrystal : EnergyGammaCrystals) {
	//	eventTotalDepositFile << EnergyDepositOneCrystal << '\t';
	//}
	//eventTotalDepositFile << "\n";

}

void OTPCRunAction::fillOutGasIonization(G4double EnergyGas) {
	eventTotalGasDepositFileBinary.write((char*)&EnergyGas, sizeof(EnergyGas));
}

void OTPCRunAction::fillOutSteps(std::vector<std::tuple<G4double, G4double, G4double, G4String>>& ProcessSteps, G4double totalEnergy) {
	decayCounter += std::any_of(std::execution::par, ProcessSteps.begin(), ProcessSteps.end(),
		[](const std::tuple<G4double, G4double, G4double, G4String>& tuple) {
			return std::get<3>(tuple).find("RadioactiveDecay") != std::string::npos;
		});
	if (ProcessSteps.size() > 0 && totalEnergy > currentEnergy * 1.001) {
		eventStepsDepositFile.open(eventStepsDepositFilePath.string() + std::format("_{}.txt", eventIndex), std::ios_base::out | std::ios_base::trunc);
		for (auto [x, y, z, step] : ProcessSteps) {
			eventStepsDepositFile << std::format("{}\t{}\t{}\t{}\n", x, y, z, step);
		}
		eventStepsDepositFile.close();
	}
}

void OTPCRunAction::updateEventCounter(bool flag) {
	eventFlagCounter += flag;
	eventIndex++;
	if (eventIndex % 10000 == 0) {
		std::cout << eventIndex << '\n';
	}
}

void OTPCRunAction::setEventFilePath(std::filesystem::path totalP, std::filesystem::path stepsP) {
	eventTotalDepositFilePath = totalP;
	eventStepsDepositFilePath = stepsP;
	std::cout << eventTotalDepositFilePath << '\n' << eventStepsDepositFilePath << '\n';
}
