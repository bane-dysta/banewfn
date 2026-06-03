# 2.3 Files of Multiwfn

## Quick navigation

- path: 2  General information > 2.3 Files of Multiwfn
- pdf pages: 34-34
- category: general
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file for setup, prerequisites, file formats, general usage rules, or broad conceptual orientation.

## Keywords and aliases

2.3, Multiwfn

## Extracted manual text

2 General information

2.3 Files of Multiwfn
You will find following files after uncompressing Multiwfn package, only the bolded files are
indispensable for running Multiwfn:
• Multiwfn.exe (Windows) or Multiwfn (Linux/Mac OS) : The executable file of Multiwfn.
• libiomp5md.dll (Windows) : Intel OpenMP Runtime library.
• settings.ini : All detail parameters for running Multiwfn are recorded here, most of them do
not need to be frequently modified. When booting up, Multiwfn will try to find and use this file in
current folder, if it is not presented in current folder, the file in the path defined by "Multiwfnpath"
environment variable will be used; if the file still cannot be found, default settings will be used
instead. If you run Multiwfn via command line, you can also directly specify position of this file via
"-set" argument, for example: Multiwfn test.wfn -set /sob/3.7/settings.ini.
The meanings of all parameters in settings.ini are not documented in this manual systematically,
since they have already been commented in detail, only those important will be mentioned in this
manual. I suggest you read through the settings.ini and find out the ones useful for you.
• “examples” folder : Some useful files, scripts and the files involved in the examples of
Chapter 4.
• LICENSE.txt : The terms that all users must follow.
• Multiwfn quick start.pdf : A short document lets new user immediately understand how to
use Multiwfn to carry out very common tasks.
• How to cite Multiwfn.pdf : Please properly cite Multiwfn according to this document.

2.4 Parallel implementation
Most time-consuming codes of Multiwfn have been parallelized by OpenMP technique. If your
CPU has multiple cores, you can greatly benefit from parallelization. To enable parallelization, just
modify “nthreads” parameter in settings.ini to a proper number. For example, your computer have
a CPU with 12-cores physical cores, then commonly you should change “nthreads” to 12.
If Multiwfn crashes during parallel calculation for a very large system, try to enlarge
“ompstacksize” in settings.ini (for Windows version) or enlarge the value of the environment
variable OMP_STACKSIZE (for Linux or Mac OS version).

2.5 Input files and wavefunction types
Wavefunction types supported by Multiwfn include restricted/unrestricted single-determinant
wavefunction, restricted open-shell wavefunction and post-HF wavefunction (in natural orbital
formalism).
Cartesian or spherical harmonic Gaussian functions with angular moment up to h are supported.
There is no upper limit of the number of atoms / basis functions / GTFs / orbitals in Multiwfn,
the actual upper limit is only decided by available memory in your computer.

13
