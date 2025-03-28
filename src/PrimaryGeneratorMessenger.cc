//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file optical/OpNovice4/src/PrimaryGeneratorMessenger.cc
/// \brief Implementation of the PrimaryGeneratorMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIdirectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* Gun)
  : G4UImessenger(), fPrimaryAction(Gun)
{
  fGunDir = new G4UIdirectory("/OpNovice4/gun/");
  fGunDir->SetGuidance("PrimaryGenerator control");

  fPolarCmd = new G4UIcmdWithADoubleAndUnit("/OpNovice4/gun/optPhotonPolar", this);
  fPolarCmd->SetGuidance("Set linear polarization angle w.r.t. (k,n) plane");
  fPolarCmd->SetParameterName("angle", true);
  fPolarCmd->SetUnitCategory("Angle");
  fPolarCmd->SetDefaultValue(-360.0);
  fPolarCmd->SetDefaultUnit("deg");
  fPolarCmd->AvailableForStates(G4State_Idle);

  fRandomDirectionCmd = new G4UIcmdWithABool("/OpNovice4/gun/randomDirection", this);
  fRandomDirectionCmd->SetGuidance("Set direction of each primary particle randomly.");
  fRandomDirectionCmd->SetDefaultValue(true);
  fRandomDirectionCmd->AvailableForStates(G4State_Idle, G4State_PreInit);

  fLidarDirectionCmd = new G4UIcmdWithABool("/OpNovice4/gun/lidarDirection", this);
  fLidarDirectionCmd->SetGuidance("swiping like lidar");
  fLidarDirectionCmd->SetDefaultValue(true);
  fLidarDirectionCmd->AvailableForStates(G4State_Idle, G4State_PreInit);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fPolarCmd;
  delete fGunDir;
  delete fRandomDirectionCmd;
  delete fLidarDirectionCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fPolarCmd) {
    G4double angle = fPolarCmd->GetNewDoubleValue(newValue);
    if (angle == -360.0 * deg) {
      fPrimaryAction->SetOptPhotonPolar();
    }
    else {
      fPrimaryAction->SetOptPhotonPolar(angle);
    }
  }
  else if (command == fRandomDirectionCmd) {
    //GetNewBoolValue
    G4bool s_randomDir = fRandomDirectionCmd->GetNewBoolValue(newValue);
    fPrimaryAction->SetRandomDirection(s_randomDir);
  }
  else if (command == fLidarDirectionCmd) {
    //LidarDirection
    G4bool s_lidarDir = fLidarDirectionCmd->GetNewBoolValue(newValue);
    fPrimaryAction->SetLidarDirection(s_lidarDir);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
