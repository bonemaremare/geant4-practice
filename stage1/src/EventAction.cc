#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

void EventAction::BeginOfEventAction(const G4Event *)
{
    fInteracted = false;
    fProcess = "none";
    fZ = 0.;
}

void EventAction::RecordFirstInteraction(const G4String &process, G4double z)
{
    fInteracted = true;
    fProcess = process;
    fZ = z;
}

void EventAction::EndOfEventAction(const G4Event *)
{
    auto analysis = G4AnalysisManager::Instance();
    analysis->FillNtupleIColumn(0, fInteracted ? 1 : 0);
    analysis->FillNtupleSColumn(1, fProcess);
    analysis->FillNtupleDColumn(2, fZ / mm);
    analysis->AddNtupleRow();
}
