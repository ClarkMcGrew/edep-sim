////////////////////////////////////////////////////////////
// $Id: EDepSim::HitSegment.cc,v 1.14 2011/06/29 04:35:53 mcgrew Exp $
//
// Define an Off Axis Tracker Hit to hold information about particles that hit
// the Tracker sensitive detectors.

#include "EDepSimRootGeometryManager.hh"
#include "EDepSimHitSegment.hh"
#include "EDepSimTrajectoryMap.hh"
#include "EDepSimLog.hh"

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

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include <cmath>
#include <iostream>
#include <map>
#include <sstream>

G4Allocator<EDepSim::HitSegment> edepHitSegmentAllocator;

EDepSim::HitSegment::HitSegment(double maxSagitta, double maxLength) 
    : fMaxSagitta(maxSagitta), fMaxLength(maxLength),
      fPrimaryId(0), fEnergyDeposit(0), fSecondaryDeposit(0), fTrackLength(0), 
      fStart(0,0,0,0), fStop(0,0,0,0) {
    fPath.reserve(500);
}

EDepSim::HitSegment::HitSegment(const EDepSim::HitSegment& rhs)
    : G4VHit(rhs), 
      fMaxSagitta(rhs.fMaxSagitta), fMaxLength(rhs.fMaxLength),
      fContributors(rhs.fContributors), fPrimaryId(rhs.fPrimaryId),
      fEnergyDeposit(rhs.fEnergyDeposit),
      fSecondaryDeposit(rhs.fSecondaryDeposit),
      fTrackLength(rhs.fTrackLength), 
      fStart(rhs.fStart), fStop(rhs.fStop) {}


EDepSim::HitSegment::~HitSegment() { }

bool EDepSim::HitSegment::SameHit(G4Step* theStep) {
    // Check that the hit and new step are in the same volume
    G4TouchableHandle touchable 
        = theStep->GetPreStepPoint()->GetTouchableHandle();
    if (fHitVolume != touchable) {
        return false;
    }

    int trackId = theStep->GetTrack()->GetTrackID();

    // Check that the hit and new step are close together.
    double endToEnd
        = (theStep->GetPostStepPoint()->GetPosition() - fPath.front()).mag();
    if (endToEnd > fMaxLength) {
        return false;
    }

    const double epsilon = 0.01;
    double deltaT = std::abs(theStep->GetPostStepPoint()->GetGlobalTime()
                             - fStop.t());
    if (deltaT>epsilon) {
        return false;
    }

    if (fContributors.front() == trackId) {
        // This is still the same track that initially created this hit.
        // Check to see if the hit should be extended, or if we should start a
        // new segment.
        double sagitta = FindSagitta(theStep);
        if (sagitta > fMaxSagitta) {
            return false;
        }
    }
    else {
        // This is not the same track that started this hit, but check to see
        // if it is a delta-ray that should be added to this segment.
        double separation = FindSeparation(theStep);
        if (separation > fMaxSagitta) {
            return false;
        }
    }

    return true;
}

int EDepSim::HitSegment::FindPrimaryId(G4Track *theTrack) {
    return EDepSim::TrajectoryMap::FindPrimaryId(theTrack->GetTrackID());
}

