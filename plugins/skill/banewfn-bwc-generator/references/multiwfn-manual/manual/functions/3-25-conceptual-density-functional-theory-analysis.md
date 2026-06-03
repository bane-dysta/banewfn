# 3.25 Conceptual density functional theory (CDFT) analysis (22)

## Quick navigation

- path: 3  Functions > 3.25 Conceptual density functional theory (CDFT) analysis (22)
- pdf pages: 333-342
- category: functions
- main menu / option numbers mentioned in title: 22
- direct child sections: 5
- total descendant sections: 5

## When to consult this file

Use this file for the actual Multiwfn function description: menu number, theory, options, inputs, and detailed behavior.

## Keywords and aliases

3.25, Conceptual, density, functional, theory, CDFT, 3.25.1, 3.25.2, Usage, 3.25.3, Orbital-weighted, Fukui, dual, descriptor, 3.25.4, Quasi-, degenerate, based, electron, 3.25.5, Nucleophilic, electrophilic, superdelocalizabilities

## Direct child sections

- **3.25.1 Theory** (pdf pages 334-336)
- **3.25.2 Usage** (pdf pages 337-338)
- **3.25.3 Special topic: Orbital-weighted Fukui function and dual descriptor** (pdf pages 339-339)
- **3.25.4 Special topic: (Quasi-)degenerate Fukui function and dual descriptor based on electron density** (pdf pages 340-341)
- **3.25.5 Special topic: Nucleophilic and electrophilic superdelocalizabilities** (pdf pages 342-342)

## Extracted manual text

3 Functions










atom is shown on screen, and a file named atomdisp.pqr can be exported, in which the
“charge” property (third to last column in the file) corresponds to the atomic contributions
to dispersion energy. If you load the file into VMD program, you can color atoms
according to the “charge” property to intuitively exhibit the atomic contributions.
Calculate dispersion density for current system: You will be asked to define grid setting,
then grid data of dispersion density will be calculated, and then can be exported as
dispdens.cub in current folder. After that, you can use such as VMD and VESTA to load
it and plot isosurface of dispersion density.
Calculate difference of atomic contributions to dispersion energy between current and
another systems: You will be asked to define fragment for present system (fragment i for
system A), and then be asked to input file path of another system and define a fragment
for it (fragment j for system B). The two systems do not necessarily correspond to the
same system, but the two fragments must have the same number of atoms and same
atomic order. After that, Multiwfn will print total dispersion energy for system B and then
for system A in turn. Then difference between contributions of atoms in fragment i to
dispersion energy of system A and contributions of atoms in fragment j to dispersion
energy of system B will be printed, and diffatomdisp.pqr can be exported, whose “charge”
property corresponds to the difference.
Calculate dispersion density difference between current and another systems. Similar to
the last function, but grid data of ∆𝜌disp is calculated and exported as dispdensdiff.cub in
current folder.
Calculate contribution of a fragment to dispersion energy
Calculate dispersion interaction energy between two fragments

Usage
This module corresponds to subfunction 4 of main function 21.
The input file must contain atom information, such as xyz, pdb, mol2, gjf, fch, mwfn, see
Section 2.5 for comprehensive introduction. If the input file contains cell information, then the
calculation will be considered as periodic.
This function needs Grimme’s dftd3 code modified by Tian Lu, which can be downloaded at
http://sobereva.com/soft/dftd3_TLmod.zip, and contains modified source code, precompiled
Windows version of executable file (dftd3.exe), precompiled Linux version of executable file (dftd3).
You should set “dftd3path” in settings.ini to actual path of the dftd3 executable file before booting
up Multiwfn, so that Multiwfn can invoke dftd3. In the case of Linux environment, do not forget to
use “chmod” to add executable permission for the dftd3.
Examples of this analysis are given in Section 4.21.4.

3.25 Conceptual density functional theory (CDFT) analysis
(22)
The conceptual density functional theory (CDFT) originally developed by Robert Parr is a

312

--- page break ---

