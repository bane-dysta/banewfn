# 4.A.7 Visually revealing electron correlation in different regions

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.7 Visually revealing electron correlation in different regions
- pdf pages: 1084-1086
- category: special-topics
- direct child sections: 2
- total descendant sections: 2

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.7, Visually, revealing, electron, correlation, different, regions, 4.A.7.1, Fractional, Occupation, Number, Weighted, Density, FOD, 4.A.7.2, Local

## Direct child sections

- **4.A.7.1 Fractional Occupation Number Weighted Electron Density (FOD)** (pdf pages 1084-1085)
- **4.A.7.2 Local electron correlation functions** (pdf pages 1086-1086)

## Extracted manual text

4 Tutorials and Examples

4.A.7 Visually revealing electron correlation in different regions
Electron correlation effect is universal in chemical systems, and can be divided into dynamic
and static (nondynamic) parts. In this section, two methods aiming at visually revealing electron
correlation effect in different local regions are introduced. The fractional occupation number
weighted electron density (FOD) described in Section 4.A.7.1 is more popular and focuses on reveal
static correlation, while the local electron correlation functions described in Section 4.A.7.2 are able
to separately reveal dynamic and nondynamic components of the electron correlation. So, both the
two methods are useful.
Both the two methods require wavefunction file containing orbitals with partial occupancy as
input. In other words, if all orbitals are either fully occupied or unoccupied, then these functions
will be vanished everywhere and electron correlation cannot be revealed. FOD was defined based
on DFT, and the partially occupied orbitals should be obtained via finite-temperature DFT
calculation (supported by ORCA, CP2K and so on). In contrast, the local electron correlation
functions are defined based on wavefunction theory, and the partially occupied orbitals should be
the natural orbitals obtained via multiconfigurational methods such as CCSD and CASSCF. Because
computational cost of finite-temperature DFT is essentially identical to common DFT and
significantly lower than any multiconfigurational method, FOD is the preferential choice if you are
only interested in static electron correlation.
Both FOD and local electron correlation functions can be integrated over the whole space to
obtain the value for quantifying strength of corresponding type of electron correlation of the whole
system.

4.A.7.1 Fractional Occupation Number Weighted Electron Density (FOD)
Theory
FOD was defined by Grimme first in Angew. Chem. Int. Ed., 54, 1 (2015) and further discussed
in Chem. Eur. J., 23, 6150 (2017). FOD is expressed as

 FOD (r) =  (1 −  2i ) i (r)

2

i

where i loops over all spin molecular orbitals, whose occupancy range is [0,1]. The orbitals are
obtained by DFT calculation with a specific electronic temperature. With a proper temperature
setting, the resulting frontier orbitals will be evidently partially occupied. For orbitals lower than
Fermi level, 1=2=1, while for other orbitals, 1=0 and 2=-1. So, FOD is equivalent to a measure
of the degree to which the electron density at each position deviates from 0 K (integer occupancy)
at a finite temperature. The larger the FOD at a position, the stronger the static correlation in the
corresponding region. Integral of FOD over the whole space is referred to as NFOD, which quantifies
overall static correlation of the whole system like the well-known T1 diagnostic value. Because T1
diagnostic is dependent of the expensive CCSD calculation, NFOD is highly recommended for
measuring static correlation of medium to large systems.
Note that FOD corresponds to the 90th user-defined function.
Examples
As an example, we perform FOD analysis for HNO2. ORCA 6.0.1 program is used to carry out
finite-temperature B3LYP calculation for this molecule at 9000 K. Note that the suitable electronic

1063

--- page break ---

4 Tutorials and Examples
temperature for FOD analysis is T=20000*ax+5000, where ax is the Hartree-Fock composition in
the employed DFT functional. Because ax of B3LYP is 0.2, the temperature used in this example is
9000 K. The ORCA input file is examples\HNO2_FOD.inp. After executing it by ORCA, you will
have HNO2_FOD.gbw, then use orca_2mkl HNO2_FOD -molden command to convert it to
HNO2_FOD.molden.input, which has been provided in “examples” folder.
First, we plot FOD isosurface map. Set “iuserfunc” in settings.ini to 90, then boot up Multiwfn
and load examples\HNO2_FOD.molden.input, then input
5 // Calculate grid data
100 // User-defined function, which corresponds to FOD now
2 // Medium quality grid
Now from screen you can see the following information, showing that integral of FOD over
the whole space using uniform grids is 0.139, which is just the NFOD index. NFOD of HNO2 is not
large (see Chem. Eur. J., 23, 6150 (2017) for rich examples), indicating that HNO2 does not show
noticeable static correlation.
Summing up all value and multiply differential element:
0.139077305675630

