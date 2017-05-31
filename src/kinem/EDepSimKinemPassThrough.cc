#include "EDepSimKinemPassThrough.hh"

#include <TROOT.h>
#include <TList.h>

#include "EDepSimLog.hh"

#include <memory>
#include <cstdlib>
#include <cstring>

#define PASSTHRUDIR "DetSimPassThru"

// void sort() {
// 	TFile f("hsimple.root");
// 	TTree *tree = (TTree*)f.Get("ntuple");
// 	Int_t nentries = (Int_t)tree->GetEntries();
// 	//Drawing variable pz with no graphics option.
// 	//variable pz stored in array fV1 (see TTree::Draw)
// 	tree->Draw("pz","","goff");
// 	Int_t *index = new Int_t[nentries];
// 	//sort array containing pz in decreasing order
// 	//The array index contains the entry numbers in decreasing order
// of pz
// 	TMath::Sort(nentries,tree->GetV1(),index);
	
// 	//open new file to store the sorted Tree
// 	TFile f2("hsimple_sorted.root","recreate");
// 	//Create an empty clone of the original tree
// 	TTree *tsorted = (TTree*)tree->CloneTree(0);
// 	for (Int_t i=0;i<nentries;i++) {
// 		tree->GetEntry(index[i]);
// 		tsorted->Fill();
// 	}
// 	tsorted->Write();
// 	delete [] index;
// }


EDepSim::KinemPassThrough* EDepSim::KinemPassThrough::fKinemPassThrough = NULL;

EDepSim::KinemPassThrough::~KinemPassThrough() {
    CleanUp();
}

EDepSim::KinemPassThrough::KinemPassThrough() {
    Init();
    EDepSimNamedDebug("PassThru",
                    "Have called the EDepSim::KinemPassThrough constructor."); 
}

EDepSim::KinemPassThrough * EDepSim::KinemPassThrough::GetInstance() {
    EDepSimNamedTrace("PassThru",
                    "Get pointer to EDepSim::KinemPassThrough instance."); 
    if (fKinemPassThrough) return fKinemPassThrough;
    fKinemPassThrough = new EDepSim::KinemPassThrough();
    if (!fKinemPassThrough) std::abort();
    return fKinemPassThrough;
}

bool EDepSim::KinemPassThrough::AddInputTree(const TTree * inputTreePtr,
                                         const char * inputFileName, 
                                         const char* generatorName) {
    EDepSimNamedDebug("PassThru",
                    "Adding a generator mc-truth input"
                    " tree to the list of input trees that can be used."); 

    CreateInternalTrees();

    if (!fInputFilesTree || !fInputKinemTree) {
        EDepSimError("Pass-through trees not created.");  
        return false;
    }
    
    if (inputTreePtr == NULL) {
        EDepSimError("NULL input tree pointer.  TTree not saved.");  
        return false;
    }
  
    std::string inputTreeName(inputTreePtr->GetName()); 
    // check that all input trees have the same name. In future may add  
    // functionality so that maintain two persistent trees simultaneously 
    if (!fInputTreeChain) {
        fFirstTreeName = inputTreeName;
        fInputTreeChain = new TChain(fFirstTreeName.c_str());
    }
  
    if (fFirstTreeName != inputTreeName) {
        EDepSimError("Input tree name not compatible: "
                   "  All pass-through trees must have same name.");
        return false;
    }

    // check if its already been added
    if (fInputTreeMap.find(inputTreePtr) != fInputTreeMap.end()) {
        EDepSimError("Input tree already in chain.");
        return false;
    }
  
    //  Add input tree to TChain
    fInputTreeChain->Add(inputFileName);

    // Clone tree if this has not been done already. We check if fPersistent
    // tree is NULL rather than looking to see if fInputTreeChain has a list
    // of clones (as for a TChain this always returns false). 
    if (fPersistentTree == NULL) {
        EDepSimNamedDebug("PassThru", "Clone the input TTree");
        fPersistentTree = (TTree*) fInputTreeChain->CloneTree(0);
    }

    // Add the input file to the file list so it can be saved in the output
    // tree.  This is used for error reporting and debugging. 
    fFileList.push_back(SetInputFileName(inputFileName));

    // fill in input tree maps
    fInputTreeMap[inputTreePtr]  = fFileList.size() - 1;
    fFirstEntryMap[inputTreePtr] = (fInputTreeChain->GetEntries() 
                                    - inputTreePtr->GetEntries());
    
    // Copy the input file name and make sure it's 0 terminated.
    std::strncpy(fInputFileName, fFileList.back().c_str(),
                 sizeof(fInputFileName));
    fInputFileName[sizeof(fInputFileName)-1] = 0;
    // Copy the generator name and make sure it's 0 terminated.
    std::strncpy(fInputFileGenerator, generatorName, 
                 sizeof(fInputFileGenerator));
    // Copy the generator name and make sure it's 0 terminated.
    std::strncpy(fInputFileTreeName, inputTreeName.c_str(), 
                 sizeof(fInputFileTreeName));
    fInputFileTreeName[sizeof(fInputFileTreeName)-1] = 0;
    fInputFilePOT = inputTreePtr->GetWeight();
    fInputFileEntries = inputTreePtr->GetEntries();
    fInputFilesTree->Fill();

    EDepSimNamedDebug("PassThru", 
                    "Have added a " << fFirstTreeName 
                    << " tree from the input file "<< inputFileName); 
    
    // Now can start copying events
    return true;
}

