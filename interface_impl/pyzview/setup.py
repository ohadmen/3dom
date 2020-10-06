from setuptools import setup, find_packages, Extension

module = Extension("zview_module",
                       sources=["pyzview/zview_inf.cpp"],
                       libraries = ["zview_inf"],   
                       
              )

with open("README.md", "r") as fh:
    long_description = fh.read()
setup(
     name='pyzview',  
     version='0.2',
     author="Ohad Menashe",
     author_email="ohad.men@gmail.com",
     description="zview python inferface",
     long_description=long_description,
     long_description_content_type="zview - a general pcl/mesh viewer",
     url="https://github.com/ohadmen/zview/tree/master/interface_impl/pyzview",
     packages=find_packages(),
     ext_modules=[module],
     
     classifiers=[
         "Programming Language :: Python :: 3",
         "License :: OSI Approved :: MIT License",
         "Operating System :: OS Independent",
     ],
 )