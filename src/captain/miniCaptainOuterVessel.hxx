
////////////////////////////////////////////////////////////////////////
// This is an auto-generated built using makePolycone.py.  It defines
// points to be used to build a G4Polycone. Prior to including this file,
// the vector fOuterVessel must be define in a way equivalent to
// 
// class Point {
//  public:
//      Point(double z, double i, double o): fZ(z), fInner(i), fOuter(o) {}
//      double fZ;
//      double fInner;
//      double fOuter;
//  };
//
// std::vector<Point> fOuterVessel;
//
////////////////////////////////////////////////////////////////////////

// Filename:  mcaptain-outer-vessel-inner-surface.txt
// Inner boundary of the miniCAPTAIN cryostat outer vessel hand digitized
// using engauge on Nov 22, 2015 by Clark McGrew.  The coordinates are
// measured in millimeters with the top of the cryostat at -X, the bottom at
// +X, and the diameter located along the Y axis.  The X coordinate is
// measured from the top of the flange with the nominal LAr surface located 9
// inches (609.6 mm) below the flange.  The first point is at the approximate
// top of the outer vessel which is located right below the flange.
//
// COMMENT: This assumes that the lid and the flange are part of the inner
// vessel.  The outer vessel stops when it reaches the lower edge of the
// flange.
//
// COMMENT: The makePolycone.py script will use this to produce a "close"
// volume.  The output needs to be edited by hand to remove the top point so
// that the outer volume is open on top.

// Filename:  mcaptain-outer-vessel-outer-surface.txt
// Inner boundary of the miniCAPTAIN cryostat inner vessel hand digitized
// using engauge on Nov 22, 2015 by Clark McGrew.  The coordinates are
// measured in millimeters with the top of the cryostat at -X, the bottom at
// +X, and the diameter located along the Y axis.  The X coordinate is
// measured from the top of the flange with the nominal LAr surface located 9
// inches (609.6 mm) below the top surface of flange.
//
// COMMENT: This includes the lid and the flange as part of the inner vessel.
// The outer vessel is assumed to stop when it reaches the lower edge of the
// flange.

fOuterVessel.push_back(Point(41.100000,0.000000,0.000000));
fOuterVessel.push_back(Point(41.400000,0.000000,0.000000));
fOuterVessel.push_back(Point(42.000000,0.000000,284.121853));
fOuterVessel.push_back(Point(42.900000,0.000000,756.172190));
fOuterVessel.push_back(Point(43.500000,0.000000,766.661813));
fOuterVessel.push_back(Point(44.100000,759.974197,766.661415));
fOuterVessel.push_back(Point(818.400000,759.782907,765.861282));
fOuterVessel.push_back(Point(836.400000,755.139136,761.909403));
fOuterVessel.push_back(Point(855.000000,749.142569,756.383495));
fOuterVessel.push_back(Point(868.800000,741.027845,749.495306));
fOuterVessel.push_back(Point(881.100000,733.795157,740.752417));
fOuterVessel.push_back(Point(884.700000,731.010878,737.675414));
fOuterVessel.push_back(Point(906.300000,709.995645,718.946849));
fOuterVessel.push_back(Point(916.200000,698.055401,706.994204));
fOuterVessel.push_back(Point(929.700000,678.405501,690.316441));
fOuterVessel.push_back(Point(942.000000,660.247535,672.000040));
fOuterVessel.push_back(Point(951.000000,643.997676,655.861266));
fOuterVessel.push_back(Point(969.300000,604.668004,618.758354));
fOuterVessel.push_back(Point(977.700000,584.921539,600.589879));
fOuterVessel.push_back(Point(987.900000,558.869961,577.763696));
fOuterVessel.push_back(Point(994.200000,542.510378,560.193321));
fOuterVessel.push_back(Point(1006.200000,506.480053,526.647928));
fOuterVessel.push_back(Point(1011.000000,488.585793,512.840250));
fOuterVessel.push_back(Point(1013.100000,480.098417,506.099598));
fOuterVessel.push_back(Point(1014.600000,473.565550,500.885063));
fOuterVessel.push_back(Point(1028.100000,414.485211,446.308366));
fOuterVessel.push_back(Point(1031.100000,400.164160,433.817789));
fOuterVessel.push_back(Point(1036.200000,375.594973,408.460567));
fOuterVessel.push_back(Point(1048.800000,306.001470,345.216915));
fOuterVessel.push_back(Point(1051.500000,291.006842,331.184670));
fOuterVessel.push_back(Point(1057.200000,240.228356,301.561042));
fOuterVessel.push_back(Point(1063.800000,162.198596,267.259999));
fOuterVessel.push_back(Point(1064.400000,154.548974,261.835296));
fOuterVessel.push_back(Point(1071.000000,53.583900,178.236617));
fOuterVessel.push_back(Point(1072.500000,30.273877,155.431911));
fOuterVessel.push_back(Point(1074.300000,0.818213,126.039164));
fOuterVessel.push_back(Point(1075.200000,0.000000,111.342791));
fOuterVessel.push_back(Point(1078.500000,0.000000,57.456088));
fOuterVessel.push_back(Point(1080.000000,0.000000,15.128415));
fOuterVessel.push_back(Point(1080.600000,0.000000,0.000000));
