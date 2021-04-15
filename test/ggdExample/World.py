import gegede.builder
from gegede import Quantity as Q

from . import MaterialDefinitions

class Builder(gegede.builder.Builder):
    '''Build a simple box world of given material and size.  The world
    will contain one daughter volume which is build using the first
    subbuilder.

    Configuration parameters:
       subbuilders -- A list of builders for the world.  Only the first 
             is used.  
       material -- The material used to build the world volume.
       dx, dy, dz -- The half size of the world box.

       Remaining configuration parameters are added to the logical
       volume as auxtype and auxvalue pairs.

    '''

    def configure(self,
                  material = "Air",
                  dx = Q("10m"),
                  dy = Q("10m"),
                  dz = Q("10m"),
                  **kwds):

        print("Configuring the world " + self.name)
        self.material = material
        self.dx = dx
        self.dy = dy
        self.dz = dz
        self.otherKeywords = kwds
        pass

    def construct(self, geom):
        print("Constructing the world " + self.name)
        MaterialDefinitions.DefineMaterials(geom)

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
        self.add_volume(volume)

        # Add the volume from the first subbuilder. 
        subBuilder = self.get_builder()
        subVolume = subBuilder.get_volume()
        pos = geom.structure.Position(subBuilder.name + "_pos",
                                      "0m","0m","0m")
        rot = geom.structure.Rotation(subBuilder.name + "_rot",
                                      "0deg","0deg","0deg")
        place = geom.structure.Placement(subBuilder.name + "_place",
                                         volume=subVolume.name,
                                         pos=pos.name,
                                         rot=rot.name)
        volume.placements.append(place.name)                

        print ("Add " + subBuilder.name)
        print ("   volume " + subVolume.name)
        print ("   placement " + place.name)
        print ("   position " + pos.name)
        print ("   rotation " + rot.name)
        
        ## Add the aux type and aux values fields to the logical volume.
        for n, v in self.otherKeywords.items():
            volume.params.append((n,v))
            pass

        pass

