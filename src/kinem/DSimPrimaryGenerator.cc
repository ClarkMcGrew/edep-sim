#include <G4UnitsTable.hh>
#include <G4Event.hh>
#include <G4TransportationManager.hh>
#include <G4Navigator.hh>
#include <G4ThreeVector.hh>
#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VisExtent.hh>

#include <Randomize.hh>

#include <TCaptLog.hxx>

#include "DSimVertexInfo.hh"
#include "kinem/DSimPrimaryGenerator.hh"
#include "kinem/DSimVKinematicsGenerator.hh"
#include "kinem/DSimVCountGenerator.hh"
#include "kinem/DSimVPositionGenerator.hh"
#include "kinem/DSimVTimeGenerator.hh"

DSimPrimaryGenerator::DSimPrimaryGenerator(DSimVKinematicsGenerator* kine,
                                             DSimVCountGenerator* count,
                                             DSimVPositionGenerator* position,
                                             DSimVTimeGenerator* time)
    : fKinematics(kine), fCount(count), fPosition(position), fTime(time) {
}

DSimPrimaryGenerator::~DSimPrimaryGenerator(void) {}

void DSimPrimaryGenerator::GeneratePrimaryVertex(G4Event* evt) {

    int count = fCount->GetCount();
    int brake = 0;
    CaptVerbose("# Generate " << count << " vertices w/ " << GetName());
    while (count > 0 && brake<1000) {
        // Stash the most recent primary vertex.  This is saving the last
        // existing primary vertex before the call to the
        // DSimVKinematicsGenerator::GeneratePrimaryVertex().
        G4PrimaryVertex* vtx = evt->GetPrimaryVertex();
        for (;vtx && vtx->GetNext(); vtx = vtx->GetNext());
        // Generate a candidate position and time for the event vertex.
        G4LorentzVector vertex = fPosition->GetPosition();
        vertex.setT(fTime->GetTime(vertex));
        // Generate the new kinematics.  This will add new primary vertices to
        // the G4Event.
        if (!fKinematics->GeneratePrimaryVertex(evt,vertex)) {
            ++brake; // Apply the brakes
            continue;
        }
        // If the vertex should be forced, then make sure that it is.  This
        // updates the position and time for all of the vertices that were
        // added to the event by the previous DSimVKinematicsGenerator call.
        // Get the first new vertex.
        if (!vtx) vtx = evt->GetPrimaryVertex();
        else vtx = vtx->GetNext();
        // Update the vertex for the new vertices.
        for (;vtx; vtx = vtx->GetNext()) {
            // Update the vertex info (and make sure it exists).  This sets
            // the generator name.
            DSimVertexInfo* vInfo 
                = dynamic_cast<DSimVertexInfo*>(vtx->GetUserInformation());
            if (!vInfo) {
                vInfo = new DSimVertexInfo();
                vtx->SetUserInformation(vInfo);
            }
            vInfo->SetName(GetName());
            if (fPosition->ForcePosition()) {
                CaptVerbose("#    Force position to" 
                         << " x: " << vertex.x()/cm << " cm"
                         << " y: " << vertex.y()/cm << " cm"
                         << " z: " << vertex.z()/cm << " cm");
                vtx->SetPosition(vertex.x(), vertex.y(), vertex.z());
            }
            if (fTime->ForceTime()) {
                CaptVerbose("#    Force time to " 
                             << vertex.t()/ns << " ns");
                vtx->SetT0(vertex.t());
            }
        }
        brake = 0;  // Remove the brakes.
        --count;
    }
}

G4String DSimPrimaryGenerator::GetName() {
    G4String name = fKinematics->GetName();
    name += ":" + fCount->GetName();
    name += "@" + fPosition->GetName();
    name += "-" + fTime->GetName();
    return name;
}
