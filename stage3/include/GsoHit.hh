#ifndef GsoHit_h
#define GsoHit_h 1

#include "G4String.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4VHit.hh"
#include "globals.hh"

class GsoHit : public G4VHit
{
public:
    G4int trackId = 0;
    G4int origin = -1;
    G4String particle;
    G4ThreeVector position;
    G4double time = 0.;
    G4double edep = 0.;
};

using GsoHitsCollection = G4THitsCollection<GsoHit>;

#endif
