import math
import numpy as np

lat = 19.614484*math.pi/180
lon = (180+110.950745)*math.pi/180

x = 6400*math.cos(lat)*math.cos(lon)
y = 6400*math.cos(lat)*math.sin(lon)
z = 6400*math.sin(lat)
print(x,y,z)

sky = np.array([x,y,z])
sky = sky/np.linalg.norm(sky)
north = np.array([0,0,1])
north = north/np.linalg.norm(north)
east = np.cross(north,sky)
east = east/np.linalg.norm(east)
north = np.cross(sky,east)
north = north/np.linalg.norm(north)
print(east, north, sky)

rr = east*0.01      #10m
ff = north*0.01     #10m
tt = sky*0.12       #120m
print(rr, ff, tt)
