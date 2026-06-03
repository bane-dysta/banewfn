# 3.21 Electron excitation analysis (18)

## Quick navigation

- path: 3  Functions > 3.21 Electron excitation analysis (18)
- pdf pages: 253-297
- category: functions
- main menu / option numbers mentioned in title: 18
- direct child sections: 19
- total descendant sections: 21

## When to consult this file

Use this file for the actual Multiwfn function description: menu number, theory, options, inputs, and detailed behavior.

## Keywords and aliases

3.21, Electron, excitation, 3.21.A, information, about, module, 3.21.0, Check, modify, export, configuration, coefficients, an, 3.21.1, Analyze, visualize, hole, distribution, transition, density, electric, magnetic, dipole, moment, 3.21.2, Plot, atom, fragment, matrix

## Direct child sections

- **3.21.A Basic information about electron excitation analysis module** (pdf pages 253-257)
- **3.21.0 Check, modify and export configuration coefficients of an excitation (-1)** (pdf pages 258-258; menu/options: -1)
- **3.21.1 Analyze and visualize hole&electron distribution, transition density, and transition electric/magnetic dipole moment density (1)** (pdf pages 259-268; menu/options: 1)
- **3.21.2 Plot atom/fragment transition matrix of various kinds as heat map (2)** (pdf pages 269-272; menu/options: 2)
- **3.21.4 Calculate ∆r index to measure charge-transfer length (4)** (pdf pages 273-273; menu/options: 4)
- **3.21.3 Analyze charge-transfer based on density difference grid data (3)** (pdf pages 274-276; menu/options: 3)
- **3.21.5 Calculate transition electric/magnetic dipole moments between all states and for each state (5)** (pdf pages 277-278; menu/options: 5)
- **3.21.6 Generate natural transition orbitals (NTOs) (6)** (pdf pages 279-280; menu/options: 6)
- **3.21.7 Calculate ghost-hunter index (7)** (pdf pages 281-281; menu/options: 7)
- **3.21.8 Calculate interfragment charge transfer in electron excitation via IFCT method (8)** (pdf pages 282-284; menu/options: 8)
- **3.21.9 Generate and export transition density matrix (9)** (pdf pages 285-286; menu/options: 9)
- **3.21.10 Decompose transition electric/magnetic dipole moment as molecular orbital pair contributions (10)** (pdf pages 287-287; menu/options: 10)
- **3.21.11 Decompose transition electric/magnetic dipole moment as basis function and atom contributions (11)** (pdf pages 288-289; menu/options: 11)
- **3.21.12 Calculate Mulliken atomic transition charges (12)** (pdf pages 290-290; menu/options: 12)
- **3.21.13 Generate natural orbitals of specific excited states (13)** (pdf pages 290-290; menu/options: 13)
- **3.21.14 Calculate Λ index to characterize electron excitation (14)** (pdf pages 291-292; menu/options: 14)
- **3.21.15 Print major MO transitions in all excited states** (pdf pages 293-293)
- **3.21.16 Charge-transfer spectrum (CTS) analysis** (pdf pages 293-294)
- **3.21.17 Electron density polarization analysis based on electron excitations** (pdf pages 295-297)

## Extracted manual text

3 Functions
attractors into one by suboption 3 in option -6.

 9 Obtain atomic contribution to population of external basins: The external basins mean the
basin defined by a cube file named basin.cub in current folder, in which the grid value corresponds
to basin index. This option aims to obtain atomic contribution to population of ELF bond basins (or
other type of basins) based on AIM partition. Please check Section 4.17.7 for example.
 10 Calculate high ELF localization domain population and volume (HELP, HELV): This
option is used to calculate the HELP and HELV, which were defined in ChemPhysChem, 14, 3714
(2013) to characterize lone pair electrons. This option appears only when the real space function
used to partition basin is ELF. See Section 4.17.8 for example on using this option to calculate HELP
and HELV.
 11 Calculate orbital compositions contributed by various basins: Via this option, you can
calculate contribution to specific orbital contributed by AIM basins or other kinds of basins, such as
ELF basins. See Section 4.8.6 for example.
 12 Assign ELF basin labels: This option is used to automatically assign labels for all basins
when the real space function used to generate basins is ELF, the basin volumes and populations are
also printed together. Examples of assigned labels: C(F2), V(O3), V(S5,F7), V(Li1,Li2,Li3). The
labels are outputted twice, at the first time the data are outputted according to basin indices, at the
second time the data are outputted according to sorted basin labels. See Section 4.17.2 for example.
Algorithm detail of automatic assignment of ELF basin labels:
Core basins are assigned first. If distance between an ELF attractor and a nucleus is smaller than a threshold,
then the corresponding basin will be assigned as core type. The threshold distances are built-in and different for
different elements. For an element, the threshold was determined as the position of outermost minimum of radial
curve of spherically averaged ELF of the atom based on high-quality atomic wavefunction of ground state. The builtin thresholds have been determined for H~Lr, therefore if the system contains element(s) heavier than Lr then this
option cannot be used, namely you have to manually determine the labels by visualizing attractor positions and/or
spatial range of basins.
After that, all other basins will be labelled as valence type. If any grid of an attractor is next to a grid of a core
basin, then the atom corresponding to the core basin will be added to the member list of this attractor. Assume that
finally an attractor has members of C1 and O2, then its label will be V(C1,O2). H and Ne are relatively special, if
distance between an attractor and nucleus of H or Ne is smaller than 0.2 Bohr, then the atom will be added to the
member list.
Notice that basin labels cannot be correctly assigned for elements using pseudopotential, because in this case
core basins cannot be assigned.

For options 3, 4, 5, 7 and 8, if the input file you used does not contain GTF information
(e.g. .cub file is used as input file and you directly use the grid data carried by it to generate basins),
then Multiwfn will prompt you to input a new file, which should contain GTF information of present
system, you can use for example mwfn/.wfn/.wfx/.fch/.molden/.gms as input.
Many examples of this module can be found in Section 4.18.
Information needed: GTFs or grid data loaded from external file (e.g. .cub), atom coordinates

3.21 Electron excitation analysis (18)
3.21.A Basic information about electron excitation analysis module
1 Overview

232

--- page break ---

3 Functions
Main function 18 contains a lot of subfunctions aiming for electron excitation analysis, namely
characterizing the electron excitation in various ways. All functions in this category fully support
single-reference methods (i.e. reference wavefunction for generating excited state wavefunction is
single Slater-determinant), including TDDFT, TDA-DFT, CIS and TDHF, while ZINDO is also
supported by transition density matrix plotting function. Other kinds of methods for excited state
problems such as EOM-CCSD, LR-CC2/3, CASSCF, CASPT2 and MRCI are not formally
supported. Examples of some of these electron excitation analysis functions are provided in Section
4.18.
Both closed-shell and open-shell systems are fully supported by all kinds of electron excitation
analyses of Multiwfn.

2 Basic knowledge about single-reference methods
Excited state wavefunction (exc) of CIS and TDA-DFT methods can be represented as
occ vir

 exc =  wia ia   wia ia
i→a

i

a

where i and a respectively run over all occupied and all virtual MOs, similarly hereafter in. 𝑎𝑖 is
the configuration state wavefunction corresponding to moving an electron from originally occupied
MO i to virtual MO a. w is known as configuration coefficient. The electron excitation in CIS or
TDA-DFT framework therefore can be represented as linear combination of orbital pair transitions.
The weighting coefficients w satisfy this normalization condition:

 (w ) = 1.0

i →a

a 2
i

Clearly, the i→a orbital pair transition has contribution of 100% × (𝑤𝑖𝑎 )2 to the electron excitation.
While for TDHF and TDDFT, excited state wavefunction also contains so-called de-excitation
part:

 exc =  wia ia +  wia ia
i →a

i a

where w and w' correspond to configuration coefficient of excitation and de-excitation, respectively.
In this cases, the normalization condition becomes:

 (w ) −  (w ) = 1

i →a

a 2
i

a 2

i a

i

The MOs used for CIS/TDHF and TDA-DFT/TDDFT are yielded by HF and DFT calculation
for ground state of present system, respectively. The Slater determinant consisted of the occupied
MOs, namely the ground state wavefunction, is known as reference state. If the reference state is
closed-shell, then  and  MOs are exactly matched with each other, and thus → orbital
transitions have one-to-one correspondence with → orbital transitions; in this situation, only one
set of orbital transition is recorded, and correspondingly, the configuration coefficients are
normalized to 0.5 instead of 1.

3 Input files
Input file of almost all electron excitation analysis functions are basically the same, except for
subfunction 3 (Analyzing charge transfer based on density difference grid data). Two kinds of input
files are needed:

233

--- page break ---

3 Functions
(1) A file containing basis function and molecular orbital information.
mwfn, .fch/.fchk, .molden and .gms files produced by excited state calculation (e.g. TDDFT) can be
directly used. This file should be loaded when Multiwfn boots up.
(2) A file containing configuration coefficients of excited states. The path of this kind of file
should be inputted when you enter corresponding analysis function, Multiwfn will load
configuration coefficients from this file. There are several situations, as shown below:
 Gaussian users: Output file (.out or .log) of CIS, TDHF, TDDFT and TDA-DFT tasks can
be used. Both single point and optimization tasks are supported; for the latter case, Multiwfn
analyzes electron excitation at the final geometry. Since by default Gaussian only outputs the
configuration coefficients whose absolute value is larger than 0.1, In order to achieve acceptable
accuracy, you must add IOp(9/40=4) keyword in the route section so that all configuration
coefficients whose magnitude larger than 0.0001 will be printed (If the calculation in Multiwfn is
found to be too expensive, using IOp(9/40=3) instead is also generally acceptable). Implicit
solvation model, including external iteration (state specific) treatment of solvent response to
transition, is fully compatible.
 ORCA users: Output file of CIS and TDA-DFT tasks can be used. Beware that TPrint
keyword should be used within %cis or %tddft, otherwise only very small amount of configuration
coefficients will be printed. TPrint x means outputting configurations whose contribution to excited
state larger than x*100%. Typically, I suggest using TPrint 1E-8. Since contribution is calculated as
square of configuration coefficient, TPrint 1E-8 simply corresponds to outputting configurations
who have absolute value of coefficients larger than 1E-4, the effect is identical to IOp(9/40=4) in
Gaussian. Below is an example input:
! PBE0 def2-SVP
%tddft
nroots 8
tprint 1E-8
end
IMPORTANT NOTE: It is also possible to use ORCA TDDFT/TDHF output file, but the analysis result may
be unreliable or even fully wrong!!! Because in TD task, ORCA only prints configuration contributions (which are
given as sum of excitation and de-excitation contributions) but does not print configuration coefficients for excitation
and de-excitation respectively. In this case, Multiwfn automatically generates configuration coefficients by
calculating square root of the configuration contributions. This treatment is sometimes reasonable, however when
de-excitation is significant, the configuration coefficients yielded in this manner must be nonsense; in addition, even
if de-excitation is completely zero, the result may still be incorrect, because actual configuration coefficients may
either be positive or negative, while the sign evidently cannot be determined from configuration contributions.

Spin-flip TDDFT output file of ORCA is also supported, you just need to add SF TRUE
into %tddft field and set spin multiplicity of reference state  3. Note that only a few functions,
including generating natural orbitals of excited state, hole-electron analysis and related analyses, are
formally supported, other functions were not tested. In particular, all analyses directly based on
transition density matrix are not supported in this case.
 ORCA users using sTDA or sTDDFT: They are approximations of regular TDA and
TDDFT, respectively. In ORCA, their calculations are based on DFT MOs. Once the single point
task of DFT has finished, the excited states will be calculated by sTDA/sTDDFT with negligible
cost. To carry out these calculations, use keywords like follows (see ORCA manual for details)
! wB97X-D3 def2-SV(P) def2/J RIJCOSX

234

--- page break ---

3 Functions
%tddft
Mode sTDDFT //The sTDDFT may also be changed to sTDA
Ethresh 7.0
PThresh 1e-4
PTLimit 30
maxcore 6000
end
It is important to note that, at least for ORCA 4.2, only the three largest configuration
coefficients are printed (unfortunately, TPrint does not work for sTDA/sTDDFT calculation),
therefore often the normalization condition of configuration coefficients is violated evidently, in this
case the analysis result is unreliable or even fully misleading! So, please take care of the "Deviation
to expected normalization value" shown on screen after loading selected excited state.
 BDF users: Output file of TDDFT task of BDF program can be used.
 CP2K users: Periodic TDDFT (with/without sTDA kernel) task of CP2K is supported.
Currently, only hole-electron analysis, NTO analysis, “Generate natural orbitals of specific excited
states “, “Check, modify and export configuration coefficients of an excitation” and “Print major
MO transitions in all excited states” are formally supported, other analyses may or may not work
(at least I have not tested).
After booting up Multiwfn, the .molden file containing all virtual orbitals and cell information
should be loaded. Then, after you enter an analysis module, output file of CP2K should be loaded.
It is noteworthy that using Multiwfn to prepare input file of TDDFT task of CP2K is quite easy.
After booting up Multiwfn, load a structure file first (for crystal, often .cif is used, see Section 2.9.3
for detail), then input cp2k and the path of the input file to export. After that, you may first enter
option -11 and then suboption 19 to extend the current cell to a supercell if needed. Then return to
the interface of creating CP2K input file, choose option 15 to enable TDDFT calculation, then input
y to allow CP2K to generate the .molden file containing all orbitals and specify how many virtual
orbitals to solve and record in the .molden file. Finally, choose option 0 to yield CP2K input file.
Then use CP2K to run the input file, after the calculation is finished, manually insert cell information
to .molden file (as mentioned in Section 2.9.2.1). Now the .molden file and output file can be used
for electron excitation analyses.
Note that the virtual orbitals recorded in .molden file should cover all virtual orbitals involved in the printed
configurations. If you do not know how many virtual orbitals should be calculated, simply set it to a very large value
so that all virtual orbitals will be solved and recorded, however in this case the .molden file may be quite large. You
can also perform TDDFT once, then check the printed configurations and find the highest virtual orbital, then
properly set the number of virtual orbitals to solve in the input file and redo a single point calculation to generate
the .molden file.

 General cases: You can also use plain text file as the input file. The format of transition
information should be completely identical to Gaussian output, for instance: (the // and all text after
it should not appear in your file)
Excited State
5 ->

1

1

6

5.7945

// Label, index, multiplicity and excitation energy (eV)

0.70642

