#ifndef EDepSim_CaptMaterialDefinition_h
#define EDepSim_CaptMaterialDefinition_h 1

namespace EDepSim {class CaptMaterialDefinitions;}

/// Define materials used in the "captain" test geometry.  This is isolated
/// here since these materials have *VERY* fake paramters used to test the
/// simulation, not to do physics.  These should probably not be included when
/// doing productions.
class EDepSim::CaptMaterialDefinitions {
public:
    CaptMaterialDefinitions() {}
    virtual ~CaptMaterialDefinitions() {}

    /// Define the materials used in the testing detector.
    void DefineMaterials(void);
};

#endif
