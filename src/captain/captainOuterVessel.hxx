
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

// Filename:  captain-outer-vessel-inner-boundary.txt
// Inner boundary of the CAPTAIN cryostat outer vessel hand digitized using
// engauge on Nov 20, 2015 by Clark McGrew.  The coordinates are measured in
// millimeters with the top of the cryostat at -X, the bottom at +X, and the
// diameter located along the Y axis.  The X coordinate is measured from the
// top of the flange with the nominal LAr surface located 24 inches (609.6 mm)
// below the flange.  The first point is at the approximate top of the outer
// vessel which is located right below the flange.
//
// COMMENT: This assumes that the lid and the flange are part of the inner
// vessel.  The outer vessel stops when it reaches the lower edge of the
// flange.
//
// COMMENT: The makePolycone.py script will use this to produce a "close"
// volume.  The output needs to be edited by hand to remove the top point so
// that the outer volume is open on top.

// Filename:  captain-outer-vessel-outer-boundary.txt
// Outer boundary of the CAPTAIN cryostat outer vessel hand digitized using
// engauge on Nov 20, 2015 by Clark McGrew.  The coordinates are measured in
// millimeters with the top of the cryostat at -X, the bottom at +X, and the
// diameter located along the Y axis.  The X coordinate is measured from the
// top of the flange with the nominal LAr surface located 24 inches (609.6 mm)
// below the flange.  The first point is at the approximate top of the outer
// vessel which is located right below the flange.
//
// COMMENT: This assumes that the lid and the flange are part of the inner
// vessel.  The outer vessel stops when it reaches the lower edge of the
// flange.
//
// COMMENT: The makePolycone.py script will use this to produce a "close"
// volume.  The output needs to be edited by hand to remove the top point so
// that the outer volume is open on top.

fOuterVessel.push_back(Point(37.800000,229.242662,232.027198));
fOuterVessel.push_back(Point(39.900000,1256.140250,1258.912585));
fOuterVessel.push_back(Point(40.500000,1354.966918,1361.454226));
fOuterVessel.push_back(Point(1849.200000,1354.682835,1360.529523));
fOuterVessel.push_back(Point(1857.900000,1353.836045,1359.774148));
fOuterVessel.push_back(Point(1871.100000,1352.018686,1357.752030));
fOuterVessel.push_back(Point(1882.500000,1349.103870,1355.845292));
fOuterVessel.push_back(Point(1900.500000,1343.486791,1351.077430));
fOuterVessel.push_back(Point(1921.500000,1333.641705,1341.289180));
fOuterVessel.push_back(Point(1937.400000,1323.306684,1332.692633));
fOuterVessel.push_back(Point(1945.800000,1317.346276,1326.519500));
fOuterVessel.push_back(Point(1955.700000,1309.845218,1317.891981));
fOuterVessel.push_back(Point(1967.400000,1295.970134,1306.551338));
fOuterVessel.push_back(Point(1978.800000,1280.313610,1294.239454));
fOuterVessel.push_back(Point(1989.000000,1265.005939,1277.903980));
fOuterVessel.push_back(Point(1992.900000,1257.300333,1271.514431));
fOuterVessel.push_back(Point(1997.100000,1248.305694,1264.163680));
fOuterVessel.push_back(Point(2005.200000,1231.294158,1248.454260));
fOuterVessel.push_back(Point(2012.400000,1218.089491,1234.170559));
fOuterVessel.push_back(Point(2022.300000,1199.613326,1212.122514));
fOuterVessel.push_back(Point(2041.500000,1159.695217,1175.959071));
fOuterVessel.push_back(Point(2067.300000,1103.768377,1119.907714));
fOuterVessel.push_back(Point(2076.900000,1081.319931,1098.604899));
fOuterVessel.push_back(Point(2096.700000,1032.093322,1050.464885));
fOuterVessel.push_back(Point(2109.900000,998.485519,1016.084595));
fOuterVessel.push_back(Point(2117.700000,978.246968,996.090698));
fOuterVessel.push_back(Point(2139.300000,923.005908,942.080372));
fOuterVessel.push_back(Point(2165.100000,846.349335,871.056021));
fOuterVessel.push_back(Point(2172.600000,824.783007,848.697406));
fOuterVessel.push_back(Point(2176.200000,813.957412,837.234610));
fOuterVessel.push_back(Point(2185.500000,783.577906,807.622388));
fOuterVessel.push_back(Point(2198.100000,739.716340,765.738419));
fOuterVessel.push_back(Point(2205.000000,715.511168,742.129894));
fOuterVessel.push_back(Point(2214.600000,681.298543,707.611781));
fOuterVessel.push_back(Point(2220.900000,658.583524,683.194489));
fOuterVessel.push_back(Point(2226.300000,634.718055,661.895682));
fOuterVessel.push_back(Point(2233.200000,603.811001,632.700117));
fOuterVessel.push_back(Point(2238.600000,576.552222,609.558919));
fOuterVessel.push_back(Point(2250.300000,518.771769,556.292737));
fOuterVessel.push_back(Point(2253.900000,501.356696,539.540265));
fOuterVessel.push_back(Point(2256.300000,489.527842,527.387239));
fOuterVessel.push_back(Point(2258.700000,475.292134,514.819926));
fOuterVessel.push_back(Point(2268.000000,419.860831,462.375738));
fOuterVessel.push_back(Point(2274.900000,370.274438,423.260594));
fOuterVessel.push_back(Point(2276.100000,361.627365,415.170620));
fOuterVessel.push_back(Point(2280.600000,328.869149,382.185234));
fOuterVessel.push_back(Point(2283.000000,310.178410,364.593028));
fOuterVessel.push_back(Point(2287.500000,262.842055,331.499446));
fOuterVessel.push_back(Point(2292.300000,212.112646,285.928236));
fOuterVessel.push_back(Point(2292.900000,204.496676,278.161865));
fOuterVessel.push_back(Point(2295.300000,160.795652,245.713432));
fOuterVessel.push_back(Point(2298.000000,111.632001,208.031950));
fOuterVessel.push_back(Point(2300.100000,67.916240,178.724131));
fOuterVessel.push_back(Point(2300.700000,46.437392,170.350468));
fOuterVessel.push_back(Point(2301.900000,1.132619,153.603143));
fOuterVessel.push_back(Point(2302.500000,0.000000,145.229480));
fOuterVessel.push_back(Point(2303.700000,0.000000,128.256110));
fOuterVessel.push_back(Point(2305.500000,0.000000,89.572422));
fOuterVessel.push_back(Point(2306.100000,0.000000,67.573959));
fOuterVessel.push_back(Point(2307.300000,0.000000,9.653423));
