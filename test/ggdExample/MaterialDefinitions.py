#########################################################################
# This is a good place to define materials used through-out the geometry.
#
# This is based on Tyler https://github.com/tyleralion/duneggd, but
# it's gone through quite a few changes.  This can provide a base that
# is copied into other gegede geometries, but don't try to use it as a
# reference.
# ^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^

#########################################################################
#
# Table of element definitions: Elements are used to define materials.
# Since an element doesn't direction define a density, it can't be
# used as a material.  The convention being used is that for a
# material like nitrogen, the element is "Elem_nitrogen", and the material
# is "Nitrogen".
#
#########################################################################
def DefineElements(geom):
    '''A nonexhaustive list of element definitions.  This can be called
    multiple times.  It is called within the MaterialDefinitions sub-module,
    but can also be called in user code.  Some elements with important
    isotopic mixtures are defined as Compositions with component Isotopes.
    '''

    # Only call once.
    try:
        DefineElements.Called += 1
        print ("DefineElements was called again.")
        return
    except AttributeError: DefineElements.Called = 0
    
    print ("Define the elements")

    ## All of the elements, isotopes and compositions are assigned to
    ## an unused variables.  The local variables are not used and are
    ## here for readability.
    h    = geom.matter.Element("Elem_hydrogen",   "H",  1,  "1.00791*g/mole" )
    b10  = geom.matter.Isotope("Iso_boron10", 5, 10, "10.01*g/mole")
    b11  = geom.matter.Isotope("Iso_boron11", 5, 11, "11.00*g/mole")
    b    = geom.matter.Composition("Elem_boron",
                                   isotopes=(("Iso_boron10",0.2),
                                             ("Iso_boron11",0.8)))
    c    = geom.matter.Element("Elem_carbon",     "C",  6,  "12.0107*g/mole")
    n    = geom.matter.Element("Elem_nitrogen",   "N",  7,  "14.0671*g/mole")
    o    = geom.matter.Element("Elem_oxygen",     "O",  8,  "15.999*g/mole" )
    f    = geom.matter.Element("Elem_fluorine",   "F",  9,  "18.9984*g/mole")
    na   = geom.matter.Element("Elem_sodium",     "Na", 11, "22.99*g/mole"  )
    mg   = geom.matter.Element("Elem_magnesium",  "Mg", 12, "24.305*g/mole" )
    al   = geom.matter.Element("Elem_aluminum",   "Al", 13, "26.9815*g/mole")
    si   = geom.matter.Element("Elem_silicon",    "Si", 14, "28.0855*g/mole")
    p    = geom.matter.Element("Elem_phosphorus", "P",  15, "30.973*g/mole" )
    s    = geom.matter.Element("Elem_sulfur",     "S",  16, "32.065*g/mole" )
    ar   = geom.matter.Element("Elem_argon",      "Ar", 18, "39.948*g/mole" )
    ar   = geom.matter.Element("Elem_potassium",  "K",  19, "39.0983*g/mole")
    ca   = geom.matter.Element("Elem_calcium",    "Ca", 20, "40.078*g/mole" )
    ti   = geom.matter.Element("Elem_titanium",   "Ti", 22, "47.867*g/mole" )
    cr   = geom.matter.Element("Elem_chromium",   "Cr", 24, "51.9961*g/mole")
    mn   = geom.matter.Element("Elem_manganese",  "Mn", 25, "54.9380*g/mole")
    fe   = geom.matter.Element("Elem_iron",       "Fe", 26, "55.8450*g/mole")
    ni   = geom.matter.Element("Elem_nickel",     "Ni", 28, "58.6934*g/mole")
    cu63 = geom.matter.Isotope("Iso_copper63", 29, 63, "62.93*g/mole")
    cu63 = geom.matter.Isotope("Iso_copper65", 29, 65, "64.93*g/mole")
    cu   = geom.matter.Composition("Elem_copper",
                                   isotopes=(("Iso_copper63",0.6917),
                                             ("Iso_copper65",0.3083)))
    zn64 = geom.matter.Isotope("Iso_zinc64", 30, 64, "63.93*g/mole")
    zn66 = geom.matter.Isotope("Iso_zinc66", 30, 66, "65.93*g/mole")
    zn67 = geom.matter.Isotope("Iso_zinc67", 30, 67, "66.93*g/mole")
    zn68 = geom.matter.Isotope("Iso_zinc68", 30, 68, "67.93*g/mole")
    zn70 = geom.matter.Isotope("Iso_zinc70", 30, 70, "69.93*g/mole")
    zn   = geom.matter.Composition("Elem_zinc",
                                   isotopes=(("Iso_zinc64",0.492),
                                             ("Iso_zinc66",0.277),
                                             ("Iso_zinc67",0.04),
                                             ("Iso_zinc68",0.185),
                                             ("Iso_zinc70",0.006)))
    br   = geom.matter.Element("Elem_bromine",    "Br", 35, "79.904*g/mole" )
    sb   = geom.matter.Element("Elem_antimony",   "Sb", 51, "121.76*g/mole" )
    xe   = geom.matter.Element("Elem_xenon",      "Xe", 54, "131.293*g/mole")
    w    = geom.matter.Element("Elem_tungsten",    "W", 74, "183.84*g/mole")
    au   = geom.matter.Element("Elem_gold",       "Au", 79, "196.966*g/mole")
    pb   = geom.matter.Element("Elem_lead",       "Pb", 82, "207.20*g/mole" )


