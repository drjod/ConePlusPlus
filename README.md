![coneplusplus](https://cloud.githubusercontent.com/assets/12182426/9603318/39a1f5ba-50b0-11e5-9af7-626ec9d8546d.jpg)

# ConePlusPlus

was used in the C++ lecture at CAU Kiel in 2015. It calculates drawdown around wells on a rectangular raster domain:

![domain](https://cloud.githubusercontent.com/assets/12182426/8676057/7f14d7e6-2a47-11e5-9e47-375a3e9b16bc.png)

The wells do not affect each other (superposition principle).

##Solver

Drawdown *s* is calculated according to Theis (1935):

![form1a](https://cloud.githubusercontent.com/assets/12182426/8675107/51d68d4c-2a42-11e5-9da6-37cf46ef6b84.jpg)

The well function *W(u)* reads

![form2b](https://cloud.githubusercontent.com/assets/12182426/8675113/569c83ea-2a42-11e5-94c6-f1c311be3e57.jpg)

The series is truncated as soon as *W(u) < 10e-5* (hardcoded). The series is considered to represent the Well function for *u<1* (maybe up to 4). *u>1* leads to a warning on the console.

##Visualization

The simulator produces the drawdown on a xy-raster. This is read by the Cone++Viewer (Qwt-Qt widget for contour lines)

![coneplusplus](https://cloud.githubusercontent.com/assets/12182426/8657360/778a6c5c-299d-11e5-9790-ac13733ce889.PNG)


##Implementation

Classes in the branch "lecture":

![uml](https://cloud.githubusercontent.com/assets/12182426/8676062/84bddb48-2a47-11e5-8f60-1c84a44ff5c0.png)

Global variable int verbosity allows selection between four console output states [0,1,2,3].
