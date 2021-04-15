import gegede.builder
from gegede import Quantity as Q

class Builder(gegede.builder.Builder):
    '''
    Build a single layer of the detector.
       material -- The material used to build the detector enclosure.
       dx, dy, dz -- The half size of the layer box.
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

        shape = geom.shapes.Box(self.name + "_shape",
                                self.dx, self.dy, self.dz)
        volume = geom.structure.Volume(self.name + "_LV",
                                       material = self.material,
                                       shape = shape)

        ## Add the aux values
        for n, v in self.otherKeywords.items():
            volume.params.append((n,v))

        self.add_volume(volume)

        pass
