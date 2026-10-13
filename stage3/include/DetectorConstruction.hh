#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"

#include "globals.hh"

class G4GenericMessenger;
class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    ~DetectorConstruction() override;

    G4VPhysicalVolume *Construct() override;

    void ConstructSDandField() override;

private:
    G4double fWidth = 5. * cm;
    G4double fThickness = 2. * cm;
    G4GenericMessenger *fMessenger = nullptr;
};

#endif