3 Functions
theory framework aiming for unraveling reactivity of chemical systems. CDFT contains numerous
concepts and quantities, some of them can be used to predict favorable reactive sites and reactive
character, and some of them can compare reactivity among different chemical species. Due to the
high popularity and important role of CDFT in quantum chemistry, as well as there are so many
meaningful relevant quantities, I believe it is very useful to develop a module to calculate all
commonly investigated quantities involved in CDFT with minimal steps.
In the Part 1 of this section, I will briefly describe the definition of all quantities that can be
studied via this module; then in the Part 2, I will show how to use this module. This module is able
to calculate the so-called orbital-weighted quantities, which will be specifically described in Part 3.
Note that aside from CDFT, Multiwfn also supports many other methods for revealing reactive
sites, see Section 4.A.4 for overview.
If the functions described in this section is used in your research, please NOT ONLY cite
original paper of Multiwfn, BUT ALSO cite the following book chapter, which comprehensively
introduces feature and implementation of this module:
Tian Lu, Qinxue Chen, Realization of Conceptual Density Functional Theory and InformationTheoretic Approach in Multiwfn Program. In Conceptual Density Functional Theory, WILEY-VCH
GmbH: Weinheim (2022); pp 631-647. DOI: 10.1002/9783527829941.ch31

3.25.1 Theory
To yield all below quantities, electronic energy (E) and electron density of N, N+1 and N-1
electron states must be available. Commonly N refers to the number of electrons carried by a
chemical system at its most stable status. Geometry optimized for N-electrons state is employed for
all calculations.
➢ Global indices
• First vertical ionization potential (I1): E(N-1) − E(N)
• First vertical electron affinity (A): E(N) − E(N+1)
• Mulliken electronegativity (): (I1+A)/2
• Chemical potential (): −
• Hardness (): I1−A, which is also equivalent to fundamental gap. See J. Am. Chem. Soc., 105,
7512 (1983). Note that according to the convention employed by many CDFT papers, the prefix of
1/2 in original definition of  is dropped.
• Softness (S): 1/. See Proc. Nati. Acad. Sci., 82, 6723 (1985)
• Electrophilicity index (): 2/(2). See J. Am. Chem. Soc., 121, 1922 (1999)
• Nucleophilicity index (NNu): EHOMO(Nu) − EHOMO(TCE), where Nu denotes nucleophile, TCE
denotes tetracyanoethylene, whose HOMO energy is almost the lowest one among all organic
molecules and therefore it is chosen as reference system. See J. Org. Chem., 73, 4615 (2008).
➢ Real space functions
• Fukui function f(r) and dual descriptor f(r): See Section 4.5.4 for detailed introduction
• Local softness: s+(r) = Sf +(r), s−(r) = Sf −(r), s0(r) = Sf 0(r) for nucleophilic, electrophilic,
radical attacks, respectively, where f(r) is Fukui function of corresponding type. See Proc. Nati.
Acad. Sci., 82, 6723 (1985)

313

--- page break ---

3 Functions
• Local hyper-softness: s(2)  S2f(r), see J. Math. Chem., 62, 461 (2024)
• Local electrophilicity index: 𝜔loc (𝐫) = 𝜔𝑓 + (𝐫)
loc
• Local nucleophilicity index: 𝑁Nu
(𝐫) = 𝑁Nu 𝑓 − (𝐫)

➢ Atom indices
• Condensed Fukui function (fA) and dual descriptor (fA): See Section 4.7.3 for detailed
introduction
• Condensed local softness
For nucleophilic attack: 𝑠𝐴+ = 𝑆𝑓𝐴+
For electrophilic attack: 𝑠𝐴− = 𝑆𝑓𝐴−
For radical attack attack: 𝑠𝐴0 = 𝑆𝑓𝐴0
• Relative electrophilicity index: 𝑠𝐴+ /𝑠𝐴− , see J. Phys. Chem. A, 102, 3746 (1998)
• Relative nucleophilicity index: 𝑠𝐴− /𝑠𝐴+, see J. Phys. Chem. A, 102, 3746 (1998)
• Condensed local electrophilicity index: 𝜔 𝐴 = 𝜔𝑓𝐴+
𝐴
• Condensed local nucleophilicity index: 𝑁Nu
= 𝑁Nu 𝑓𝐴−
(2)
• Condensed local hyper-softness: 𝑠𝐴 ≈ 𝑆 2 ∆𝑓𝐴

➢ cubic electrophilicity index
The electrophilicity index cubic introduced in J. Phys. Chem. A, 124, 2090 (2020) is somewhat
special, it also relies on N-2 electron states. It includes higher-order term than the aforementioned
electrophilicity index . Its definition is



