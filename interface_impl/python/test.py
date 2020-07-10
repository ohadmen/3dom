#%%
import build.zview_bindings as m
print("1")
zv=m.interface()
print("2")
zv.loadFile("../../../models/horse.stl")
# zv.addPoints("pytest",10,[float(x) for x in range(30)])
print("3")
# %%
