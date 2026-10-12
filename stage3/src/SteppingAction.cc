#include "SteppingAction.hh"

#include "EventAction.hh"
#include "TrackInformation.hh"

#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Positron.hh"
#include "G4Step.hh"
#include "G4VProcess.hh"

void SteppingAction::UserSteppingAction(const G4Step *step)
{
    auto track = step->GetTrack();

    auto info = static_cast<TrackInformation *>(track->GetUserInformation());
    if (info == nullptr)
    {
        info = new TrackInformation(-1);
        track->SetUserInformation(info);
    }

    G4int originForSecondaries = info->GetOrigin();

    auto prePoint = step->GetPreStepPoint();
    auto postPoint = step->GetPostStepPoint();
    const G4VProcess *process = postPoint->GetProcessDefinedStep();
    G4bool isGammaInteractionInWater =
        track->GetDefinition() == G4Gamma::Definition() && prePoint->GetTouchableHandle()->GetVolume()->GetName() == "Water" && process != nullptr

        && process->GetProcessName() != "Transportation";
    const auto *secondaries = step->GetSecondaryInCurrentStep();

    if (isGammaInteractionInWater)
    {
        Interaction interaction;
        interaction.parent = info->GetOrigin();
        interaction.trackId = track->GetTrackID();
        interaction.process = process->GetProcessName();
        interaction.position = postPoint->GetPosition();
        interaction.time = postPoint->GetGlobalTime();
        interaction.energyIn = prePoint->GetKineticEnergy();
        interaction.directionIn = prePoint->GetMomentumDirection();
        interaction.energyOut = postPoint->GetKineticEnergy();
        if (interaction.energyOut > 0.)
            interaction.directionOut = postPoint->GetMomentumDirection();
        interaction.nSecondaries = secondaries->size();

        for (const auto *secondary : *secondaries)
        {
            G4double energy = secondary->GetKineticEnergy();
            if (secondary->GetDefinition() == G4Electron::Definition() && energy > interaction.electronEnergy)
            {
                interaction.electronEnergy = energy;
                interaction.electronDirection = secondary->GetMomentumDirection();
            }
            if (secondary->GetDefinition() == G4Positron::Definition())
            {
                interaction.positronEnergy = energy;
            }
        }

        G4int id = fEventAction->AddInteraction(interaction);
        info->SetOrigin(id);
        originForSecondaries = id;
    }
    for (const auto *secondary : *secondaries)
    {
        const_cast<G4Track *>(secondary)->SetUserInformation(new TrackInformation(originForSecondaries));
    }
}