cubic =   1 +


 

3 2 

In practice, it is calculated as

cubic =


( cubic )2 
cubic
1+


2 cubic 
2cubic  3(cubic )


where

cubic = (1/ 6)(−2 A − 5 I1 + I 2 )
cubic = I1 − A
 cubic = 2 I1 − I 2 − A
where I2 is the second vertical ionization potential and defined as E(N-2) − E(N-1). Correspondingly,
𝐴
there is a cubic form of condensed local electrophilicity index 𝜔cubic
= 𝜔cubic 𝑓𝐴+ . In J. Phys. Chem.
𝐴
A, 124, 2090 (2020) is shown that 𝜔cubic
value of halogen atom (which behaves as Lewis acid due
to its -hole) in halogen-bond dimers R-X···NH3 has excellent correlation with calculated binding
energies (however, note that they employed AIM partition for atomic spaces rather than the
Hirshfeld partition utilized in the present module).
➢ Electrophilic descriptor ()
The electrophilic descriptor () was introduced in Int. J. Quantum Chem., 124, e27366 (2024),
it was shown to correlate with Mayr’s electrophilic parameter (E) significantly better than the
electrophilicity index () using a test set consisting of 35 organic molecules. In contrast to , whose

314

--- page break ---

3 Functions
derivation is only based on second-order Taylor expansion of system energy, derivation of  is based
on third-order expansion, which makes  explicitly involve hyperhardness. Like cubic, calculation
of  also relies on N-2 electron state.
The working equation for computing  is as follows
2

        
 =   −   + 
     2 6
with

 =  2 − 2 − 
It is important to note that the , , ,  involved in above equations should be calculated in a
different way than those described earlier, namely

=a
 = −a
 = 2(b − ac)
 = −3c(b − ac)
where

c=

I 2 − 2 I1 + A '
2 I 2 − I1 − A '

I − A ' I1 + A '
b= 1
−
c
2
2
I + A ' I1 − A '
a=− 1
+
c
2
2
with A’ = E(N+1) − E(N), which is electron affinity but differs with standard definition by the sign.
➢ Fukui potential and dual descriptor potential
Definition and physical meaning of Fukui potential were carefully discussed in J. Phys. Chem.
A, 115, 2325 (2011) and Int. J. Quantum Chem., 101, 520 (2005). Fukui potential is complementary
to ESP in understanding energy variation in the early stage of chemical reaction when electron
transfer is nonnegligible. For example, when an electrophile is attacking by a nucleophile, the
external potential generated by the nucleophile felt by the electrophile is
−
𝑓𝑁−phile
(𝐫′)
ESP
(𝐫) − ∆𝑁 ∫
𝑣𝑁−phile (𝐫) ≈ 𝑉𝑁−phile
d𝐫′
|𝐫 − 𝐫′|
where N is number of transferred electron from the electrophile to nucleophile (negative value in
−
this case), 𝑓𝑁−phile
is Fukui function f− of the nucleophile. Three kinds of Fukui potential are defined
as follows
𝑓 −/+/0 (𝐫′)
d𝐫 ′
|𝐫 − 𝐫′|
ESP is only able to predict regioselectivity dominated by electrostatics effect with assumption
that there is no electron transfer, however, evidently this assumption is far from true for general
chemical reactions (but basically true for many noncovalent interactions). Clearly, Fukui potential
should be more focused on in general reactions, especially for those with significant electron transfer.
𝑉𝑓−/+/0 (𝐫) = ∫

315

--- page break ---

3 Functions
Fukui potential is more rigorous than Fukui function in predicting regioselectivity, as
emphasized in J. Phys. Chem. A, 115, 2325 (2011) that “It is the value of the Fukui potential, more
than the value of the Fukui function itself, that determines the reactive site”. However, Fukui
potential is not so popular as Fukui function, because their distribution characteristics usually
coincide with each other, while evaluation of Fukui potential needs calculating ESP twice, which is
considerably more expensive than evaluating electron density twice. According to finite difference
definition of Fukui function, for example, 𝑉𝑓− can be evaluated as
𝑉𝑓− (𝐫) = ∫

𝑓 − (𝐫′)
𝜌𝑁 (𝐫′) − 𝜌𝑁−1 (𝐫′)
ESP
d𝐫′ = ∫
d𝐫′ = 𝑉𝑁−1
(𝐫) − 𝑉𝑁ESP (𝐫)
|𝐫 − 𝐫′|
|𝐫 − 𝐫′|

