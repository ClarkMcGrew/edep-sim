
////////////////////////////////////////////////////////////////////////
// This is an auto-generated built using makePolycone.py.  It defines
// points to be used to build a G4Polycone. Prior to including this file,
// the vector fInnerVessel must be define in a way equivalent to
// 
// class Point {
//  public:
//      Point(double z, double i, double o): fZ(z), fInner(i), fOuter(o) {}
//      double fZ;
//      double fInner;
//      double fOuter;
//  };
//
// std::vector<Point> fInnerVessel;
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

fOuterVessel.push_back(Point(40.100000,1353.940020,1356.711193));
fOuterVessel.push_back(Point(40.300000,1354.966918,1361.454328));
fOuterVessel.push_back(Point(1849.300000,1354.673102,1360.529471));
fOuterVessel.push_back(Point(1858.000000,1353.826312,1359.758051));
fOuterVessel.push_back(Point(1871.200000,1351.999249,1357.740141));
fOuterVessel.push_back(Point(1882.400000,1349.134195,1355.869325));
fOuterVessel.push_back(Point(1900.500000,1343.486791,1351.077430));
fOuterVessel.push_back(Point(1921.600000,1333.576705,1341.239205));
fOuterVessel.push_back(Point(1937.400000,1323.306684,1332.692633));
fOuterVessel.push_back(Point(1945.900000,1317.273762,1326.432353));
fOuterVessel.push_back(Point(1956.000000,1309.571478,1317.630541));
fOuterVessel.push_back(Point(1967.700000,1295.607113,1306.234589));
fOuterVessel.push_back(Point(1978.900000,1280.173948,1294.095083));
fOuterVessel.push_back(Point(1989.000000,1265.005939,1277.903980));
fOuterVessel.push_back(Point(1992.900000,1257.300333,1271.514431));
fOuterVessel.push_back(Point(1997.100000,1248.305694,1264.163680));
fOuterVessel.push_back(Point(2005.200000,1231.294158,1248.454260));
fOuterVessel.push_back(Point(2012.400000,1218.089491,1234.170559));
fOuterVessel.push_back(Point(2022.400000,1199.408317,1211.901229));
fOuterVessel.push_back(Point(2041.600000,1159.480247,1175.743674));
fOuterVessel.push_back(Point(2067.500000,1103.328479,1119.467689));
fOuterVessel.push_back(Point(2077.500000,1079.834647,1097.235956));
fOuterVessel.push_back(Point(2096.900000,1031.590204,1049.945008));
fOuterVessel.push_back(Point(2109.900000,998.485519,1016.084595));
fOuterVessel.push_back(Point(2117.500000,978.758459,996.588125));
fOuterVessel.push_back(Point(2139.400000,922.750162,941.811781));
fOuterVessel.push_back(Point(2165.100000,846.349335,871.056021));
fOuterVessel.push_back(Point(2172.700000,824.495456,848.378995));
fOuterVessel.push_back(Point(2176.200000,813.957412,837.234610));
fOuterVessel.push_back(Point(2185.700000,782.878949,806.985566));
fOuterVessel.push_back(Point(2200.500000,731.471305,757.675672));
fOuterVessel.push_back(Point(2206.600000,709.809063,736.423810));
fOuterVessel.push_back(Point(2214.800000,680.585780,706.836629));
fOuterVessel.push_back(Point(2221.100000,657.794192,682.419337));
fOuterVessel.push_back(Point(2226.300000,634.718055,661.895682));
fOuterVessel.push_back(Point(2233.200000,603.811001,632.700117));
fOuterVessel.push_back(Point(2238.800000,575.542637,608.648386));
fOuterVessel.push_back(Point(2250.800000,516.353009,554.016404));
fOuterVessel.push_back(Point(2253.900000,501.356696,539.540265));
fOuterVessel.push_back(Point(2256.400000,488.934688,526.880863));
fOuterVessel.push_back(Point(2258.700000,475.292134,514.819926));
fOuterVessel.push_back(Point(2268.100000,419.236155,461.811822));
fOuterVessel.push_back(Point(2275.000000,369.553849,422.634574));
fOuterVessel.push_back(Point(2276.200000,360.906775,414.437612));
fOuterVessel.push_back(Point(2280.600000,328.869149,382.185234));
fOuterVessel.push_back(Point(2283.100000,309.344012,363.860020));
fOuterVessel.push_back(Point(2287.500000,262.842055,331.499446));
fOuterVessel.push_back(Point(2292.400000,211.055784,284.861668));
fOuterVessel.push_back(Point(2292.900000,204.496676,278.161865));
fOuterVessel.push_back(Point(2295.500000,157.153901,242.922211));
fOuterVessel.push_back(Point(2298.200000,107.795622,205.240729));
fOuterVessel.push_back(Point(2300.100000,67.916240,178.724131));
fOuterVessel.push_back(Point(2300.300000,61.538983,175.932910));
fOuterVessel.push_back(Point(2301.900000,1.132619,153.603143));
fOuterVessel.push_back(Point(2302.100000,0.000000,150.811922));
fOuterVessel.push_back(Point(2303.700000,0.000000,128.256110));
fOuterVessel.push_back(Point(2305.700000,0.000000,85.274234));
fOuterVessel.push_back(Point(2305.900000,0.000000,77.227382));
fOuterVessel.push_back(Point(2307.500000,0.000000,0.000000));
