#ifndef DSimVertexInfo_hh_seen
#define DSimVertexInfo_hh_seen

#include <G4VUserPrimaryVertexInformation.hh>
#include <globals.hh>

class G4PrimaryVertex;

/// Hold information that is associated with each primary vertex.  This holds
/// the particle reaction code that generated the vertex, the name of the
/// generator that created the vertex, and internal information from the
/// kinematics generator about the physics that generated the vertex.
class DSimVertexInfo : public G4VUserPrimaryVertexInformation {
public:
    explicit DSimVertexInfo();
    virtual ~DSimVertexInfo();

    /// Set the generator name.  This is set in DSimPrimaryGenerator after
    /// the vertex is generated so it should not be set when the vertex is
    /// created in a kinematics generator.
    void SetName(const G4String& name) {fName = name;}

    /// Get the generator name.  The name has the format of
    /// <kine>:<count>@<pos>-<time> where <kine> is the kinematics generator,
    /// <count> is the count generator, <pos> is the position generator, and
    /// <time> is the time generator.
    G4String GetName() const {return fName;}

    /// Set reaction that created this vertex.  This is defined by each
    /// kinematics generator.
    /// \todo We need to define a standard format for the reaction names.
    void SetReaction(const G4String& r) {fReaction = r;}

    /// Get the reaction code that created this vertex.
    const G4String& GetReaction() const {return fReaction;}

    /// Set the file that this vertex came from.  
    void SetFilename(const G4String& f) {fFilename = f;}

    /// Get the file that this vertex came from.
    const G4String& GetFilename() {return fFilename;}

    /// Set the index of the interaction within the input interaction file.
    void SetInteractionNumber(int i) {fInteractionNumber = i;}

    /// Get the index of the interaction within the input interaction file.
    int GetInteractionNumber() const {return fInteractionNumber;}

    /// Set the cross section for the reaction that created this vertex.  This
    /// should be in normal HEP units.
    void SetCrossSection(double xs) {fCrossSection = xs;}

    /// Get the cross section for the reaction that created this vertex.
    double GetCrossSection() const {return fCrossSection;}

    /// Set the differential cross section for the kinematics that created
    /// this vertex.  This should be in normal HEP units.
    void SetDiffCrossSection(double xs) {fDiffCrossSection = xs;}
    
    /// Get the differential cross section for the kinematics of the reaction
    /// that created this vertex.
    double GetDiffCrossSection() const {return fDiffCrossSection;}

    /// Set the weight for the vertex.  This is used if the generator is using
    /// a weighted sample (e.g. over sampling the high energy part of the
    /// neutrino spectrum smooth out the distributions).
    void SetWeight(double w) {fWeight = w;}
    
    /// Get the weight of the vertex.  This will be one if it's not filled.
    double GetWeight() const {return fWeight;}

    /// Set the probability of the interaction that created this vertex.  This
    /// includes the effect of the cross section, path length through the
    /// material, etc.
    void SetProbability(double p) {fProbability = p;}

    /// Get the probability of the interaction.
    double GetProbability() const {return fProbability;}

    /// Return the informational vertices that are associated with this event.
    /// These vertices usually contain information associated with the primary
    /// interaction, such as the incoming neutrino and nucleon.
    virtual const G4PrimaryVertex* GetInformationalVertex(int i = 0) const;

    /// Add an informational vertex to this event.
    virtual void AddInformationalVertex(G4PrimaryVertex* vtx);

    /// Return the number of informational vertices.
    virtual int GetNumberOfInformationalVertex() const;

    /// Required by G4UserPrimaryVertexInformation.
    void Print() const {};
   
private:
    /// The generator name.  
    G4String fName;

    /// The reaction code as defined by the kinematics generator.
    G4String fReaction;

    /// The list of informational vertices associated with this primary
    /// vertex.
    G4PrimaryVertex* fInformationalVertex;

    /// The file that this vertex came from.
    G4String fFilename;

    /// The index of the interaction in the input neutrino file.
    int fInteractionNumber;

    /// The total cross section of the interaction creating this vertex.  This
    /// is zero if it's not filled.
    double fCrossSection;

    /// The differential cross section of the interaction kinematics creating
    /// this vertex.  This is zero if it's not filled.
    double fDiffCrossSection;
    
    /// The weight for the interaction creating this vertex.  This is one if
    /// it's not filled.
    double fWeight;

    /// The overall probability of this interaction given the cross sections,
    /// path lengths through materials, &c).  This is one if it is not filled.
    double fProbability;
};
#endif
