//////////////////////////////////////////////////////////////////
// A singleton class to pass through generator mc-truth information 
// used input for detsim.
//////////////////////////////////////////////////////////////////
#ifndef DSimKinemPassThrough_hh
#define DSimKinemPassThrough_hh

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TChain.h"


/// Stores generator mc-truth pass-through information used as input into
/// detsim.  This class is a singleton which allows a single tree to persist
/// throughout multiple instantiations of the neutrino primary vertex
/// generators. This is so an exact copy of only the mc-truth information
/// (associated with all primary vertices) used as input for the detsim
/// detector simulation can be made. These primary vertices can come from
/// multiple input files and this class also records bookkeeping information
/// pertaining to the type and name and other useful information, such as POT
/// normalisation, about the input tree and the input file used.
///
/// The following illistrates how the entries are copied: First add input
/// trees to TChain by calling,
///
/// \code
///    DSimKinemPassThrough::AddInputTree("file1.root", treePtr1) ,
/// \endcode
///
/// and then adding a second tree,
///
/// \code
///    DSimKinemPassThrough::AddInputTree("file2.root", treePtr2) ,
/// \endcode
///
/// for two trees with N_1 and N_2 events and stored in file1 and file2
/// respectively.  These trees corresponding to the trees pointed at in
/// detsim by treePtr1 and treePtr2 respectively. Now calling
/// 
/// \code
///    DSimKinemPassThrough::AddEntry(i, treePtrX) ,
/// \endcode
///
/// (where X can be 1 or 2) will copy the i'th entry from the TChain when X
/// = 1 and the (N_1 + i)'th entry when X = 2. There is no limit to the
/// number of input trees.
class DSimKinemPassThrough {
public:
    /// for relating input tree pointers to the input file.
    typedef std::map<const TTree *, int>  TreeToInt;

    ~DSimKinemPassThrough();

    ///  Returns (or initialises if first time) the instance 
    ///  of the singleton.
    static DSimKinemPassThrough * GetInstance();

    ///  Add an input tree to the TChain of input trees that rootracker
    ///  entries will be copied from. A map between the tree pointer that
    ///  detsim is using and the correct segment of the TChain is also made.
    bool AddInputTree(const TTree * inputTreePtr,
                      const char * inputFileName, 
                      const char* generatorName);

    ///  Copy the i'th entry from segment of the TChain corresponding to the
    ///  input tree pointed at (in detsim) by inputTreePtr.
    bool AddEntry(const TTree * inputTreePtr, const int origEntry);

    ///  Return the position (entry number) that the most recent entry to be
    ///  copied to the pass-through tree will have.
    int  LastEntryNumber();
  
private:
    /// Private constructor.
    DSimKinemPassThrough();

    ///  Static pointer to singleton instance.
    static DSimKinemPassThrough * fKinemPassThrough;
  
    /// Create the bookkeeping and file list trees.  This also creates the
    /// directory.
    void CreateInternalTrees();

    /// Clean up all of the allocated pointers.
    void CleanUp(); 

    /// Set default values for all fields.
    void Init();

    /// Set the name of the input file being read.
    std::string SetInputFileName(std::string name);

    /// Persistent tree that stores entries from multiple temp trees.    
    TTree* fPersistentTree; 

    /// TChain to store input trees.
    TChain* fInputTreeChain;

    /// Store the first tree name.
    std::string fFirstTreeName; 

    /// for relating input tree pointers to the input file.
    TreeToInt fInputTreeMap; 

    /// for relating input tree to first entry in TChain.
    TreeToInt fFirstEntryMap; 

    /// Used to store list of files before they are written to tree.
    std::vector<std::string> fFileList;

    // =====================================
    /// Tree relating all events in the persistent tree to an input file
    TTree* fInputKinemTree;

    /// Used to fill the bookkeeping tree "inputFileNum" field.  This is
    /// the entry number of the input file in the input file tree.
    int fInputFileNumber;  

    /// Used to fill the bookkeeping tree "origEntryNum" field.  This is the
    /// entry number of the event in the original tree.
    int fOrigEntryNumber;

    // =====================================
    /// Tree containing list of input files.
    TTree* fInputFilesTree; 

    /// Used to fill input file list tree with input file name.
    char fInputFileName[1024];

    /// Used to fill the name of the generator for this file.
    char fInputFileGenerator[128];

    /// Used to fill the name of the tree for this file.
    char fInputFileTreeName[128];

    /// Used to fill input file list tree "filePOT" field.  
    double fInputFilePOT;

    /// Used to fill input file list tree "nEntriesTot" field.  This is the
    /// total number of entries in the input tree.
    int fInputFileEntries;
};
#endif
