#ifndef EventAction_h
#define EventAction_h 1

#include "G4String.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;

class EventAction : public G4UserEventAction
{
public:
    EventAction() = default;
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event *event) override;
    void EndOfEventAction(const G4Event *event) override;

    G4bool HasInteracted() const { return fInteracted; }
    void RecordFirstInteraction(const G4String &process, G4double z);

private:
    G4bool fInteracted = false;
    G4String fProcess = "none";
    G4double fZ = 0.;
};

#endif
