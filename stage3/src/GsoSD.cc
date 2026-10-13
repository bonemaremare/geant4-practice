#include "GsoSD.hh"

#include "TrackInformation.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"

GsoSD::GsoSD(const G4String &name, const G4String &hitsCollectionName)
    : G4VSensitiveDetector(name)
{
    collectionName.insert(hitsCollectionName);
}

void GsoSD::Initialize(G4HCofThisEvent *hce)
{
    fHits = new GsoHitsCollection(SensitiveDetectorName, collectionName[0]);
    G4int id = G4SDManager::GetSDMpointer()->GetCollectionID(fHits);
    hce->AddHitsCollection(id, fHits);
}

G4bool GsoSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.)
        return false;

    auto track = step->GetTrack();
    auto prePoint = step->GetPreStepPoint();
    auto postPoint = step->GetPostStepPoint();

    auto hit = new GsoHit();
    hit->trackId = track->GetTrackID();
    auto info = static_cast<TrackInformation *>(track->GetUserInformation());
    if (info != nullptr)
        hit->origin = info->GetOrigin();
    hit->particle = track->GetDefinition()->GetParticleName();
    hit->position = 0.5 * (prePoint->GetPosition() + postPoint->GetPosition());
    hit->time = prePoint->GetGlobalTime();
    hit->edep = edep;

    fHits->insert(hit);
    return true;
}
