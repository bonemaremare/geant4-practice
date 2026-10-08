#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    auto nist = G4NistManager::Instance();

    G4Material *vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    G4Material *water = nist->FindOrBuildMaterial("G4_WATER");

    G4double worldHalf = 0.5 * m;
    auto worldSolid = new G4Box("World", worldHalf, worldHalf, worldHalf);
    auto worldLogical = new G4LogicalVolume(worldSolid, vacuum, "World");
    auto worldPhysical = new G4PVPlacement(nullptr,
                                           G4ThreeVector(),
                                           worldLogical, "World",
                                           nullptr,
                                           false, 0,
                                           true);

    G4double waterThickness = 10. * cm;
    auto waterSolid = new G4Box("Water", 25. * cm, 25. * cm, 0.5 * waterThickness);
    auto waterLogical = new G4LogicalVolume(waterSolid, water, "Water");
    new G4PVPlacement(nullptr, G4ThreeVector(), waterLogical, "Water", worldLogical, false, 0, true);

    return worldPhysical;
}
