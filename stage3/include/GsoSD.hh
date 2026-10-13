#ifndef GsoSD_h
#define GsoSD_h 1

#include "GsoHit.hh"

#include "G4VSensitiveDetector.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

class GsoSD : public G4VSensitiveDetector
{
public:
    GsoSD(const G4String &name, const G4String &hitsCollectionName);
    ~GsoSD() override = default;

    void Initialize(G4HCofThisEvent *hce) override;
    G4bool ProcessHits(G4Step *step, G4TouchableHistory *history) override;

private:
    GsoHitsCollection *fHits = nullptr;
};

#endif
