////////////////////////////////////////////////////////////
// $Id: DSimHitSegment.cc,v 1.14 2011/06/29 04:35:53 mcgrew Exp $
//
// Define an Off Axis Tracker Hit to hold information about particles that hit
// the Tracker sensitive detectors.

#include <cmath>
#include <iostream>
#include <map>
#include <sstream>

#include <G4EventManager.hh>
#include <G4Event.hh>
#include <G4VTrajectory.hh>
#include <G4VTrajectoryPoint.hh>
#include <G4TrajectoryContainer.hh>
#include <G4TouchableHandle.hh>
#include <G4Step.hh>

#include <G4UnitsTable.hh>
#include <G4VisAttributes.hh>
#include <G4VVisManager.hh>
#include <G4Polyline.hh>
#include <G4Color.hh>

#include "DSimRootGeometryManager.hh"
#include "DSimHitSegment.hh"
#include "DSimTrajectoryMap.hh"

G4Allocator<DSimHitSegment> DSimHitSegmentAllocator;

DSimHitSegment::DSimHitSegment(double maxSagitta, double maxLength) 
    : fMaxSagitta(maxSagitta), fMaxLength(maxLength) {
    fPath.reserve(500);
}

DSimHitSegment::~DSimHitSegment() { }

bool DSimHitSegment::SameHit(G4Step* theStep) {
    // Check that the hit and new step are in the same volume
    G4TouchableHandle touchable 
        = theStep->GetPreStepPoint()->GetTouchableHandle();
    if (fHitVolume != touchable) return false;

    // Check that the hit and new step are close together.
    double endToEnd
        = (theStep->GetPostStepPoint()->GetPosition() - fPath.front()).mag();
    if (endToEnd > fMaxLength) return false;

    double deltaT = std::abs(theStep->GetPostStepPoint()->GetGlobalTime()
                             - fStopT);
    if (deltaT>1*ns) return false;

    int trackId = theStep->GetTrack()->GetTrackID();
    if (fContributors.front() == trackId) {
        // This is still the same track that initially created this hit.
        // Check to see if the hit should be extended, or if we should start a
        // new segment.
        double sagitta = FindSagitta(theStep);
        if (sagitta > fMaxSagitta) return false;
    }
    else {
        // This is not the same track that started this hit, but check to see
        // if it is a delta-ray that should be added to this segment.
        double separation = FindSeparation(theStep);
        if (separation > fMaxSagitta) return false;
    }

    return true;
}

int DSimHitSegment::FindPrimaryId(G4Track *theTrack) {
    return DSimTrajectoryMap::FindPrimaryId(theTrack->GetTrackID());
}

void DSimHitSegment::AddStep(G4Step* theStep, double start, double end) {
    G4TouchableHandle touchable 
        = theStep->GetPreStepPoint()->GetTouchableHandle();
    G4ThreeVector prePos = theStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector postPos = theStep->GetPostStepPoint()->GetPosition();
    G4StepStatus stepStatus = theStep->GetPostStepPoint()->GetStepStatus();
    G4ParticleDefinition* particle =  theStep->GetTrack()->GetDefinition();
    double energyDeposit = theStep->GetTotalEnergyDeposit();
    double stepLength = (prePos-postPos).mag();
    double trackLength = theStep->GetStepLength();

    if (trackLength < 0.75*stepLength || trackLength < stepLength - 1*mm) {
        CaptWarn("Track length shorter than step: " 
                  << trackLength/mm << " mm "
                  << "<" << stepLength/mm << " mm");
        CaptWarn("    Volume: "
                  << theStep->GetTrack()->GetVolume()->GetName());
        CaptWarn("    Particle: " << particle->GetParticleName()
                  << " Depositing " << energyDeposit/MeV << " MeV");
        CaptWarn("    Total Energy: " 
                  << theStep->GetTrack()->GetTotalEnergy());
    }

    trackLength = std::max(trackLength,stepLength);

    if (energyDeposit <= 0.) {
        CaptWarn("DSimHitSegment:: No energy deposited: " << energyDeposit);
    }

    if (trackLength <= 0.) {
        CaptWarn("DSimHitSegment:: No track length: " << trackLength);
    }

    // Occasionally, a neutral particle will produce a particle below
    // threshold, and it will be recorded as generating the hit.  All of the
    // energy should be deposited at the stopping point of the track.
    if (stepStatus == fPostStepDoItProc
        && std::abs(particle->GetPDGCharge()) < 0.1) {
        double origStep = stepLength;
        G4ThreeVector dir = (postPos - prePos).unit();
        stepLength = trackLength = std::min(0.5*mm,0.8*origStep);
        prePos = postPos - stepLength*mm*dir;
        CaptDebug("DSimHitSegment:: " << particle->GetParticleName()
                   << " Deposited " << energyDeposit/MeV << " MeV");
        CaptDebug("    Original step: " << origStep/mm << " mm");
        CaptDebug("    New step: " << stepLength/mm << " mm");
    }

    if (stepLength>fMaxLength || trackLength>fMaxLength) {
        G4Track* trk = theStep->GetTrack();
        CaptWarn("DSimHitSegment:: Long step in "
                  << trk->GetVolume()->GetName());
        CaptWarn("  Step Length is " 
                  << stepLength/mm 
                  << " mm and track length is " 
                  << trackLength/mm << " mm");

        CaptWarn("  PID: " << particle->GetParticleName()
                  << " E: " << trk->GetTotalEnergy()/MeV << " MeV"
                  << " (kin: " << trk->GetKineticEnergy()/MeV << " MeV"
                  << " Deposit: " 
                  << energyDeposit/MeV << "MeV"
                  << " Status: " << theStep->GetPreStepPoint()->GetStepStatus()
                  << " -> " << theStep->GetPostStepPoint()->GetStepStatus());
        
        const G4VProcess* proc = theStep->GetPostStepPoint()
            ->GetProcessDefinedStep();
        if (!proc) {
            CaptWarn("    Step Limit Reached");
        }
        else {
            CaptWarn("    Process: " << proc->GetProcessName()
                      <<"/"<< proc->GetProcessTypeName(proc->GetProcessType()));
        }
    }

    /// First make sure the step has been initialized.
    if (!fHitVolume) {
        fHitVolume = touchable;
        fPrimaryId = FindPrimaryId(theStep->GetTrack());
        fStartX = prePos.x();
        fStartY = prePos.y();
        fStartZ = prePos.z();
        fStartT = theStep->GetPreStepPoint()->GetGlobalTime();
        fPath.push_back(G4ThreeVector(fStartX,fStartY,fStartZ));
        fStopX = postPos.x();
        fStopY = postPos.y();
        fStopZ = postPos.z();
        fStopT = theStep->GetPostStepPoint()->GetGlobalTime();
        fPath.push_back(G4ThreeVector(fStopX,fStopY,fStopZ));
        fContributors.push_back(theStep->GetTrack()->GetTrackID());
    }
    else {
        // Record the tracks that contribute to this hit.
        int trackId = theStep->GetTrack()->GetTrackID();
        fContributors.push_back(trackId);

        // Check to see if we have a new stopping point.
        if (trackId == fContributors.front()
            && (fPath.back()-prePos).mag()<0.1*mm) {
            fStopX = postPos.x();
            fStopY = postPos.y();
            fStopZ = postPos.z();
            fStopT = theStep->GetPostStepPoint()->GetGlobalTime();
            fPath.push_back(G4ThreeVector(fStopX,fStopY,fStopZ));
        }
    }

    fEnergyDeposit += energyDeposit;
    fTrackLength += trackLength;
}

