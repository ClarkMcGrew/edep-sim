#include "TEventChangeManager.hxx"
#include "TVEventChangeHandler.hxx"
#include "TGUIManager.hxx"
#include "TEventDisplay.hxx"
#include "TEventManager.hxx"
#include "TRecurseGeoManager.hxx"

#include "TG4Event.h"

#include <TQObject.h>
#include <TGButton.h>
#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TGeoVolume.h>
#include <TEveGeoShape.h>
#include <TEveGeoNode.h>
#include <TEveManager.h>

#include <iostream>

ClassImp(EDep::TEventChangeManager)

namespace {

    /// This takes a geometry id and "clones" it into the Eve display.
    TEveGeoShape* GeometryClone(TGeoVolume* currVolume) {

        TGeoShape* currShape = currVolume->GetShape();
        TEveGeoShape *fakeShape = new TEveGeoShape(currShape->GetName());
        
        TGeoMatrix* currMat = gGeoManager->GetCurrentMatrix();
        TGeoMatrix* mat = currMat->MakeClone();
        fakeShape->SetTransMatrix(*mat);
        
        // Clone the shape so that it can be displayed.  This has to play some
        // fancy footsie to get the gGeoManager memory management right.  It
        // first saves the current manager, then gets an internal geometry
        // manager used by TEveGeoShape, and then resets the old manager once
        // the shape is created.
        TGeoManager* saveGeom = gGeoManager;
        gGeoManager = fakeShape->GetGeoMangeur();
        TGeoShape* clonedShape 
            = dynamic_cast<TGeoShape*> (currShape->Clone("fakeShape"));
        fakeShape->SetShape(clonedShape);
        gGeoManager = saveGeom;
        
        return fakeShape;
    }

    struct PickVolumes: TRecurseGeoManager {
        std::vector<std::string> fVolumesToShow;
        TEveElementList* fEveList;
        PickVolumes(TEveElementList* eveList) : fEveList(eveList) {}
        bool Action(int depth) {
            if (!fEveList) return true;
            TGeoNode* node = gGeoManager->GetCurrentNode();
            std::string name(node->GetName());
            for (std::vector<std::string>::iterator v = fVolumesToShow.begin();
                 v != fVolumesToShow.end(); ++v) {
                if (name.find(*v) == std::string::npos) continue;
                std::cout << "Show volume " << node->GetName() << std::endl;
                TEveGeoShape *shape = GeometryClone(node->GetVolume());
                shape->SetMainColor(node->GetVolume()->GetLineColor());
                shape->SetMainTransparency(90);
                fEveList->AddElement(shape);
                break;
            }
            return true;
        }
    };
}

TTree* EDep::gEDepSimTree = NULL;
TG4Event* EDep::gEDepSimEvent = NULL;
int EDep::gEDepSimEntryNumber = 0;

EDep::TEventChangeManager::TEventChangeManager() {
    TGButton* button = EDep::TEventDisplay::Get().GUI().GetNextEventButton();
    if (button) {
        button->Connect("Clicked()",
                        "EDep::TEventChangeManager", 
                        this,
                        "ChangeEvent(=1)");
    }

    button = EDep::TEventDisplay::Get().GUI().GetDrawEventButton();
    if (button) {
        button->Connect("Clicked()",
                        "EDep::TEventChangeManager", 
                        this,
                        "ChangeEvent(=0)");
    }

    button = EDep::TEventDisplay::Get().GUI().GetPrevEventButton();
    if (button) {
        button->Connect("Clicked()",
                        "EDep::TEventChangeManager", 
                        this,
                        "ChangeEvent(=-1)");
    }

    ClearVolumesToShow();
}

EDep::TEventChangeManager::~TEventChangeManager() {}

void EDep::TEventChangeManager::SetEventSource(TFile* source) {
    if (!source) {
        std::cout << "Invalid event source" << std::endl;
        return;
    }
    fEventSource = source;
    gEDepSimTree = (TTree*) fEventSource->Get("EDepSimEvents");
    if (!gEDepSimTree) {
        std::cout << "Missing the event tree" << std::endl;
        return;
    }
    gEDepSimTree->SetBranchAddress("Event",&gEDepSimEvent);
    gEDepSimEntryNumber = 0;
    gEDepSimTree->GetEntry(gEDepSimEntryNumber);
    
    fEventSource->Get("EDepSimGeometry");
    
    if (GetShowGeometry() && !fVolumesToShow.empty()) {
        TEveElementList* simple = new TEveElementList("simplifiedGeometry");
        PickVolumes visitor(simple);
        visitor.fVolumesToShow.clear();
        visitor.fVolumesToShow.reserve(fVolumesToShow.size());
        std::copy(fVolumesToShow.begin(), fVolumesToShow.end(),
                  std::back_inserter(visitor.fVolumesToShow));
        visitor.Apply();
        gEve->AddGlobalElement(simple);
    }
    
    NewEvent();
    UpdateEvent();
}

void EDep::TEventChangeManager::AddNewEventHandler(
    EDep::TVEventChangeHandler* handler) {
    fNewEventHandlers.push_back(handler);
}

void EDep::TEventChangeManager::AddUpdateHandler(
    EDep::TVEventChangeHandler* handler) {
    fUpdateHandlers.push_back(handler);
}

void EDep::TEventChangeManager::ChangeEvent(int change) {
    std::cout << "Change Event by " << change << " entries" << std::endl;
    if (!GetEventSource()) {
        std::cout << "Event source is not available" << std::endl;
        UpdateEvent();
        return;
    }

    gEDepSimEntryNumber += change;
    if (gEDepSimEntryNumber >= gEDepSimTree->GetEntries()) {
        gEDepSimEntryNumber = gEDepSimTree->GetEntries() - 1;
    }
    if (gEDepSimEntryNumber < 0) gEDepSimEntryNumber = 0;

    if (change != 0) NewEvent(); 

    UpdateEvent();
}

void EDep::TEventChangeManager::NewEvent() {
    std::cout << "New Event" << std::endl;

    gEDepSimTree->GetEntry(gEDepSimEntryNumber);

    // Run through all of the handlers.
    for (Handlers::iterator h = fNewEventHandlers.begin();
         h != fNewEventHandlers.end(); ++h) {
        (*h)->Apply();
    }

}

void EDep::TEventChangeManager::UpdateEvent() {

    if (!gEDepSimEvent) {
        std::cout << "Invalid event" << std::endl;;
        return;
    }

    // Run through all of the handlers.
    for (Handlers::iterator h = fUpdateHandlers.begin();
         h != fUpdateHandlers.end(); ++h) {
        (*h)->Apply();
    }

    // Make sure EVE is up to date.
    gEve->Redraw3D(kTRUE);
}
