#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"

RunAction::RunAction()
{
    auto analysis = G4AnalysisManager::Instance();
    analysis->SetDefaultFileType("csv");
    analysis->SetVerboseLevel(1);

    analysis->CreateNtuple("result", "first interaction of the primary gamma");
    analysis->CreateNtupleIColumn("interacted");
    analysis->CreateNtupleSColumn("process");
    analysis->CreateNtupleDColumn("z_mm");
    analysis->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run *)
{
    G4AnalysisManager::Instance()->OpenFile("water_box");
}

void RunAction::EndOfRunAction(const G4Run *)
{
    auto analysis = G4AnalysisManager::Instance();
    analysis->Write();
    analysis->CloseFile();
}
