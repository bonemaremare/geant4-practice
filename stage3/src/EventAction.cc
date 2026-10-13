#include "EventAction.hh"

#include "GsoHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

void EventAction::BeginOfEventAction(const G4Event *)
{
    fInteractions.clear();
    fEdepStepping = 0.;
}

G4int EventAction::AddInteraction(const Interaction &interaction)
{
    G4int id = fInteractions.size();
    fInteractions.push_back(interaction);
    fInteractions.back().id = id;
    return id;
}

void EventAction::EndOfEventAction(const G4Event *event)
{
    auto analysis = G4AnalysisManager::Instance();

    G4int eventId = event->GetEventID();

    if (fHitsCollectionId < 0)
    {
        fHitsCollectionId = G4SDManager::GetSDMpointer()->GetCollectionID("GSO/GsoHits");
    }
    auto hits = static_cast<GsoHitsCollection *>(event->GetHCofThisEvent()->GetHC(fHitsCollectionId));
    G4double edep = 0.;
    for (std::size_t k = 0; k < hits->entries(); ++k)
    {
        edep += (*hits)[k]->edep;
    }

    auto vertex = event->GetPrimaryVertex();
    auto primary = vertex->GetPrimary();
    G4ThreeVector start = vertex->GetPosition();
    G4ThreeVector direction = primary->GetMomentumDirection();
    G4int c = 0;
    analysis->FillNtupleIColumn(0, c++, eventId);
    analysis->FillNtupleDColumn(0, c++, primary->GetKineticEnergy() / MeV);
    analysis->FillNtupleDColumn(0, c++, start.x() / mm);
    analysis->FillNtupleDColumn(0, c++, start.y() / mm);
    analysis->FillNtupleDColumn(0, c++, start.z() / mm);
    analysis->FillNtupleDColumn(0, c++, direction.x());
    analysis->FillNtupleDColumn(0, c++, direction.y());
    analysis->FillNtupleDColumn(0, c++, direction.z());
    analysis->FillNtupleIColumn(0, c++, fInteractions.size());
    analysis->FillNtupleIColumn(0, c++, hits->entries());
    analysis->FillNtupleDColumn(0, c++, edep / MeV);
    analysis->FillNtupleDColumn(0, c++, fEdepStepping / MeV);
    analysis->AddNtupleRow(0);

    for (const auto &i : fInteractions)
    {
        G4int c = 0;
        analysis->FillNtupleIColumn(1, c++, eventId);
        analysis->FillNtupleIColumn(1, c++, i.id);
        analysis->FillNtupleIColumn(1, c++, i.parent);
        analysis->FillNtupleIColumn(1, c++, i.trackId);
        analysis->FillNtupleSColumn(1, c++, i.creator);
        analysis->FillNtupleSColumn(1, c++, i.process);
        analysis->FillNtupleDColumn(1, c++, i.position.x() / mm);
        analysis->FillNtupleDColumn(1, c++, i.position.y() / mm);
        analysis->FillNtupleDColumn(1, c++, i.position.z() / mm);
        analysis->FillNtupleDColumn(1, c++, i.time / ns);
        analysis->FillNtupleDColumn(1, c++, i.energyIn / MeV);
        analysis->FillNtupleDColumn(1, c++, i.directionIn.x());
        analysis->FillNtupleDColumn(1, c++, i.directionIn.y());
        analysis->FillNtupleDColumn(1, c++, i.directionIn.z());
        analysis->FillNtupleDColumn(1, c++, i.energyOut / MeV);
        analysis->FillNtupleDColumn(1, c++, i.directionOut.x());
        analysis->FillNtupleDColumn(1, c++, i.directionOut.y());
        analysis->FillNtupleDColumn(1, c++, i.directionOut.z());
        analysis->FillNtupleDColumn(1, c++, i.electronEnergy / MeV);
        analysis->FillNtupleDColumn(1, c++, i.electronDirection.x());
        analysis->FillNtupleDColumn(1, c++, i.electronDirection.y());
        analysis->FillNtupleDColumn(1, c++, i.electronDirection.z());
        analysis->FillNtupleDColumn(1, c++, i.positronEnergy / MeV);
        analysis->FillNtupleDColumn(1, c++, i.chargedEnergy / MeV);
        analysis->FillNtupleIColumn(1, c++, i.nSecondaries);
        analysis->AddNtupleRow(1);
    }
    for (std::size_t k = 0; k < hits->entries(); ++k)
    {
        const GsoHit *h = (*hits)[k];
        G4int c = 0;
        analysis->FillNtupleIColumn(2, c++, eventId);
        analysis->FillNtupleIColumn(2, c++, h->trackId);
        analysis->FillNtupleIColumn(2, c++, h->origin);
        analysis->FillNtupleSColumn(2, c++, h->particle);
        analysis->FillNtupleDColumn(2, c++, h->position.x() / mm);
        analysis->FillNtupleDColumn(2, c++, h->position.y() / mm);
        analysis->FillNtupleDColumn(2, c++, h->position.z() / mm);
        analysis->FillNtupleDColumn(2, c++, h->time / ns);
        analysis->FillNtupleDColumn(2, c++, h->edep / MeV);
        analysis->AddNtupleRow(2);
    }
}
