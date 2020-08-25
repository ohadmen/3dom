#%%
import build.pyzview as m
import numpy as np
print("1")
zv=m.interface()

print("2")
# zv.loadFile("../../models/horse.stl")

pts = np.random.randint(-100,100,size=[100,4]).astype(np.float32)/100
zv.addColoredPoints("pytestC",pts)
print("3")