// MO pairs and configuration coefficients
// Use a blank line to separate each excited state

Excited State
5 ->

7

2

1

7.8943
0.63860

235

--- page break ---

3 Functions
5 ->

8

Excited State
5 ->

0.30006

3

1

7

7.8943
-0.30006

5 ->

8

0.63860

4 <-

8

0.01000

Example of unrestricted TDDFT calculation is given below. Note that spin multiplicity is set
to 0, meaning undefined, since the excited states produced by this kind of calculation are not pure
spin states.
Excited State

1

0

2.07774

600B -> 601B

-0.676085

598A -> 602A

0.454805

600A -> 603A

0.416875

...ignored

Excited State

2

0

2.07792

599B -> 601B

0.561757

598A -> 601A

0.496762

600B -> 603B

0.468381

...ignored

Evidently, the above mentioned two kinds of files must correspond to the same geometry and
same calculation level. For example, if the MOs in the .fch were produced at B3LYP/6-31G* level
while the Gaussian output file corresponds to the TDDFT task carried out at PBE0/6-31G* level,
the analysis results will be completely meaningless.
 Special case for GAMESS-US and Firefly users: If you are a user of Firefly or GAMESSUS program, you do not need to separately provide two kinds of files as mentioned above for
electron excitation analysis. If the input file used for Multiwfn is TDDFT output file with .gms suffix,
the Multiwfn will not only load basis function and molecular orbital information from this file when
Multiwfn boots up, but also load configuration coefficients of excited states when performing
electron excitation analysis. The H2CO_TDDFT_Firefly.gms and H2CO_TDDFT_GAMESS.gms in
"examples\excit" folder are example file of TDDFT output file of Firefly and GAMESS-US,
respectively.
For Firefly user, you should decrease the "PRTTOL" parameter in $TDDFT so that more
configuration coefficients could be printed.
For GAMESS-US, there is no option used to control the printing threshold of configuration
coefficients, therefore some analysis results may be not very accurate because some configurations,
which have nonnegligible contributions, may be ignored. In addition, CIS task of GAMESS-US is
not supported by Multiwfn.
Output file of excited state optimization and frequency tasks of GAMESS-US and Firefly is
not supported.

236

--- page break ---

3 Functions

3.21.0 Check, modify and export configuration coefficients of an
excitation (-1)
This function allows one to check, modify and export coefficient of configuration coefficients.
The input files needed by this function have been detailedly described at the beginning of Section
3.21, namely you should load a file containing basis function information when Multiwfn boots up,
then load a file containing configuration coefficients of excited states. The summary of all
recognized excited states will be printed on screen, you should select one of them, the configuration
coefficients of orbital pairs involved in this electron excitation will be loaded. After that, up to 10
orbital pairs that have largest absolute contribution to the excitation are automatically shown. Then
in the newly appeared menu, you can find below options:
1 Set coefficient of an orbital pair: You can use this option to replace the loaded configuration
coefficients of an orbital pair with inputted value.
2 Set coefficient for specific range of orbital pairs: This option is used to replace a batch of
loaded configuration coefficients with inputted values. You should input index range of the occupied
MOs and virtual MOs corresponding to the orbital transitions.
After manually modifying coefficients use above two options, in order to make the
modification affect following electron excitation analysis, you should use option -3 to export the
modified coefficients as plain text file, and then use this file as the second kind of input file for
electron excitation analyses.
-1 Retrieve original coefficient of all orbital pairs: If configuration coefficients have been
manually modified by above two options, you can select this option to retrieve the coefficients to
the original loaded values.
-2 Print coefficient (and contribution to excitation) of some orbital pairs: You can use this
option to print configuration coefficients whose absolute value are larger than specific value,
meantime the corresponding contributions to the electron excitation are shown together. Via this
option you can easily find out which orbital pair transition has crucial contribution to the electron
excitation.
-3 Export current excitation information to a plain text file: Basic information and
configuration coefficients of currently selected excited state can be exported to a specific plain text
file. This file can then be employed as the second kind of input file for various electron excitation
analysis functions of Multiwfn, e.g. hole-electron analysis and NTO analysis, and then the analysis
result will correspond to the modified configuration coefficients.
Obviously, if you set coefficient of some orbital pairs to zero, then their contributions to the
quantities you studied will be completely ignored; while if you have cleaned all coefficients except
for a specific orbital pair, then the resulting quantities will only reveal characters of this orbital
transition.
The example given in Section 4.18.10 utilized this function.

237

--- page break ---

3 Functions

3.21.1 Analyze and visualize hole&electron distribution, transition
density, and transition electric/magnetic dipole moment density (1)
This very powerful module is used to analyze and visualize hole-electron distribution,
transition density and transition electric/magnetic dipole moment density. Moreover, hole and
electron can be decomposed to orbital pair contributions as well as atom and fragment contributions;
furthermore, the atom/fragment contributions can be directly plotted as heat map for visual
inspection.

3.21.1.1 Theory
There are many knowledge points involved in this module, they will be described below first.
Theory 1: Real space representation of hole and electron
Process of single-electron excitation can be described as "an electron leaves hole and goes to
electron", the "hole" and "electron" can be defined in different ways. If an excitation can be perfectly
described as HOMO→LUMO transition, then hole and electron could be simply represented by
HOMO and LUMO, respectively. However, in most practical cases, the single orbital pair
representation is not suitable, excitations have to be represented as transition of multiple MO pairs
with corresponding weighting coefficients.
How to represent hole and electron distributions when there is no single dominant MO pair
transition? One way is using natural transition orbital (NTO) analysis, as introduced in Section
3.21.6. Unfortunately, in many cases, even the MOs have been transformed to NTOs, there is still
no single NTO pair that has dominating contribution. The best representation of hole and electron
may the one introduced in this Section, the idea was originally proposed by me and my collaborator
Cheng Zhong in 2013. Although the paper detailedly introducing this method has not been published,
if this theory is involved in your study, please cite my this work: Carbon, 165, 461-467 (2020) DOI:
10.1016/j.carbon.2020.05.023, in which hole-electron analysis is utilized and briefly described.
It can be shown that density distribution of hole and electron can be perfectly defined as
hole
hole
 hole (r ) = (loc)
(r ) + (cross)
(r ) =  ( wia )2  i (r ) i (r ) +   wia waj  i (r ) j (r )
i →a

 (r ) = 
ele

ele
(loc)

(r ) + 

ele
(cross)

i →a j i →a

(r ) =  ( w )  a (r ) a (r ) +   wia wib a (r) b (r)
i →a

a 2
i

i → a i →b  a

note that the notions used here:
occ vir

  

i →a

i

a

occ occ vir

   

i →a j i →a

i

j i a

where  denotes MO wavefunction. "loc" and "cross" stand for the contribution of local term and
cross term to the hole and electron distribution, respectively. Note that the definition of hole and
electron given above is in density form rather than wavefunction form, hence the hole and electron
do not have phase (If you really need phase information of hole and electron, you should resort on
NTO analysis, see Section 3.21.6).
Due to the orthonormality of MOs and the fact that the sum of square of all configuration
coefficients is 1.0, it is clear that



hole

  (r ) d r = 1

(r ) d r = 1

ele

238

--- page break ---

3 Functions
This is an important property that any reasonable definition of hole and electron distribution should
satisfy, it indicates that one electron is excited.
The overlap function between hole and electron distribution can be defined as

S m (r ) = min[  hole (r ),  ele (r )]
namely taking the minimal value of hole and ele everywhere. Another function for measuring the
overlap is

S r (r ) =  hole (r )  ele (r )
It is evident that Sr is always equal or larger than Sm. Both the two definitions are reasonable, but I
prefer to use Sr, since its graphical effect is better and its mathematical meaning is more clear.
The charge density difference (CDD) between excited state and ground state can be easily
evaluated as

 (r ) =  ele (r ) −  hole (r )
NOTE: Beware that if you are a Gaussian user, the  calculated in this way is obviously different
to the  produced via subtracting excited state density by ground state density, unless you specified
keyword density=rhoci when generating .wfn/wfx file of excited state. Because by default the
excited state density exported to .wfn/wfx file by Gaussian is relaxed density rather than unrelaxed
density (which is directly constructed by MOs and excited state configuration coefficients). In other
words, unrelaxed excited state density can be simply written as 𝜌excited (𝐫) = 𝜌ground (𝐫) −
𝜌hole (𝐫) + 𝜌ele (𝐫) , while deriving relaxed excited state density requires employing the very
complicated "Z-vector" method.
After generalization, above definitions of hole and electron can also be applied to TDHF and
TDDFT cases, where de-excitations must be taken into account. The generalized local terms are
hole
(loc)
=  ( wia )2 i −  ( wia )2 i
i →a



ele
(loc)

i a

=  ( w ) a −  ( wia )2 a
i →a

a 2
i

i a

where i=|i|2 stands for electron density of orbital i, w' denotes configuration coefficient of deexcitation. The generalized cross terms are
hole
 (cross)
=   wia waj i j −   wia wjai j
i →a j i →a

i a j i a

ele
 (cross)
=   wia wib a b −   wia wib a b
i →a i →b  a

i  a i b  a

If combining local and cross terms together, the expressions of hole and electrons could be
simply expressed as

 hole =  wia waj i j −  wia wja i j
i, j →a

i, ja

 ele =  wia wib a b −  wia wib a b
i → a ,b

i  a ,b

Theory 2: Contribution of MOs, basis functions, atoms and fragments to hole and
electron distributions

239

--- page break ---

3 Functions
In order to investigate which MOs have significant contributions to hole and electron, I defined
the contribution of occupied MO to hole and contribution of virtual MO to electron as follows

ihole = [(wia )2 − ( wia )2 ]

a 2
a 2
ele
a = [( wi ) − ( wi ) ]

a

i

Below normalization conditions are held evidently:



hole
i

 = 1

=1

ele
a

i

a

Contribution to hole/electron by an atom can be easily evaluated using real space partition like
Hirshfeld, Hirshfeld-I and Becke. For example, contribution to hole by atom A using Hirshfeld
partition:

hole
=  wAHirsh (r )  hole (r ) d r
A
where 𝑤𝐴Hirsh is weighting function of atom A under Hirshfeld partition, see Section 3.9.1 for its
detail.
Mulliken-like partition is also possible, and the working equation is derived as follows.
Considering the normalization condition of the hole (de-excitation part is temporarily ignored for
simplicity)





   w w    d r = 1
i , j →a

a
i

a
j

i

j



a a
w
w
C
C



dr = 1


i
j

,
i

,
j


  i , j →a
 

C C     d r = 1
 w w 
 

i , j →a

a
i

a
j

,i

,j

C C S   = 1
 w w 
 

i , j →a

a
i

a
j

,i

,j

,

where  denotes basis function, S and C are overlap matrix and coefficient matrix, respectively. If
we employ Mulliken-like method to partition the

C C S  term as atomic contributions,

 
,i

,j

,

then we can define contribution of atom A to hole in below form



 hole
=  wia waj 12   C ,i C , j S  , +  C ,i C , j S  , 
A
i, j →a
  A
  A 

Above treatment can be similarly applied to de-excitation part of hole as well as electron. The
actual working equations used to evaluate atomic contribution to hole and electron are





hole
=  wia waj 12   Tij, +  Tij,  −  wia wja 12   Tij, +  Tij, 
A
i , j →a
  A
  A
  A 
 i , j a
  A 


a b 1
ab
ab 
ab
ab 
a b 1
ele
A =  wi wi 2   T , +  T ,  −  wi wi 2   T , +  T , 
i → a ,b
  A
  A
  A 
 i  a ,b
  A 

where T is an intermediate matrix for facilitating calculation, it is defined as T , = C ,i C , j S  , .
ij

240

--- page break ---

3 Functions
Contribution of a basis functions  to hole and electron can be defined as





hole
=  wia waj 12  Tij, + Tij,   −  wia wja 12   Tij, +  Tij,  

i , j →a


 
 i , j a
 


a b 1
ab
ab 
ab
ab 
a b 1
ele
 =  wi wi 2   T , +  T ,   −  wi wi 2   T , +  T ,  
i → a ,b


 
 i  a ,b
 

In order to significantly save computational time, Multiwfn ignores all terms if magnitude of product of
corresponding two configuration coefficients is less than 0.001. The loss of accuracy due to this trick is negligible.

For both Mulliken-like and Hirshfeld partitions, fragment contributions to hole and electron
can be simply evaluated by summing up atomic contributions:
hole
hole
frag =   A

ele
ele
frag =   A

A frag

A frag

Furthermore, I defined contribution of atom and fragment to charge density difference
(variation of electron population of the atom and fragment) as
hole
CDD
= ele
A
A − A

ele
hole
CDD
frag =  frag −  frag

Overlap between hole and electron in atom and fragment spaces are defined as geometry
average of their contributions:
hole
ovlp
= ele
A
A A

hole
ovlp
ele
frag =
frag  frag

Notice that the overlap in this form is not additive, namely  A +  B
ovlp

ovlp

 ovlp
AB .

Mulliken-like partition works reasonably for most cases, however, it is incompatible with
diffuse functions. Another well-known shortcoming of this partition is that some atomic
contributions may be small negative values in certain situations, obviously in this case the overlap
between hole and electron in corresponding atomic spaces cannot be evaluated, so Multiwfn
automatically sets the overlap values to zero. Obviously, when diffuse functions must be employed
(e.g. anionic system, Rydberg excited state), or you have observed notable negative atomic
contribution to hole or electron, you have to change to Hirshfeld partition, which is more robust but
computational cost is higher.
Mulliken-like and Hirshfeld partitions can be directly selected in hole-electron analysis module.
Due to the extreme flexibility of Multiwfn, you may also use other ways to determine atomic
contributions to hole and electron, such as Becke and Hirshfeld-I partitions. However, you have to
manually evaluate them. For example, if you want to employ Becke partition for hole and electron,
you should first export cube file of hole or electron, then set "iuserfunc" in settings.ini to -1 (in this
case the user-defined function will correspond to the interpolated function based on the grid data),
then load hole or electron cube file into Multiwfn, use subfunction 1 of main function 15 to integrate
"user-defined function" in each Becke's atomic fuzzy space. Note that if in the main function 15,
you first select option -4 to define a fragment and then use subfunction 1 to integrate user-defined
function, then the sum of results of all atoms will correspond to the fragment contribution.
Theory 3: Quantitative characterization of hole and electron distribution in the whole
space

241

--- page break ---

