#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"

RunAction::RunAction()
{
    auto analysis = G4AnalysisManager::Instance();
    analysis->SetDefaultFileType("csv");
    analysis->SetVerboseLevel(1);

    analysis->CreateNtuple("event", "one row per event");
    analysis->CreateNtupleIColumn("event");
    analysis->CreateNtupleDColumn("e0_MeV");
    analysis->CreateNtupleDColumn("x0_mm");
    analysis->CreateNtupleDColumn("y0_mm");
    analysis->CreateNtupleDColumn("z0_mm");
    analysis->CreateNtupleDColumn("dx0");
    analysis->CreateNtupleDColumn("dy0");
    analysis->CreateNtupleDColumn("dz0");
    analysis->CreateNtupleIColumn("n_interactions");
    analysis->FinishNtuple();

    analysis->CreateNtuple("interaction", "gamma interaction in water");
    analysis->CreateNtupleIColumn("event");
    analysis->CreateNtupleIColumn("id");
    analysis->CreateNtupleIColumn("parent");
    analysis->CreateNtupleIColumn("track");
    analysis->CreateNtupleSColumn("process");
    analysis->CreateNtupleDColumn("x_mm");
    analysis->CreateNtupleDColumn("y_mm");
    analysis->CreateNtupleDColumn("z_mm");
    analysis->CreateNtupleDColumn("t_ns");
    analysis->CreateNtupleDColumn("e_in_MeV");
    analysis->CreateNtupleDColumn("dx_in");
    analysis->CreateNtupleDColumn("dy_in");
    analysis->CreateNtupleDColumn("dz_in");
    analysis->CreateNtupleDColumn("e_out_MeV");
    analysis->CreateNtupleDColumn("dx_out");
    analysis->CreateNtupleDColumn("dy_out");
    analysis->CreateNtupleDColumn("dz_out");
    analysis->CreateNtupleDColumn("e_e_MeV");
    analysis->CreateNtupleDColumn("dx_e");
    analysis->CreateNtupleDColumn("dy_e");
    analysis->CreateNtupleDColumn("dz_e");
    analysis->CreateNtupleDColumn("e_pos_MeV");
    analysis->CreateNtupleIColumn("n_secondaries");
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
