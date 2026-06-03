# 6.1 Setting up running environment for Gaussian in Windows

## Quick navigation

- path: 6 Appendix > 6.1 Setting up running environment for Gaussian in Windows
- pdf pages: 1132-1132
- category: appendix
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file for technical appendices, implementation notes, Gaussian environment setup, and low-level details.

## Keywords and aliases

6.1, Setting, up, running, environment, Gaussian, Windows

## Extracted manual text

6 Appendix

6 Appendix
6.1 Setting up running environment for Gaussian in
Windows
Some functions of Multiwfn can directly invoke Gaussian (the precondition is that "gaupath"
in settings.ini has been set to actual path of Gaussian executable file). In order to make Windows
version of Gaussian run normally in this case, you must define "GAUSS_EXEDIR" environment
variable, otherwise error “No executable for file l1.exe” will occur and Gaussian running will fail,
because Gaussian does not know where to find the l1.exe executable file. Below are steps for setting
up this environment variable.
(1) For Windows XP users: Enter “Control panel”-“System properties”-“Advanced”
(2) For Windows 7 users: Enter “Control panel”-“System”-“Advanced system setting”“Advanced”
(3) For Windows 10 users: Click right mouse button on the start button, enter “Control panel”“System”-“Advanced system setting”-“Advanced”
After that, click “Environment variables” button, then click “New” button (in “User variables”
frame), input GAUSS_EXEDIR as variable name, input installation directory of Gaussian as variable
value (e.g. D:\study\g09w\, assuming that g09.exe is in this folder).
In addition, it is very important to note that when Multiwfn invoke Gaussian in Windows
environment, Gaussian will search for Default.Rou in current folder rather than in Gaussian scratch
path. Therefore, if there are important settings in Default.Rou, such as the default number of cores
to be utilized, you should copy this file to current folder to make the settings take effect during
calculations.

6.2 The routines for evaluating real space functions
Below are the routines in function.f90 file. You can make use of them to construct new real
space function yourself. For more details, please check the comments in the code of corresponding
routine.
Calculate real space functions
function calcfuncall: A wrapper for calculating any supported real space function at a given
point
function userfunc: User-defined real space function
function linintp3d: Function value obtained by trilinear interpolation from grid data in memory
function splineintp3D: Function value obtained by cubic B-spline interpolation from grid data
in memory
function fmo: Orbital wavefunction value

1111
