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
/// \file optical/OpNovice4/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"

#include "PrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr)
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // create a messenger for this class
  fGunMessenger = new PrimaryGeneratorMessenger(this);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("e+");

  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleTime(0.0 * ns);
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0 * cm, 0.0 * cm, 0.0 * cm));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));
  fParticleGun->SetParticleEnergy(500.0 * keV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if (fRandomDirection) {
    G4double theta = CLHEP::halfpi * G4UniformRand();
    G4double phi = CLHEP::twopi * G4UniformRand();
    G4double x = std::cos(theta);
    G4double y = std::sin(theta) * std::sin(phi);
    G4double z = std::sin(theta) * std::cos(phi);
    G4ThreeVector dir(x, y, z);
    fParticleGun->SetParticleMomentumDirection(dir);
  }
  else if (fLidarDirection)
  {
    auto eventID = anEvent->GetEventID();
    G4double x = ((eventID-(eventID % 256))/256.0)/100.0;
    G4double y = ((eventID % 256)-127.5)/100.0;
    G4double z = 1;
    G4ThreeVector dir(x, y, z);
    fParticleGun->SetParticleMomentumDirection(dir);
  }

  if (fParticleGun->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
    if (fPolarized)
      SetOptPhotonPolar(fPolarization);
    else
      SetOptPhotonPolar();
  }
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::SetOptPhotonPolar()
{
  G4double angle = G4UniformRand() * 360.0 * deg;
  SetOptPhotonPolar(angle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
  if (fParticleGun->GetParticleDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
    G4ExceptionDescription ed;
    ed << "The particleGun is not an opticalphoton.";
    G4Exception("PrimaryGeneratorAction::SetOptPhotonPolar", "OpNovice4_004", JustWarning, ed);
    return;
  }

  fPolarized = true;
  fPolarization = angle;

  G4ThreeVector normal(1., 0., 0.);
  G4ThreeVector kphoton = fParticleGun->GetParticleMomentumDirection();
  G4ThreeVector product = normal.cross(kphoton);
  G4double modul2 = product * product;

  G4ThreeVector e_perpend(0., 0., 1.);
  if (modul2 > 0.) e_perpend = (1. / std::sqrt(modul2)) * product;
  G4ThreeVector e_paralle = e_perpend.cross(kphoton);

  G4ThreeVector polar = std::cos(angle) * e_paralle + std::sin(angle) * e_perpend;
  fParticleGun->SetParticlePolarization(polar);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrimaryGeneratorAction::SetRandomDirection(G4bool val)
{
  fRandomDirection = val;
}
void PrimaryGeneratorAction::SetLidarDirection(G4bool val)
{
  fLidarDirection = val;
}
