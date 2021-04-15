import gegede.builder
from gegede import Quantity as Q

class Builder(gegede.builder.Builder):
    '''
    Build a single layer of the detector.
       material -- The material used to build the detector enclosure.
       dx, dy, dz -- The half size of the layer box.

       Remaining configuration parameters are added to the logical
       volume as <auxilliary auxtype="key" auxvalue="value" /> pairs.
    '''

    def configure(self,
                  material = "Air",
                  dx = Q("50cm"),
                  dy = Q("50cm"),
                  dz = Q("1cm"),
                  **kwds):
        print("Configuring a detector layer " + self.name)
        self.material = material
        self.dx = dx
        self.dy = dy
        self.dz = dz
        self.otherKeywords = kwds
        pass

    def construct(self, geom):
        print("Constructing a detector layer " + self.name)

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

        ## Add the constructed volume to the builder.    
        self.add_volume(volume)

        ## Add the aux type and aux values fields to the logical volume.
        for n, v in self.otherKeywords.items():
            volume.params.append((n,v))

        pass