3 Functions
The overall distribution of hole and electron can be quantitatively characterized in following
ways, they are quite useful for identifying type of electron excitations.
To characterize overlapping extent of hole and electron, Sm index and Sr index are defined as
follows (Sr must be equal or larger than Sm index)

Sm index =  Sm (r ) d r   min[  hole (r ),  ele (r )]d r
Sr index =  Sr (r ) d r    hole (r )  ele (r ) d r
Centroid can be calculated to reveal most representative position of hole and electron
distribution. For example, X coordinate of centroid of electron is written as

X ele =  x ele (r ) d r
where x is X component of position vector r.
The charge transfer (CT) length in X/Y/Z can be measured by distance between centroid of
hole and electron in corresponding directions:

Dx = X ele − X hole

Dy = Yele − Yhole

Dz = Z ele − Z hole

The total magnitude of CT length is referred to as D index:

D index =| D | ( Dx )2 + ( Dy )2 + ( Dz )2
It is noteworthy that the variation of dipole moment of excited state (corresponding to
unrelaxed density) with respect to ground state in X, Y and Z can be simply calculated as

x = −( X ele − X hole )

y = −(Yele − Yhole )

z = −( Z ele − Z hole )

The RMSD of hole and electron can be used to characterize their extent of spatial distribution.
For example, X component of RMSD of hole is expressed as

 hole,x =

 (x − X

hole

)2  hole (r )d r

The |hole| and |ele| are referred to as hole and ele indices, they measure overall RMSD of hole and
electron, respectively.
The difference between RMSD of electron and hole in X/Y/Z direction can be measured via
, while overall difference can be measured via  index

  =  ele,  −  hole, 

 = {x, y, z}

 index =| σ ele | − | σ hole |
H measures average degree of spatial extension of hole and electron distribution in X/Y/Z
direction, HCT is that in CT direction, and H index is an overall measure

H  = ( ele,  +  hole,  ) / 2

 = {x, y, z}

H CT =| H  uCT |
H index = (| σ ele | + | σ hole |) / 2

242

--- page break ---

3 Functions
where uCT is unit vector in CT direction and can be straightforwardly derived using centroid of hole
and electron.
t index is designed to measure separation degree of hole and electron in CT direction:

t index = D index − H CT
If t index<0, it implies that hole and electron is not substantially separated due to CT. Clear
separation of hole and electron distributions must correspond to evidently positive t index.
The hole delocalization index (HDI) and electron delocalization index (EDI) are defined as
follows

HDI = 100 

 [

EDI = 100 

 [  (r)] d r

hole

ele

(r )]2 d r
2

It is found that the smaller the HDI (EDI), the larger the spatial delocalization of hole (electron); in
other words, the more evenly distributed throughout the system. HDI and EDI are pretty useful in
quantifying breadth of spatial distribution (although |hole| and |ele| can also reveal this point, they
are not suitable when hole or electron are concentrated in multiple areas).
There are often many nodes or complicated fluctuations in hole and electron distributions. In
order to make visual study of hole and electron easier, Chole and Cele functions are defined as follows.
The function behavior of Chole and Cele is similar to Gaussian function, they are highly smooth
functions, the value asymptotically approaches zero from centroid of hole/electron.

 ( x − X ele ) 2 ( y − Yele ) 2 ( z − Z ele ) 2 

Cele (r ) = Aele exp  −
−
−
2
2
2


2

2

2

ele,
x
ele,
y
ele,
z


 ( x − X hole ) 2 ( y − Yhole ) 2 ( z − Z hole ) 2 

Chole (r ) = Ahole exp  −
−
−
2
2
2


2

2

2

hole, x
hole, y
hole, z


The factor A is introduced so that Chole and Cele are normalized.
In fact, the definition of RMSD, Chole, Cele, H and t indices introduced above was motivated by
J. Chem. Theory Comput., 7, 2498 (2011), these quantities were originally used to analyze electron
excitation based on density difference, but I found all of them work well under the framework of
hole-electron analysis. Also note that many details of these indices have been modified when
introduced to hole-electron analysis framework.
The above defined quantitative indices could be used for distinguishing type of electron
excitation. My empirical rule is summarized as follows, it should be suitable for most cases.
Excitation type

Index
D

Sr

t

LE

small

medium ~ large

<0

∆
small

Single direction CT

large

?

?

?

243

--- page break ---

3 Functions
Centrosymmetric CT

small

?

<0

large

Rydberg

small

Small

<0

large

In the table, three kinds of excitations are involved:
• Local excitation (LE): The hole and electron occupy similar spatial region.
• Charge-transfer excitation (CT): The spatial separation of hole and electron is large, leading
to evident displacement of charge density. The CT may be single directional or multiple directional
(centrosymmetric CT is a special case of the latter).
• Rydberg excitation: Electron mainly consists of very diffuse MOs, therefore the overlap
between electron and hole must be small. This type of excitation in general does not lead to
prominent long-range displacement of charge density.
Theory 4: Transition density matrix and transition density
(One-electron, spinless) transition density matrix between excited state and ground state of an
N-electron system in real space representation is defined as follows (real type of wavefunctions is
assumed, so complex conjugation sign is omitted)

T (r; r ')  T (r1; r1 ') =   0 (x1 , x 2 ,

x N ) exc (x1 ', x 2 ,

x N ) d 1 d x 2 d x3

dx N

where 0 is Slater-determinant of ground state wavefunction. x is spin-space coordinate,  stands
for spin coordinate. The T is called as matrix because it has two continuous indices.
For excited state wavefunction generated by single-reference methods, after expanding exc
and applying Slater-Condon rule, it can be easily shown that T can be explicitly written as

T (r; r ') =  wiai (r) a (r ')
i

a

If we only take the diagonal terms of the transition density matrix, then we obtain transition
density

T (r) =  wiai (r) a (r)
i

a