Furthermore, dual descriptor potential (DDP) was introduced in J. Math. Chem., 62, 1094
(2024), which is defined as
∆𝑓(𝐫 ′ ) ′
𝑓 + (𝐫 ′ ) − 𝑓 − (𝐫′ ) ′
𝐷𝐷𝑃(𝐫) = ∫
d𝐫
=
d𝐫 = 𝑉𝑓+ (𝐫) − 𝑉𝑓− (𝐫)
∫
|𝐫 − 𝐫 ′ |
|𝐫 − 𝐫 ′ |
Unlike dual descriptor, which often has many nodal planes hindering discussion, distribution of
DDP is much smoother, enabling researchers to identify preferential reactive sites easier.
Like the Fukui function, the more positive the 𝑉𝑓+ is in a region, the more likely it is for
nucleophilic reaction to occur, and the region where the 𝑉𝑓− is more positive, the more likely it is
for electrophilic reaction to occur. Similar to dual descriptor, the more positive and negative the DPP
is in a region, the more likely this region is susceptible to undergo nucleophilic and electrophilic
attacks, respectively.

3.25.2 Usage
For using the present module, namely main function 22, the file loaded after booting up
Multiwfn is relatively arbitrary, the only requirement is that the atomic information in this file is
identical to the system under study.
After entering the present module, you will see a menu, in which the options 2, 3 and 9 are
used to calculate above quantities. Before using them, generally you should provide N.wfn, N-1.wfn
and N+1.wfn in current folder, which contain wavefunction and electronic energy of N, N-1 and
N+1 states respectively for present system, the geometries must be the same and correspond to the
optimized geometry of N state. The calculation level of the three files must also be the same. If any
of the three .wfn files is missing, Multiwfn will ask you to manually input path of .wfn file for
corresponding state (.wfx, .fch and .mwfn files are also allowed, since they also carry wavefunction
and electronic energy information).
Option 2: Used to calculate all aforementioned global indices and atomic indices, the result
will be exported to CDFT.txt in current folder. Because as mentioned in Section 4.7.3, Hirshfeld
method is an ideal choice for calculating condensed Fukui functions and may be other relevant
atomic indices, therefore Hirshfeld charges are automatically calculated and used for evaluation of
all atomic indices. Nucleophilicity index as well as its local version are dependent of HOMO energy
of TCE, which should be calculated using the same level for present system, notice that these indices
printed in present module simply employ the EHOMO(TCE) = -0.335198 Hartree calculated at the
commonly used B3LYP/6-31G* level (clearly, if you want to get more reliable result and your
current calculation level is not B3LYP/6-31G*, you should calculate EHOMO(TCE) yourself and then
manually evaluate these indices).

316

--- page break ---

3 Functions
Option 3: Used to calculate grid data of Fukui function, dual descriptor and functions related
to them, then their isosurface maps can be directly visualized, the grid data can be exported to cube
files in current folder. In this option you can set the scale factor to be multiplied to the calculated
grid data. For example, if you set the scale factor to the global softness outputted by option 2, then
the scaled f −(r) Fukui function will correspond to s−(r).
Option 9: Similar to option 3, but used to calculate grid data of Fukui potential and dual
descriptor potential. Because grid data of ESP is needed to be calculated for each charged state,
which is often expensive, you should choose a proper grid setting to avoid being too time consuming.
Generation of .wfn files
You can manually prepare the .wfn files used by options 2 and 3, alternatively, you can use
option 1 of present module to automatically realize the preparation work.
After choosing option 1, you will be prompted to input Gaussian keywords for single point
task, then Multiwfn asks you to input charge and spin multiplicity for N, N+1 and N-1 states in turn,
then Gaussian single point input files N.gjf, N+1.gjf and N-1.gjf will be generated in current folder
(the geometry in these files correspond to the geometry in the input file of Multiwfn). Then, you can
manually use Gaussian to run them to obtain N.wfn, N+1.wfn and N-1.wfn, or if Gaussian has been
installed on your computer, you can directly let Multiwfn to invoke Gaussian to calculate them (in
this case the "gaupath" parameter in settings.ini must have been set to actual path of Gaussian
executable file), after calculations the three .wfn files will appear in current folder.
Sometimes we need to use mixed basis set, in this case you should prepared a file named
basis.txt in current folder, which records the definition of basis set (may be also accompanied by
pseudopotential definition). If the inputted keyword contains "gen" or "genecp", then the content of
basis.txt will be automatically appended to the end of the generated .gjf files.
Multiwfn is also able to generate input file of ORCA for producing the three .wfn files. You
should choose option -2 and select ORCA, then choosing option 1 will generate N.inp, N-1.inp and
N+1.inp. If you have set “orcapath” in settings.ini to actual path of ORCA executable file, you can
directly let Multiwfn to invoke ORCA to run them to yield N.wfn, N-1.wfn and N+1.wfn;
alternatively, you can run them by ORCA manually, and then put the resulting N.wfn, N-1.wfn and
N+1.wfn in current folder.
To use present module to study large organic systems, commonly I suggest using B3LYP/631G* level, because this level is inexpensive, while the quality of the yielded quantities is already
satisfactory.
Note on calculating cubic and 
By default Multiwfn does not calculate cubic and , because they rely on N-2 electron state. If
you need them, you should first select option -1 to switch the status to "Yes". Then you can use
option 1 to help you to prepare .wfn file for N, N+1, N-1, N-2 electron states, or you manually
provide them. Then after selecting option 2, the resulting CDFT.txt file will contain condensed local

