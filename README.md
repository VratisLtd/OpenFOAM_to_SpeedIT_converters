OpenFOAM_to_SpeedIT_converters
==============================

# Introduction

SpeedIT FLOW™ is a general-purpose finite volume-based Computational Fluid 
Dynamics (CFD) flow solver that fully runs on GPU. It accelerates pressure-
velocity solution procedure for Navier-Stokes equation, namely SIMPLE and PISO. 
SpeedIT FLOW is easy to use by CFD engineers with all levels of expertise. It 
can be invoked easily as a standalone application. 

You can get more information on our [website](www.vratis.com)

This software is an OpenFOAM to SpeedIT FLOW converter. It allows for a 
conversion of  ready OpenFOAM test cases to SpeedIT FLOW format. 

OpenFOAM to SpeedIT FLOW converters can be downloaded from the [GitHub]
(https://github.com/VratisLtd/OpenFOAM\_to\_SpeedIT_converters)

# Requirements

* OpenFOAM ver. 1.7.1 - 2.0.1

# Installation

Set up the OpenFOAM environment by sourcing one of the configuration files. 
Usualy $WM\_PROJECT\_DIR/etc/bashrc. For more information please check 
[OpenFOAM installation website](http://www.openfoam.org/download/source.php), 
section Setting Environment Variables.

To compile converters it is enough to navigate to each converter directory and 
execute _wmake_. For example for the icoFoam\_to\_speedit:

$ cd /path\_to\_converters_directory/icoFoam\_to\_speedit

$ wmake

Converters will be copied to $FOAM\_USER\_APPBIN directory. 

# Usage

To launch the converter it is enough to execute icoFoam\_to\_speedit command 
inside the case directory or:

$ icoFoam\_to\_speedit -case /path\_to\_case\_directory/

As a result a _sitf_ directory will be created. In this directory all the data 
needed for the launch of the SpeedIT FLOW solvers is kept.

# List of converters

**OpenFOAM solver -> converter**

* icoFoam -> icoFoam\_to\_speedit
* pisoFoam -> pisoFoam\_to\_speedit
* simpleFoam with laminar turbulence model -> simpleFoam\_to\_speedit
* simpleFoam with kOmegaSST turbulence model -> simpleFoamTurb\_to\_speedit

# Contact

For technical questions please contact us at <support@vratis.com>.

For general inquires please contact us at <info@vratis.com>.