T(r) can be studied as a common real space function, for example, visualized in terms of isosurface
map. Assuming that there is only one dominant orbital transition, for example, HOMO→LUMO,
then T(r) is simply HOMO(r)LUMO(r). Therefore, it is easy to understand, if a region has large
magnitude of transition density, the hole and electron must be strongly coupled in this region; while
if a region has small distribution of T(r), then overlap between hole and electron in this area should
be insignificant. Clearly, T(r) is a useful function for characterizing underlying nature of electron
excitation, and its main distribution characteristics is closely related to the Sr(r) function.
Note that due to the orthonormality of MOs, integral of T(r) over the whole space is exactly
zero. If the excited state and ground state correspond to different spin states, due to the
orthonormality of spin coordinate, T(r;r') must be a zero matrix, and T(r) is correspondingly zero
everywhere. However, notice that only spatial part of T(r) is taken into account when Multiwfn
evaluates it, therefore you are still able to study T(r) for e.g. S0→T1 excitation.
Theory 5: Transition electric/magnetic dipole moment density
Note that there are many kinds of transition dipole moment, including transition electric dipole
moment, transition magnetic dipole moment, transition velocity dipole moment and so on. The word
"transition dipole moment" commonly refers to transition electric dipole moment.
X, Y and Z components of transition electric dipole moment density can be written as negative

244

--- page break ---

3 Functions
of product of X, Y and Z coordinate variables and transition density, respectively:

Tx (r ) = − xT (r )

Ty (r ) = − yT (r )

Tz (r ) = − zT (r )

Integrating transition electric dipole moment density over the whole space yields transition
dipole moment D

Dx =  Tx (r ) d r

Dy =  Ty (r ) d r

Dz =  Tz (r ) d r

Obviously, one can conveniently study contribution to transition electric dipole moment of various
molecular regions by plotting transition electric dipole moment density.
Next, we look at transition magnetic dipole moment. The operator for magnetic dipole moment
due to movement of electrons is the angular momentum operator L (see e.g. Theor. Chim. Acta, 6,
341 (1966))

L = −i (r  ) = ˆiLx + ˆjLy + kˆ Lz
  
 
 

 
 
= −i ˆi  y − z  + ˆj  z − x  + kˆ  x − y 
y 
z 
x 
 x
 y
  z
where i, j, k are unity vectors in X, Y and Z directions, respectively. Therefore, the X component of
transition magnetic dipole moment can be explicitly defined as below. Noticed that in order to
provide a real value I ignored the imaginary and negative signs simultaneously; the symbol "←"
denotes de-excitation MO pairs in TDHF/TDDFT formalism.

M x = 0 y


 exc




−z
 =  wia  i y − z  a −  wjb  j y − z  b
z
y
z
y
z
y
i→a
j b

My and Mz can be defined similarly.
We can define transition magnetic dipole moment density component mi(r) by considering the



relationship M i = mi (r ) d r i = x, y, z , so that distribution of transition magnetic dipole
moment can be visualized in terms of e.g. isosurface map. Explicit expression of mi(r) of X
component is given below, Y and Z components can be defined similarly.

 

 



mx (r ) =  wiai (r )  y a (r ) − z a (r ) −  wjb j (r )  y b (r ) − z b (r )
y
y
i →a
 z
 j b
 z

Theory 6: Coulomb attraction between hole and electron (exciton binding energy)
The "electron" of course carries negative charge, while "hole" can be regarded as carrying
positive charge, therefore formally there is a Coulomb attractive energy between them, its negative
value is known as exciton binding energy, which is a positive value. This term can be calculated via
simple Coulomb formula (in atomic unit form):

EC =  

 hole (r1 )  ele (r2 )
| r1 − r2 |

d r1 d r2

Some discussions about the exciton binding energy can be found in e.g. J. Chem. Phys., 143, 244905
(2015) and J. Phys. Chem. C, 121, 17088 (2017).
Note that the exciton binding energy calculated in above form is different to the exciton binding energy defined
in another form, namely EC=(IP-EA)-Eoptical gap (see Mater. Horiz., 1, 17 (2014) for more details), because electronic

245

--- page break ---

3 Functions
correlation and orbital relaxation effects are involved in practical electron ionization and electron affinity processes;
moreover, in fact there is an exchange term in EC (though it is negligible when separation of hole and electron is
significant). All of these factors are ignored in the evaluation of exciton binding energy in Multiwfn.

In Multiwfn, above integral is directly calculated based on evenly distributed grid data of hole
and electron. Notice that although the code has been substantially optimized and parallelized, the
computational cost is still high, therefore you need to wait patiently during calculation. The cost is
formally proportionally to square of the number of grids; therefore, the cost of medium quality grid
will be higher than low quality grid by one order of magnitude.

3.21.1.2 Usage and Functions
The input files needed by present module have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states. The
summary of recognized excited states will be printed on screen, you should select the excited state
that you want to carry out aforementioned analyses. Each time only one state can be analyzed, if
you want to analyze another state, you should exit this function, then enter again and select another
state.
Present module has many functions, they will be described below in turn.
Function 1: Visualize and analyze hole, electron and transition density and so on
After you enter this function, you are requested to set up grid data, then grid data will be
calculated for hole distribution, electron distribution, overlap of hole and electron, transition density,
transition electric/magnetic dipole moment density, charge density difference and Cele/Chole
functions.
After calculation of grid data is finished, various quantities introduced in Section 3.21.1.1 will
be evaluated based on the evenly distributed grid data and then shown on screen, their meanings
should be very easy to understand. The outputted transition electric/magnetic dipole moment is
calculated by integrating grid data of transition dipole moment density, the value should be very
close to the one directly outputted by quantum chemistry program. The ideal value of the integral
of hole or electron over the whole space is 1.0, while for transition density the ideal value is 0. If
the actual outputted values deviate too far from expected values, then the printed t index, H index,
D index, Sm index and so on may be unreliable. There are three reasons may lead to this problem:
(1) The grid quality is too poor. Higher number of grid points should be used
(2) The spatial extent of the grid data is too narrow, you should enlarge extension distance so
that the grid data could cover broader regions
(3) You forgot to use the IOp(9/40=x) option mentioned at the beginning of Section 3.21, as a
result, only very small number of configuration coefficients are loaded
In post-processing menu, grid data of hole, electron, transition density, Sm/Sr and so on can be
directly visualized as isosurface map, or be exported as cube file in current folder by corresponding
options. You can also choose corresponding option to calculate Coulomb attractive energy between
hole and electron distribution, notice that this calculation is expensive even if you only choose low
quality grid.
By default, the transition magnetic dipole moment density is not evaluated because it is less
important than the transition electric dipole moment density. If you want to calculate it, select option

246

--- page break ---

3 Functions
-1 before entering this function.
For large systems, if computational cost for grid data is too high and you only need to
qualitatively examine isosurface map of hole, electron, transition density and so on, in Gaussian you
can safely use IOp(9/40=3) instead of IOp(9/40=4), so that smaller number of configurations will
be taken into account.
Function 2: Show molecular orbital contribution to hole and electron distribution
You only need to input printing threshold, then contribution of MO to hole and electron
distribution will be shown. This function is very useful to identify which MOs have significant
contribution to hole and electron. Below is an output example:
MO

126, Occ:

2.00000

Hole:

0.29664

Electron:

0.00000

MO

127, Occ:

2.00000

Hole:

0.19783

Electron:

0.00000

MO

128, Occ:

2.00000

Hole:

0.38666

Electron:

0.00000

MO

130, Occ:

0.00000

Hole:

0.00000

Electron:

0.08058

MO

131, Occ:

0.00000

Hole:

0.00000

Electron:

0.46282

MO

132, Occ:

0.00000

Hole:

0.00000

Electron:

0.16703

MO

133, Occ:

0.00000

Hole:

0.00000

Electron:

0.22976

Sum of hole:

1.00000

Sum of electron:

1.00000

Function 3: Show atom or fragment contribution to hole and electron and plot the
contributions as heat map
After you enter this function, many quantities mentioned in "Theory 2" of Section 3.21.1.1 will
be printed on screen, below is an output example. Mulliken type of partition is used to derive the
atomic contributions.
Contribution of each non-hydrogen atom to hole and electron:
1(C )

Hole:

1.37 %

Electron:

8.96 %

Overlap:

3.50 %

Diff.:

7.59 %

2(C )

Hole: 11.86 %

Electron:

0.74 %

Overlap:

2.97 %

Diff.: -11.11 %

3(C )

Hole:

8.96 %

Electron: 11.00 %

Overlap:

9.93 %

Diff.:

2.04 %

...[ignored]
14(N )

Hole:

0.18 %

Electron: 23.80 %

Overlap:

2.07 %

Diff.:

23.62 %

15(O )

Hole:

3.07 %

Electron: 17.23 %

Overlap:

7.28 %

Diff.:

14.16 %

16(O )

Hole:

3.07 %

Electron: 17.23 %

Overlap:

7.28 %

Diff.:

14.16 %

In the output, the "Overlap" is simply the geometry average of "Hole" and "Electron", while "Diff."
is obtained by subtracting "Hole" from "Electron". Since hydrogens commonly do not participate in
electron excitations of interest, by default hydrogens are ignored, but you can choose "Toggle if
taking hydrogens into account" option to switch status.
If you need contribution of molecular fragments to above mentioned quantities, you can select
option "-1 Load fragment definition" and then input the number of fragments and atomic index of
each fragment in turn. Fragment definition can also be loaded from an external plain text file, in
which each fragment occupies a line, for example
1,3,6-10,12
2,4,5
11
13-15

This example totally defines four fragments, the first fragment consists of atom 1,3,6,7,8,9,10,12.
Once defining fragments is completed, contribution of the fragments to various quantities will be

247

--- page break ---

3 Functions
immediately printed on screen.
Composition of atom/fragment in hole and electron, as well as hole-electron overlap in various
atom/fragment spaces can be plotted as heat map, so that their distribution character can be very
vividly exhibited. Below is an example, the color corresponds to function value, while abscissa
corresponds to atom index.

From the graph, you can immediately recognize that this is a local excitation, since most part of
both hole and electron are distributed on the fragment consisted of atom 1~14. In particular, atoms
7 and 8 are the atoms that contribute most to this electron excitation. If you load fragment definition
before plotting, then the abscissa of the heat map will correspond to fragment index. In the menu,
there are also options used to adjust color scale, ratio of the map and interval between labels in X
axis.
A very detailed example of this hole-electron module is given in Section 4.18.1. Example of
analyzing transition density and transition dipole moment density using this module is given in
Section 4.18.2.1. More discussion and examples can be found from my blog article "Using Multiwfn
to perform hole-electron analysis to fully investigate electron excitation character" (in Chinese,
http://sobereva.com/434).
Information needed: See beginning of Section 3.21.

3.21.2 Plot atom/fragment transition matrix of various kinds as heat
map (2)
This function is used to plot atom transition matrix (ATM) of various kinds as heat map (colorfilled matrix map). The ATM refers to any kind of atom based matrix that represents electron
transition information between two states. For example, it may correspond to the atom based
transition density matrix (see below), the atom-atom charge transfer matrix, the atom transition
dipole moment matrix and so on. In this function, the ATM can also be further transformed to
fragment transition matrix (FTM) and then plotted as heat map.
Although this function can also plot heat map for other matrices, the major purpose of
developing this function is plotting atom or fragment based transition density matrix, therefore I
will first introduce theories related to transition density matrix.
Theories about transition density matrix (TDM)
Below, the word "TDM" refers to the transition density matrix in basis function representation.
The TDM between ground state and an excited state can be calculated as (de-excitation transitions
have been ignored for simplicity)

248

--- page break ---

3 Functions
occ vir

Ptran =  wia CiCa
i

a

where Ci denotes the expansion coefficient of basis function  in MO i. It is worth to note in passing
that the TDM in real space representation, which is introduced in Section 3.21.1.1, can be
constructed easily via TDM in basis function representation ( stands for basis function):

T (r; r ') =  Ptran   (r)  (r ')




The off-diagonal elements of TDM essentially represent the coupling between various basis
functions during electron excitation. Assume there are only two basis functions and meantime the
excitation can be perfectly represented as i→a MO transition, then the TDM could be explicitly
written as below form (notice that the index of the elements has been rearranged according to
convention of TDM heat map)

1, 2 2, 2  C1iC2 a
P tran = 
=
1,1 2,1   C1iC1a

C2iC2 a 
C2iC1a 

tran
If magnitude of off-diagonal element 𝑃1,2
is large, it implies that basis functions 1 and 2

significantly participate in occupied orbital i and virtual orbital a, respectively. More generally, we
may say that basis functions 1 and 2 have large contribution to hole and electron, respectively, in
this case the two basis functions are strongly coupled during the excitation. The diagonal terms are
also meaningful, if element 𝑃tran
has large magnitude, it implies that basis function  must
,
simultaneously have large contribution to both hole and electron.
Since TDM in general is not a symmetric matrix, in order to make certain discussions easier,
some literatures employ below symmetrized form

P =
tran

Ptran + Ptran
2

The TDM can be contracted to atom based form according to correspondence between basis
functions and atoms, it will be symbolized as p. In Multiwfn, below construction ways are available:

Way 1 : p AB =  ( Ptran ) 2
  A B

Way 2 : p AB =

 ( P )
 

tran 2

 A B

Way 3 : p AB =  | Ptran |
  A B

Way 4 : p AA =  | Ptran |,
 A

p Atran
B =

 ( P )
 

tran 2

 A B

where  and  denote the basis functions centered at atom A and on B, respectively. Both original
form and symmetrized form of TDM could be employed here.
If way 1 is employed, the p will correspond to the matrix of so-called correlated electron-hole
probability diagram (CEHPD), its (A,B) element was interpreted as the probability of simultaneously
finding a hole in atom A and an electron in atom B (this interpretation is not strictly true in general

249

--- page break ---

3 Functions
cases). See J. Chem. Phys., 113, 10002 (2000) and J. Am. Chem. Soc., 129, 14257 (2007) for
tran
example, in which the authors used 𝑃̅
obtained at ZINDO level.
If the p is constructed in way 2, 3 or 4, the resulting matrix may be referred to as atom transition
density matrix. For example, the way 4 has been employed in Chem. Rev., 102, 3171 (2002).
However, according my experiences, using way 2 or 3 is more preferred, since I found that the
diagonal terms obtained in way 4 is often too large compared to the off-diagonal terms.
Assume that the TDM used to construct p was not symmetrized, the general structure of the
resulting p could be expressed in below form

1, N
 
p  electron 
 1,2

 1,1

 N,N

 

 N ,2 

 N ,1 
hole

2, N

2,2
2,1

In complete analogy with the discussion about TDM, the physical meaning of the matrix elements
of p can be roughly understood as follows, irrespective of the choice of the specific way of
constructing the p:
 Diagonal terms: If (A,A) is large, it implies that atom A has large contribution to both hole and
electron, therefore the electron excitation should result in evident charge reorganization within
atom A
 Off-diagonal terms: If (A,B) is large, then atom A should have large contribution to hole and
meantime atom B should have large contribution to electron, implying that electron excitation
leads to CT from A to B
The "hole" and "electron" mentioned above are highly abstract concepts, although they have the same physical
meaning as the those defined in the hole-electron analysis (Section 3.21.1), one cannot expect that the pattern of the
p defined in any one of above ways is always very close to the atom-atom charge transfer matrix, which is much
more strictly defined and more meaningful.

If symmetrized form of TDM was used to build p, then CT directional information will not be
reflected by p. In this case, if off-diagonal term (A,B)=(B,A) is large, then we can simply say that
coherence between atoms A and B is strong during the electron excitation, in other words, charge
transfer occurs between atoms A and B.
The heat map of p is particularly useful for analyzing large-size and highly conjugated
molecules. Commonly hydrogens are omitted in the plot to make the map compact, since hydrogens
rarely participate in electron excitation of chemical interest.
If fragments are defined, the p (or other kinds of atom transition matrix) can further be
contracted to fragment based form:

pRS = 

 p

Afrag R Bfrag S

AB

This form is very convenient when one wishes to study role of various fragments in electron
excitation.
Input files
Since there are different types of atom transition matrix, and the matrix can be passed to
Multiwfn in different ways, there are several circumstances as shown below, you should use proper
input files. The file that should be loaded when Multiwfn boots up is always the file containing basis

250

--- page break ---

3 Functions
function information, and it should correspond to another file that needed to be loaded when you
enter present function.
(1) Plotting heat map of p in usual way
You should load a file containing configuration coefficient information of excited states when
you enter this function (see beginning of Section 3.21). Then Multiwfn will automatically generate
TDM between ground state and you selected excited state, and at the same time you can choose if
symmetrizing the resulting TDM in aforementioned way.
(2) Plotting heat map of p based on the TDM recorded in Gaussian output file
You should load Gaussian output file of electron excitation task when you enter this function.
The keywords density=transition=x IOp(6/8=3) must be specified in Gaussian input file, so that
TDM between ground state and excited state x can be printed in output file by Link 601 of Gaussian.
Via this way, not only the TDM of CIS/TDHF/TDA-DFT/TDDFT can be plotted, but also the TDM
generated by the EOM-CCSD and semi-empirical ZINDO method can be plotted.
Note 1: The TDM outputted by Gaussian is in aforementioned symmetrized form.
Note 2: If your ground state is singlet state while you used such as TD=triplet to request Gaussian to compute
triplet excited states, then the outputted TDM will be exactly zero due to spin forbidden, and thus Multiwfn is unable
to plot corresponding TDM map. However, it is possible to draw spatial part of the singlet-triplet TDM. To do this,
you should let Multiwfn itself to generate TDM, see (1).
Note 3: If the basis set you used contains diffuse basis functions, in rare cases, the TDM outputted by Gaussian
is incorrect, and thus the resulting heat map will be useless.
In summary, if the method you are using is not ZINDO, do not let Multiwfn to load TDM directly from Gaussian
output file.

(3) Plotting heat map of p based on the TDM recorded in a plain text file
You should load a file named tdmat.txt when you enter this function, Multiwfn will read TDM
from this file. Commonly, the tdmat.txt is generated by subfunction 9 of main function 18 (see
Section 3.21.9 for detail), which can not only generate TDM between ground state and an excited
state, but can also generate TDM between two excited states. An example file has been provided as
examples\excit\tdmat.txt.
For above three cases, you can choose the way used to contract the TDM to the p.
(4) Plotting atom transition dipole moment matrix
You should load a file named one of AAtrdip.txt, AAtrdipX.txt, AAtrdipY.txt, AAtrdipZ.txt when
you enter this function, Multiwfn will read atom transition dipole moment matrix from this file.
Commonly, they are generated by subfunction 11 of main function 18 (see Section 3.21.11 for detail).
By plotting heat map of these matrices, one can easily recognize which atoms and which interatomic
couplings notably affect transition dipole moment.
(5) Plotting atom-atom charge transfer matrix
You should load a file named atmCTmat.txt when you enter this function, Multiwfn will read
atom-atom charge transfer matrix from this file. Commonly, the atmCTmat.txt is generated by
subfunction 8 of main function 18 (see Section 3.21.8 for detail). By plotting heat map of this kind
of matrix, charge transfers between various atoms or fragments as well as charge reorganization
sites can be intuitively recognized.
Hint: In fact, you can also make the tdmat.txt or AAtrdip.txt/atmCTmat.txt contain other kind of matrices so that
they can be plotted as heat map via present module. For example, you can export bond order matrix as bndmat.txt
using corresponding subfunction in main function 9, then rename it as atmCTmat.txt and delete the first line from it,
then if you load this file into Multiwfn when entering present module, the plotted heat map will correspond to the
bond order matrix.

Usage
After loading all needed files and generating all needed data as mentioned above, you will enter
the interface for plotting heat map of the atom transition matrix (ATM). Some options are self-

251

--- page break ---

3 Functions
explanatory, others are described below:
Option 0: Showing heat map of ATM on screen. By default, labels in abscissa and ordinate of
this map correspond to indices of non-hydrogen atoms.
Option 1: The same as option 0, but save the heat map as graphical file in current folder.
Option 3: Exporting the ATM as matrix.txt in current folder, so that it can then be conveniently
plotted by some third-party tools such as Origin and Sigmaplot.
Option 4: Switching the status that if hydrogens will be included in the heat map
Option 5: Changing upper and lower limits of color scale. By default, they are automatically
set to maximum and minimum matrix elements of ATM, respectively.
Option 6: Changing the number of interpolation steps between grid data. If you want to make
the graph look smooth, it should be set to a large value (the default 10 is already quite large); if the
value is set to 1, then interpolation will not be performed, in this case each square grid in the map
exactly corresponds to a matrix element.
Option 8: Determining if performing normalization. If the status is switched to "Yes", then
normalization factor will be applied so that the sum of all elements of ATM is equal to unity.
If you select option "-1 Define fragments", fragment definition can be directly inputted or be
loaded from a plain text file, which should look like below, each fragment occupies a line:
1,3,6-10,12
2,4,5
11
13-15

Then Multiwfn will contract the atom transition matrix to fragment transition matrix (FTM). After
that, the matrix to be plotted or exported in present module will be FTM instead of ATM
An example of plotting and studying p matrix is given in Section 4.18.2.2; example of
analyzing transition dipole moment matrix is given in Section 4.18.2.3; example of plotting atomatom charge transfer matrix is given in Section 4.18.8.

3.21.4 Calculate ∆r index to measure charge-transfer length (4)
Theory
In the paper J. Chem. Theory Comput., 9, 3118 (2013), r index was proposed to measure CT
length during electron excitation. The r can be expressed as

r =  ri a
i ,a

where ∆𝑟𝑖𝑎 is contribution of orbital transition between i and a to the r index:

ri a =

( Kia )2
 r −  r
 ( Kia )2 a a i i
i, a

The index i and a run over all occupied and virtual MOs, respectively.  is orbital wavefunction.
Assume that the method you used to calculate electron excitation is CIS or the TDDFT under TammDancoff approximation, then 𝐾𝑖𝑎 is simply the configuration coefficient corresponding to excitation

252

--- page break ---

3 Functions
of i→a. While if the method you used is TDHF or TDDFT, then 𝐾𝑖𝑎 = 𝑤𝑖𝑎 + 𝑤′𝑎𝑖 , where 𝑤𝑖𝑎 and
𝑤′𝑎𝑖 denote the configuration coefficient corresponding to excitation of i→a and de-excitation of
ia, respectively.
r is especially useful for diagnosing when certain classes of DFT functionals are failure for
TDDFT purpose. When r is large, pure functionals such as BLYP and PBE, and the hybrid
functionals with low Hartree-Fock exchange composition such as B3LYP and PBE0, will not work
well. In this case, long-range corrected functionals should be employed; for instance, CAM-B3LYP
and ωB97XD.
It is worth to mention that if an electron excitation can be perfectly represented by one pair of
MO transition, then the r index and D index defined in hole-electron analysis framework will be
exactly identical in principle:

r =  a r  a − i r  i   r  a (r ) d r −  r  i (r ) d r
2

2

D index = D =  r ele (r)d r −  r hole (r)d r =  r  a (r) d r −  r  i (r) d r
2

2

However, their values outputted by Multiwfn should be marginally different, since they are
evaluated based on different numerical integration algorithms.
Usage
The input files needed by present module have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states.
After entering present function (subfunction 4 of main function 18), you will be prompted to
select the excited states for which the r will be calculated, then the results will be printed on screen
immediately.
If you only selected one state, then Multiwfn will ask you to choose if decomposing the r into
orbital pair contributions. If you inputted e.g. 0.01, then orbital pairs which have contribution to r
larger than 0.01 will be printed. From the output, you can easily identify which orbital pairs have
significant contribution to charge-transfer of electron excitation.
An example of present function is provided as Section 4.18.4.
Information needed: See beginning of Section 3.21.

3.21.3 Analyze charge-transfer based on density difference grid data (3)
Theory
In the paper J. Chem. Theory Comput., 7, 2498 (2011), the authors proposed a method for
analyzing charge-transfer (CT) during electron transition, present function fully implements this
analysis method. It is also probable that this method can be used to study CT in other processes,
such as formation of molecular complex. In the original paper, the author only discussed the cases
when charge-transfer is in one-dimension, while in Multiwfn this scheme has been generalized to
three-dimension case. In addition, some quantities introduced below are not proposed in the original
paper but proposed by me, definition of some quantities in the original paper have also been

253

--- page break ---

3 Functions
modified by me to make the analysis more meaningful.
The electron density variation between excited state (EX) and ground state (GS) is

 (r ) =  EX (r ) −  GS (r )
Notice that the geometry used in calculating EX and GS must be identical, otherwise the resulting
 will be meaningless. Therefore, present function can only be used to characterize "vertical"
process.  can be divided into positive and negative parts, namely + and −. Of course, the integral
of + and -− over the whole space should be equal. If evident inequality is observed, that means the
error in numerical integral is unneglectable, and higher quality of grid (i.e. larger number of grid
points) is required. Even though what you analyzed is single-electron excitation, the magnitude of

+ and − as well as their integrals over the whole space in principle can also be larger than 1.0, this
is because excitation of an electron must lead to reorganization of distribution of the rest of electrons,
which also make contribution to .
The transferred charge qCT is the magnitude of the integral of + and − over the whole space.
It is important to correctly recognize the physical meaning of this quantity. qCT only corresponds to
the total amount of charge whose distribution is perturbed during electron excitation, it does not
correspond to net charge transfer from one fragment to another fragment (e.g. from donor group to
acceptor group)
The barycenter of positive and negative parts of  can be computed as

R + =  r + (r ) d r /   + (r ) d r
R − =  r − ( r ) d r /   − ( r ) d r
The Cartesian component coordinates of R+ will be referred to as X+, Y+, Z+ below, while that of R−
will be referred to as X−, Y−, Z−.
The distance between the two barycenters measures the CT length, its three Cartesian
components:

Dx = X + − X −

Dy = Y+ − Y−

Dz = Z + − Z −

The D index is defined as ( Dx ) + ( Dy ) + ( Dz ) | R + − R − | , which characterizes total CT
2

2

2

length.
The dipole moment variation caused by electron excitation can be evaluated as

 X = ( X + − X − ) qCT Y = (Y+ − Y− ) qCT  Z = ( Z + − Z − ) qCT
The RMSDs of distribution of + and − in each direction are defined as

 (r )( '−  ) d r
 = 
  (r ) d r
2

a

a

a,

a

where a={+,-}, ’={x,y,z}, ={X, Y, Z}. x, y and z are Cartesian components of position vector r.
For example, +,y can be explicitly written as

 +, y

  (r)( y − Y ) d r
=
  (r ) d r
+

+

+

254

2

--- page break ---

3 Functions
The difference between RMSD of + and − in X/Y/Z direction can be measured via , while
overall difference can be measured via the  index

  =  + , −  −,

 = {x, y, z}

 index =| σ + | − | σ − |
It is noteworthy that D index is zero for exactly centrosymmetric systems, therefore, it is useless for
discussing CT problem of such kind of system. However,  index is often useful to identify this
type of excitation, since in this case  index must be large because diffuseness extent of + is much
higher than −.
C+ and C− functions are defined aiming for visualizing CT more intuitively than . Their
structures are similar to Gaussian function, the value asymptotically approaches zero from the
centroid of the function.

 ( x − X + ) 2 ( y − Y+ ) 2 ( z − Z + ) 2 

C + (r ) = A+ exp −
−
−
2
2
2


2

2

2

+,x
+, y
+, z


 ( x − X − ) 2 ( y − Y− ) 2 ( z − Z − ) 2 

C − (r ) = A− exp −
−
−
2
2
2


2

2

2

−, x
−, y
−, z


The normalization factor A is introduced so that the integrals of C+ and C− over the whole space are
equal to that of + and −, respectively.
H measures average degree of spatial extension of − and + in X/Y/Z direction, HCT is that in
CT direction, and H index is an overall measure:

H  = ( + , +  − ,  ) / 2

 = {x, y, z}

H CT =| H  uCT |
H index = (| σ + | + | σ − |) / 2
where uCT is unit vector in CT direction and can be straightforwardly derived using centroid of −
and +.
t index measures separation degree of + and −:

t index = D index − H CT
If t index<0, it implies that − and + are not substantially separated due to CT. Clear separation of
− and + distributions must correspond to evidently positive t index.
I defined another quantity to measure overlapping extent between C+ and C−:

S+− =  C+ (r ) / A+ C− (r ) / A− d r
If the value equals to 1, that means the two functions are completely superposed, else if the value
equals to zero, it indicates that the distribution of them are completely separated. This index is
dimensionless.
Usage
Because all numerical integrals mentioned above are computed based on evenly distributed
grid data, user needs to generate grid data of  by using custom operation of main function 5, see
Section 3.7.1, or load a file (e.g. cube file) containing grid data of density difference when Multiwfn
boots up. After that, enter subfunction 3 of main function 18, all aforementioned quantities will be

255

--- page break ---

3 Functions
shown on screen immediately. The "Overlap integral between C+ and C-" term is the S+− introduced
above. In the post-processing menu, user can choose to visualize C+ and C−, or export grid data for
the two functions to cube file in current folder.
An example is given in Section 4.18.3.
Information needed: Grid data of electron density difference

3.21.5 Calculate transition electric/magnetic dipole moments between
all states and for each state (5)
This function is used to calculate transition electric/magnetic dipole moment between all states
(including both ground state and excited states). This function is also able to print electric dipole
moment for each state.
For two states i and j, the transition electric dipole moment is defined as i|-r|j; when i=j,
this quantity corresponds to electric dipole moment of this state contributed by electrons. The
transition electric dipole moment is formally defined as −ii|r|j, but the moment outputted by
this function corresponds to i|r|j.
The input files needed by present function have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states. If
you need very accurate transition dipole moments, you should use IOp(9/40=5) keyword of
Gaussian or TPrint 1E-10 keyword of ORCA to make the program print as much configuration
coefficients as possible.
After you enter present function, summary of all excitations will be printed. "Normalization"
should be as close as possible to expected value (0.5 and 1.0 for closed- and open-shell reference
states, respectively). If the deviation is large, then the resulting transition dipole moments must have
large error, and you must make your quantum chemistry program output more configuration
coefficients.
In the interface you can use option 0 to choose the type of (transition) dipole moments to be
calculated, by default they are electric, but you can change to magnetic. You can use options 1 ~ 4
to choose the task to conduct, see below, in which options 3 and 4 are available only if the (transition)
dipole moment to be calculated is set to electric.
• Option 1: Output transition electric dipole moments between all states (including both ground
state and excited states) to screen
• Option 2: The same as 1, but output to transdipmom.txt in current folder.
• Option 3: Generate input file of SOS module of Multiwfn as SOS.txt in current folder. Then
if you use the SOS.txt as input file, you can use SOS module to evaluate (hyper)polarizability, see
Section 3.27.2 for detail.
• Option 4: Output electric dipole moment of each excited state to dipmom.txt in current folder.
Note that both electronic and nuclear contributions to the value are taken into account (this is clearly
different to the values corresponding to i=j cases printed by option 1 and 2, which only considers
contribution from electrons).

256

--- page break ---

3 Functions

The calculation process of all the tasks consists of three stages:
Stage 1: Calculate dipole moment integrals between all basis functions
Stage 2: Calculate dipole moment integrals between all MOs
Stage 3: Calculate dipole moment integrals between all excited states. Usually this is the most
time-consuming step.
If the output file of quantum chemistry program includes both singlet and triplet excited states,
for example, you used TD(50-50) keyword in Gaussian and reference state is closed-shell, only
aforementioned tasks (1) and (2) are available, and transition dipole moment of all singlet-singlet
pairs (including ground state) and triplet-triplet pairs will be calculated by Multiwfn and outputted
separately, while singlet-triplet pairs are ignored because due to spin-forbidden the result must be
zero. In addition, excitation energies between S0 and all excited states are printed at the end of
output. This function is of great importance if you want to use PySOC code to calculate spin-orbit
coupling matrix element, see my blog article "Using Gaussian+PySOC to calculate spin-orbit
coupling matrix element under TDDFT" (http://sobereva.com/411, in Chinese) for detail.
There is a parameter "maxloadexc" in settings.ini, if this value is not 0 (default) and the actual
number of excited states is higher than this value, then only the first "maxloadexc" excited states
will be loaded and subjected to transition electric dipole moment calculation.
Calculation of transition dipole moments between excited states is quite time-consuming for
large systems. However, if you only need them between ground state and excited states, the data can
always be quickly calculated. In this case, before starting calculation, you should select option “-1:
Toggle if only calculating between ground and excited states” to change its status to “Yes”, then
transition dipole moments between excited states will not be calculated and printed.
Examples of this function is given in Section 4.18.5.
Information needed: See beginning of Section 3.21.
Appendix
The formulae used to derive transition electric/magnetic dipole moment between electronic
states used in the present function are given as follows
(1) Transition electric dipole moment between ground state and an excited state K:
occ vir

occ vir

i

i

D0→ K =  wiK,a  i − r  a +  wi,Ka  i − r  a
a

a

where w and w’ are coefficients of excitation and de-excitation configurations respectively. i and a
denote indices of occupied and unoccupied molecular orbitals, respectively.
(2) Transition magnetic dipole moment between ground state and an excited state K:
occ vir

occ vir

i

i

M 0→ K =  wiK,a  i r    a −  wi,Ka  i r    a
a

a

See Eqs. 22 and 24 in J. Chem. Phys., 66, 3460 (1977) on why the consideration of excitations
and de-excitations is different for evaluating electric and magnetic transition dipole moments.

257

--- page break ---

3 Functions
(3) Transition electric/magnetic dipole moment between excited states K and L:
occ vir

occ vir

i

j

TK → L =  wiK,a  wLj ,bV iajb
a

b

with

  i vˆ  a

 −  vˆ  a
V iajb =  0 i
 v −  i vˆ  i +  a vˆ  a
0

(i = j, a  b)
(i  j, a = b)
(i = j, a = b)
(i  j, a  b)

occ

v 0 = l  l vˆ  l
l

where 𝑤
̃ denotes both w and w’. v0 corresponds to ground state property with  being orbital
occupancy. The operator 𝐯̂ = −𝐫 is for transition electric dipole moment and 𝐯̂ = 𝐫 × ∇ is for
transition magnetic dipole moment. For TD case, the V terms between excitation and de-excitation
configurations is simply ignored. In addition, when calculating V terms between de-excitation
configurations, it is replaced with −V.

3.21.6 Generate natural transition orbitals (NTOs) (6)
Theory
This function is used to generate natural transition orbitals (NTOs). NTO was proposed in J.
Chem. Phys., 118, 4775 (2003), it has become a very popular and useful way to analyze character
electron excitation obtained by single-reference methods.
Transition of electronic state is often not predominated by only one MO pair, in many cases
multiple MO pair transitions simultaneously have non-negligible contributions, which can be
evaluated as square of corresponding configuration coefficient. This fact brings great hindrance of
analyzing electron excitation character by simply visualizing related MOs. The NTO method aims
to relieve this difficulty, it separately performs unitary transformation for occupied MOs and virtual
MOs, so that only one or very few number of orbital pairs have dominant contributions.
The basic procedure of yielding NTOs is outlined below:
(1) Generating transition density matrix in MO basis (T). Assume that the system has nocc
occupied MOs and nvir virtual MOs, then T has dimension of (nocc, nvir), its (i,l) element is simply
constructed as

Ti ,l = wia
where i<nocc, l<nvir and a=l+nocc. 𝑤𝑖𝑎 stands for configuration coefficient corresponding to i→a
orbital transition. Note that for TD formalism, there may be some de-excitations, their configuration
coefficients are simply ignored in constructing the T.
(2) Generating temporary matrix for occupied and virtual orbitals, respectively

Tocc = TTT

Tvir = TT T

Evidently, both Tocc and Tvir are square matrices, their dimensions are nocc and nvir, respectively.
(3) Diagonalizing Tocc and Tvir to obtain eigenvalues and eigenvectors

258

--- page break ---

3 Functions
−1
Uocc
Tocc Uocc = Λ occ

−1
U vir
Tvir U vir = Λ vir

(4) The diagonal terms of occ and vir are eigenvalues of occupied and virtual NTOs,
respectively. For the former, the eigenvalues are commonly sorted from low to high, while for the
latter, the eigenvalues are commonly sorted from high to low. A NTO pair consists of an occupied
NTO and a virtual NTO sharing the same eigenvalue. Eigenvalue of a NTO pair multiplied by 100
is just its percentage contribution to the electron excitation.
For CIS and TDA-DFT, the range of eigenvalue must be 0.0~1.0. However, in the TDHF and
TDDFT cases, due to presence of de-excitations, which is not explicitly considered in the NTO
analysis, it is possible that a NTO pair has eigenvalue slightly larger than 1.0, in this situation you
can simply treat it as 1.0 (However, if the value is much larger than 1.0, the TDDFT result may be
unreliable, and I suggest using TDA-DFT instead).
(5) MOs are transformed to NTOs via unitary transformation matrix U
NTO
MO
Cocc
= Cocc
Uocc

NTO
Cvir
= CMO
vir U vir

MO
MO
where 𝐂occ
and 𝐂vir
are coefficient matrix of occupied MOs and virtual MOs in original basis
functions, respectively; their columns correspond to different MOs. The counterpart matrices with
NTO superscript denote coefficient matrix of occupied and virtual NTOs.

Implementation and Usage
The input files needed by present function have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states.
After you enter present function, you should input the index of the electron excitation to be
studied, then Multiwfn will load corresponding configuration coefficients and generate NTOs
according to the equations shown above, and then output eigenvalues of NTO pairs. Next, you can
choose if exporting the NTOs to .fch/.molden/.mwfn file. If you choose to export, then you can use
Multiwfn to load the newly generated file to visualize the NTOs, analyze NTO orbital composition
and so on (note that in this case the data in orbital energy field in fact is NTO eigenvalues).
Present function works for both restricted and unrestricted reference states; for the latter, the
result of Alpha part and Beta part are calculated and printed separately, you only need to pay
attention to the NTO pairs having largest eigenvalues (e.g. the largest eigenvalue of Alpha part is
0.03, while the largest eigenvalue of Beta part of 0.95, that means this electron excitation is
dominated by transition of the Beta NTO pair)
According to my experiences, NTO analysis often works equally well as the hole-electron
analysis introduced in Section 3.21.1, namely both of them are able to avoid necessity of inspecting
many MOs when discussing electron excitation. An additional advantage of NTO analysis over holeelectron analysis is that the orbital phase information is retained; however, in some cases NTO
analysis completely fails, namely even after transformation from MO to NTO representation, there
is still no dominant orbital pair transition. Clearly in this case you have to resort to hole-electron
analysis.
An example of generating and analyzing NTOs is provided in Section 4.18.6.
Information needed: See beginning of Section 3.21

259

--- page break ---

3 Functions

3.21.7 Calculate ghost-hunter index (7)
Theory background
The ghost states are spurious very low-lying charge transfer (CT) excited states with excitation
wavelength usually around 1000 nm or more, they result from evidently incorrect asymptotic
behavior of exchange potential of pure DFT functionals or the hybrid functionals having low
Hartree-Fock exchange composition in long range electron interaction. Since the ghost states are
unreal states, they should be ignored when discussing electron excitations and plotting electronic
spectra. When ghost states are identified, then DFT functionals with relatively high global HF
exchange composition (e.g. M06-2X and BH&HLYP), or long-range corrected functional (e.g.
B97XD), or range-separated functional with high HF exchange composition at long range of
electronic interaction (e.g. CAM-B3LYP), should be employed to get rid of them.
The lower bound of TDDFT excitation energy of a CT state can be expressed as
𝜔low = 𝐼𝑃𝐷 − 𝐸𝐴𝐴 − 1/𝑅
where IPD is ionization potential of electron donor moiety (energy consumption of leaving an
electron), EAA is electron affinity of electron acceptor moiety (its negative is energy lowering due
to receiving an electron), and R denotes the electrostatic interaction between the hole and electron
after the CT excitation. According to Koopmans’ theorem, IP−HOMO, EA−LUMO, and we assume
that the excitation fully corresponds to HOMO→LUMO transition, where HOMO and LUMO are
completely localized in donor and acceptor regions respectively, we have
𝜔low  − 𝜀HOMO + 𝜀LUMO − 1/𝑅
In practice, electron excitation is contributed by multiple orbital transitions, so weighted MO
energies should be employed instead. In addition, the R may be estimated using DCT index (see later).
So, the above equation can be converted to



a 2
1
 ( wi )

low =  
( a −  i )  −
a 2
DCT
i , a  ( wi )
 i ,a

term 2
term 1

where i and a denote occupied and virtual MOs, respectively. The summation loops all TDDFT
configurations. w is configuration coefficient.  stands for MO energy.
In J. Comput. Chem., 38, 2151 (2017), the authors proposed ghost-hunter index (MAC) to
diagnose if an excited state yielded by current TDDFT calculation may be a ghost state. The MAC is
simply the low shown above.
Notice that in the original MAC paper, they erroneously used w rather than w2 in the above
equation, and the sign in front of a is wrong, these problems have been fixed in their later
publication J. Chem. Phys., 154, 204102 (2021). This paper and the original MAC paper did not
explicitly mention how to deal with de-excitation configurations, in the implementation in Multiwfn,
all de-excitation configurations are ignored.
Since the MAC corresponds to the theoretical lower bound of CT excitation energy calculated
by TDDFT (ETDDFT), in the paper of ghost-hunter index, it is argued that
𝐸TDDFT < 𝑀AC → ghost CT state
𝐸TDDFT > 𝑀AC → real CT state

260

--- page break ---

3 Functions
Ghost-hunter index is undoubtedly useful, however, according to my experience, this criterion is
often too stringent. I suggest only regard ETDDFT<MAC as a necessary rather than sufficient condition
for determining presence of ghost CT state.
Evaluation of ghost-hunter index
To calculate the MAC, you should perform hole-electron analysis as usual, see introduction in
Section 3.21.1 and example in Section 4.18.1. Once calculation of grid data of hole and electron is
finished, Multiwfn automatically prints the MAC index as well as its two terms (see above equation
for the meaning of the two terms).
Beware that in Multiwfn, the DCT used in the MAC expression is evaluated as the distance
between centroid of electron and hole distributions, this case corresponds to adopting unrelaxed
density of excited state. It is more or less different to the DCT evaluated in original paper of MAC
(referred to as DCT' below), which is calculated as centroid distance between positive and negative
parts of density difference between relaxed excited state density and ground state density. The DCT
calculated by hole-electron analysis module of Multiwfn is not only reasonable enough, but also
much cheaper than DCT', since evaluating TDDFT relaxed density for large systems is fairly
expensive. However, if you really want to calculate MAC index based on DCT', you should obtain the
first term of MAC via electron-hole analysis module, and then obtain DCT' via subfunction 3 of main
function 18 (see the example given in Section 4.18.3) and manually calculate the second term of
MAC (namely -1/DCT'), and finally sum up the two terms to derive MAC.
It is worth to mention that the MAC index is in principle only applicable to one-dimension CT
case, if the CT takes place in multiple directions, then this index is incapable of correctly identifying
ghost state.
Calculation of MAC is involved in the hole-electron analysis example in Section 4.18.1.
Information needed: See beginning of Section 3.21

3.21.8 Calculate interfragment charge transfer in electron excitation
via IFCT method (8)
Theory
Interfragment charge transfer is a very important phenomenon in electron excitation process. I
devised an albeit simple but quite useful way of evaluating amount of interfragment charge transfer
between any number of fragments, the method is described below (to be published). This method
will be referred to as IFCT (InterFragment Charge Transfer).
The IFCT method contains three steps:
(1) Calculating atomic contribution to hole and electron (see introduction of the concept of hole
and electron in Section 3.21.1.1)
(2) Calculating fragment contributions to hole and electron by summing up atomic
contributions
(3) Constructing interfragment charge transfer matrix Q. Its (R,S) element corresponds to the
electron transfer from fragment R to fragment S during the excitation:

261

--- page break ---

3 Functions

QR , S = R ,hole S ,ele
where R,hole and S,ele denote contribution of fragment R to hole and contribution of fragment S to
electron, respectively. Above formula is very easy to comprehend, it essentially assumes that
electron transfer from R to S is proportional to both composition of R in hole (where electron leaves)
and composition of S in electron (where electron goes).
Then three additional useful quantities could be defined:
·Electron net transferred from fragments S to R: pS → R = QS , R − QR , S
·Variation of electron population of fragment R: pR =

p

S R

S →R

=  (QS , R − QR ,S )
S R

·Intrafragment electron redistribution of fragment R: QR,R
By the way, it is easy to show that variation of electron population of a fragment evaluated in
above way is quite reasonable:

pR =  (QS ,R − QR ,S ) =  ( S ,hole  R ,ele −  R ,hole  S ,ele )
S R

S R

=  R ,ele   S ,hole −  R ,hole   S ,ele
S R

S R

=  R ,ele (1 −  R ,hole ) −  R ,hole (1 −  R ,ele )
=  R ,ele −  R ,hole
It is easy to comprehend that this is a quite reasonable way of evaluating variation of electron
population of fragment R, and thus well demonstrated reasonableness of the interfragment charge
analysis formalism introduced above.
In addition, it is worth to note that sum of amount of interfragment transferred electrons and
amount of intrafragment redistribution electrons exactly equals to unity, reflecting the fact that only
one electron is excited:

 Q
R S R

R,S

+  QS , S
S

=  QR , S
R

S

R

S

=   R , hole  S , ele
=   R , hole   S , ele
R

S

=1
On the evaluation of CT%
The concepts of charge transfer percentage (CT%) and its complement local excitation
percentage (LE%) are frequently involved in electron excitation studies. In the IFCT framework,
they can be defined in two different ways, I believe it is useful to explicitly distinguish them.
• Intrinsic CT% and LE%: The former is evaluated as CT% = 100% × ∑𝑅 ∑𝑆≠𝑅 𝑄𝑅,𝑆 , and the
latter is evaluated as LE% = 100% × ∑𝑆 𝑄𝑆,𝑆 , clearly they sum up to 100%. This definition works

262

--- page break ---

3 Functions
for any number of fragment.
• Apparent CT% and LE%: This definition only works for two fragment cases. Apparent CT%
is simply evaluated as 100%|pR|, and apparent LE% is define as 100%−CT%, where R denotes
either fragment.
In the case of two fragments (R and S), the intrinsic CT% and apparent CT% must be somewhat
different, both of them have their own value. Intrinsic CT% represents the amount of electrons that
essentially participate in charge transfer, which does not reflect the cancellation effect between
electron transfers of R→S and SR. In contrast, the apparent CT% corresponds to the apparent
phenomenon of net electron transfer between R and S, namely the cancellation of the bidirectional
electron transfer is taken into account. Clearly, intrinsic CT% must be equal or larger than apparent
CT%, and they are equal only if the interfragment charge transfer is completely single directional,
that is hole and electron fully and respectively localize on the two fragments.
It is worth to emphasize that %CT is directly dependent of the definition of fragments, because
it characterizes amount of charge transfer between the fragments. In the limiting case, you define
the whole system as a single fragment, then %CT must be exactly zero for all excitations.
Usage
The input files needed by the IFCT analysis have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states.
After entering this function, you need to choose the method for calculating fragment
distribution to hole and electron, and select the excited state to be studied, then input the total number
of fragments, after that you should define each fragment in turn by inputting atomic indices. If you
prefer to load fragment definition from a plain text file, you can input 0 when Multiwfn let you set
the total number of fragments, then you can input the path of the file containing fragment definition,
the file format should look like below, definition of each fragment occupies a line:
1,3,6-10,12
2,4,5
11
13-15

If in this step you input -1, Multiwfn will not carry out regular IFCT analysis but export a file named
atmCTmat.txt in current folder, this file records atom-atom charge transfer matrix, whose element
is defined as QA,B = A,holeB,ele. If you input path of this file after entering the function used to plot
atom/fragment transition matrix (see Section 3.21.2), this matrix could be plotted as heat map so
that you can visually study its matrix elements.
Once definition of fragments is completed, Multiwfn will calculate and print contribution of
all defined fragments to hole and electron, as well as amount of electron transfer between fragments.
In addition, net electron transfer as well as variation of electron population of each fragment are
also printed. Below is an output instance:
Variation of population number of fragment

1:

-0.25313

Variation of population number of fragment

2:

-0.23110

Variation of population number of fragment

3:

0.48423

Intrafragment electron redistribution of fragment

263

1:

0.00334

--- page break ---

3 Functions
Intrafragment electron redistribution of fragment

2:

0.31271

Intrafragment electron redistribution of fragment

3:

0.02419

Transferred electrons between fragments:
1 ->

2:

0.11977

1 <-

2:

0.00874

Net

1 ->

2:

0.11103

1 ->

3:

0.14299

1 <-

3:

0.00089

Net

1 ->

3:

0.14210

2 ->

3:

0.36476

2 <-

3:

0.02263

Net

2 ->

3:

0.34213

If two fragments are defined, then both intrinsic and apparent CT(%) and LE(%) will then be
printed, while if more than two fragments are defined, only intrinsic CT(%) and LE(%) are printed.
Usually Mulliken-like partition is reasonable choice for evaluating fragment contribution to
hole and electron, see "Theory 2" of Section 3.21.1.1 for detail of this method. However, diffuse
functions must not be employed in this case, otherwise the result may be very misleading. Hirshfeld
method is more robust and fully compatible with diffuse functions, but it is evidently more
expensive. When diffuse functions do not occur, the IFCT result under Mulliken-like partition and
Hirshfeld partition are in good agreement with each other.
An example of IFCT analysis is given in Section 4.18.8.
Information needed: See beginning of Section 3.21

3.21.9 Generate and export transition density matrix (9)
According to MO expansion coefficients and configuration coefficients, transition density
matrix (TDM) in basis function representation can be constructed by this function. Two kinds of
TDMs can be generated:
(1) TDM between ground state and a selected excited state K:
occ vir

Ptran =  wiK,aCiC a
i

a

where w corresponds to coefficient of the configurations involved in the excitation, Ci denotes the
expansion coefficient of basis function  in MO i. Excitation and de-excitation cases are not
distinguished in this context (PS: The TDM constructed in this way is suitable for studying transition
electric dipole moment, but not suitable for studying transition velocity and magnetic dipole moment,
see Eqs. 22, 23 and 24 in J. Chem. Phys., 66, 3460 (1977)).
(2) TDM between two selected excited states K and L:
occ vir

occ vir

i

j

PKL =  wiK,a  w Lj ,bViajb
a

b

 CaC b
 −C C

iajb
V =   i  j
 P − CiC i + CaC a
 0

264

(i = j , a  b )
(i  j , a = b )
(i = j , a = b )
(i  j , a  b )

--- page break ---

3 Functions
where P is density matrix of ground state. For TD case, the V between excitation and de-excitation
configurations is simply ignored. In addition, when calculating V between de-excitation
configurations, it is replaced with −V.
Once generation of TDM has been finished, you can choose if symmetrizing the TDM. There
are two ways
• Way 1: Ptran = ( Ptran + Ptran ) / 2
• Way 2: Ptran = ( Ptran + Ptran ) / 2
The way 1 is reasonable and should be used in common case. However, it should be noted that the
TDM generated by Gaussian program corresponds to the one symmetrized by way 2, therefore you
should choose way 2 if you want the resulting TDM follows convention of Gaussian.
The generated matrix will be outputted to tdmat.txt in current folder. You can also choose to
output TDM.fch in current folder, whose “Total SCF Density” field will correspond to TDM (this
file is useful if you would like to calculate TrEsp type of atomic transition charges by making use
of cubegen utility in Gaussian, see Section 4.A.9 for detail).
Note that when ground state and excited state have different spin multiplicities, due to the
orthonormality of spin coordinates, although in principle the transition density matrix should be
zero, the outputted matrix is not, because Multiwfn only takes spatial part of the MOs into account
during constructing the matrix.
The input files needed by present function have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states.
The example in Section 4.18.2.4 and Section 4.18.9 utilized present function.
Information needed: See beginning of Section 3.21
Appendix: Derivation of the formula of evaluating TDM between two excited states
The TDM between two excited states K and L in real space representation is

T KL (r; r ') =   K (r, r2 ,

, rN ) L (r ', r2 ,

, rN ) d r2

d rN

where the excited state wavefunctions are represented by linear combination of singly excited Slater
determinants
occ vir

 K =  wiK,a  ia
i

occ vir

 L =  w Lj ,b bj

a

j

b

We have
occ vir

occ vir

i

j

T KL (r; r ') =  wiK,a  w Lj ,b   ia (r, r2 ,
a

, rN ) bj (r ', r2 ,

, rN ) d r2

d rN

b

̂ = ∑𝑖 ℎ𝑖 , integral between two
Slater-Condon rule shows that for an single-electron operator ℵ
𝑎 ̂
𝑏
singly excited determinants, namely ⟨𝑖 |ℵ|𝑗 ⟩, satisfies

265

--- page break ---

3 Functions

=0

(i  j, a  b)

= ahb

(i = j, a  b)

=− j h i

(i  j, a = b)

N

= p h p − i h i + a h a

(i = j , a = b)

p

Without performing the integral and view the h operator as 1, based on the above relations we
have

0
(i  j , a  b )

K
L

wi ,a wi ,b a (r ) b (r ') (i = j, a  b)


i
a
b

−  wiK,a w Lj ,a j (r )i (r ') (i  j, a = b)
T KL (r; r ') = 
i
j
a

N


K
L 
  wi ,a wi ,a    p (r ) p (r ') − i (r )i (r ') +  a (r ) a (r ')  (i = j, a = b)
 i a
 p

Given that

T KL (r; r ') =  PKL   (r )  (r ')




 k (r ) =  C  k   (r )


where  is basis function, we can finally reach the formula or evaluating PKL shown earlier in this
section. For example, in the case of i=j, ab:

T KL (r; r ') =  wiK,a wiL,b a (r ) b (r ')
i

a

b

i

a

b

=  wiK,a wiL,b  CaC b   (r )  (r ')
P

KL





=  w w CaC b
K
i ,a

i

a

L
i ,b

b

3.21.10 Decompose transition electric/magnetic dipole moment as
molecular orbital pair contributions (10)
Theoretical chemists often prefer to study electron excitations in terms of molecular orbital
transitions, this function helps them in this respect. This function decomposes transition electric or
magnetic dipole moment from ground state to an excited state of interest as molecular orbital pair
contributions to provide users a deeper insight into electron excitation.
Theory
As mentioned in Section 3.13.1, oscillator strength (f) of an electron excitation directly relates
to the integral area of the corresponding absorption peak. f has direct relationship with transition
electric dipole moment Dtran (in atomic unit):

266

--- page break ---

3 Functions

f =

2
E  | D tran |2
3

where E denotes the transition energy between the two electronic states. Clearly, Dtran is a crucial
quantity of electron excitations and largely determines optical absorption. Dtran between ground state
and an excited state is calculated as follows

Dtran =  ( wi ,a + wi,a ) i − r  a
i ,a

where i and a loop over all occupied and virtual MOs, respectively. w and w are configuration
coefficient of excitations and de-excitations, respectively.  denotes molecular orbital wavefunction.
It is clear that the transition dipole moment can be straightforwardly decomposed into contribution
of various MO pairs. Via such a decomposition, one can easily study why some excitations have
relatively large oscillator strength and thus have strong absorption, and why some excitations only
have small oscillator strength and thus they are difficult to observe in electronic spectrum.
Transition magnetic dipole moment Mtran is also an important quantity of electron excitation,
because Mtran and Dtran collectively determine rotatory strength, which determines electronic circular
dichroism (ECD) and circularly polarized luminescence (CPL) spectra. Mtran between ground state
and an excited state is calculated as follows

M tran =  (wi ,a − wi,a ) i r    a
i ,a

Obviously, Mtran can also be straightforwardly decomposed into contribution of various MO pairs.
Usage
The input files needed by present function have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states when
you enter this function.
In this function, you will be asked to select the type of transition dipole moment, and will be
prompted to choose the excited state for which the transition dipole moment will be decomposed as
MO pairs, then a menu appears. You can select corresponding option to make Multiwfn output
contribution of every MO pair to transdip.txt in current folder, or let Multiwfn sort the MO pairs
according to their contributions to transition dipole moment and then output the first few or dozens
of terms, so that you can immediately identify the most important MO transitions. In addition, you
can request Multiwfn to only output MO pairs with contribution larger than a given threshold.
An example of this function is given in Section 4.18.10.
Information needed: See beginning of Section 3.21

3.21.11 Decompose transition electric/magnetic dipole moment as basis
function and atom contributions (11)
This function is used to decompose the transition electric or magnetic dipole moment between
ground state and a selected excited state, or between two excited states, into contributions from
various basis functions and atoms. The result is exported to trdipcontri.txt. Therefore, from which

267

--- page break ---

3 Functions
you can easily examine which part of the system has significant impact on excitation properties such
as oscillator strength.
There are many possible ways to realize the decomposition. In this function Mulliken-like
partition is employed due to its simplicity. The contribution of basis function μ to transition electric
dipole moment vector is evaluated as

D  = Ptran   − r   +

(

1
Ptran   − r  + Ptran  − r  

2  

)

and the contribution of basis function μ to transition magnetic dipole moment vector is evaluated as

M  = Ptran   r     +

(

1
 Ptran   r    + Ptran  r    
2  

)

where Ptran is transition density matrix from ground state to the excited state of interest. The
contribution from an atom is simply the sum of the contribution from the basis functions belonging
to it.
Since Mulliken partition is incompatible with diffuse functions, the decomposition result is
unreliable if diffuse functions are presented in the basis set you used. In this case, the best way to
study contribution from various atoms is visualizing the transition dipole moment density (see
Section 3.21.1).
This function also asks you if outputting atom transition dipole moment matrix, if you choose
y, then X, Y, Z components of the matrix will be exported to AAtrdipX.txt, AAtrdipY.txt, AAtrdipZ.txt
in current folder, respectively, the matrix elements are defined as follows (I take atom-atom
contribution matrix of transition electric dipole moment as example, the matrix for transition
magnetic dipole moment is defined similarly and thus not explicitly shown here)

DAX, B =  Ptran   − x  
  A B

D

Y
A, B

=  Ptran   − y  
  A B

DAZ, B =  Ptran   − z  
  A B

𝑋
For example, the term 𝐷𝐴,𝐵
corresponds to joint contribution of A-B atomic pair to X component of

transition dipole moment, the sum of all elements of DX equals to X component of transition dipole
moment of current system. Total transition dipole moment matrix (sum of square of X, Y, Z) is
exported as AAtrdip.txt in current folder. All of these .txt files can be directly plotted as colored
matrix map (heat map) by atom transition matrix plotting module (see Section 3.21.2 for detail).
The input files needed by present function have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states when
you enter this function.
The example of Section 4.18.11 utilized this function.
Information needed: See beginning of Section 3.21

268

--- page break ---

3 Functions

3.21.12 Calculate Mulliken atomic transition charges (12)
This function is used to calculate atomic transition charges, which is useful for studying
Coulomb coupling between ground state and excited state (exciton coupling) of two molecules, see
e.g. J. Phys. Chem. B, 110, 17268 (2006) and Photosynth. Res., 111, 47 (2012).
Transition population of a basis function μ derived by Mulliken method is

tran = Ptran +  S ( Ptran + Ptran ) / 2
 

So, the Mulliken atomic transition charge of atom A should be −

  . Sum of all atomic

tran

A

transition charges must be zero because the total number of electrons keeps unchanged during
electron excitation.
The input files needed by present function have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states when
you enter this function. After that, you should choose the excited state for which the Mulliken
transition charges will be calculated. Then the result will be outputted to atmtrchg.chg file in current
folder, the format of this kind of file has been introduced in Section 2.5, the last column of this file
corresponds to the transition charges.
Below is an example of the calculation. Boot up Multiwfn and input
examples\excit\N-phenylpyrrole.fch
18 // Electron excitation analysis
12 // Calculate Mulliken transition charges
N-phenylpyrrole.out
3 // Study the transition from ground state to the third excited state
Then you will find atmtrchg.chg in current folder.
Note that in Multiwfn it is also possible to calculate the TrEsp (transition charge from
electrostatic potential) introduced in J. Phys. Chem. B, 110, 17268 (2006), which is derived by ESP
fitting method based on transition density. See Section 4.A.9 on how to do this. For studying exciton
coupling purpose, TrEsp should work better than Mulliken atomic transition charge, but for large
systems, cost of evaluating the former is is significantly higher than the latter.
Information needed: See beginning of Section 3.21

3.21.13 Generate natural orbitals of specific excited states (13)
This function is used to generate natural orbitals (NOs) for a batch of selected excited states,
and then export the NOs as .mwfn file. After that, if you want to perform wavefunction analysis for
an excited state, you can simply load corresponding .mwfn file. Of course, you can also calculate
e.g. density difference between two excited states using corresponding two .mwfn files via custom
operation feature of main functions 3, 4 and 5.
To use this function, you should load a file containing basis function information, and then load

269

--- page break ---

3 Functions
a file containing configuration coefficients when you enter this file, see beginning of Section 3.21
for detail. After that, you will be prompted to input the indices of the excited state for which NOs
will be generated. For each selected excited state, the program will do below steps:
(1) Generating density matrix of excited state PES (note that the density matrix constructed in
this way corresponds to unrelaxed density):

PES = PGS + Plocal + Pcross
where PGS is density matrix of ground state, Plocal and Pcross are local part and cross part of
variation of density matrix of excited state with respect to ground state, respectively.
The local part is calculated as

Plocal = (wia )2 (−Pii + Paa ) + ( wia )2 ( −Paa + Pii ) .
i →a

i a

where i and j loop over all occupied MOs, while a and b loop over all virtual MOs. The matrix like
Prs is evaluated as follows, where Cr is column vector of expansion coefficients of MO r

P rs = Cr CTs
The cross part is calculated as

Pcross =   wia wib P ab −   wia wib P ab
i → a i →b  a

i  a i b  a

−   w w P +   wia waj Pij
i →a j i →a

a
i

a
j

ij

i a j i a

(2) Diagonalizing the PES to yield NOs. Each NO is an eigenvector of PES, the accompanied
eigenvalue is occupation number of the NO.
(3) Exporting information of basis function and NOs to .molden file. If the excited state you
selected is 2, then they will be exported as NO_0002.mwfn in current folder.
This function supports both closed-shell and open-shell reference states. For the latter case,
density matrices of alpha and beta spins are calculated separately, and natural orbitals of alpha and
beta spins are generated and exported to the .mwfn file respectively.
The example given in Section 4.18.13 fully utilizes this function.
In addition, in http://sobereva.com/wfnbbs/viewtopic.php?pid=2446 I illustrated the full steps
of generating natural orbitals for states solved by SF-TDDFT calculation of ORCA program.
Information needed: See beginning of Section 3.21

3.21.14 Calculate Λ index to characterize electron excitation (14)
Theory
In the paper J. Chem. Phys., 128, 044118 (2008),  index was proposed to distinguish types of
electron excitations. The form of the  index and the r index (see Section 3.21.4) is very similar.
 index can be expressed as

 =   ia
i ,a

270

--- page break ---

3 Functions
where ai is contribution of MO transition between i and a to the  index:

( Kia )2
 =
 (r )  (r ) d r
 ( Kia )2  i a
a
i

i, a

All quantities involved in above expression are identical those in r index. The integral corresponds
to overlap extent of the MO i and a, it is calculated numerically via Becke's multicenter grid-based
integration approach. The default grid is a good compromise between cost and accuracy; if you want
to change it, you can set "iautogrid" in settings.ini to 0 and then specify "radpot" and "sphpot" in
settings.ini as your expected values. Note that calculation cost of the  index is by far higher than
the r index, because of the above numerical integral step is expensive, especially for large systems.
The theoretical lower and upper limits of the  index are 0.0 and 1.0, respectively; the former
(latter) corresponds to the case that hole and electron are completely separated (perfectly
overlapped).
As the r index, the  index is useful for distinguishing type of electron excitations. Notice
that their intrinsic characteristics are different, the r index is essentially an indicator of
configuration weighted orbital separation distance, while  index reflects configuration weighted
orbital overlapping extent. In some sense, the physical nature of r and  indices are similar to the
D and Sr indices defined in hole-electron analysis framework, respectively (see Section 3.21.1.1),
however I believe that the D and Sr indices are more reasonable, since their physical meanings are
more clear and couplings between different configurations are fully taken into account. Therefore,
without special reasons, using D and Sr indices is more recommended.
It is worth to mention that if an electron excitation can be perfectly represented by one pair of
MO transition, then the  index and Sr index defined in hole-electron analysis framework will be
exactly identical in principle:

 =   i (r )  a (r ) d r
Sr =   hole (r )  ele (r ) d r =   i (r )  a (r ) d r =  i (r )  a (r ) d r
2

2

However, their values outputted by Multiwfn should be marginally different, since they are
evaluated based on different numerical integration algorithms.
Usage
The input files needed by present module have been detailedly described at the beginning of
Section 3.21, namely you should load a file containing basis function information when Multiwfn
boots up, and then load a file containing configuration coefficient information of excited states.
After entering present function (subfunction 14 of main function 18), the matrix containing
overlap integral between norms of all occupied and unoccupied MOs will be evaluated first, then
you will be prompted to select the excited states for which the  will be calculated, then the results
will be printed on screen immediately.
If you only selected one state, then Multiwfn will ask you to choose if decomposing the  into
orbital pair contributions. If you inputted e.g. 0.01, then orbital pairs which have contribution to 
larger than 0.01 will be printed.

271

--- page break ---

3 Functions
An example of present function is provided as Section 4.18.4.
Information needed: See beginning of Section 3.21.

3.21.15 Print major MO transitions in all excited states
This is a useful function used to show major MO transitions for all excited states, so that you
can quickly recognize basic characteristics of various excited states in terms of MOs.
Below is an example. Boot up Multiwfn and input
examples\excit\D-pi-A.out // Output file of TDDFT task of Gaussian
18 // Electron excitation analysis
15 // The present function
You can see below information immediately, including excitation energy, spin multiplicity,
notable MO transitions and their contributions of each excited state.
#

1

3.9069 eV

317.35 nm

f=

0.01880

Spin multiplicity= 1:

f=

0.63550

Spin multiplicity= 1:

f=

0.00010

Spin multiplicity= 1:

0.01350

Spin multiplicity= 1:

H-4 -> L 81.9%, H-4 -> L+2 12.1%
#

2

4.0624 eV

305.20 nm

H -> L 86.0%, H-3 -> L 5.3%
#

3

4.4166 eV

280.72 nm

H-6 -> L 85.3%, H-6 -> L+2 11.9%
#

4

4.7912 eV

258.77 nm

f=

H-2 -> L 54.5%, H -> L+1 27.6%, H-3 -> L+1 6.4%
#

5

4.8872 eV

253.69 nm

f=

0.00790

Spin multiplicity= 1:

H -> L+3 57.3%, H-2 -> L 17.0%, H-1 -> L+2 8.8%, H-1 -> L 8.0%

From above output, for example, we can find HOMO-4 → LUMO transition contributes 81.9% to
the excitation from ground state to S1 state.
For open-shell cases, orbital spins are explicitly indicated. For example, Ha-4 means
HOMOalpha−4.
By default, only MO transitions with contribution larger than 5% are printed. The printing
threshold corresponds to 10 times of "compthres" parameter in settings.ini.
You can use output file of ZINDO/CIS/TDHF/TDA-DFT/TDDFT task of Gaussian, ORCA,
GAMESS-US/Firefly as input file. Unlike most functions in main function 18, the file containing
basis function information is not needed in the present function.

3.21.16 Charge-transfer spectrum (CTS) analysis
This function is used to calculate data for plotting the charge-transfer spectrum (CTS). At the
meantime, major characters given by IFCT analysis of all excited states are presented.
The idea of CTS was firstly proposed by me in Carbon, 187, 78-85 (2022) DOI:
10.1016/j.carbon.2021.11.005 for studying spectrum nature of C18@Li complex, please cite this
paper if this method is employed in your study.
Theory of CTS
Please first recall the hole-electron analysis introduced in Section 3.21.1. For every excited

272

--- page break ---

3 Functions
state, it is able to calculate hole and electron distributions. Using Mulliken-like partition or Hirshfeld
partition, contributions of various fragments to hole and electron can be calculated. Then, according
to the IFCT analysis introduced in Section 3.21.8, amount of intrafragment electron redistribution
and amount of interfragment electron transfer can be calculated. Sum of all redistribution terms and
electron transfer terms of an excited state equals to unity.
As introduced in Section 3.13.1, UV-Vis spectrum is obtained via broadening excitation
energies (Eexc) and oscillator strength (f) of all excited states by Gaussian function (G).
Mathematically, the spectrum curve is expressed as

 ( E ) = c fi G( E − Eiexc )
i

where (E) is molar absorption coefficient at energy E. i loops over all excited states. c is a constant.
The CTS aims at graphically exhibit contribution of electron transfer component and
redistribution component to UV-Vis spectrum. The idea is very simple, and only the f will be
modified. Assume there are two fragments, A and B, then the absorption curve of CTS corresponding
to electron transfer from A to B is expressed as

 A, B ( E ) =  fiQiA, BG( E − Eiexc )

i
𝐴,𝐵
where 𝑄𝑖 is amount of electron transfer from A to B of excited state i.

The absorption curve of CTS corresponding to electron redistribution within fragment A is
expressed as

 A, A ( E ) =  fiQiA, AG( E − Eiexc )

i
𝐴,𝐴
where 𝑄𝑖 is amount of electron redistribution within fragment A of excited state i.
Because 𝑄𝑖𝐴,𝐴 + 𝑄𝑖𝐵,𝐵 + 𝑄𝑖𝐴,𝐵 + 𝑄𝑖𝐵,𝐴 = 1 (see Section 3.21.8 for proof), sum of four types of

charge-transfer spectra is exactly the UV-Vis spectrum:

 A, A ( E ) +  B , B ( E ) +  A, B ( E ) +  B , A ( E ) =  ( E )
It is obvious that charge-transfer spectra are able to make the underlying nature of significant
peaks of UV-Vis spectrum very easy to recognize. In order words, the total UV-Vis spectrum is
decomposed as different subparts corresponding to different physical natures.
Procedure of plotting CTS
(1) Prepare input files for present function. The input file is exactly identical to hole-electron
or IFCT analysis, namely a file containing basis function information and a file containing
configuration coefficients of excited states. See Section 3.21.A for detail of generation of these files
via quantum chemistry programs.
(2) Calculate IFCT data and generate data files used for plotting CTS. Boot up Multiwfn and
load the file containing basis function information. Enter present function (subfunction 16 of main
function 18). Input the number of fragments (there is no upper limit), input atomic indices for each
fragment, then input the path of the file containing configuration coefficients. Finally, choose the
method for calculating fragment contributions to hole and electron. After that, contribution to hole
and electron of each fragment will be computed for every excited state in turn.
Hint: If there are very large number of atoms and excited states and diffuse functions were not employed,
choosing Mulliken method is suggested because it is fairly fast. However, if diffuse functions were employed, you
have to choose the more expensive but more robust Hirshfeld method.

After the calculation is complete, you can find IFCTdata.txt in current folder, which contains

273

--- page break ---

3 Functions
full IFCT data for all excited states. The IFCTmajor.txt in current folder records major IFCT terms
(those with contribution larger than 5%), from which you can easily recognize major characters of
all excited states. You also have a batch of files in the newly created "CT_multiple" subfolder of
current folder; in which the CT_multiple.txt is the file used in spectrum plotting module of Multiwfn;
if you open it by text editor you can see it contains path of many files with labels. Specifically, the
total_spectrum.txt is used to plot UV-Vis spectrum, the files with "ET_" prefix is used to plot
interfragment electron transfer spectra, the files with "Redis_" prefix is used to plot intrafragment
electron redistribution spectra. Note that when the "CT_multiple" subfolder is moved, you should
also manually modify the paths of the included files.
(3) Boot up Multiwfn, use the CT_multiple.txt in "CT_multiple" subfolder as input file, then
enter main function 11, select "UV-Vis", and choose option 0 to plot the spectrum. You will find the
interfragment electron transfer spectrum and intrafragment electron redistribution spectrum together
with UV-Vis spectrum are shown. You can also use the rich options in the interface to improve the
graph, see Section 3.13.3 for explanation.
An example of calculating IFCT data for a batch of excited states and plotting CTS is given in
Section 4.18.16.

3.21.17 Electron density polarization analysis based on electron
excitations
1. Introduction
An applied external potential, v(r), can cause polarization of electron density of a chemical
system. Usually, the corresponding variation of electron density, which will be referred to as density
polarization (pol) later, can be obtained by taking difference between the densities obtained with
and without v. Obviously, this needs two single point calculations to generate wavefunction files
of the respective status.
J. Phys. Chem. A, 124, 633 (2020) proposed a novel way of evaluating and analyzing pol based
on electron excitation calculations (e.g. TDDFT). Currently only the v consisting of one or more
point charges are explored. There are some practical applications illustrated in this paper, also this
method has been employed in J. Comput. Chem., 42, 1118 (2021) to study substitution effect on the
performance of Mo-oxo catalyst. This method has two unique advantages:
(1) After a regular electron excitation calculation with a sufficient number of excited states,
one can easily obtain the pol induced by an arbitrary time-independent v. That means one do not
need to perform a quantum chemistry calculation for each v of interest. But note that the pol
obtained via this method is less accurate than that obtained via the aforementioned traditional
method, because this method was derived based on the low-order perturbation theory. This also
implies that the v should not be too strong. For example, it may be a point charge of no more than
0.1 e, while 0.5 e may be too large unless it was placed far from the system.
(2) More importantly, this method is able to provide deep understanding of and chemical
insights into pol in terms of electron excitations. One can gain information about which excitation(s)
contribute significantly to the pol, and discuss why the contributions are significant by further
analyzing the distribution of the transition density of the excitations. In other words, this method

274

--- page break ---

3 Functions
decomposes pol to reveal its nature.
2. Theory
According to perturbation theory, the ground state wavefunction perturbed by v can be linearly
expanded by the ground (k=0) and excited states (k>0) without the perturbation at the same geometry:
∞
(0)

|Ψ0 ⟩ = ∑ 𝑐𝑘 |ψ𝑘 ⟩
𝑘=0

where
(0)

𝑐𝑘 = −

(0)

⟨Ψ𝑘 |𝛿𝑣|Ψ0 ⟩
(0)

(0)

𝐸𝑘 − 𝐸0

=−

∫ 𝜌0𝑘 (𝐫)𝛿𝑣(𝐫)d𝐫
(0)

(0)

𝐸𝑘 − 𝐸0

in which the denominator is the excitation energy of excited state k, while 𝜌0𝑘 is the transition density
between ground state and this excited state. Definition of transition density can be found in Section
3.21.1.1. Clearly, for an excited state, the smaller the excitation energy, and the larger the effective
overlap (with care of phase cancellation) between the distribution of transition density and external
potential, the larger the magnitude of the coefficient. Note that the magnitude is much more sensitive
to the latter. The square of the coefficients of k>0 can be understood as the contributions from the
electronic states. The square of the coefficient of ground state (k=0) is always close to 1.0 because
of the assumption of weak v.
Further, with the condition 𝑐02 ≈ 1, the density polarization, which is the difference between
the electron density with and without the perturbation (𝜌pert and 𝜌0), can be evaluated as follows
∞

𝜌pol (𝐫) = 𝜌pert (𝐫) − 𝜌0 (𝐫) ≈ 2 ∑ 𝑐𝑘 𝜌0𝑘 (𝐫)
𝑘=1

Integral of pol over the whole space must be zero since the perturbation does not alter the
number of electrons, nonetheless, it is useful to characterize the amount of electrons polarized (N);
to this aim, one can integrate positive or negative part of pol, or equivalently, calculate it as 𝛿𝑁 =
1
2

∫ |𝜌pol (𝐫)|d𝐫.

Finally, according to the second-order perturbation theory, the first-order perturbation
correction energy represents the interaction energy between the external potential and permanent
electron distribution
(0)

(0)

𝐸 (1) = ⟨Ψ0 |𝛿𝑣|Ψ0 ⟩ = ∫ 𝜌0 (𝐫)𝛿𝑣(𝐫)d𝐫
and the second-order correction energy represents the energetic stabilization experienced by the
system by distorting its electron density (caused by mix of ground and excited states) in response to
the perturbation
∞

𝐸

(2)

=∑
𝑘>0

(0)

(0)

2

|⟨Ψ𝑘 |𝛿𝑣|Ψ0 ⟩|
(0)
(0)
𝐸0 − 𝐸𝑘

∞
(0)

(0)

= ∑ 𝑐𝑘2 (𝐸0 − 𝐸𝑘 )
𝑘>0

It is seen that E(2) can be exactly decomposed into contributions of different excited states, from
which one can recognize the significance of different excited states in the response to the
perturbation from an energetic point of view.
3. Practical guidance

275

--- page break ---

3 Functions
Arbitrary number of point charges can be taken as the external potential, which is expressed as
𝛿𝑣(𝐫) = − ∑𝑛𝑖 𝑞𝑖 ⁄|𝐫 − 𝐑 𝑖 | , where qi and Ri are value and coordinate vector of point charge i,
respectively. The negative sign comes from the fact that electron carries charge of -1 e. The point
charge(s) can be placed anywhere as long as you believe it is meaningful. Please check the original
paper for illustrative applications. For example, to approximately mimic the effects of an electronwithdrawing (electron-donating) substituent, one can place a point charge of +0.1 e (-0.1 e) at the
nuclear position of the atom linking to the substituent, see J. Comput. Chem., 42, 1118 (2021) for
instance. For another example, to examine the effect of a Lewis base approaching the system on the
electron distribution, a slight negative point charge can be placed at the position of the Lewis base
atom in the expected reaction complex.
Theoretically, any methods for calculating excited states may be used in this analysis as long
as excited state wavefunction is available. However, in the present implementation, only the
methods based on singly excited configurations, are supported, such as CIS and TDDFT. Usually
TDDFT with a proper DFT functional is the preferential choice. Generally, I recommend using
B97XD or CAM-B3LYP in combination with def-TZVP.
In the expressions give above, infinite number of excited states are involved, clearly this is
inaccessible in the practical study. The larger the number of excited states to be calculated, the higher
the computational cost in the quantum chemistry program and Multiwfn, while the lower the risk
that important excited states are overlooked. In the original papers and J. Comput. Chem., 42, 1118
(2021), 50 excited states were taken into account, this choice is likely to be a good starting point,
but larger number of states may be needed in certain cases.
In Multiwfn, the integral involved in evaluating {ck} is calculated based on uniform grids. The
smaller the grid spacing, the better the accuracy, while the higher the cost. For a small system, I
suggest using the very fine grid spacing 0.1 or 0.15 Bohr, while for a large system, considering the
high computational cost, a larger grid spacing such as 0.2 or 0.25 Bohr have to be used (to guarantee
numerical accuracy, performing a convergence test of grid spacing is suggested).
4. Usage
The present function is able to calculate pol, {ck}, E(2), N. The needed input files for this
analysis are exactly the same as those described in Section 3.21.A.
The steps of using this function are as follows:
(1) Load a file containing basis function information when Multiwfn boots up. For example,
the .fchk file resulting from TDDFT calculation of Gaussian
(2) Enter subfunction 17 of main function 18
(3) Input total number of point charges, and then input X, Y, Z coordinates and charge value
for each of them
(4) Setting up grid
(5) Load a file containing the configuration coefficients from an excited state calculation, such
as Gaussian output file of TDDFT task
Then Multiwfn starts to calculate data for each excited state. Once calculation is finished,
excitation energy, ck and E(2) of each excited state is shown on screen, and then total E(2) and N are
given. Integral of pol over the whole space is also shown, the more it is close to 0, indicating that
the more the current grid quality is satisfactory.
Later, post-processing menu appears, in which pol, v, and transition densities of the excited
states of interest (for example, the ones with largest |ck|), and be directly plotted as isosurface maps,

276
