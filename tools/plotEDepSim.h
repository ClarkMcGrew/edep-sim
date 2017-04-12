// TEST MACRO FOR USE WITH ROOT5.
#ifndef plotEDepSim_h
#define plotEDepSim_h

/// Fill a trajectory, but don't plot it.  This returns a pointer to the
/// object that needs to be drawn.
TObject* EDepSimFillTrajectory(int trackId);

/// Fill and plot a particular trajectory on the current canvas.  This returns
/// a pointer to the drawn object.  The returned object is owned by
/// ClearTrajectories.
TObject* EDepSimPlotTrajectory(int trackId);

/// Plot all the trajectories.
int EDepSimPlotTrajectories();

/// Plot all the energy deposition from one detector.
int EDepSimPlotSegmentDetector(std::string name);

/// Plot the energy deposition from all of the detectors.
int EDepSimPlotSegmentDetectors();

/// Clear all of the plotted trajectories.
void EDepSimClearTrajectories();

/// Clear all the plotted hits.
void EDepSimClearHits();

/// Clear everything.
void EDepSimClearAll();

/// Plot the event.  If an argument isn't provided, a default value of -1 is
/// used and the current event is drawn.  If the drawGeom is true, then the
/// geometry is draw, otherwise not (default: true);
void EDepSimPlotEvent(int i, bool drawGeom);

#endif
