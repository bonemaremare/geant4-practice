#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4GenericMessenger.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "GsoSD.hh"

DetectorConstruction::DetectorConstruction()
{
    fMessenger = new G4GenericMessenger(this, "/gso/", "GSO block");
    fMessenger->DeclarePropertyWithUnit("width", "cm", fWidth, "x,y size of GSO block");
    fMessenger->DeclarePropertyWithUnit("thickness", "cm", fThickness, "z size of the GSO block");
}

DetectorConstruction::~DetectorConstruction()
{
    delete fMessenger;
}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    auto nist = G4NistManager::Instance();

    G4Material *vacuum = nist->FindOrBuildMaterial("G4_Galactic");

    auto gso = new G4Material("GSO", 6.71 * g / cm3, 3);
    gso->AddElement(nist->FindOrBuildElement("Gd"), 2);
    gso->AddElement(nist->FindOrBuildElement("Si"), 1);
    gso->AddElement(nist->FindOrBuildElement("O"), 5);

    G4double worldHalf = 0.5 * m;
    auto worldSolid = new G4Box("World", worldHalf, worldHalf, worldHalf);
    auto worldLogical = new G4LogicalVolume(worldSolid, vacuum, "World");
    auto worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), worldLogical,
                                           "World", nullptr, false, 0, true);

    auto gsoSolid = new G4Box("GSO", 0.5 * fWidth, 0.5 * fWidth, 0.5 * fThickness);
    auto gsoLogical = new G4LogicalVolume(gsoSolid, gso, "GSO");
    new G4PVPlacement(nullptr, G4ThreeVector(), gsoLogical, "GSO", worldLogical, false, 0, true);

    G4cout << "GSO block:" << fWidth / cm << "x" << fWidth / cm << "x" << fThickness / cm << "cm,density" << gso->GetDensity() / (g / cm3)
           << "g/cm3" << G4endl;

    return worldPhysical;
}
void DetectorConstruction::ConstructSDandField()
{
    auto sd = new GsoSD("GSO", "GsoHits");
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    SetSensitiveDetector("GSO", sd);
}