double DSimHitSegment::FindSagitta(G4Step* theStep) {
    const G4ThreeVector& point = fPath.back();
    const G4ThreeVector& preStep = theStep->GetPreStepPoint()->GetPosition();

    // Make sure that the step began at the end of the current segment.  If
    // not, return a ridiculously large sagitta.
    if ((point-preStep).mag()>0.01*mm) return 10*m;

    const G4ThreeVector& postStep = theStep->GetPostStepPoint()->GetPosition();
    // The proposed new segment direction; 
    G4ThreeVector newDir = (postStep-point).unit();
    
    // Initialize the maximum sagitta found.
    double maxSagitta = 0.0;

    G4ThreeVector& front = fPath.front();

    // Loop over the existing path points and see if any would fall outside of
    // the tolerance.
    for (std::vector<G4ThreeVector>::iterator p = fPath.begin();
         p != fPath.end();
         ++p) {
        G4ThreeVector delta = ((*p)-front);
        double s = (delta*newDir);
        maxSagitta = std::max(maxSagitta,(delta - s*newDir).mag());
        if (maxSagitta > fMaxSagitta) break;
    }

    return maxSagitta;
}

double DSimHitSegment::FindSeparation(G4Step* theStep) {
    G4ThreeVector& front = fPath.front();
    const G4ThreeVector& back = fPath.back();
    const G4ThreeVector& preStep = theStep->GetPreStepPoint()->GetPosition();
    const G4ThreeVector& postStep = theStep->GetPostStepPoint()->GetPosition();
    G4ThreeVector dir = (back-front).unit();

    // Check to make sure the beginning of the new step isn't after the
    // end of this segment.
    if ((preStep-back)*dir>0.0) return 10*m;

    // Check to make sure the beginning of the new step isn't before the
    // beginning of this segment.
    G4ThreeVector frontDelta = preStep-front;
    double cosDelta = frontDelta*dir;
    if (cosDelta<0.0) return 10*m;

    // Find the distance from the segment center line to the initial point of
    // the new step.
    double s1 = (frontDelta - cosDelta*dir).mag();


    // Find the distance from the segment center line to the final point of
    // the new step.
    G4ThreeVector rearDelta = postStep-front;
    cosDelta = rearDelta*dir;
    double s2 = (rearDelta - cosDelta*dir).mag();
    
    return std::max(s1,s2);
}

void DSimHitSegment::Draw(void) {
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager) {
        G4Polyline line;
        line.push_back(G4Point3D(fStartX, fStartY, fStartZ));
        line.push_back(G4Point3D(fStopX, fStopY, fStopZ));
        G4Color color(1.,0.,1.);
        G4VisAttributes attribs(color);
        line.SetVisAttributes(attribs);
        pVVisManager->Draw(line);
    }
}

void DSimHitSegment::Print(void) {
    std::cout << "Hit Energy Deposit: " 
        << G4BestUnit(fEnergyDeposit,"Energy")
        << std::endl;
}

double DSimHitSegment::GetLength() const {
    return (G4Point3D(fStartX, fStartY, fStartZ)
            - G4Point3D(fStopX, fStopY, fStopZ)).mag();
}
