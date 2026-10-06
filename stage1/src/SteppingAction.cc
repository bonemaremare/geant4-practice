#include "SteppingAction.hh"

#include "EventAction.hh"

#include "G4Step.hh"
#include "G4VProcess.hh"

void SteppingAction::UserSteppingAction(const G4Step *step)
{
    if (fEventAction->HasInteracted())
        return;

    auto track = step->GetTrack();
    if (track->GetTrackID() != 1)
        return;

    auto prePoint = step->GetPreStepPoint();
    if (prePoint->GetTouchableHandle()->GetVolume()->GetName() != "Water")
        return;

    auto postPoint = step->GetPostStepPoint();
    const G4VProcess *process = postPoint->GetProcessDefinedStep();
    if (process == nullptr)
        return;
    if (process->GetProcessName() == "Transportation")
        return;

    fEventAction->RecordFirstInteraction(process->GetProcessName(), postPoint->GetPosition().z());
}
