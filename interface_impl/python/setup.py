from setuptools import setup, find_packages, Extension

extensions = [Extension("pyzview.zview",
                       ["pyzview.cpp"],
                       include_dirs=["../../zview/interface"],
                       # runtime_library_dirs = ["/home/ohad/dev/zview/bin/"],
                       # libraries = ["zview_inf"],   
                       extra_objects = ["/home/ohad/dev/zview/bin/libzview_inf.so"],
              ),
]
with open("README.md", "r") as fh:
    long_description = fh.read()
setup(
     name='pyzview',  
     version='0.1',
     author="Ohad Menashe",
     author_email="ohad@menashe.co",
     description="zview python inferface",
     long_description=long_description,
   long_description_content_type="text/markdown",
     url="https://github.com/ohadmen/zview",
     packages=find_packages(),
     ext_modules=extensions,
     
     classifiers=[
         "Programming Language :: Python :: 3",
         "License :: OSI Approved :: MIT License",
         "Operating System :: OS Independent",
     ],
 )