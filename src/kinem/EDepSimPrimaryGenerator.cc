#include <G4UnitsTable.hh>
#include <G4Event.hh>
#include <G4TransportationManager.hh>
#include <G4Navigator.hh>
#include <G4ThreeVector.hh>
#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VisExtent.hh>
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include <Randomize.hh>

#include <EDepSimLog.hh>

#include "EDepSimVertexInfo.hh"
#include "kinem/EDepSimPrimaryGenerator.hh"
#include "kinem/EDepSimVKinematicsGenerator.hh"
#include "kinem/EDepSimVCountGenerator.hh"
#include "kinem/EDepSimVPositionGenerator.hh"
#include "kinem/EDepSimVTimeGenerator.hh"

EDepSim::PrimaryGenerator::PrimaryGenerator(EDepSim::VKinematicsGenerator* kine,
                                             EDepSim::VCountGenerator* count,
                                             EDepSim::VPositionGenerator* position,
                                             EDepSim::VTimeGenerator* time)
    : fKinematics(kine), fCount(count), fPosition(position), fTime(time) {
}

EDepSim::PrimaryGenerator::~PrimaryGenerator(void) {}

void EDepSim::PrimaryGenerator::GeneratePrimaryVertex(G4Event* evt) {

    int count = fCount->GetCount();
    int brake = 0;
    EDepSimVerbose("# Generate " << count << " vertices w/ " << GetName());
    while (count > 0 && brake<1000) {
        // Stash the most recent primary vertex.  This is saving the last
        // existing primary vertex before the call to the
        // EDepSim::VKinematicsGenerator::GeneratePrimaryVertex().
        G4PrimaryVertex* vtx = evt->GetPrimaryVertex();
        for (;vtx && vtx->GetNext(); vtx = vtx->GetNext());
        // Generate a candidate position and time for the event vertex.
        G4LorentzVector vertex = fPosition->GetPosition();
        vertex.setT(fTime->GetTime(vertex));
        // Generate the new kinematics.  This will add new primary vertices to
        // the G4Event.
        EDepSim::VKinematicsGenerator::GeneratorStatus generatorStatus
            = fKinematics->GeneratePrimaryVertex(evt,vertex);
        if (generatorStatus == EDepSim::VKinematicsGenerator::kFail) {
            ++brake; // Apply the brakes
            continue;
        }
        if (generatorStatus == EDepSim::VKinematicsGenerator::kEndEvent) {
            break;
        }
        // If the vertex should be forced, then make sure that it is.  This
        // updates the position and time for all of the vertices that were
        // added to the event by the previous EDepSim::VKinematicsGenerator
        // call.  Get the first new vertex.
        if (!vtx) vtx = evt->GetPrimaryVertex();
        else vtx = vtx->GetNext();
        // Update the vertex for the new vertices.
        for (;vtx; vtx = vtx->GetNext()) {
            // Update the vertex info (and make sure it exists).  This sets
            // the generator name.
            EDepSim::VertexInfo* vInfo 
                = dynamic_cast<EDepSim::VertexInfo*>(vtx->GetUserInformation());
            if (!vInfo) {
                vInfo = new EDepSim::VertexInfo();
                vtx->SetUserInformation(vInfo);
            }
            vInfo->SetName(GetName());
            if (fPosition->ForcePosition()) {
                EDepSimVerbose("#    Force position to" 
                         << " x: " << vertex.x()/cm << " cm"
                         << " y: " << vertex.y()/cm << " cm"
                         << " z: " << vertex.z()/cm << " cm");
                vtx->SetPosition(vertex.x(), vertex.y(), vertex.z());
            }
            if (fTime->ForceTime()) {
                EDepSimVerbose("#    Force time to " 
                            << vertex.t()/ns << " ns");
                vtx->SetT0(vertex.t());
            }
        }
        brake = 0;  // Remove the brakes.
        --count;

        // Check if the generator flagged that this vertex must end an event.
        if (generatorStatus == EDepSim::VKinematicsGenerator::kLastVertex) {
            break;
        }
    }
}

G4String EDepSim::PrimaryGenerator::GetName() {
    G4String name = fKinematics->GetName();
    name += ":" + fCount->GetName();
    name += "@" + fPosition->GetName();
    name += "-" + fTime->GetName();
    return name;
}
