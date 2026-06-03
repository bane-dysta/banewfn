# front matter and mandatory notices

This file combines the preface pages that the manual explicitly asks all users to read first.

- pdf pages: 2-21
- purpose: essential usage notes, support channels, reading order, platform-specific caveats

## Included bookmark entries

- **!!!!!!!!!! ALL USERS MUST READ !!!!!!!!!!** (pdf pages 2-2)
- **Linux and Mac OS USERS MUST READ** (pdf pages 3-21)

## Extracted manual text

!!!!!!!!!! ALL USERS MUST READ !!!!!!!!!!
1. Please feel free to ask questions about using Multiwfn by posting topic on
Multiwfn English forum (http://sobereva.com/wfnbbs) or Multiwfn Chinese forum
(http://bbs.keinsci.com/wfn)! I am always willing to provide as much help as I can for
any Multiwfn user!!! \()/
2. To very quickly understand how to use Multiwfn to carry out very common
tasks, please check "Multiwfn quick start.pdf" in the Multiwfn binary package.
3. The BEST way to get started is reading through Chapter 1, Sections 2.1~2.5 of
Chapter 2, and then follow the tutorials in Chapter 4. After that if you want to learn
more about theoretical backgrounds and details of Multiwfn, then read Section 2.6~2.8
of Chapter 2 and Chapter 3. Note that the tutorials and examples given in Chapter 4
only cover most important and frequently used functions rather than all functions of
Multiwfn.
4. Different functions of Multiwfn require different type of input file, please
carefully read Section 2.5 for explanation.
5. Usually Multiwfn runs in interactive mode, however Multiwfn can also be run
in command-line mode, and it is very convenient to write shell script to make Multiwfn
deal with a large number of input files. See Sections 5.2 and 5.3 for detail.
6. If you do not know how to copy the output of Multiwfn from command-line
window to a plain text file, consult Section 5.4. If you do not know how to enlarge
screen buffer size of command-line window of Windows system, consult Section 5.5.
7. If error “No executable for file l1.exe” appears in screen when Multiwfn is
invoking Gaussian, you should set up Gaussian environment variable first. For
Windows version, you can refer Appendix 1. (Note: Most functions in Multiwfn DO
NOT require Gaussian installed on your local machine)
8. The so-called “current folder” in this manual and in prompts of Multiwfn refers
to the path where you are invoking Multiwfn. If you boot up Multiwfn by clicking the
icon of executable file in Windows platform, the “current folder” is the folder
containing Multiwfn executable file. In the case of command-line mode, if you are in
“D:\study\” directory when invoking Multiwfn, then “D:\study\” is “current directory”.
9. Please frequently check Multiwfn website and update the program to the latest
version. Multiwfn is always in active development, useful new functions are frequently
added, bugs are continuously fixed and efficiency is continuously improved.

i

--- page break ---

Linux and Mac OS USERS MUST READ
1. If you are new to Multiwfn, using Windows version is highly recommended,
since this version of Multiwfn is more stable than others, the appearance of GUI has
the best effect and you do not need to set up your system.
2. See Section 2.1.2 and 2.1.3 on how to install Linux and Mac OS versions of
Multiwfn, respectively.
3. Transparent style does not work when showing isosurfaces in GUI window. But
if you choose to save the graph as image file, the transparent isosurfaces can be rendered
normally in the resulting graphical file.
4. Using Mac OS version is deprecated, some functions may not work normally.

ii

--- page break ---

About the manual
This content in the manual is organized in the following sequence:
Chapter 1 Overview: Briefly describes what is Multiwfn and what Multiwfn can
do.
Chapter 2 General information: Introduces all aspects beyond specific functions
of Multiwfn, such as how to install, how to use, supported file types, etc.
Chapter 3 Functions: Describes all functions and related theories of Multiwfn in
detail.
Chapter 4 Tutorials and examples: Plenty of practical examples are provided for
helping users to use Multiwfn and understand value of various analysis methods. The
Section 4.A contains some advanced tutorials, important overviews and special topics.
Chapter 5 Skills: Some useful skills in using Multiwfn.
Appendix
In general, italics font is used for inputted commands, journal names and filenames
throughout this manual. Very important contents are highlighted by red color or bolded.
You can directly jump to specific section by choosing corresponding entry in
bookmark window of your pdf reader.
The purpose of this manual is not only helping users to use Multiwfn, but also
introducing related theories. I hope any quantum chemist can benefit from this manual.
Albeit I have tried to write this manual as readable as possible for beginners, some
topics request the readers have basic knowledge of quantum chemistry. If the readers
have carefully read the book "Quantum Chemistry 7ed" written by Ira. Levine, then
they will never encounter any difficulty during reading through the whole manual.

iii

--- page break ---

Contents
!!!!!!!!!! ALL USERS MUST READ !!!!!!!!!! ..........................................i
Linux and Mac OS USERS MUST READ ............................................ ii
1 Overview ................................................................................................1
2 General information ...........................................................................10
2.1 Install..................................................................................................................................... 10
2.1.1 Windows version ............................................................................................................ 10
2.1.2 Linux version ................................................................................................................. 10
2.1.3 Mac OS version .............................................................................................................. 11
2.2 Using Multiwfn ..................................................................................................................... 12
2.3 Files of Multiwfn .................................................................................................................. 13
2.4 Parallel implementation ........................................................................................................ 13
2.5 Input files and wavefunction types........................................................................................ 13
2.6 Real space functions.............................................................................................................. 20
2.7 User-defined real space function ........................................................................................... 33
2.8 Graphic formats and image size ............................................................................................ 49
2.9 Analysis of periodic systems ................................................................................................. 49
2.9.1 Wavefunction analysis on wavefunction of cluster model ......................................... 49
2.9.2 Wavefunction analysis on periodic wavefunction ...................................................... 51
2.9.2.1 Generating .molden wavefunction file .................................................................... 51
2.9.2.2 Functions available for analyzing periodic wavefunction ....................................... 52
2.9.2.3 Related parameters for analyzing periodic wavefunctions ...................................... 53
2.9.3 Other kinds of analyses on periodic systems.............................................................. 54

3 Functions ..............................................................................................56
3.2 Showing molecular structure and viewing orbitals / isosurfaces (0) ..................................... 56
3.3 Outputting all properties at a point (1) .................................................................................. 59
3.4 Outputting and plotting specific property in a line (3) .......................................................... 60
3.5 Outputting and plotting specific property in a plane (4) ....................................................... 61
3.5.1 Graph types .................................................................................................................... 61
3.5.2 Setting up grid, plane and plotting region ...................................................................... 66
3.5.3 Options in post-processing interface .............................................................................. 69
3.5.4 Setting up contour lines .................................................................................................. 69
3.5.5 Plot critical points, paths and interbasin paths on plane graph....................................... 70
3.6 Outputting and plotting specific property within a spatial region (5) ................................... 72
3.7 Custom operation, promolecular and deformation properties (options 0, -1, -2 in main
functions 3, 4, 5) ......................................................................................................................... 76
3.7.1 Custom operation for multiple wavefunctions (0) ......................................................... 76

iv

--- page break ---

3.7.2 Promolecular and deformation properties (-1, -2) .......................................................... 77
3.7.3 Generation of atomic wavefunctions.............................................................................. 77
3.7.4 Sphericalization of atom wavefunction .......................................................................... 78
3.8 Checking & modifying wavefunction (6) ............................................................................. 79
3.9 Population analysis and calculation of atomic charges (7) ................................................... 82
3.9.1 Hirshfeld atomic charge (1)............................................................................................ 83
3.9.2 Voronoi deformation density (VDD) atom population (2) ............................................. 84
3.9.3 Mulliken atom & basis function population analysis (5) ............................................... 84
3.9.4 Löwdin atom & basis function population (6) ............................................................... 86
3.9.5 Modified Mulliken atom population defined by Ros & Schuit (SCPA) (7) ................... 86
3.9.6 Modified Mulliken atom population defined by Stout & Politzer (8) ............................ 87
3.9.7 Modified Mulliken atom population defined by Bickelhaupt (9)................................... 87
3.9.8 Becke atomic charge with atomic dipole moment correction (10) ................................. 88
3.9.9 Atomic dipole moment corrected Hirshfeld atomic charges (ADCH, 11) ..................... 88
3.9.10 CHELPG (Charges from electrostatic potentials using a grid based method) ESP fitting
atomic charge (12)................................................................................................................... 90
3.9.11 Merz-Kollmann (MK) ESP fitting atomic charge (13) ................................................. 93
3.9.12 AIM atomic charge (14) ............................................................................................... 94
3.9.13 Hirshfeld-I atomic charge (15) ..................................................................................... 94
3.9.14 CM5 and 1.2*CM5 atomic charge (16, -16) ................................................................ 97
3.9.15 Electronegativity Equalization Method (EEM) atomic charge (17) ............................. 98
3.9.16 Restrained ElectroStatic Potential (RESP) atomic charge (18) .................................. 101
3.9.16.1 Theory ................................................................................................................. 101
3.9.16.2 Usage and some details ....................................................................................... 106
3.9.17 PEOE (Partial equalization of orbital electronegativity) or Gasteiger charge (19) .... 109
3.9.18 Minimal Basis Iterative Stockholder (MBIS) charge (20) ......................................... 111
3.10 Orbital composition analysis (8) ....................................................................................... 113
3.10.1 Output basis function, shell and atom composition in a specific orbital by Mulliken,
Stout-Politzer and SCPA approaches (1, 2, 3) ....................................................................... 113
3.10.2 Define fragment 1 and 2 (-1, -2) ................................................................................ 114
3.10.3 Output composition of fragment 1 and inter-fragment composition by Mulliken, StoutPolitzer and SCPA approaches (4, 5, 6)................................................................................. 115
3.10.4 Orbital composition analysis by natural atomic orbital approach (7) ........................ 115
3.10.5 Calculate atom and fragment contributions by Hirshfeld or Hirshfeld-I method (8,10)
............................................................................................................................................... 117
3.10.6 Calculate atom and fragment contributions by Becke method (9) ............................. 118
3.10.7 Calculate atom and fragment contributions by AIM method (11) .............................. 118
3.10.100 Evaluate oxidation state by LOBA and mLOBA method (100) ............................. 118
3.11 Bond order analysis (9) ..................................................................................................... 119
3.11.1 Mayer bond order analysis (1).................................................................................... 120
3.11.2 Multi-center bond order analysis (2, -2, -3) ............................................................... 121
3.11.3 Wiberg bond order analysis in Löwdin orthogonalized basis (3) ............................... 125
3.11.4 Mulliken bond order analysis (4) and decomposition (5)........................................... 126
3.11.5 Orbital occupancy-perturbed Mayer bond order (6)................................................... 126

v

--- page break ---

3.11.6 Fuzzy bond order (7) .................................................................................................. 127
3.11.7 Laplacian bond order (8) ............................................................................................ 128
3.11.8 Decompose Wiberg bond order in NAO basis as atomic orbital pair contributions (9)
............................................................................................................................................... 128
3.11.9 Intrinsic bond strength index (IBSI) (10) ................................................................... 129
3.11.10 AV1245 index (approximate multi-center bond order for large rings) and AVmin .. 131
3.12 Plotting density-of-states (DOS), photoelectron spectrum (PES), and COHP (10) .......... 133
3.12.1 Theory ........................................................................................................................ 133
3.12.2 Input file ..................................................................................................................... 135
3.12.3 Options for plotting DOS and basic usage ................................................................. 136
3.12.4 Local DOS.................................................................................................................. 138
3.12.5 Photoelectron spectrum .............................................................................................. 138
3.12.6 COHP ......................................................................................................................... 140
3.13 Plotting IR, Raman, UV-Vis, ECD, VCD, ROA and NMR spectra (11) ........................... 141
3.13.1 Theory ........................................................................................................................ 142
3.13.2 Input file ..................................................................................................................... 145
3.13.3 Usage and options ...................................................................................................... 149
3.13.4 Plotting multiple systems together and weighted spectrum ....................................... 151
3.13.5 Plotting NMR spectrum ............................................................................................. 152
3.13.6 Partial vibrational spectrum (PVS) and partial vibrational density-of-states (PVDOS)
............................................................................................................................................... 155
3.13.6.1 Theory ................................................................................................................. 155
3.13.6.2 Usage................................................................................................................... 158
3.13.7 Predicting color based on UV-Vis spectrum curve..................................................... 159
3.14 Topology analysis (2) ........................................................................................................ 161
3.14.1 Theory ........................................................................................................................ 161
3.14.2 Search critical points .................................................................................................. 163
3.14.3 Generate topology paths ............................................................................................. 166
3.14.4 Generate interbasin surfaces....................................................................................... 166
3.14.5 Visualize, analyze, modify and export results ............................................................ 167
3.14.6 Calculate the aromaticity indices based on topology properties of electron density .. 169
3.15 Quantitative analysis of molecular surface (12) ................................................................ 170
3.15.1 Theory ........................................................................................................................ 170
3.15.2 Numerical algorithm .................................................................................................. 174
3.15.2.1 Analysis on the whole molecular surface ............................................................ 174
3.15.2.2 Analysis on local molecular surface .................................................................... 176
3.15.3 Parameters and options .............................................................................................. 178
3.15.4 Options in post-processing menu ............................................................................... 180
3.15.5 Special topic: Hirshfeld and Becke surface analyses ................................................. 184
3.16 Processing grid data (13) ................................................................................................... 187
3.16.0 Visualize isosurface of present grid data (-2) ............................................................. 187
3.16.1 Export present grid data to Gaussian-type cube file (0) ............................................. 187
3.16.2 Output all data points with value and coordinate (1) ................................................. 187
3.16.3 Output data points in a XY/YZ/XZ plane (2, 3, 4)..................................................... 187

vi

--- page break ---

3.16.4 Output average data of XY/YZ/XZ planes in a range of Z/X/Y (5, 6, 7) ................... 188
3.16.5 Output data points in a plane defined three atom indices or three points (8, 9) ......... 188
3.16.6 Output data points in specified value range (10) ........................................................ 188
3.16.7 Grid data calculation (11) ........................................................................................... 188
3.16.8 Map values of a cube file to specified isosurface of present grid data (12) ............... 189
3.16.9 Set value of the grid points that far away from / close to some atoms (13) ............... 189
3.16.10 Set value of the grid points outside overlap region of two fragments (14) .............. 190
3.16.11 If data value is within certain range, set it to a specified value (15) ........................ 190
3.16.12 Scale data range of present grid data (16) ................................................................ 190
3.16.13 Show statistic data of grid points in specific spatial and value ranges (17) ............. 191
3.16.14 Plot (local) integral curve and plane-averaged curve in X/Y/Z direction (18) ......... 191
3.17 Adaptive natural density partitioning (AdNDP) analysis (14) .......................................... 193
3.17.1 Theory ........................................................................................................................ 193
3.17.2 Input file ..................................................................................................................... 195
3.17.3 Options ....................................................................................................................... 196
3.18 Fuzzy atomic space analysis (15) ...................................................................................... 198
3.18.0 Basic concepts ............................................................................................................ 198
3.18.1 Integration of a real space function in fuzzy atomic spaces (1) ................................. 201
3.18.2 Integration of a real space function in overlap spaces (8) .......................................... 202
3.18.3 Atomic and molecular multipole moments and <r2> (2) ............................................ 202
3.18.4 Atomic overlap matrix and fragment overlap matrix (3, 33)...................................... 205
3.18.5 Localization index (LI) and delocalization index (DI) (4, 44) ................................... 206
3.18.5.1 Theoretical background ....................................................................................... 206
3.18.5.2 Usage................................................................................................................... 209
3.18.6 Para-delocalization index (PDI) (5) ........................................................................... 209
3.18.7 Aromatic fluctuation index (FLU) and FLU-π (6,7) .................................................. 210
3.18.8 Condensed linear response kernel (CLRK) (9) .......................................................... 211
3.18.9 Para linear response index (PLR) (10) ....................................................................... 212
3.18.10 Multi-center delocalization index (11) ..................................................................... 212
3.18.11 Information-theoretic aromaticity index (12) ........................................................... 213
3.18.12 Atomic effective volume, free volume, polarizability and C6 coefficient (13)......... 213
3.19 Charge decomposition analysis and plotting orbital interaction diagram (16) .................. 215
3.19.1 Theory ........................................................................................................................ 215
3.19.2 Input file ..................................................................................................................... 218
3.19.3 Usage.......................................................................................................................... 219
3.20 Basin analysis (17) ............................................................................................................ 222
3.20.1 Theory ........................................................................................................................ 222
3.20.2 Numerical aspects ...................................................................................................... 223
3.20.3 Usage.......................................................................................................................... 227
3.21 Electron excitation analysis (18) ....................................................................................... 232
3.21.A Basic information about electron excitation analysis module ................................... 232
3.21.0 Check, modify and export configuration coefficients of an excitation (-1) ............... 237
3.21.1 Analyze and visualize hole&electron distribution, transition density, and transition
electric/magnetic dipole moment density (1) ........................................................................ 238

vii

--- page break ---

3.21.1.1 Theory ................................................................................................................. 238
3.21.1.2 Usage and Functions ........................................................................................... 246
3.21.2 Plot atom/fragment transition matrix of various kinds as heat map (2) ..................... 248
3.21.4 Calculate ∆r index to measure charge-transfer length (4) .......................................... 252
3.21.3 Analyze charge-transfer based on density difference grid data (3) ............................ 253
3.21.5 Calculate transition electric/magnetic dipole moments between all states and for each
state (5).................................................................................................................................. 256
3.21.6 Generate natural transition orbitals (NTOs) (6) ......................................................... 258
3.21.7 Calculate ghost-hunter index (7) ................................................................................ 260
3.21.8 Calculate interfragment charge transfer in electron excitation via IFCT method (8) . 261
3.21.9 Generate and export transition density matrix (9) ...................................................... 264
3.21.10 Decompose transition electric/magnetic dipole moment as molecular orbital pair
contributions (10) .................................................................................................................. 266
3.21.11 Decompose transition electric/magnetic dipole moment as basis function and atom
contributions (11) .................................................................................................................. 267
3.21.12 Calculate Mulliken atomic transition charges (12) .................................................. 269
3.21.13 Generate natural orbitals of specific excited states (13)........................................... 269
3.21.14 Calculate Λ index to characterize electron excitation (14)....................................... 270
3.21.15 Print major MO transitions in all excited states ....................................................... 272
3.21.16 Charge-transfer spectrum (CTS) analysis ................................................................ 272
3.21.17 Electron density polarization analysis based on electron excitations ....................... 274
3.22 Orbital localization analysis (19) ...................................................................................... 277
3.23 Visual study of weak interaction (20)................................................................................ 282
3.23.1 Noncovalent interaction (NCI) analysis (1) ............................................................... 282
3.23.2 NCI analysis based on promolecular density (2)........................................................ 291
3.23.3 Averaged NCI analysis (aNCI. 3) ............................................................................... 292
3.23.4 Density Overlap Regions Indicator (DORI) analysis (5) ........................................... 294
3.23.5 Independent Gradient Model (IGM) analysis based on promolecular density (10) ... 294
3.23.6 IGM analysis based on Hirshfeld partition of molecular density (IGMH) (11) ......... 300
3.23.7 Visualization of van der Waals potential (6) .............................................................. 301
3.23.8 Interaction region indicator (IRI) and IRI-pi analysis (4) .......................................... 302
3.23.9 Averaged independent gradient model (aIGM) analysis (12) ..................................... 303
3.23.10 Modified IGM (mIGM) analysis (-10) ..................................................................... 304
3.23.11 Averaged modified IGM (amIGM) analysis (-12) .................................................... 304
3.24 Energy decomposition analysis (21) ................................................................................. 305
3.24.1 Energy decomposition analysis based on molecular forcefield (EDA-FF) ................ 305
3.24.2 Shubin Liu's energy decomposition ........................................................................... 309
3.24.3 SobEDA and sobEDAw energy decomposition analyses ........................................... 310
3.24.4 Analysis of atomic contribution to dispersion energy ................................................ 311
3.25 Conceptual density functional theory (CDFT) analysis (22) ............................................ 312
3.25.1 Theory ........................................................................................................................ 313
3.25.2 Usage.......................................................................................................................... 316
3.25.3 Special topic: Orbital-weighted Fukui function and dual descriptor.......................... 318
3.25.4 Special topic: (Quasi-)degenerate Fukui function and dual descriptor based on electron

viii

--- page break ---

density ................................................................................................................................... 319
3.25.5 Special topic: Nucleophilic and electrophilic superdelocalizabilities ........................ 321
3.26 Extended Transition State - Natural Orbitals for Chemical Valence (ETS-NOCV) analysis
(23) ............................................................................................................................................ 322
3.26.1 Theory ........................................................................................................................ 322
3.26.2 Implementation details ............................................................................................... 325
3.26.3 Usage.......................................................................................................................... 327
3.27 (Hyper)polarizability analysis (24) ................................................................................... 329
3.27.1 Parse output of (hyper)polarizability task of Gaussian and evaluate relevant quantities
............................................................................................................................................... 329
3.27.2 Study (hyper)polarizability by sum-over-states (SOS) method and two- or three-level
model analyses ...................................................................................................................... 335
3.27.2.1 Calculation of (hyper)polarizability .................................................................... 335
3.27.2.2 Two-level and three-level model analyses for hyperpolarizability...................... 338
3.27.3 Study (hyper)polarizability density ............................................................................ 340
3.27.5 Visualize (hyper)polarizability via unit sphere and vector representations ................ 343
3.28 Electron delocalization and aromaticity analyses (25) ...................................................... 345
3.28.3 Generate iso-chemical shielding surfaces (ICSS) and related quantities ................... 345
3.28.4 Obtain NICSZZ value for non-planar or tilted system ................................................ 347
3.28.6 Calculate HOMA and Bird aromaticity index ............................................................ 348
3.28.7 HOMAc and HOMER................................................................................................ 350
3.28.13 NICS-1D scan curve map, integral NICS (INICS) and FiPC-NICS ........................ 350
3.28.14 NICS-2D scan plane map ......................................................................................... 351
3.100 Other functions, part 1 (100) ........................................................................................... 352
3.100.1 Draw scatter graph between two functions and generate their cube files ................ 352
3.100.2 Export various files or generate input file of quantum chemistry programs ............ 353
3.100.3 Calculate molecular van der Waals volume ............................................................. 355
3.100.4 Integrate a function over the whole space ................................................................ 356
3.100.5 Show overlap integral between alpha and beta orbitals ........................................... 357
3.100.6 Monitor SCF convergence process of Gaussian....................................................... 358
3.100.8 Generate Gaussian input file with initial guess combined from fragment wavefunctions
............................................................................................................................................... 359
3.100.9 Evaluate interatomic connectivity and atomic coordination number ....................... 361
3.100.11 Calculate overlap and centroid distance between two orbitals ............................... 362
3.100.12 Biorthogonalization between alpha and beta orbitals ............................................. 363
3.100.14 Calculate LOLIPOP (LOL Integrated Pi Over Plane) ............................................ 365
3.100.15 Calculate intermolecular orbital overlap ................................................................ 366
3.100.17 Generate Fock/KS matrix based on orbital energies and coefficients .................... 367
3.100.18 Yoshizawa's electron transport route analysis ........................................................ 368
3.100.19 Generate new wavefunction by combining fragment wavefunctions .................... 370
3.100.20 Calculate Hellmann-Feynman forces ..................................................................... 370
3.100.21 Calculate properties based on geometry information for specific atoms ............... 371
3.100.22 Detect π orbitals, set occupation numbers and calculate π composition ................ 374
3.100.23 Fit function distribution to atomic value ................................................................ 377

ix

--- page break ---

3.200 Other functions, part 2 (200) ........................................................................................... 378
3.200.1 Calculate core-valence bifurcation (CVB) index and related quantities .................. 378
3.200.2 Calculate atomic and bond dipole moments in Hilbert space .................................. 383
3.200.3 Generate cube file for multiple orbital wavefunctions ............................................. 385
3.200.4 (moved to Section 3.28.3) ........................................................................................ 385
3.200.5 Plot radial distribution function for a real space function ........................................ 385
3.200.6 Analyze correspondence between orbitals in two wavefunctions ............................ 386
3.200.7 (moved to Section 3.27.1) ........................................................................................ 388
3.200.8 (moved to Section 3.27.2) ........................................................................................ 388
3.200.9 Calculate average bond length and average coordinate number .............................. 388
3.200.10 Output various kinds of integral between orbitals .................................................. 389
3.200.11 Calculate center, first/second moments, radius of gyration, and <r^2> of a function
............................................................................................................................................... 389
3.200.12 Calculate energy index (EI) or bond polarity index (BPI) ..................................... 391
3.200.13 Evaluate orbital contributions to density difference or other grid data .................. 392
3.200.14 Domain analysis (obtaining properties within isosurfaces of a function) .............. 394
3.200.15 Calculate electron correlation index ....................................................................... 396
3.200.16 Generate natural orbitals, natural spin orbitals (NSO) and spin natural orbitals (SNO)
based on the density matrix in .fch/.fchk file ........................................................................ 397
3.200.17 Calculate Coulomb and exchange integral between two orbitals ........................... 398
3.200.18 Calculate bond length/order alternation (BLA/BOA) and angle/dihedral alternation
............................................................................................................................................... 400
3.200.19 Calculate spatial delocalization index (SDI) for orbitals or a function .................. 401
3.200.20 Bond order density (BOD) and natural adaptive orbital (NAdO) analyses ............ 402
3.200.21 Perform Löwdin orthogonalization between occupied orbitals.............................. 406
3.300 Other functions, part 3 (300) ........................................................................................... 407
3.300.1 Viewing free regions and calculating free volume in a cell ..................................... 407
3.300.2 Fitting atomic radial density as linear combination of multiple STOs or GTFs ...... 409
3.300.2.1 Algorithm and technical details ......................................................................... 409
3.300.2.2 Usage................................................................................................................. 412
3.300.4 Simulating scanning tunneling microscope (STM) image ....................................... 415
3.300.5 Calculate electric dipole/multipole moments and electronic spatial extent.............. 418
3.300.6 Calculate energies of present orbitals by inputting Fock matrix .............................. 420
3.300.7 Geometry relevant operations on the present system ............................................... 420
3.300.8 Plot surface distance projection map ........................................................................ 423
3.300.9 Determine Fermi level.............................................................................................. 425

4 Tutorials and Examples ....................................................................428
Prologue and generation of input files ...................................................................................... 428
4.0 View orbitals and structure.................................................................................................. 431
4.0.1 Viewing molecular orbitals of cycloheptatriene ........................................................... 431
4.0.2 Viewing natural bond orbitals (NBO) of ethanol ......................................................... 433
4.0.3 Using Multiwfn + VMD to rapidly plot high-quality orbital isosurface map .............. 435
4.1 Calculate properties at a point ............................................................................................. 438

x

--- page break ---

4.1.1 Show all properties of triplet water at a given point .................................................... 438
4.1.2 Calculate ESP at nuclear positions to evaluate interaction strength of H2O∙∙∙HF ........ 440
4.2 Topology analysis................................................................................................................ 442
4.2.1 Atoms-in-molecules (AIM) topology analysis and aromaticity analysis for 2-pyridoxine
2-aminopyridine .................................................................................................................... 442
4.2.2 Topology analysis of localized orbital locator (LOL) for acetic acid ........................... 448
4.2.3 Plot real space function along bond path ..................................................................... 451
4.2.4 Decompose properties at a critical point as orbital contributions ................................ 452
4.2.5 Easily plot high quality AIM topology map in VMD visualization program based on
Multiwfn outputs ................................................................................................................... 455
4.2.6 Topology analysis in special ways: G-C...G-C base pair as an example ...................... 458
4.2.7 Topology analysis via refining attractors located by basin analysis: Spin density of
biradical as an example ......................................................................................................... 462
4.2.8 Topology analysis for density difference: Deformation density of H2O as an example
............................................................................................................................................... 464
4.2.9 Topology analysis for electrostatic potential (ESP) ..................................................... 466
4.2.10 Topology analysis for van der Waals potential ........................................................... 471
4.2.11 Topology analysis for interaction region indicator (IRI) and reduced density gradient
(RDG) ................................................................................................................................... 472
4.3 Output and plot various properties in a line ........................................................................ 475
4.3.1 Plot the spin density curve of triplet formamide along carbon and oxygen atoms....... 475
4.3.2 Study Fermi hole and Coulomb hole of H2 .................................................................. 476
4.3.3 Study interatomic interaction via PAEM-MO method ................................................. 479
4.4 Output and plot various properties in a plane ..................................................................... 482
4.4.1 Illustration of plotting color-filled map and contour line map ..................................... 482
4.4.1.1 Plotting electron density of hydrogen cyanide ...................................................... 482
4.4.1.2 Plotting localized orbital locator (LOL) for FOX-7 .............................................. 485
4.4.2 Shaded relief map with projection effect of electron localization function (ELF) of
monofluoroethane ................................................................................................................. 488
4.4.3 Plotting plane map without contributions from some atoms ........................................ 489
4.4.4 Contour map of electrostatic potential of chlorine trifluoride ...................................... 491
4.4.5 Contour map of two orbital wavefunctions .................................................................. 493
4.4.6 Gradient + contour map with topology paths of electron density of hydrogen peroxide
............................................................................................................................................... 495
4.4.7 Deformation map of electron density of acetyl chloride .............................................. 499
4.4.8 Plot difference map of electron density and ELF for water tetramer with respect to its
constituent monomers ........................................................................................................... 500
4.4.9 Plotting LOL-π map for porphyrin to reveal favorable electron delocalization path ... 503
4.4.10 Plotting gradient line and vector field map of electrostatic potential to reveal electric
field of LiF ............................................................................................................................ 507
4.4.11 Plotting a pretty 4p orbital of Kr atom ....................................................................... 509
4.4.12 Showing extrema of a function on contour line(s) ..................................................... 510
4.5 Generate grid data and view isosurface map ....................................................................... 511
4.5.1 Electron localization function of chlorine trifluoride ................................................... 512

xi

--- page break ---

4.5.2 Laplacian of electron density of 1,3-butadiene ............................................................ 513
4.5.3 Calculate ELF- and ELF-π to study aromaticity of benzene ..................................... 514
4.5.4 Use Fukui function and dual descriptor to study favorable site of electrophilic attack for
phenol.................................................................................................................................... 517
4.5.4.1 Fukui function ....................................................................................................... 518
4.5.4.2 Dual descriptor ...................................................................................................... 520
4.5.5 Plot difference map of electron density to study electron transfer of imidazole coordinated
magnesium porphyrin............................................................................................................ 522
4.5.6 Study electron delocalization range function EDR(r;d) of anionic water dimer .......... 526
4.5.7 Study orbital overlap distance function D(r) of thioformic acid .................................. 527
4.6 Modify and check wavefunction ......................................................................................... 528
4.6.1 Delete certain Gaussian functions ................................................................................ 528
4.6.2 Valence electron density analysis: Example of removing contributions from certain
orbitals to real space functions .............................................................................................. 529
4.6.3 Translate and duplicate graphene primitive cell wavefunction to periodic system ...... 531
4.7 Population analysis and atomic charge calculation ............................................................. 533
4.7.0 Mulliken population analysis on triplet ethanol ........................................................... 533
4.7.1 Calculate Hirshfeld and CHELPG atomic charges as well as fragment charge for chlorine
trifluoride .............................................................................................................................. 535
4.7.2 Calculate and compare ADCH atomic charges with Hirshfeld atomic charges for
acetamide .............................................................................................................................. 536
4.7.3 Calculate condensed Fukui function and condensed dual descriptor ........................... 537
4.7.4 Illustration of computing Hirshfeld-I atomic charges .................................................. 539
4.7.5 Calculating EEM atomic charges for ethanol-water cluster ......................................... 541
4.7.6 Determining correspondence between basis functions and atomic orbitals via population
analysis .................................................................................................................................. 543
4.7.7 Illustration of deriving RESP charges and normal ESP fitting charges with extra
constraints ............................................................................................................................. 545
4.7.7.1 Example 1: Deriving RESP charges for dopamine in ethanol environment.......... 545
4.7.7.2 Example 2: Taking multiple conformations into account during RESP charge
calculation of dopamine ............................................................................................................ 548
4.7.7.3 Example 3: Imposing equivalence constraint in ESP fitting of Dimethyl phosphate
.................................................................................................................................................. 550
4.7.7.4 Example 4: Evaluation of atomic charges of aspartic acid residue with equivalence
and charge constraints ............................................................................................................... 551
4.7.7.5 Example 5: Example of setting equivalence constraint according to local or global
point group symmetry ............................................................................................................... 554
4.7.7.6 Example 6: RESP charge calculation with additional fitting centers .................... 557
4.7.7.7 Skill 1: Using two times of one-stage fitting to equivalently realize standard RESP
two-stage fitting ........................................................................................................................ 560
4.7.7.8 Skill 2: Quickly obtaining RESP charges from molecular structure file by only one
command ................................................................................................................................... 561
4.7.7.9: Special topic: Calculation of RESP2 charges....................................................... 562
4.7.8 Examine electrostatic potential reproducibility of atomic charges............................... 565

xii

--- page break ---

4.7.9 Calculate PEOE (Gasteiger) charge ............................................................................. 568
4.7.9 Calculate CM5 and 1.2*CM5 charges ......................................................................... 568
4.8 Molecular orbital composition analysis .............................................................................. 570
4.8.1 Analyze acetamide by Mulliken method ...................................................................... 570
4.8.2 Analyze water by natural atomic orbital method.......................................................... 573
4.8.3 Analyze acetamide by Hirshfeld and Becke method .................................................... 576
4.8.4 Calculate oxidation state by LOBA/mLOBA method .................................................. 578
4.8.5 Quantifying extent of spatial delocalization of orbitals via orbital delocalization index
(ODI) ..................................................................................................................................... 580
4.8.5.1 Example of calculating ODI based on orbital composition................................... 580
4.8.5.2 Calculating ODI based on orbital composition for a batch of orbitals .................. 582
4.8.5.3 Calculating ODI for a fragment ............................................................................ 583
4.8.6 Calculate orbital composition contributed by AIM basins and other type of basins .... 584
4.9 Bond order analysis ............................................................................................................. 586
4.9.1 Mayer bond order and fuzzy bond order analysis on acetamide .................................. 586
4.9.2 Multi-center bond order analysis on Li6 cluster and phenanthrene .............................. 589
4.9.3 Calculate Laplacian bond order (LBO) ........................................................................ 593
4.9.4 Decomposition analysis of Wiberg bond order in NAO basis for formaldehyde ......... 594
4.9.5 Study orbital contributions to Mulliken bond order for C-C bond of CH3CONH2 ...... 595
4.9.6 Using intrinsic bond strength index (IBSI) to measure strength of chemical bonds .... 597
4.9.11 Example of using AV1245 and AVmin indices to study aromaticity .......................... 598
4.9.11.1 Using AV1245 and AVmin to study local aromaticity of phenanthrene .............. 599
4.9.11.2 Using AV1245 and AVmin to measure global aromaticity of porphyrin ............. 600
4.10 Plot density-of-states (DOS) maps .................................................................................... 603
4.10.1 Plot total, partial and overlap DOS for N-phenylpyrrole ........................................... 603
4.10.2 Plot local DOS for 1,3-butadiene ............................................................................... 611
4.10.3 Plot DOS map for unrestricted open-shell system: Na3O@Si12C12 ........................... 613
4.10.4 Plot photoelectron spectrum (PES) for Cr3Si12− cluster ............................................. 615
4.10.5 Plot MO-PDOS map to reveal PDOS contributed by different MOs for cyclo[18]carbon
............................................................................................................................................... 617
4.10.6 Calculate d-band center for transition metal clusters ................................................. 620
4.10.7 Plot COHP for C60 fullerene and N-phenylpyrrole .................................................... 622
4.11 Plot various kinds of spectra ............................................................................................. 624
4.11.1 Plot infrared (IR) spectrum for NH3BF3..................................................................... 624
4.11.2 Plot UV-Vis spectrum and contributions from individual transitions for acetic acid . 626
4.11.3 Plot electronic circular dichroism (ECD) spectrum for asparagine ............................ 628
4.11.4 Plot conformational weighted UV-Vis and ECD spectra for plumericin .................... 631
4.11.5 Plot Raman and pre-resonance spectra for 2-methyloxirane ...................................... 635
4.11.6 Simultaneously plot multiple systems ........................................................................ 636
4.11.7 Plot VCD and ROA spectra for chiral molecule S-methyloxirane ............................. 638
4.11.8 Skill: Plot spectrum for a batch of files via shell script .............................................. 640
4.11.9 Skill: Use spikes to indicate position of transition levels ........................................... 641
4.11.10 Plotting NMR spectrum............................................................................................ 643
4.11.10.1 Plotting 1H and 13C NMR spectra for acetaldehyde .......................................... 643

xiii

--- page break ---

4.11.10.2 Plotting NMR spectra for pyridine based on scaling method ............................ 645
4.11.10.3 Plotting conformation weighted NMR spectrum for valine .............................. 646
4.11.10.4 Plotting multiple systems simultaneously ......................................................... 649
4.11.11 Plotting fluorescence spectrum of BODIPY............................................................. 649
4.11.12 Plotting partial vibrational spectrum (PVS) and partial vibrational density-of-states
(PVDOS) ............................................................................................................................... 651
4.11.12.1 PVS-NC decomposition analysis of IR spectrum of C18B9N9 complex ......... 651
4.11.12.2 PVS-I decomposition analysis of IR spectrum of C18B9N9 complex ............. 656
4.11.12.3 PVDOS analysis of vibrational spectrum C18B9N9 complex.......................... 656
4.11.12.4 PVS-NC decomposition analysis of VCD spectrum of phenylalanine.............. 659
4.11.12.5 Plotting directional IR spectrum........................................................................ 659
4.11.13 Plotting directional UV-Vis spectrum....................................................................... 659
4.11.14 Predicting color of indigo and allura red .................................................................. 661
4.11.14.1 Predicting color of indigo based on theoretical calculation............................... 661
4.11.14.2 Predicting color of allura red based on experimental UV-Vis spectrum ........... 663
4.12 Quantitative analysis of molecular surface ....................................................................... 663
4.12.1 Electrostatic potential analysis on phenol molecular surface ..................................... 663
4.12.2 Average local ionization energy analysis (ALIE) on phenol molecular surface......... 670
4.12.3 Atomic local molecular surface analysis for acrolein................................................. 672
4.12.4 Quantitative analysis of Fukui function on molecular surface of phenol ................... 674
4.12.5 Becke surface analysis on guanine-cytosine base pair ............................................... 677
4.12.6 Hirshfeld surface analysis and fingerprint plot analysis on urea crystal .................... 679
4.12.7 Predict density of molecular crystal of FOX-7 .......................................................... 685
4.12.8 Quantitative analysis of orbital overlap distance function D(r) on thioformic acid
molecular surface .................................................................................................................. 686
4.12.9 Evaluate vdW surface area of the whole system as well as individual fragment ....... 688
4.12.10 Quantification of area of sigma-hole and pi-hole ..................................................... 690
4.12.11 Basin-like analysis of molecular surface for electrostatic potential ......................... 693
4.12.12 Estimate kinetic diameter for small molecules......................................................... 695
4.12.13 Using local electron affinity and local electron attachment energy to reveal
electrophilic regions .............................................................................................................. 697
4.13 Process grid data ............................................................................................................... 700
4.13.1 Extract data points in a plane ..................................................................................... 700
4.13.2 Perform mathematical operation on grid data ............................................................ 701
4.13.3 Scaling numerical range of grid data.......................................................................... 702
4.13.4 Screen isosurfaces in local regions............................................................................. 702
4.13.4.1 Screen isosurfaces inside or outside a region ...................................................... 702
4.13.4.2 Screen isosurfaces outside overlap region of two fragments .............................. 703
4.13.5 Acquire barycenter of a molecular orbital .................................................................. 705
4.13.6 Plot charge displacement curve .................................................................................. 706
4.13.7 Evaluation of electron density overlap ....................................................................... 708
4.13.8 Integrate electron density in a cylindrical region ....................................................... 710
4.14 Adaptive natural density partitioning (AdNDP) analysis .................................................. 712
4.14.1 Analyze Li5+ cluster.................................................................................................... 712

xiv

--- page break ---

4.14.2 Analyze B11− cluster ................................................................................................... 713
4.14.3 Analyze phenanthrene ................................................................................................ 717
4.14.4 Analyze Au20 cluster................................................................................................... 720
4.15 Fuzzy atomic space analysis ............................................................................................. 722
4.15.1 Study delocalization index of benzene ....................................................................... 722
4.15.2 Study aromaticity of phenanthrene by PDI, FLU, FLU-π and PLR ........................... 723
4.15.3 Calculate fragment dipole moment to exhibit local polarity ...................................... 724
4.15.4 Calculate atomic effective, free volumes, atomic polarizability and atomic C6 coefficient:
Oxirane and SiH4 as examples .............................................................................................. 727
4.15.5 Visualizing atomic electric dipole and quadrupole moments ..................................... 730
4.15.5.1 Plotting atomic dipole moments .......................................................................... 730
4.15.5.2 Plotting atomic quadrupole moments .................................................................. 732
4.16 Charge decomposition analysis and plotting orbital interaction diagram ......................... 734
4.16.1 Closed-shell interaction case: COBH3 ....................................................................... 734
4.16.2 Open-shell interaction case: CH3NH2 ........................................................................ 738
4.16.3 More than two fragments case: Pt(NH3)2Cl2 .............................................................. 740
4.16.4 CDA analysis based on restricted open-shell wavefunction: ethane .......................... 741
4.17 Basin analysis.................................................................................................................... 744
4.17.1 AIM basin analysis for HCN and Li6 ......................................................................... 744
4.17.2 Example of ELF basin analysis .................................................................................. 751
4.17.3 Basin analysis of electrostatic potential for H2O........................................................ 757
4.17.4 Basin analysis of electron density difference for H2O ............................................... 761
4.17.5 Study source function in AIM basins ......................................................................... 763
4.17.6 Local region basin analysis for polyyne ..................................................................... 765
4.17.7 Evaluate atomic contribution to population of ELF basins ........................................ 766
4.17.8 Calculating high ELF localization domain population and volume (HELP, HELV).. 767
4.17.9 Evaluate atomic contributions to system electronic energy ....................................... 769
4.17.10 Plotting ELF isosurface map colored by basin types ............................................... 771
4.18 Electron excitation analysis............................................................................................... 773
4.18.1 Using hole-electron analysis to fully characterize electron excitations ..................... 773
4.18.1.1 Example 1: NH2-biphenyl-NO2........................................................................... 774
4.18.1.2 Example 2: Ru(bpy3)2+ cation in water ............................................................... 785
4.18.2 Illustration of transition density (matrix) and transition dipole moment density (matrix)
analysis .................................................................................................................................. 787
4.18.2.1 Analyzing transition density and transition dipole moment density in real
space.......................................................................................................................................... 788
4.18.2.2 Plotting and analyzing transition density matrix (TDM) .................................... 791
4.18.2.3 Plotting and analyzing transition dipole moment matrix .................................... 797
4.18.2.4 Investigating transition density and transition density matrix between excited
states .......................................................................................................................................... 799
4.18.3 Analyze charge-transfer during electron excitation based on electron density difference
............................................................................................................................................... 802
4.18.4 Calculate ∆r and Λ indices to characterize various electron excitations for Nphenylpyrrole ........................................................................................................................ 804

xv

--- page break ---

4.18.5 Calculate dipole moment of each excited state and transition dipole moment between all
states for 4-nitroaniline ......................................................................................................... 807
4.18.6 Generate and analyze natural transition orbitals (NTOs) for uracil ........................... 808
4.18.8 Using IFCT method and heat map of charge transfer matrix to study interfragment
charge transfer during electron excitation ............................................................................. 811
4.18.8.1 IFCT analysis for 4-nitroaniline .......................................................................... 811
4.18.8.2 Plotting heat map of charge transfer matrix to intuitively understand nature of
electron excitation ..................................................................................................................... 814
4.18.9 Generate transition density matrix and transform it to orbital representation ............ 816
4.18.10 Obtain molecular orbital pair contributions to transition dipole moment ................ 817
4.18.11 Plot transition dipole moment vector contributed by molecular fragments as arrows
............................................................................................................................................... 819
4.18.13 Study electronic structure of a single excited state and difference between two excited
states ...................................................................................................................................... 821
4.18.16 Plot charge-transfer spectrum and calculate major characters of all excited states: Nphenylpyrrole as an instance ................................................................................................. 824
4.18.17 Example of performing electron density polarization analysis based on electron
excitations ............................................................................................................................. 827
4.19 Orbital localization analysis .............................................................................................. 831
4.19.1 Localizing molecular orbital of 1,3-butadiene by Pipek-Mezey method ................... 831
4.19.2 Analyze variation of localized molecular orbitals for SN2 reaction .......................... 834
4.19.3 Characterize Re-Re bond of [Re2Cl8]2- anion............................................................. 835
4.19.4 Study bond dipole moment based on two-center LMOs for CH3NH2 ....................... 837
4.20 Visual study of weak interactions...................................................................................... 838
4.20.1 Studying weak interaction in 2-pyridoxine 2-aminopyridine by NCI method ........... 838
4.20.2 Studying weak interaction in DNA by NCI method based on promolecular density . 841
4.20.3 Visually studying weak interaction for water in bulk environment by aNCI method 843
4.20.4 Revealing both chemical bonding and weak interaction regions in phenol dimer by IRI
analysis .................................................................................................................................. 849
4.20.5 Simultaneously revealing covalent and noncovalent interactions in phenol dimer by
DORI analysis ....................................................................................................................... 853
4.20.6 Visualizing and analyzing van der Waals potential .................................................... 853
4.20.6.1 Example 1: Helicene ........................................................................................... 854
4.20.6.2 Example 2: Cyclo[18]carbon .............................................................................. 856
4.20.10 Visualize and quantify weak interactions by Independent Gradient Model (IGM) .. 859
4.20.10.1 Example 1: Guanine-cytosine (GC) base pair ................................................... 859
4.20.10.2 Example 2: C60-coronene dimer ........................................................................ 866
4.20.10.3 Example 3: Oxazolidinone trimer ..................................................................... 868
4.20.11 Using IGMH (IGM based on Hirshfeld partition of molecular density) to study weak
interactions ............................................................................................................................ 871
4.20.12 Using mIGM to study weak interactions.................................................................. 872
4.20.13 Using amIGM to reveal weak interactions in dynamic environments ..................... 873
4.21 Energy decomposition analysis ......................................................................................... 876
4.21.1 Examples of energy decomposition analysis based on forcefield (EDA-FF)............. 876

xvi

--- page break ---

4.21.1.1 Example 1: Water dimer ...................................................................................... 877
4.21.1.2 Example 2: Circumcoronene-Cytosine-Guanine trimer ...................................... 880
4.21.2 Shubin Liu's energy decomposition analysis for ethane rotation barrier.................... 887
4.21.3 Examples of sobEDA and sobEDAw energy decomposition analyses....................... 888
4.21.4 Examples of analysis of atomic contribution to dispersion energy ............................ 888
4.21.4.1 Investigating dispersion energy contributed by atoms in helicene and dispersion
density ....................................................................................................................................... 888
4.21.4.2 Difference in dispersion energy between two conformations of actos ................ 890
4.21.4.3 Adsorption of toluene on zeolite ......................................................................... 892
4.22 Examples of conceptual density functional theory (CDFT) analysis ................................ 894
4.22.1 Automatically calculate conceptual density functional theory quantities for phenol . 894
4.22.2 Illustration of studying orbital-weighted Fukui function and orbital-weighted dual
descriptor............................................................................................................................... 898
4.22.3 Example of CDFT analysis in (quasi-)degenerate HOMO/LUMO case .................... 904
4.22.3.1 Fukui function and dual descriptor of benzene ................................................... 904
4.22.3.2 Local softness and local hyper-softness of C60 fullerene .................................... 906
4.22.4 Example of plotting Fukui potential and dual descriptor potential ............................ 908
4.23 Examples of ETS-NOCV analysis .................................................................................... 909
4.23.1 A simple closed-shell instance: CO-BH3 .................................................................... 909
4.23.2 A simple open-shell instance: Ethane ......................................................................... 917
4.23.3 An open-shell system with multiple bond: Ethene ..................................................... 920
4.23.4 Weak interaction instance: A-T base pair ................................................................... 922
4.23.5 Transition metal coordinate instance: (CO)5Cr=CH2 ................................................. 924
4.23.6 More than two fragments instance: Transition state of trimerization of acetylene..... 926
4.24 Examples of (hyper)polarizability analyses ...................................................................... 928
4.24.1 Parse output file of “polar” task of Gaussian to obtain (hyper)polarizability and calculate
related quantities ................................................................................................................... 928
4.24.2 Studying polarizability and hyperpolarizability based on sum-over-states (SOS) method
............................................................................................................................................... 932
4.24.2.1 Calculate polarizability and hyperpolarizability for NH3 .................................... 932
4.24.2.2 Perform two- and three-level model analysis for first hyperpolarizability of NH2biphenyl-NO2 ............................................................................................................................ 936
4.24.3 Example of studying (hyper)polarizability density .................................................... 939
4.24.5 Example of using unit sphere representation to visually study (hyper)polarizability 944
4.24.5.1 First-order hyperpolarizability of CH3NHCHO .................................................. 944
4.24.5.2 Polarizability and second-order hyperpolarizability of cyclo[18]carbon ............ 947
4.25 Examples of electron delocalization and aromaticity analyses ......................................... 949
4.25.3 Study iso-chemical shielding surface (ICSS) and magnetic shielding distribution for
benzene ................................................................................................................................. 950
4.25.6 Calculate HOMA and Bird aromaticity index for phenanthrene ................................ 955
4.25.13 Example of plotting one-dimension NICS curve, calculating integral (INICS) and
FiPC-NICS ............................................................................................................................ 956
4.25.13.1 Example 1: NICSZZ curve of infinitene ............................................................. 956
4.25.13.2 Example 2: NICSsigma,ZZ and NICSpi,ZZ curves of benzene................................ 958

xvii

--- page break ---

4.25.13.3 Example 3: Calculate FiPC-NICS index for benzene ....................................... 960
4.25.14 Example of plotting two-dimension NICS plane map ............................................. 961
4.25.14.1 Plotting NICSZZ plane map above 1 Å of coronene .......................................... 961
4.25.14.2 Plotting NICSZZ plane map above 1 Å of a phenyl ring of N(phenyl)3............. 963
4.100 Other functions (Part 1) ................................................................................................... 965
4.100.4 Calculate kinetic energy and nuclear attraction potential energy of phosgene by
numerical integration ............................................................................................................ 965
4.100.8 Perform simple energy decomposition by using combined fragment wavefunctions
............................................................................................................................................... 966
4.100.12 Biorthogonalization analysis for orbitals of unrestricted open-shell wavefunction967
4.100.14 Calculate LOLIPOP index...................................................................................... 971
4.100.14.1 Example of planar system: phenanthrene........................................................ 971
4.100.14.2 Example of non-planar system: biphenyl ........................................................ 972
4.100.15 Calculate intermolecular orbital overlap integral of DB-TTF ................................ 974
4.100.18 Yoshizawa's electron transmission route analysis for phenanthrene ...................... 975
4.100.19 ELF analysis on the whole wavefunction combined from fragment wavefunctions
............................................................................................................................................... 976
4.100.21 Examples of characterizing geometric structure .................................................... 979
4.100.21.1 Calculate molecular diameter and length/width/height for alphacyclodextrin............................................................................................................................... 979
4.100.21.2 Study molecular planarity of [14]annulene ..................................................... 981
4.100.21.3 Study evolution of molecular planarity of cyclo[18]carbon in molecular
dynamics trajectory ................................................................................................................... 983
4.100.21.4 Evaluating cavity diameter and graphically illustrating its region .................. 984
4.100.22 Analyze π electron character of non-planar system: cycloheptatriene ................... 986
4.200 Other functions (Part 2) ................................................................................................... 990
4.200.4 (moved to Section 4.25.3) ........................................................................................ 990
4.200.5 Plot radial distribution function of electron density ................................................. 990
4.200.6 Studying correspondence between orbitals in different wavefunction files ............. 994
4.200.6.1 Revealing relationship between HF and MP2 orbitals of CH3NH2 ................... 994
4.200.6.2 Study contribution of lone pair of nitrogen to MOs of dopamine ..................... 995
4.200.7 (moved to section 4.24.1) ......................................................................................... 997
4.200.8 (moved to section 4.24.2) ......................................................................................... 997
4.200.12 Calculate energy index (EI) and bond polarity index (BPI) ................................... 997
4.200.13 Study orbital contributions to density difference ................................................... 998
4.200.13.1 Contribution of MOs to Fukui function f − of phenol ...................................... 998
4.200.13.2 Contribution of NBO orbitals to Fukui function f − of 1,3-butadiene ........... 1000
4.200.13.3 Contribution of NBO orbitals to density difference between S0 and S1 states
of H2CO .................................................................................................................................. 1002
4.200.14 Domain analysis examples ................................................................................... 1004
4.200.14.1 Integrate real space functions within reduced density gradient (RDG)
isosurface to study weak interaction quantitatively ................................................................ 1004
4.200.14.2 Visualize molecular cavity and calculate its volume by domain analysis
module..................................................................................................................................... 1007

xviii

--- page break ---

4.200.14.3 Integrate electron density difference in its isosurfaces .................................. 1011
4.200.18 Studying bond length/order alternation (BLA/BOA) as well as alteration of bond
angle and dihedral for specific paths ................................................................................... 1013
4.200.18.1 BLA and BOA of thiophene oligomer........................................................... 1013
4.200.18.2 Study variation of bond lengths, bond angles and dihedrals in the ring of
cyclo[18]carbon ...................................................................................................................... 1015
4.200.19 Examples of calculating spatial delocalization index........................................... 1017
4.200.20 Using bond order density and natural adaptive orbital to study chemical bonds . 1019
4.200.20.1 Plot bond order density for N2 molecule ....................................................... 1019
4.200.20.2 Study BOD and NAdO orbitals for C-C bonds in butadiene ........................ 1021
4.200.20.3 Study interaction between two fragments using BOD/NAdO....................... 1024
4.300 Other functions (Part 3) ................................................................................................. 1026
4.300.1 Example of visualizing free regions and calculating free volume in a cell ............ 1026
4.300.2 Example of fitting radial atomic density as STOs or GTFs ................................... 1029
4.300.2.1 Crudely fitting radial density of silicon as several STOs ................................ 1029
4.300.2.2 Accurately fitting radial density of bromine as many GTFs ........................... 1032
4.300.3 (moved to Section 4.24.5) ...................................................................................... 1033
4.300.4 Example of simulating scanning tunneling microscope (STM) image .................. 1033
4.300.4.1 Simulating constant height STM image for phenanthrene .............................. 1034
4.300.4.2 Simulating constant current STM image for phenanthrene ............................. 1035
4.300.5 Calculate electric dipole moment, multipole moments and electronic spatial extent for
uracil ................................................................................................................................... 1037
4.300.6 Calculating orbital energies: NTO orbital as an example....................................... 1039
4.300.8 Plotting surface distance projection map for [Ru(bpy)3]2+ cation coordinate ........ 1041
4.A Special topics and advanced tutorials ............................................................................... 1043
4.A.1 Study variation of electronic structure along IRC path ............................................. 1043
4.A.2 Calculation of spin population .................................................................................. 1048
4.A.3 Overview of methods for studying aromaticity ......................................................... 1049
4.A.4 Overview of methods for predicting reactive sites .................................................... 1053
4.A.5 Overview of methods for studying weak interactions ............................................... 1055
4.A.6 Calculate odd electron density .................................................................................. 1059
4.A.7 Visually revealing electron correlation in different regions ...................................... 1063
4.A.7.1 Fractional Occupation Number Weighted Electron Density (FOD) ................... 1063
4.A.7.2 Local electron correlation functions ................................................................... 1065
4.A.8 Analyze wavefunction higher than CCSD level ........................................................ 1066
4.A.9 Calculate TrEsp (transition charge from electrostatic potential) charges and analyze
exciton coupling .................................................................................................................. 1070
4.A.10 Intuitively exhibiting atomic properties by coloring atoms..................................... 1075
4.A.11 Overview of methods for studying chemical bonds ................................................ 1077
4.A.12 Overview of methods for analyzing electron excitation .......................................... 1084
4.A.13 Plot electrostatic potential colored van der Waals surface map and penetration graph of
van der Waals surfaces ........................................................................................................ 1090
4.A.14 Very easily rendering cube files as state-of-the-art isosurface map via VMD script
............................................................................................................................................. 1098

xix

--- page break ---

4.A.15 Calculating information-theoretic quantities and some relevant quantities............. 1100

5 Skills ................................................................................................. 1101
5.1 Make Multiwfn support more quantum chemistry programs ............................................ 1101
5.2 Running Multiwfn in silent mode ..................................................................................... 1101
5.3 Running Multiwfn in batch mode ..................................................................................... 1103
5.4 Copy outputs from command-line window to clipboard................................................... 1105
5.5 Make command-line window capable to record more outputs ......................................... 1106
5.6 Rapidly load a file into Multiwfn ...................................................................................... 1107
5.7 Make use of cubegen utility in Gaussian package to reduce computational time of electrostatic
potential analyses .................................................................................................................... 1107
5.8 Some tips on achieving ideal drawing quality .................................................................. 1109

6 Appendix ........................................................................................... 1111
6.1 Setting up running environment for Gaussian in Windows............................................... 1111
6.2 The routines for evaluating real space functions ............................................................... 1111
6.3 Detail of built-in atomic densities ..................................................................................... 1114
6.4 Details about supplying inner-core electron density for the wavefunctions involving
pseudopotential ....................................................................................................................... 1115
6.5 Check sanity of wavefunction ........................................................................................... 1116
6.6 Special functions ............................................................................................................... 1117
6.6.1 Add Bq atoms at specific positons ......................................................................... 1117
6.6.2 Calculate nuclear attractive energy between a fragment and an orbital ................. 1117
6.6.3 Output Becke's integration points .......................................................................... 1117
6.6.4 Make orbitals equivalent to basis functions ........................................................... 1118
6.6.5 Generating promolecular wavefunction ................................................................. 1118
6.7 About providing Fock/KS matrix to Multiwfn .................................................................. 1118

xx
