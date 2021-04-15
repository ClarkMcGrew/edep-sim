import sys
import gegede.builder
from gegede import Quantity as Q

class Builder(gegede.builder.Builder):
    '''
    Build the detector enclosure with several layers.
       material -- The material used to build the detector enclosure.
       subbuilders -- A list of builders for the layers of the enclosures.
       repetitions -- The number of repetions of the stack of layers.
       dx, dy, dz -- The half size of the world box.

       Remaining configuration parameters are added to the logical
       volume as auxtype and auxvalue pairs.
    '''

    def configure(self,
                  material = "Air",
                  dx = Q("1m"),
                  dy = Q("1m"),
                  dz = Q("1m"),
                  repetitions = 1,
                  **kwds):
        print("Configuring the detector enclosure " + self.name)
        self.material = material
        self.dx = dx
        self.dy = dy
        self.dz = dz
        self.repetitions = repetitions
        self.otherKeywords = kwds
        pass

    def construct(self, geom):
        print("Constructing the detector enclosure " + self.name)

        ## Build the volume for this layer.  The conventions are that
        ## the shape name ends in "_shape", and the volume name ends
        ## in "_LV".  This reflects what is going to be build in
        ## GEANT4 (a G4Shape and a G4LogicalVolume).
        ##
        ## The shape and volume names need to be unique inside the
        ## GDML file.
        shape = geom.shapes.Box(self.name + "_shape",
                                self.dx, self.dy, self.dz)
        volume = geom.structure.Volume(self.name + "_LV",
                                       material = self.material,
                                       shape = shape)

        ## Add the constructed volume to the builder.  This can be
        ## added before the volume is fully constructed.
        self.add_volume(volume)

        ####################################################
        ## Add a stack of layers to the parent volume.
        ####################################################

        builders = self.get_builders()

        ## Determine the full size of the stack of layers
        halfX = Q("0m")
        halfY = Q("0m")
        halfZ = Q("0m")
        for rep in range(self.repetitions):
            for builder in builders:
                halfX = max(halfX,builder.dx)
                halfY = max(halfX,builder.dx)
                halfZ += builder.dz

        ## Make sure the parent volume is big enough.
        if self.dx < halfX:
            print ("Invalid geometry")
            sys.exit(1)
        if self.dy < halfY:
            print ("Invalid geometry")
            sys.exit(1)
        if self.dz < halfZ:
            print ("Invalid geometry")
            sys.exit(1)

        ## Place the sub volumes in the parent volume.  The
        ## coordinates are relative to the parent volume.
        centerX = Q("0m")
        centerY = Q("0m")
        centerZ = - halfZ
        for rep in range(self.repetitions):
            for builder in builders:
                centerZ += builder.dz
                vol = builder.get_volume()
                pos = geom.structure.Position(builder.name+"_pos"+str(rep),
                                              centerX, centerY, centerZ)
                rot = geom.structure.Rotation(builder.name+"_rot"+str(rep),
                                              "0deg","0deg","0deg")
                place = geom.structure.Placement(builder.name+"_place"+str(rep),
                                                 volume=vol.name,
                                                 pos=pos.name,
                                                 rot=rot.name)
                volume.placements.append(place.name)
                centerZ += builder.dz
                pass
            pass
        
        ## Add the aux type and aux values fields to the logical volume.
        for n, v in self.otherKeywords.items():
            volume.params.append((n,v))
            pass

        pass