cubic, global cubic, , as well as I2.
An example of using present module to calculate various CDFT quantities for phenol is
provided in Section 4.22.1. The example of calculating Fukui potential and dual descriptor potential
for maleic anhydride is given in Section 4.22.4.

317

--- page break ---

3 Functions

3.25.3 Special topic: Orbital-weighted Fukui function and dual
descriptor
Theory
The originally defined Fukui function and dual descriptor do not work well when frontier
molecular orbitals are (quasi-)degenerate. For example, when HOMO and HOMO-1 have very
similar or exactly identical energies, the Fukui function f − may be unable to give meaningful result
or the result is fully misleading; in addition, when the system shows point group symmetry, such as
C60 fullerene, the distribution of f − is usually not in consistency with molecular symmetry, this is an
apparently unexpected observation.
In order to address these problems, in J. Comput. Chem., 38, 481 (2017), the authors proposed
orbital-weighted Fukui function, and in J. Phys. Chem. A, 123, 10556 (2019), they further proposed
orbital-weighted dual descriptor, they are summarized below (the 𝑓𝑤0 is defined by me)

f w+ (r ) =





i = LUMO

wi |  i (r ) |2

exp[ −(  −i ) 2 ]

wi =



 exp[−(   ) ]

i = LUMO

− i 2


exp[ −(  −i )2 ]
wi = HOMO
 exp[−(  −i )2 ]

HOMO

f w− (r ) =  wi | i (r ) |2
i

i
+
w

−
w

f (r ) = [ f (r ) + f (r )] / 2
0
w

f w (r ) = f w+ (r ) − f w− (r )
where i and i are energy and wavefunction of orbital i;  is chemical potential and approximately
calculated as (EHOMO+ELUMO)/2 in the above formulae. The  is an adjustable parameter, in principle
its best value is the one able to make the functions have ideal predictability of local reactivity.
Clearly the most appropriate  is dependent of practical system, usually 0.1 Hartree is a worthtrying guess. If you find the orbital-weighted functions under this value do not work well, you can
try to properly change it and redo calculations.
Compared to the frozen orbital approximation form of f −, namely f −(r)=|HOMO(r)|2, the
advantage of 𝑓𝑤− is that it takes all orbitals into account with different weights. From the expression
it can be seen that the closer a low-lying orbital energy is to the HOMO energy, the greater its weight.
Evidently degenerate orbitals share the same weight. The Gaussian function involved in the formula
behaves as a decay function, the larger the , the higher the contribution of low-lying orbitals to the
𝑓𝑤− . When energy difference between HOMO and HOMO-1 is significant, there will be no reason
to use 𝑓𝑤− instead of f −. The situation is similar for 𝑓𝑤+ , 𝑓𝑤0 and ∆𝑓𝑤 .
Usage
Since orbital-weighted functions involve virtual orbitals, you should use .mwfn, .fch, .molden
or .gms file as input file. Commonly, the geometry in the input file should correspond to the
optimized geometry of N-electron state; however, it is also possible to study them for
nonequilibrium structure, such as a point in intrinsic reaction coordinate (IRC).
Only closed-shell single-determinant wavefunction is acceptable. Diffuse functions should not
be used if you intend to calculate 𝑓𝑤+ , 𝑓𝑤0and ∆𝑓𝑤 , since they utilize virtural orbitals, whose chemical