#########################################################################
#
# Table of standard materials: Materials are used in a geometry to
# build volumes.  A material is defined as molecules when the atomic
# number density is provided, and mixtures with the mass density is
# provided.
#
#########################################################################
def DefineMaterials(geom):
    '''A very nonexhaustive list of common materials.  This can be
    called multiple times.  The expectation is that this will be
    used by every builder, which will then defined specific materials
    that are not defined here.
    '''

    # Only call once.
    try:
        DefineMaterials.Called += 1
        print ("DefineMaterials was called again.")
        return
    except AttributeError: DefineMaterials.Called = 0
    
    DefineElements(geom)

    print ("Define the materials")
    
    ## Standard Molecules.  For the purpose of this code, a molecule
    ## is any material that is defined by the number of each component
    ## (e.g. Water ,which is two hydrogens, and one oxygen).
    H2O   = geom.matter.Molecule("Water", density="1.0*kg/l",
                              elements=(("Elem_oxygen",1),
                                        ("Elem_hydrogen",2)))
    CO2   = geom.matter.Molecule("CO2", density="1.562*g/cc",
                              elements=(("Elem_carbon",1),
                                        ("Elem_oxygen",2)))
    SiO2  = geom.matter.Molecule("SiO2", density="2.2*g/cc",
                              elements=(("Elem_silicon",1),
                                        ("Elem_oxygen",2)))
    methane  = geom.matter.Molecule("Methane",
                                 density="0.000716*g/cc",
                                 elements=(
                                    ("Elem_carbon",1),
                                    ("Elem_hydrogen",4)))
    ethane  = geom.matter.Molecule("Ethane",
                                density="0.00134*g/cc",
                                elements=(("Elem_carbon",2),
                                          ("Elem_hydrogen",6)))
    propane  = geom.matter.Molecule("Propane",
                                 density="0.00197*g/cc",
                                 elements=(("Elem_carbon",3),
                                           ("Elem_hydrogen",8)))
    isobutane  = geom.matter.Molecule("Isobutane",
                                   density="0.00260*g/cc",
                                   elements=(("Elem_carbon",4),
                                             ("Elem_hydrogen",10)))
    Kapton   = geom.matter.Molecule("Kapton",   density="1.4*g/cc",
                                 elements=(("Elem_carbon",22),
                                           ("Elem_oxygen",5),
                                           ("Elem_nitrogen",2)))
    Tungsten = geom.matter.Molecule("Tungsten", density="19.3*g/cc",
                                 elements=(("Elem_tungsten",1),))
    Gold     = geom.matter.Molecule("Gold",     density="19.3*g/cc",
                                 elements=(("Elem_gold",1),))
    Iron     = geom.matter.Molecule("Iron",     density="7.874*g/cc",
                                 elements=(("Elem_iron",1),))
    Graphite = geom.matter.Molecule("Graphite", density="2.23*g/cc",
                                 elements=(("Elem_carbon",1),))
    Calcium  = geom.matter.Molecule("Calcium",  density="1.55*g/cc",
                                 elements=(("Elem_calcium",1),))

    # Molecules for Rock and fibrous_glass Mixtures
    FeO   = geom.matter.Molecule("FeO",   density="5.745*g/cc",
                              elements=(("Elem_iron",1),("Elem_oxygen",1)))
    Al2O3 = geom.matter.Molecule("Al2O3", density="3.97*g/cc",
                              elements=(("Elem_aluminum",2),("Elem_oxygen",3)))
    MgO   = geom.matter.Molecule("MgO",   density="3.58*g/cc",
                              elements=(("Elem_magnesium",1),("Elem_oxygen",1)))
    CaO   = geom.matter.Molecule("CaO",   density="3.35*g/cc",
                              elements=(("Elem_calcium",1),("Elem_oxygen",1)))
    Na2O  = geom.matter.Molecule("Na2O",  density="2.27*g/cc",
                              elements=(("Elem_sodium",2),("Elem_oxygen",1)))
    P2O5  = geom.matter.Molecule("P2O5",  density="1.562*g/cc",
                              elements=(("Elem_phosphorus",2),("Elem_oxygen",5)))
    TiO2  = geom.matter.Molecule("TiO2",  density="4.23*g/cc",
                              elements=(("Elem_titanium",1),("Elem_oxygen",2)))
    Fe2O3 = geom.matter.Molecule("Fe2O3", density="5.24*g/cc",
                              elements=(("Elem_iron",2),("Elem_oxygen",3)))

    ## Common mixtures.  For the purpose of this code, a mixture
    ## is any material that is defined by the mass fraction of each component
    ## (e.g. Water has 11.2% hydrogen and 88.8% oxygen).
    rock  = geom.matter.Mixture("Rock",
                             density = "2.82*g/cc",
                             components = (
                                 ("SiO2",   0.5267),
                                 ("FeO",    0.1174),
                                 ("Al2O3",  0.1025),
                                 ("Elem_oxygen", 0.0771),
                                 ("MgO",    0.0473),
                                 ("CO2",    0.0422),
                                 ("CaO",    0.0382),
                                 ("Elem_carbon", 0.0240),
                                 ("Elem_sulfur", 0.0186),
                                 ("Na2O",   0.0053),
                                 ("P2O5",   0.0007),
                             ))

    dirt  = geom.matter.Mixture("Dirt", density = "1.7*g/cc",
                             components = (
                                 ("Elem_oxygen",    0.438),
                                 ("Elem_silicon",   0.257),
                                 ("Elem_sodium",    0.222),
                                 ("Elem_aluminum",  0.049),
                                 ("Elem_iron",      0.019),
                                 ("Elem_potassium", 0.015),
                             ))

    air   = geom.matter.Mixture("Air",
                             density = "0.001225*g/cc",
                             components = (
                                 ("Elem_nitrogen", 0.781154),
                                 ("Elem_oxygen",   0.209476),
                                 ("Elem_argon",    0.00934)
                             ))

    steel    = geom.matter.Mixture("Steel",
                                density = "7.9300*g/cc",
                                components = (
                                    ("Elem_iron",     0.7298),
                                    ("Elem_chromium", 0.1792),
                                    ("Elem_nickel",   0.0900),
                                    ("Elem_carbon",   0.0010)
                                ))

    carbonSteel = geom.matter.Mixture("CarbonSteel", density="7.9*g/cc",
                                   components = (
                                       ("Elem_carbon",0.0030),
                                       ("Elem_copper",0.0025),
                                       ("Elem_iron",0.98),
                                       ("Elem_manganese",0.0103),
                                       ("Elem_phosphorus",0.00090),
                                       ("Elem_silicon",0.00280),
                                       ("Elem_sulfur",0.00050)
                                   ))

    SS304 = geom.matter.Mixture("SS304",
                             density="7.9*g/cc",
                             components = (("Elem_carbon",0.0008),
                                           ("Elem_manganese",0.02),
                                           ("Elem_phosphorus",0.00045),
                                           ("Elem_sulfur",0.0003),
                                           ("Elem_silicon",0.0075),
                                           ("Elem_chromium",0.18),
                                           ("Elem_nickel",0.08),
                                           ("Elem_nitrogen",0.0010),
                                           ("Elem_iron",0.70995)))

    concrete = geom.matter.Mixture("Concrete", density="2.5*g/cc",
                                components = (
                                    ("CaO",0.6661),
                                    ("SiO2",0.2345),
                                    ("Al2O3",0.0445),
                                    ("Fe2O3",0.0307),
                                    ("MgO",0.0242)
                                ))

    epoxy = geom.matter.Molecule("Epoxy", density="1.25*g/cc",
                            elements = (
                                    ("Elem_carbon",6),
                                    ("Elem_hydrogen",6),
                                    ("Elem_oxygen",1)
                            ))

    glass = geom.matter.Mixture("Glass", density="2.70*g/cc",
                             components = (
                                 ("Elem_silicon",0.2743),
                                 ("Elem_boron",0.0166),
                                 ("Elem_aluminum",0.0207),
                                 ("Elem_sodium",0.0449),
                                 ("Elem_potassium",0.0821),
                                 ("Elem_zinc",0.0882),
                                 ("Elem_titanium",0.0292),
                                 ("Elem_oxygen",0.4440)
                             ))

    fr4 = geom.matter.Mixture("FR4", density="1.850*g/cc",
                           components = (
                               ("Epoxy",0.206),
                               ("Glass",0.794)
                           ))

    try:
        # Try to use a new feature
        scintillator  = geom.matter.Mixture("Scintillator",
                                            density="1.05*g/cc",
                                            components = (
                                                ("Elem_carbon",   0.916),
                                                ("Elem_hydrogen", 0.084)
                                            ),
                                            properties = (
                                                ("BIRKSCONSTANT", (0.126,)),
                                            ))
    except:
        # Fall back if the new feature doesn't exist.
        print ("ERROR: #######################################")
        print ("ERROR: GGD material properties not implemented")
        print ("ERROR: #######################################")
        scintillator  = geom.matter.Mixture("Scintillator",
                                            density="1.05*g/cc",
                                            components = (
                                                ("Elem_carbon",   0.916),
                                                ("Elem_hydrogen", 0.084)
                                            ))

    pureLead  = geom.matter.Mixture("PureLead",   density="11.342*g/cc",
                                    components=(("Elem_lead",1),))
    lead  = geom.matter.Mixture("Lead", density="11.342*g/cc",
                                components=(
                                    ("Elem_lead",0.95),
                                    ("Elem_antimony",0.05)
                                ))

    vaccuum =  geom.matter.Mixture("Vaccuum",
                                density="1.0E-25*g/cc",
                                components=(("Air",1.0),))
