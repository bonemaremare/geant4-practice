#ifndef EventAction_h
#define EventAction_h 1

#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>

class G4Event;

struct Interaction
{
    G4int id = 0;
    G4int parent = -1;
    G4int trackId = 0;
    G4String process;
    G4ThreeVector position;
    G4double time = 0.;
    G4double energyIn = 0.;
    G4ThreeVector directionIn;
    G4double energyOut = 0.;
    G4ThreeVector directionOut;
    G4double electronEnergy = 0.;
    G4ThreeVector electronDirection;
    G4double positronEnergy = 0.;
    G4int nSecondaries = 0;
};

class EventAction : public G4UserEventAction
{
public:
    EventAction() = default;
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event *event) override;
    void EndOfEventAction(const G4Event *event) override;

    G4int AddInteraction(const Interaction &interaction);

private:
    std::vector<Interaction> fInteractions;
};

#endif