Then choose option -1 to visualize isosurface map and set isovalue to 0.005 a.u., you will see
the following map. It shows that the static correlation mainly comes from the region above and
below the nitrogen, and the circular region surrounding O2.

Due to the very flexible design of Multiwfn, contributions to NFOD from basis functions, shells,
angular moments, atoms, fragments can also be derived. Next, I illustrate how to use Mulliken
population analysis to realize this purpose (many other population methods such as Löwdin and
Hirshfeld are also feasible). In the main menu of Multiwfn, we input
6 // Modifying wavefunction
26 // Set occupation of some orbitals
0 // Select all orbitals
fod // Making occupancy state of the selected orbitals correspond to FOD analysis
Now from screen you can see NFOD is 0.139077, which corresponds to the sum of current orbital
occupation numbers (in addition, it is noteworthy that, with the current occupancy, if you calculate
electron density via usual steps, you will obtain FOD).
Next, we input
q // Return
-1 // Return to main menu
7 // Population analysis
5 // Mulliken analysis
1 // Output Mulliken population and atomic charges on screen
Now you can see following information, the population just corresponds to atomic contribution
to NFOD. “Net charge” is meaningless in this situation. It is seen that static correlation in this system

1064

--- page break ---

4 Tutorials and Examples
mostly comes from N1 and O2, then O3, which is fully in line with the isosurface map
Atom

1(N )

Population:

0.05877131

Net charge:

6.94122869

Atom

2(O )

Population:

0.05320397

Net charge:

7.94679603

Atom

3(O )

Population:

0.02487142

Net charge:

7.97512858

Atom

4(H )

Population:

0.00223030

Net charge:

0.99776970

Since the Mulliken population analysis decomposes electron population in a very detailed
manner, you can also find more information about nature of NFOD. For example, the following
information on screen shows that static correlation is almost exclusively generated by p electrons.
Population of each type of angular moment orbitals:
Atom

1(N ) s: 0.0104 p: 0.0477 d: 0.0007 f: 0.0000 g: 0.0000 h: 0.0000

Atom

2(O ) s:-0.0003 p: 0.0535 d: 0.0001 f: 0.0000 g: 0.0000 h: 0.0000

Atom

3(O ) s:-0.0013 p: 0.0261 d: 0.0001 f: 0.0000 g: 0.0000 h: 0.0000

Atom

4(H ) s: 0.0020 p: 0.0002 d: 0.0000 f: 0.0000 g: 0.0000 h: 0.0000

Sum

s:

0.0107 p:

0.1275 d:

0.0008 f:

0.0000 g:

0.0000 h:

0.0000

4.A.7.2 Local electron correlation functions
Theory
The local total, dynamic and nondynamic electron correlation functions proposed in J. Chem.
Theory Comput., 13, 2705 (2017) are real space functions respectively aiming at revealing total,
dynamic and nondynamic electron correlation in various regions. They correspond to user-defined
functions 87, 88 and 89, respectively, and defined as follows:
• Local total electron correlation function: I T (r ) = 14

  (1 −  ) |  (r) | , i denotes index of
2

i

i

i

i

natural spin orbital,  is corresponding occupation number. Note that in some cases,  may be
marginally larger than 1.0 or negative, Multiwfn automatically sets it to 1.0 and 0.0 respectively to
make the calculation feasible.
• Local dynamic electron correlation function: I D (r) = 14

   (1 −  ) − 2 (1 −  ) |  (r) |

2

i

i

i

i

i

i

• Local nondynamic electron correlation function: I ND (r ) = 12

 (1 −  ) |  (r) |

2

i

i

i

i

It is clear that IT(r) = ID(r) + IND(r). It is noteworthy that the form of these functions is closely
related to the OED introduced in Section 4.A.6.
Integral of the local total, dynamic and nondynamic electron correlation functions respectively
correspond to the total, dynamic and nondynamic correlation indices proposed in Phys. Chem. Chem.
Phys., 18, 24015 (2016).
Examples
As an example, let us plot IT for OC-BH3. Set "iuserfunc" in settings.ini to 87, then boot up
Multiwfn and input
examples\COBH3_CCSD.wfn //Wavefunction file containing CCSD/def2-SVP natural orbitals
5 // Grid data calculation
100 // User-defined function, currently corresponding to IT
2 // Medium quality grid
-1 // Visualize isosurface

1065