void EDepSim::HitSegment::AddStep(G4Step* theStep) {
    G4TouchableHandle touchable 
        = theStep->GetPreStepPoint()->GetTouchableHandle();
    G4ThreeVector prePos = theStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector postPos = theStep->GetPostStepPoint()->GetPosition();
    G4StepStatus stepStatus = theStep->GetPostStepPoint()->GetStepStatus();
    G4ParticleDefinition* particle =  theStep->GetTrack()->GetDefinition();
    double energyDeposit = theStep->GetTotalEnergyDeposit();
    double stepLength = (prePos-postPos).mag();
    double trackLength = theStep->GetStepLength();
    double nonIonizingDeposit = theStep->GetNonIonizingEnergyDeposit();
    
    if (trackLength < 0.75*stepLength || trackLength < stepLength - 1*mm) {
        EDepSimWarn("Track length shorter than step: " 
                 << trackLength/mm << " mm "
                 << "<" << stepLength/mm << " mm");
        EDepSimWarn("    Volume: "
                  << theStep->GetTrack()->GetVolume()->GetName());
        EDepSimWarn("    Particle: " << particle->GetParticleName()
                 << " Depositing " << energyDeposit/MeV << " MeV");
        EDepSimWarn("    Total Energy: " 
                  << theStep->GetTrack()->GetTotalEnergy());
    }

    trackLength = std::max(trackLength,stepLength);

    EDepSimTrace("Add Step with " << energyDeposit 
              << " in " << theStep->GetTrack()->GetVolume()->GetName());

    if (energyDeposit <= 0.) {
        EDepSimWarn("EDepSim::HitSegment:: No energy deposited: " << energyDeposit);
    }

    if (trackLength <= 0.) {
        EDepSimWarn("EDepSim::HitSegment:: No track length: " << trackLength);
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
        EDepSimDebug("EDepSim::HitSegment:: " << particle->GetParticleName()
                  << " Deposited " << energyDeposit/MeV << " MeV");
        EDepSimDebug("    Original step: " << origStep/mm << " mm");
        EDepSimDebug("    New step: " << stepLength/mm << " mm");
    }

    if (stepLength>fMaxLength || trackLength>fMaxLength) {
        G4Track* trk = theStep->GetTrack();
        EDepSimWarn("EDepSim::HitSegment:: Long step in "
                  << trk->GetVolume()->GetName());
        EDepSimWarn("  Step Length is " 
                  << stepLength/mm 
                  << " mm and track length is " 
                  << trackLength/mm << " mm");

        EDepSimWarn("  PID: " << particle->GetParticleName()
                  << " E: " << trk->GetTotalEnergy()/MeV << " MeV"
                  << " (kin: " << trk->GetKineticEnergy()/MeV << " MeV"
                  << " Deposit: " 
                  << energyDeposit/MeV << "MeV"
                  << " Status: " << theStep->GetPreStepPoint()->GetStepStatus()
                  << " -> " << theStep->GetPostStepPoint()->GetStepStatus());
        
        const G4VProcess* proc = theStep->GetPostStepPoint()
            ->GetProcessDefinedStep();
        if (!proc) {
            EDepSimWarn("    Step Limit Reached");
        }
        else {
            EDepSimWarn("    Process: " << proc->GetProcessName()
                      <<"/"<< proc->GetProcessTypeName(proc->GetProcessType()));
        }
    }

    /// First make sure the step has been initialized.
    if (!fHitVolume) {
        fHitVolume = touchable;
        fPrimaryId = FindPrimaryId(theStep->GetTrack());
        fStart.set(prePos.x(),prePos.y(),prePos.z(),
                   theStep->GetPreStepPoint()->GetGlobalTime());
        fPath.push_back(fStart.vect());
        fStop.set(postPos.x(),postPos.y(),postPos.z(),
                      theStep->GetPostStepPoint()->GetGlobalTime());
        fPath.push_back(fStop.vect());
        fContributors.push_back(theStep->GetTrack()->GetTrackID());
    }
    else {
        // Record the tracks that contribute to this hit.
        int trackId = theStep->GetTrack()->GetTrackID();
        if (trackId != fContributors.front()) fContributors.push_back(trackId);

        // Check to see if we have a new stopping point.
        if (trackId == fContributors.front()
            && (fPath.back()-prePos).mag()<0.1*mm) {
            fStop.set(postPos.x(),postPos.y(),postPos.z(),
                      theStep->GetPostStepPoint()->GetGlobalTime());
            fPath.push_back(G4ThreeVector(fStop.x(),fStop.y(),fStop.z()));
        }
    }

    fEnergyDeposit += energyDeposit;
    fSecondaryDeposit += nonIonizingDeposit;
    fTrackLength += trackLength;

    EDepSimDebug("EDepSim::HitSegment:: Deposit " << particle->GetParticleName()
              << " adds " << energyDeposit/MeV << " MeV"
              << " to " << fContributors.front()
              << " w/ " << fEnergyDeposit
              << " L " << trackLength
              << " " << fTrackLength);
}

double EDepSim::HitSegment::FindSagitta(G4Step* theStep) {
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
        double dist = (delta*newDir);
        maxSagitta = std::max(maxSagitta,(delta - dist*newDir).mag());
        if (maxSagitta > fMaxSagitta) break;
    }

    return maxSagitta;
}

double EDepSim::HitSegment::FindSeparation(G4Step* theStep) {
    G4ThreeVector& front = fPath.front();
    const G4ThreeVector& back = fPath.back();
    const G4ThreeVector& preStep = theStep->GetPreStepPoint()->GetPosition();
    const G4ThreeVector& postStep = theStep->GetPostStepPoint()->GetPosition();
    G4ThreeVector dir = (back-front).unit();

    // Check to make sure the beginning of the new step isn't after the
    // end of this segment.
    if ((preStep-back)*dir>0.0) return 1000*m;

    // Check to make sure the beginning of the new step isn't before the
    // beginning of this segment.
    G4ThreeVector frontDelta = preStep-front;
    double cosDelta = frontDelta*dir;
    if (cosDelta<0.0) return 1000*m;

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

void EDepSim::HitSegment::Draw(void) {
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager) {
        G4Polyline line;
        line.push_back(G4Point3D(fStart.x(), fStart.y(), fStart.z()));
        line.push_back(G4Point3D(fStop.x(), fStop.y(), fStop.z()));
        G4Color color(1.,0.,1.);
        G4VisAttributes attribs(color);
        line.SetVisAttributes(attribs);
        pVVisManager->Draw(line);
    }
}

void EDepSim::HitSegment::Print(void) {
    std::cout << "Hit Energy Deposit: " 
        << G4BestUnit(fEnergyDeposit,"Energy")
        << std::endl;
}

double EDepSim::HitSegment::GetLength() const {
    return (fStop.vect()- fStart.vect()).mag();
}