318

--- page break ---

3 Functions
meaning may be severely broken when diffuse functions are employed.
In main function 22, four options are related to the orbital-weighted calculation:
• Option 4: Set the  parameter used in the subsequent orbital-weighted calculations
• Option 5: Print the highest 10 weights (i.e. the {w} in the aforementioned formulae) involved
in the orbital-weighted calculations. This option is useful to check if current  parameter is
reasonable and helps user to better understand how the orbital-weighted method works
• Option 6: Calculating condensed 𝑓𝑤+ , 𝑓𝑤− , 𝑓𝑤0 and 𝑓𝑤 values, in other words, calculating
integration of these functions in Hirshfeld atomic spaces. The result is useful in quantitatively
examining net amount of these functions at various atoms. The default radial and angular integration
points are usually fine enough, if you find the sum of condensed 𝑓𝑤+ or 𝑓𝑤− deviates from 1.0
evidently, you should set "iautointgrid" parameter in settings.ini to 0 and then properly enlarge
"radpot" and "sphpot" parameters.
• Option 7: Calculating grid data of 𝑓𝑤+ , 𝑓𝑤− , 𝑓𝑤0 and 𝑓𝑤 functions, then you can directly
visualize their isosurfaces or export them as cube files so that you can render them via third-part
softwares such as VMD and ChimeraX.
Examples of using this module to calculate orbital-weighted Fukui function and orbitalweighted dual descriptor are provided in Section 4.22.2.

3.25.4 Special topic: (Quasi-)degenerate Fukui function and dual
descriptor based on electron density
Theory
In the last section, I have introduced orbital-weighted Fukui function and dual descriptor, which
are suitable when frontier molecular orbitals are (quasi-)degenerate. However, they are defined
based on orbital approximation, namely orbital relaxation effect is not taken into account, while this
effect cannot be always safely overlooked. In J. Comput. Chem., 37, 2279 (2016), an alternative
form of Fukui function and dual descriptor that work for (quasi-)degenerate HOMO/LUMO case
was proposed, and this form is defined directly based on electron density, that means orbital
relaxation effect is fully taken into account as the original Fukui function and dual descriptor. This
(quasi-)degenerate Fukui function and dual descriptor based on electron density will be referred to
as fQ and fQ, respectively.
The idea of fQ is very simple. If at electron state N the degree of degeneracy of LUMO and
HOMO is p and q, respectively, then three forms of fQ are evaluated as follows

f Q+ (r ) =
f Q− (r ) =

 N + p (r ) −  N (r )
p
 N (r ) −  N − q ( r )
q
+
Q

f (r ) = [ f (r ) + f Q− (r )] / 2
0
Q

fQ can be evaluated based on 𝑓Q+ and 𝑓Q− as usual

f Q (r) = f Q+ (r) − f Q− (r)

319

--- page break ---

