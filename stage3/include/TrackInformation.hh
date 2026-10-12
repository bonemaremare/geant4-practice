#ifndef TrackInformation_h
#define TrackInformation_h 1

#include "G4VUserTrackInformation.hh"
#include "globals.hh"

class TrackInformation : public G4VUserTrackInformation
{
public:
    explicit TrackInformation(G4int origin) : fOrigin(origin) {}
    ~TrackInformation() override = default;

    G4int GetOrigin() const { return fOrigin; }
    void SetOrigin(G4int origin) { fOrigin = origin; }

private:
    G4int fOrigin = -1;
};

#endif
