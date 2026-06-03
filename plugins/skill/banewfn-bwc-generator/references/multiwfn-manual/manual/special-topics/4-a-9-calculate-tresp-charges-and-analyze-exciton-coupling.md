# 4.A.9 Calculate TrEsp (transition charge from electrostatic potential) charges and analyze exciton coupling

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.9 Calculate TrEsp (transition charge from electrostatic potential) charges and analyze exciton coupling
- pdf pages: 1091-1095
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.9, TrEsp, transition, charge, electrostatic, potential, charges, analyze, exciton, coupling

## Extracted manual text

4 Tutorials and Examples
for the CCSDT wavefunction.
The procedure of analyzing CI wavefunction produced by MRCC is exactly the same as those
shown above. Below is an example input file of calculating elongated LiH at FCI/aug-cc-pVDZ
level without frozen core treatment.
basis=aug-cc-pvdz
calc=fci
mem=2500MB
dens=1
core=0

geom=xyz
2

H

0.0

0.0

0.0

Li

0.0

0.0

3.0

4.A.9 Calculate TrEsp (transition charge from electrostatic potential)
charges and analyze exciton coupling
1. Theories about TrEsp
General form of electrostatic potential (ESP) of a molecule, say A, can be written as

ZI
 (r ')
−  aa '
dr'
|r −r' |
I | r − RI |

 aaA ' (r ) =  a ,a ' 

where ZI and RI are nuclear charge and coordinate of atom I, respectively.  is Kronecker function.
a,a is transition density between state a and a'.
The ESP we commonly studied is ESP of a single state, i.e. a=a'. When a and a' correspond to
different states, the potential may be referred to as "transition electrostatic potential", which
measures ESP exerted by excitation corresponding to a-a' transition.
It is known as exact ESP of a single state can often be well approximately represented as the
potential evaluated based ESP fitting charges (e.g. CHELPG and MK charges, see Sections 3.9.10
and 3.9.11),

ZI
 (r ')
qaI
− a
dr'  
|r −r' |
I | r − RI |
I | r − RI |

 aA (r ) = 

where 𝑞𝑎𝐼 is ESP fitting charge of atom I at electronic state a.
In light of this, J. Phys. Chem. B, 110, 17268 (2006) puts forward the concept of TrEsp
(transition charge from electrostatic potential), and shows that exact transition ESP can be well
approximated as below

aaA ' (r ) = − 

 aa ' (r ')
|r −r' |

I
qaa
'
|
r
−
R
I
I |

dr'  

𝐼
where 𝑞𝑎𝑎′
is TrEsp of atom I derived from transition density of a-a'.

1070

--- page break ---

4 Tutorials and Examples
The way of calculation of TrEsp charges is almost exact the same as evaluation of common
ESP fitting charges, the only differences are that the nuclear contribution should be ignored, and
density of a single state should be replaced with transition density between two states.

2. Example of calculating TrEsp charges
Now I use a simple molecule 4-nitroaniline to illustrate how to calculate TrEsp charges for its
S0-S2 transition. Here I assume you are a Gaussian user (If you prefer to use ORCA program, the
steps will be slightly lengthy, but can be significantly simplified via writing a shell script. Please
check #2 of this post for detail: http://sobereva.com/wfnbbs/viewtopic.php?pid=389).

First, run the Gaussian input file examples\4-Nitroaniline_TrESP.gjf, the keywords
PBE1PBE/6-31g(d) TD density=transition=2 out=wfn mean transition density between ground
state (S0) to S2 will be generated at TD-PBE0/6-31G(d) level, and then it will be automatically
diagonalized to yield corresponding natural orbitals, which are finally saved to specified .wfn file.
If you are confused or do not have Gaussian in hand, you can directly download related files from
http://sobereva.com/multiwfn/extrafiles/TrEsp.zip
Boot up Multiwfn and input
S0S2.wfn // The .wfn file generated in above process. Can be found in the TrEsp.zip
7 // Population analysis
12 // CHELPG fitting method (you can also use MK or RESP method instead)
5 // Choose form of ESP
3 // Transition electronic (i.e. the ESP specific for evaluating TrEsp)
1 // Start calculation
Calculation of ESP for even medium-sized systems is time-consuming, you need to wait
patiently. Finally, the TrEsp charges are shown on the screen:
Center

Charge

1(C )

0.2134106035

2(C )

-0.1679471876

3(C )

0.1841565861

[...ignored]
16(O )

-0.0948987209

Sum of charges:
RMSE:

0.000970

-0.0000000000
RRMSE:

0.044908

The sum of charges is exactly zero, which is what we expected, because electronic transition process
does not alter total number of electrons. Then if you want to export X, Y, Z coordinate and TrESP
of atoms to .chg file (see Section 2.5 on description of this format), you can input y.
Beware that, the TrEsp charges obtained in this way must then be manually divided by √2!
This is because the natural orbitals in the exported .wfn file were generated based on symmetrized

1071

--- page break ---

4 Tutorials and Examples
form of transition density matrix (TDM), however the symmetrization was done via a strange way
by Gaussian, namely TDMi,j=(TDMi,j+TDMj,i)/ √2 rather than TDMi,j=(TDMi,j+TDMj,i)/2 as
expected, therefore this problem should be manually fixed via dividing the resulting charges by √2.
In fact, in Multiwfn the transition charge can also be calculated by Mulliken method via holeelectron analysis module, see Section 3.21.1.3, and the computational cost is almost negligible.
However, Mulliken transition charges must not be as good as TrEsp charges for approximately
representing transition electrostatic potential and analyzing intermolecular exciton coupling
purposes.
Skill 1: Accelerating calculation of TrEsp by making use of cubegen utility
If number of your CPU cores is limited (less than 10 cores), making use of cubegen utility in
Gaussian package may notably decrease cost of ESP relevant analyses, please read Section 5.7 for
detail. The cubegen can also be used to reduce computational cost of TrEsp charges, the procedure
is described below.
Since cubegen calculate ESP based on density matrix information in .fch/fchk file, we must
first generate TDM and store it into a .fch file, the function mentioned in Section 3.21.9 can do this.
We first use PBE1PBE/6-31g(d) TD IOp(9/40=4) keywords in Gaussian to carry out electron
excitation calculation and meantime keep the .fch file, the corresponding files for 4-Nitroaniline are
4-Nitroaniline_IOp.gjf, 4-Nitroaniline_IOp.out and 4-Nitroaniline.fchk in the aforementioned
TrEsp.zip package.
Boot up Multiwfn and input
4-Nitroaniline.fchk
18 // Electron excitation analysis
9 // Generate and export TDM
1 // Generate TDM between ground state and excited state
4-Nitroaniline_IOp.out
2 // Generate TDM between S0 and S2
y // Symmetrize the resulting TDM in usual way, namely TDMi,j=(TDMi,j+TDMj,i)/2
y // Export TDM.fch, whose density matrix field corresponds to the just generated TDM
Please make sure that "cubegenpath" parameter in settings.ini has been set to actual path of
cubegen utility in Gaussian folder, then reboot Multiwfn and input
TDM.fch
7 // Population analysis
12 // CHELPG fitting method
5 // Choose form of ESP
3 // The ESP type specific for evaluating TrEsp
1 // Start calculation
Immediately the TrESP charges are shown on screen. You do not need to manually divide the
resulting charges by √2, because the TDM generated by Multiwfn has already been symmetrized in
a correct way.
It is worth to note that if you want to verify whether the fitted TrEsp charges are reasonable,
you can compare the electric dipole moment computed via these charges and the transition electric
dipole moment printed by Gaussian (or other quantum chemistry codes). As it is well known that
ESP fitting charges are able to well reproduce electric dipole moment, commonly the TrEsp charges

1072

--- page break ---

4 Tutorials and Examples
are also able to well reproduce actual electric transition dipole moment.
After calculation of TrEsp charges, we choose y to let Multiwfn export the charges to TDM.chg
file in current folder. Then boot up and load this file, you will find below information on screen
Component of electric dipole moment:
X=

-0.011607 a.u.

(

-0.029501 Debye )

Y=

-1.781254 a.u.

(

-4.527495 Debye )

Z=

0.000011 a.u.

(

0.000029 Debye )

In 4-Nitroaniline_IOp.out you can find below information
Ground to excited state transition electric dipole moments (Au):
state

X

Y

Z

Dip. S.

Osc.

1

-0.0000

-0.0000

0.0001

0.0000

0.0000

2

-0.0165

-1.7911

0.0000

3.2083

0.3408

3

0.0210

0.0188

0.0000

0.0008

0.0001

Since the electric dipole moment evaluated based on our TrEsp charges is very close to the
exact transition electric dipole moment, it is clear that our TrEsp charges must be reasonable.
Skill 2: Imposing customized charge constraint during TrEsp fitting process
The restrained electrostatic potential (RESP) module of Multiwfn has been detailedly
introduced in Section 3.9.16. As you can see, this module is more general and more powerful than
the MK or CHELPG module, because you can arbitrarily impose customized constraints on the
resulting ESP fitting charges, for example, you can request some atoms must have exactly the same
charge, or request sum of a batch of charges must equal to a predefined value.
Here, I present an example to illustrate how to use the RESP module to calculate TrEsp based
on MK fitting grid with additional constraint that atomic charge of all hydrogens must be zero.
Firstly, write a plain text file (e.g. chgcons.txt) with below content:
7 0
8 0
9 0
10 0

This file will be used in the RESP module. The 7~10 are atom indices of the hydrogens, the 0 means
their charges will be constraint to zero during fitting.
Boot up Multiwfn and input
S0S2.wfn // The .wfn file we previously used
7 // Population analysis
18 // RESP module
11 // Choose form of ESP
3 // Transition electronic
6 // Set charge constraint in one-stage fitting
1 // Load charge constraint setting from external plain text file
chgcons.txt // The file containing charge constraint
2 // Start one-stage ESP fitting calculation with customized constraint. The default fitting grid
is MK (you can also change to CHELPG by option 3)
The result is
Center
1(C )

Charge
0.158613

1073

--- page break ---

4 Tutorials and Examples
...
7(H )

0.000000

8(H )

-0.000000

9(H )

0.000000

10(H )

-0.000000

11(N )

0.178112

...

Clearly, our charge constraints have been in effect, and all other atoms still have reasonable
TrEsp charges. You can learn more about the RESP module by reading corresponding example in
Section 4.7.7. It is worth to note that when you select “Transition electric” in option 3, the default
atom equivalence constraint is automatically removed and the restraint strength in one-stage fitting
is automatically set to zero, since these treatments are not useful in current case.

3. Evaluating exciton coupling energy based on TrEsp
General form of intermolecular Coulomb interaction energy can be expressed as

 A (r )  bB,b (r ')
ZI ZJ
+   a ,a
drdr'
|r −r' |
I A J B | R I − R J |

VaaA,',Bbb ' =  a ,a ' b,b ' 
−  a ,a '  
I A

Z I  bB,b (r )
Z  A (r )
d r −  b ,b '   J a , a
dr
| RI − r |
| RJ − r |
J B

This quantity may have different physical meaning. For example
𝐴,𝐵
𝑉00,00
: Coulomb interaction energy between A and B in their ground states
𝐴,𝐵
𝑉00,11 : Coulomb interaction energy between A in ground state and B in the first excited state
𝐴,𝐵
𝐴,𝐵
𝑉01,10
= 𝑉10,01
: Excitation energy transfer couplings between transition of the two molecules
𝐴,𝐵
Calculation of the integrals in 𝑉𝑎𝑎
′ ,𝑏𝑏 ′ is difficult, there is a method known as transition density

cube (TDC), which calculates the integrals by numerical integration based on evenly distributed
grids, its cost is extremely high for large system. Fortunately, it was shown that by using TrEsp
𝐴,𝐵
charges calculated for two molecules, their exciton coupling energy 𝑉𝑎𝑎
′ ,𝑏𝑏 ′ can be readily

evaluated using below formula at commonly satisfactory accuracy:

qaI ,a ' qbJ,b'
IA J B | R I − R J |

VaaA,',Bbb '  

In Multiwfn, you can easily calculate exciton coupling energy based on TrEsp charges of two
molecules. The steps are briefly outlined below:
(1) Optimize dimer structure
(2) Extract coordinate of each monomer and write it into Gaussian input file, properly change
keywords and perform electron excitation calculation to yield .wfn file containing natural orbitals
derived from transition density. Note that nosymm keyword must be used to avoid Gaussian
automatically translating and rotating the overall monomer coordinate.
(3) Generate TrEsp charges for each monomer using respective .wfn file generated at last step,
then export TrEsp charges as .chg file.
(4) Manually combine content of the two monomer .chg file as a single .chg file. The monomer

1074