3 Functions
Clearly, if both HOMO and LUMO are nondegenerate, then fQ and fQ will be equivalent to
the original form of Fukui function and dual descriptor, f and f, respectively.
To reasonably calculate fQ and fQ, it is crucial to properly determine p and q. Commonly they
can be assigned by inspecting energies of several lowest unoccupied MOs and several highest
occupied MOs, respectively. If energy difference between an occupied (unoccupied) MO and
HOMO (LUMO) is very small, e.g. less than 0.01 eV, then they may be regarded as degenerate.
Obvious, there is no strict energy threshold for judging orbital degeneracy, and in some cases you
may need to judge by considering various factors, e.g. HOMO-LUMO gap, reasonableness of actual
calculation result, orbital shape, etc.
Note that fQ and fQ are defined only for closed-shell case.
Multiwfn is not only able to calculate fQ and fQ, but also able to calculate local properties
described in Section 3.25.1 (except for cubic) based on them. For example, local softness with
consideration of degeneracy is product of global softness and fQ. Note that the involved first VEA
and VIP are still evaluated as usual, namely VEA = E(N) − E(N+1) and VIP = E(N-1) − E(N).
Spin multiplicity must be properly chosen for calculating wavefunction file of N+p and N-q
states, usually they should be set to p+1 and q+1, see J. Comput. Chem., 37, 2279 (2016) for detailed
discussion. This setting commonly is able to guarantee that the attached (detached) electrons equally
enter (leave from) all degenerate LUMOs (HOMOs).
Usage
In main function 22, both p and q are defaulted to be 1, namely degeneracy of frontier molecular
orbitals (FMOs) is not taken into account. To manually set p and q and thus consider the
(quasi-)degeneracy effect in the subsequent calculations, you should choose option “-3 Set degree
of FMO degeneracy” first. Then information of 10 lowest unoccupied MOs and that of 10 highest
occupied MOs will be listed on screen (in the case that the input file contains wavefunction
information), you should properly input p and q according to the listed MO energies.
After setting p and q, you can use option 1 to use Multiwfn to help you generate input files of
single point task of Gaussian or ORCA code for N, N+p and N-q states, you will be asked to input
net charge and spin multiplicity for these states. In addition, if either p or q is not equal to 1, then
Multiwfn will also ask you if also generating input file for N+1 and/or N-1 states, because E(N+1)
and E(N-1) are needed to evaluate first VIP, VEA and related quantities in option 2. After running
these input files, .wfn files of the states will be yielded. Of course you can also manually generate
wavefunction files for the states via any of your favourite quantum chemistry codes.
Finally, you can use option 2 or option 3 to calculate various quantities or functions that defined
in the CDFT framework, the output content is exactly identical to the default nondegenerate case,
though p and q have been properly considered in the current situation. Note that if p or q is not equal
to 1, and at the same time either N+1.wfn or N-1.wfn is not available in current folder, then quantities
related to first VIP and VEA will not be calculated and outputted by option 2.
An example is given in Section 4.22.3.

320

--- page break ---

3 Functions

3.25.5

Special

topic:

Nucleophilic

and

electrophilic

superdelocalizabilities
Theory
Nucleophilic and electrophilic delocalizabilities are also known as nucleophilic and
electrophilic superdelocalizabilities, they were proposed by Schüürmann in Environ. Toxicof. Chem.,
9, 417 (1990) and Quant. Struct.-Act. Relat., 9, 326 (1990), and have been employed as molecular
descriptors for building quantitative structure-activity relationship (QSAR) equations. In the
Schüürmann’s work, nucleophilic superdelocalizability (DN) and electrophilic superdelocalizability
(DE) of atom A are defined as follows, respectively
unocc

C2 ,i
A  −  i

D N ( A) = 2  
i

C2 ,i
D ( A) = 2
i A  i − 
occ

E

where i is energy of molecular orbital i,  = (EHOMO + ELUMO)/2, A stands for basis function 
of atom A, and C is coefficient matrix. Evidently, both DN and DE are negative.
However, the expression of superdelocalizabilities by Schüürmann is only suitable for semiempirical calculation, which employs orthonormal basis functions. In Sci. Rep., 5, 13695 (2015), a
different version of electrophilic superdelocalizability was proposed and it is compatible with nonorthonormal basis functions. In this work, it was shown that electrophilic superdelocalizability of
an atom is closely related to its atomic polarizability.
The above definitions of superdelocalizability are based on molecular orbital expansion
coefficients; in contrast, in Multiwfn, superdelocalizabilities are evaluated based on Hirshfeld
partition of atomic spaces, this form is more robust and fully compatible with diffuse functions.
Specifically, in Multiwfn, the nucleophilic and electrophilic superdelocalizabilities are calculated as
follows
unocc

D N ( A) = 2 
i

 A,i
 − i

 A,i
i i − 

occ

D E ( A) = 2

where A,i is composition of atom A in orbital i calculated by Hirshfeld method, see Section 3.10.5
for detail. Multiwfn also calculates the superdelocalizabilities without the  shift parameter, namely
unocc

D N 0 ( A) = 2 
i
occ

D E 0 ( A) = 2
i

 A,i
− i

 A,i

i

Usage
Since superdelocalizabilities involve virtual orbitals, you should use .mwfn, .fch, .molden
or .gms file as input file. Only closed-shell single-determinant wavefunction is acceptable. Diffuse
functions should not be used if you intend to calculate DN and DN0, since it utilizes virtual orbitals,

321