void EDepSim::KinemPassThrough::CreateInternalTrees() {
    TFile* output = NULL;;
    TIter files(gROOT->GetListOfFiles());
    TObject* object;
    while ((object = files.Next())) {
	TFile* file = dynamic_cast<TFile*>(output);
        if (!file) continue;
        if (!file->IsOpen()) continue;
        std::string fileOption(file->GetOption());
        if (fileOption.find("CREATE") != std::string::npos) {
            output = file;
            continue;
        }
    }
    
    if (!output) {
        return;
    }
    
    output->cd();

    // Check if the directory exists (and possibly create it).
    if (!output->Get(PASSTHRUDIR)) {
        output->mkdir(PASSTHRUDIR,"DETSIM Pass-Through Information");
    }

    // Make sure we are in the pass-thru directory.
    output->cd(PASSTHRUDIR);

    // Create the book keeping three that connects a particular entry to the
    // entry in the original file.
    if (fInputKinemTree == NULL) {
        EDepSimNamedDebug("PassThru", "Create InputKinem TTree");
        // This adds the tree to the current output file.
        fInputKinemTree = new TTree("InputKinem",
                                     "Map kinematics with input files");
        fInputKinemTree->Branch("inputFileNum" , &fInputFileNumber); 
        fInputKinemTree->Branch("inputEntryNum", &fOrigEntryNumber);
    }

    if (fInputFilesTree == NULL) {
        EDepSimNamedDebug("PassThru","Create inputFileList TTree");
        // This adds the tree to the current output file.
        fInputFilesTree = new TTree("InputFiles","Input file information");
        fInputFilesTree->Branch("fileName", &fInputFileName,
                                "fileName/C");
        fInputFilesTree->Branch("generatorName", &fInputFileGenerator, 
                                "generatorName/C");
        fInputFilesTree->Branch("treeName", &fInputFileTreeName, 
                                "treeName/C");
        fInputFilesTree->Branch("filePOT", &fInputFilePOT);
        fInputFilesTree->Branch("fileEntries", &fInputFileEntries);
    }
}

bool 
EDepSim::KinemPassThrough::AddEntry(const TTree* inputTree, int origEntry) {
    if (!fPersistentTree) {       
        EDepSimNamedDebug("PassThru", "Cannot copy entry from tree "
                        "since  fPersistentTree is NULL."); 
        return false;
    }

    CreateInternalTrees();

    // Search the input tree maps for a TTree pointer that matches the
    // inputTree.
    TreeToInt::iterator treeid_iter = fInputTreeMap.find(inputTree);
    TreeToInt::iterator firstentry_iter = fFirstEntryMap.find(inputTree);
    if (treeid_iter == fInputTreeMap.end() 
        || firstentry_iter == fFirstEntryMap.end()) {
        EDepSimError("Cannot copy entry from tree not in list of input trees.");
        return false;
    }
  
    fInputFileNumber = treeid_iter->second;
    int first_event_in_chain = firstentry_iter->second;
  
    // Now fill temp tree with i'th entry from first_event_in_chain of TChain
    // of input trees.
    if (fInputTreeChain->GetEntry(origEntry+first_event_in_chain)) {
        fOrigEntryNumber = origEntry;
        fPersistentTree->Fill();
        fInputKinemTree->Fill(); // Also store book keeping info.
    
        EDepSimNamedTrace("PassThru", 
                        "Copied entry " << origEntry
                        << " from " << fFirstTreeName
                        <<  " tree in file "
                        << fFileList[fInputFileNumber]); 
    
        return true;
    }

    EDepSimError("Cannot copy entry " << origEntry+first_event_in_chain 
               << " from TChained Tree.  Make sure entry is in input tree!"); 
    return false;
}

int EDepSim::KinemPassThrough::LastEntryNumber() {
    // The most recent entry number is the sum of the entry number in
    // the current tree plus the number of entries already stored in 
    // persistent tree.
    if (!fPersistentTree) {
        EDepSimError("No entries in fPersistent tree.");
        return -1;
    }
    return fPersistentTree->GetEntries() - 1;
}

std::string EDepSim::KinemPassThrough::SetInputFileName(std::string name) {
    // Remove the path from the input file name. 
    std::string::size_type start_pos = name.rfind("/");
    if (start_pos == std::string::npos) start_pos = 0; else ++start_pos;
    std::string baseName(name,start_pos);

    return baseName;
}

void EDepSim::KinemPassThrough::CleanUp() {
    // Delete the output trees so that they can not be written twice (which
    // they would be as detsim calls TFile::Write() which writes anything in
    // memory to the TFile.
  
    if (fPersistentTree) {delete fPersistentTree;}
    if (fInputFilesTree) {delete fInputFilesTree;}
    if (fInputKinemTree) {delete fInputKinemTree;}
    if (fInputTreeChain) {delete fInputTreeChain;}

    Init();

    return;
}

void EDepSim::KinemPassThrough::Init() {
    fPersistentTree = NULL; 
    fInputFilesTree = NULL;
    fInputKinemTree = NULL;
    fInputTreeChain = NULL;
    fFirstTreeName.clear();
    fFileList.clear();
    fInputFileNumber = -1;
    fOrigEntryNumber = -1;
    fInputFileName[0] = 0;
    fInputFileGenerator[0] = 0;
    fInputFileTreeName[0] = 0;
    return;
}
