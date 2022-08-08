#ifndef TEventDisplay_hxx_seen
#define TEventDisplay_hxx_seen

namespace EDep {
    class TEventDisplay;
    class TGUIManager;
    class TEventChangeManager;
    class TPlotDigitsHits;
    class TPlotTimeCharge;
}

/// A singleton class for an event display based on EVE.
class EDep::TEventDisplay {
public: 

    /// Get a pointer to the singleton instance of the event display.  This
    /// creates the event display the first time it is called.
    static TEventDisplay& Get(void);

    /// Deconstruct the event display.
    virtual ~TEventDisplay();

    /// Return a reference to the gui manager.
    EDep::TGUIManager& GUI() {return *fGUIManager;}

    /// Return a reference to the event change manager.
    EDep::TEventChangeManager& EventChange() {return *fEventChangeManager;}

    /// Get a color from the palette using a linear value scale.
    int LinearColor(double val, double minVal, double maxVal);

    /// Get a color from the palette using a logarithmic value scale.
    int LogColor(double val, double minVal, double maxVal, double magScale=5.0);

    /// Convert an charge into a crude energy.
    double CrudeEnergy(double charge) {return fEnergyPerCharge*charge;}
    
private:
    // Prevent direct construction.
    TEventDisplay();

    // Actually initialize the event display
    void Init();

    // Hold the static instance of the event display.
    static EDep::TEventDisplay* fEventDisplay;

    // The gui manager.
    TGUIManager* fGUIManager;

    // The event change manager.
    TEventChangeManager* fEventChangeManager;

    // The digit drawing class.  This is connected directly to the buttons.
    //TPlotDigitsHits* fPlotDigitsHits;

    // The hit time and charge drawing class.  This is connected directly to
    // the buttons.
    //TPlotTimeCharge* fPlotTimeCharge;

    // The base color index of the palette to use.
    int fColorBase;

    // The number of colors in the palette.
    int fColorCount;

    // The crude energy per charge conversion.
    double fEnergyPerCharge;
    
};

#endif
