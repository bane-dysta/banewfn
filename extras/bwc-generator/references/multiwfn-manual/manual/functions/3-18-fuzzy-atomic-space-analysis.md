# 3.18 Fuzzy atomic space analysis (15)

## Quick navigation

- path: 3  Functions > 3.18 Fuzzy atomic space analysis (15)
- pdf pages: 219-235
- category: functions
- main menu / option numbers mentioned in title: 15
- direct child sections: 13
- total descendant sections: 15

## When to consult this file

Use this file for the actual Multiwfn function description: menu number, theory, options, inputs, and detailed behavior.

## Keywords and aliases

3.18, Fuzzy, atomic, space, 3.18.0, concepts, 3.18.1, Integration, real, spaces, 3.18.2, overlap, 3.18.3, molecular, multipole, moments, r2, 3.18.4, matrix, fragment, 3.18.5, Localization, index, LI, delocalization, DI, 3.18.6, Para-delocalization, PDI, 3.18.7

## Direct child sections

- **3.18.0 Basic concepts** (pdf pages 219-221)
- **3.18.1 Integration of a real space function in fuzzy atomic spaces (1)** (pdf pages 222-222; menu/options: 1)
- **3.18.2 Integration of a real space function in overlap spaces (8)** (pdf pages 223-223; menu/options: 8)
- **3.18.3 Atomic and molecular multipole moments and <r2> (2)** (pdf pages 223-225; menu/options: 2)
- **3.18.4 Atomic overlap matrix and fragment overlap matrix (3, 33)** (pdf pages 226-226; menu/options: 3, 33)
- **3.18.5 Localization index (LI) and delocalization index (DI) (4, 44)** (pdf pages 227-229; menu/options: 4, 44)
- **3.18.6 Para-delocalization index (PDI) (5)** (pdf pages 230-230; menu/options: 5)
- **3.18.7 Aromatic fluctuation index (FLU) and FLU-π (6,7)** (pdf pages 231-231; menu/options: 6,7)
- **3.18.8 Condensed linear response kernel (CLRK) (9)** (pdf pages 232-232; menu/options: 9)
- **3.18.9 Para linear response index (PLR) (10)** (pdf pages 233-233; menu/options: 10)
- **3.18.10 Multi-center delocalization index (11)** (pdf pages 233-233; menu/options: 11)
- **3.18.11 Information-theoretic aromaticity index (12)** (pdf pages 234-234; menu/options: 12)
- **3.18.12 Atomic effective volume, free volume, polarizability and C6 coefficient (13)** (pdf pages 234-235; menu/options: 13)

## Extracted manual text

3 Functions
used as input file. Then after a simple transformation, orbital energies are immediately outputted.
AdNDP analysis is relatively complicated and not a black box, please follow the examples in
Section 4.14 before using this module to analyze your systems.
Information needed: NBO output file (with AONAO DMNAO keywords), .fch file (only
needed when visualizing and exporting cube file for AdNDP orbitals, or exporting AdNDP orbitals
as .mwfn file), plain text file (containing Fock matrix. Only needed if you want to gain orbital
energies)

3.18 Fuzzy atomic space analysis (15)
3.18.0 Basic concepts
Before introducing each individual function, here I first introduce some basic concepts of fuzzy
atomic space.
Atomic space is the local space attributed to specific atom in the whole three-dimension
molecular space. Below we will express atomic space as weighing function w. The methods used to
partition the whole space into atomic spaces can be classified to two categories:
1 Discrete partition methods: The two representative methods are Bader's partition (also
known as AIM partition) and Voronoi partition. They partition molecular space discretely, so any
point can be attributed to only one atom, in other words,

 wA (r) = 1

wA (r) = 0

if r  A
if r   A

where ΩA is atomic space of atom A.
2 Fuzzy partition methods: The representative methods include Hirshfeld, Becke, HirshfeldI, MBIS and ISA. They partition molecular space contiguously, atomic spaces overlap with each
other, any point may be simultaneously attributed to many atoms to different extent, and the weights
are normalized to unity. In other words, below two conditions hold for all atoms and any point

0  w A (r )  1

A

 w (r ) = 1
B

B

The most significant advantage of fuzzy partition may be that the integration of real space
function in fuzzy atomic space is much easier than in discrete atomic space. By using Becke's
numerical DFT integration scheme (J. Chem. Phys., 88, 2547 (1988)), high accuracy of integration
in fuzzy atomic space can be achieved for most real space functions at the expense of relatively low
computation effort. In the fuzzy atomic space analysis module of Multiwfn, all integrations are
realized by this scheme. The more integration points are used, the higher integration accuracy can
be reached, one can adjust the number of points by "radpot" and "sphpot" parameter in settings.ini.
In fuzzy atomic space analysis module of Multiwfn, one can obtain many properties that based

198

--- page break ---

3 Functions
on fuzzy atomic spaces. Currently, the most widely used definitions of fuzzy atomic spaces, namely
Hirshfeld, Hirshfeld-I and Becke are supported, they are introduced below. One can choose which
fuzzy atomic spaces will be used by option -1.
Hirshfeld atomic space: In Theor. Chim. Acta (Berl.), 44, 129 (1977), Hirshfeld defined the
atomic space as

w AHirsh (r ) =

 Afree (r − R A )
  Bfree (r − R A )
B

free

where R is coordinate of nucleus, ρ denotes spherically averaged atomic electron density in freestate.
In option -1, you will found two options "Hirshfeld" and "Hirshfeld*". The former uses
atomic .wfn files to calculate the weights, they must be provided yourself or let Multiwfn
automatically invoke Gaussian to generate them, see Section 3.7.3 for detail. The latter evaluates
the weights directly based on built-in radial atomic densities and thus is more convenient, detail can
be found in Appendix 3. I strongly suggest using "Hirshfeld*" instead of "Hirshfeld".
Hirshfeld-I (HI) atomic space: This is a well-known extension of Hirshfeld method, it was
proposed in J. Chem. Phys., 126, 144111 (2007). Commonly the atomic space defined by HI is more
physically meaningful than that of Hirshfeld, since it can respond actual molecular environment.
Unfortunately, HI is much more expensive than Hirshfeld due to its iterative nature. Details of
Hirshfeld-I and its implementation in Multiwfn have been introduced in Section 3.9.13 and thus will
not be repeated here. When you choose HI in option -1, Multiwfn will first perform regular HI
iterations (If you are confused by the operations, please consult the example of computing HI
charges in Section 4.7.4). After HI atomic spaces have converged, you can do subsequent analyses.
MBIS atomic space: Like HI, MBIS refine atomic spaces iteratively. See Section 3.9.18 for
details. When you choose MBIS in option -1, you will enter the interface of performing MBIS
iteration, you should choose option 1 to start the atomic space refinement process. After MBIS
atomic spaces have converged, you can do subsequent analyses.
Becke atomic space: First, consider a function p

p( d ) = (3 / 2)d − (1 / 2)d 3
which can be iterated many times

f1 ( d ) = p( d )
f 2 (d ) = p[ p( d )]
f 3 ( d ) = p{ p[ p( d )]}
...
Then define a function s

sk (t ) = (1 / 2)[1 − f k (t )]
The plot of sk versus to t is

199

--- page break ---

3 Functions

1.0

k=1
k=2
k=3
k=4
k=5

0.9
0.8
0.7

sk(t)

0.6
0.5
0.4
0.3
0.2
0.1
0.0
-1.0

-0.8

-0.6

-0.4

-0.2

0.0

0.2

0.4

0.6

0.8

1.0

t

From above graph it can be seen that sk gradually reduces from 1 to 0 with t varying from -1 to 1.
The larger the k is, the sharper the curve becomes. The weighting function of Becke atomic space is
based on simple transformation of sk, for details please consult original paper J. Chem. Phys., 88,
2547 (1988).

w ABecke (r ) =

PA (r )
 PB (r )
B

PA (r ) =  sk ( AB (r ))
B A

a AB =

u AB
2
u AB − 1

u AB =

 AB (r ) =  AB (r ) + a AB (1 −  AB (r ) 2 )
 AB − 1
 AB + 1

 AB =

a AB = −0.5 if a AB  −0.5

if a AB  0.5
a AB = 0.5
r −r
 AB (r ) = A B
R AB = R A − R B
R AB

R Acov
RBcov

rA = r − R A

rB = r − R B

where R stands for coordinates of nucleus. Rcov denotes covalent radius.
The number of iterations, namely k value, can be set by option -3. The default value (3) is
appropriate for most cases. The definition of the covalent radius used to generate Becke atomic
space can be chosen by option -2. Through corresponding suboptions, one can directly select a set
of built-in radii (CSD radii, modified CSD radii, Pyykkö radii, Suresh radii, Hugo radii), load radii
information from external plain text file (the format required is described in the program prompts),
or modify current radii by manual input.
The origin paper of CSD radii is Dalton Trans., 2008, 2832, these radii were deduced from
statistic of Cambridge Structural Database (CSD) for the elements with atomic numbers up to 96.
Pyykkö radii was defined in Chem. Eur. J., 15, 186 (2008), which covers the entire periodic table,
Groups 1–18, Z=1–118. Suresh radii was proposed in J. Phys. Chem. A, 105, 5940 (2001), which is
based on theoretically calculated geometries of H3C-EHn, the defined radii cover most of main group
and transition elements in periodic table. Hugo radii was proposed in Chem. Phys. Lett., 480, 127
(2009), which has clear physical meaning and is based on atomic ionization energy. Notice that

200

--- page break ---

3 Functions
Hugo radii for hydrogen is rather large (even larger than Kr by 0.01 Bohr).
I found it is inappropriate to directly use any covalent radii definition shown above to define
Becke's atomic space. The covalent radii of metal elements in IA and IIA groups are always large,
e.g. CSD radius of lithium is 1.28 Å. While covalent radii of elements in such as VIIA group are
always small, e.g. CSD radius of fluorine is only 0.58 Å. For main groups, the elements with small
(large) covalent radius generally have large (small) electronegativity. So, in molecule environment,
the atoms with small (large) covalent radius prefer to withdraw (donate) electrons to expand (shrink)
their effective size, this behavior makes actual radii of main group elements in each row equalized.
In order to faithfully reflect this behavior, I defined the so-called "modified CSD radii", namely the
CSD radii of all main group elements (except for the first row) are replaced by CSD radii of the IVA
group element in corresponding row, while transition elements still use their original CSD radii. The
modified CSD radii are the default radii definition for Becke's atomic space.
The Becke atomic space of carbon in acetamide constructed by default parameters is illustrated
below

3.18.1 Integration of a real space function in fuzzy atomic spaces (1)
This function is used to integrate real space function f in atomic spaces

I A =  wA (r) f (r)d r
A

For example, if f is chosen as electron density, then IA will be the electron population number
of atom A.
f may be also chosen as the real space functions involving coordinates of two electrons, such
as exchange-correlation density and source function. For this case, the coordinate of reference point
can be set by option -10 (this is equivalent to set "refxyz" in settings.ini). If you have carried out
topology analysis, you can also use option -11 to set a critical point as reference point, this is
especially convenient for studying source function (for which bond critical point is usually set as
reference point).

201

--- page break ---

3 Functions
The "% of sum" and "% of sum abs" in output are defined as ( I A /

 I ) 100% and
B

B

( I A /  I B )  100% , respectively.
B

By default, all atomic spaces will be integrated. If you only need integral value of certain atoms,
you can use option -5 to define the atom list.

3.18.2 Integration of a real space function in overlap spaces (8)
This function is used to integrate specified real space function f in overlap spaces between
atomic pairs

I AB =  wA (r)wB (r) f (r)d r
A

For example, if f is chosen as electron density, then IAB will be the number of electrons shared
by atom A and B. f may be also chosen as the real space functions involving coordinates of two
electrons.
Integrals of positive and negative parts of f are outputted separately. Meanwhile, sum of
diagonal elements ∑𝐴 𝐼𝐴𝐴 , sum of non-diagonal elements ∑𝐴 ∑𝐵≠𝐴 𝐼𝐴𝐵 and sum of all elements
∑𝐴 ∑𝐵 𝐼𝐴𝐵 for positive and negative parts are also outputted together.
Currently only the fuzzy atomic space defined by Becke can be employed in this function.

3.18.3 Atomic and molecular multipole moments and <r2> (2)
This function is used to evaluate atomic and molecular monopole, dipole, quadrupole moments
and octopole moments as well as <r2>. All units in the output are in a.u.
In below formulae, superscript A means an atom named A. x, y and z are the components of
electron coordinate r relative to nuclear coordinate R.

x = rx − RxA

y = ry − RyA

z = rz − RzA

and r2 = x2 + y2 + z2.
Atomic monopole moment due to electrons is just negative of electron population number

p A = −  w A (r )  ( r ) d r
Atomic charges are outputted together, namely qA = pA + ZA, where Z denotes nuclear charge.
Atomic dipole moment is useful to measure polarization of electron distribution around the
atom, which is defined as

  xA 
 x
 A
A
μ =   y  = −   y  w A (r )  (r ) d r
 
  zA 
 z 
 
Its magnitude, or say its norm, is

202

--- page break ---

3 Functions

μ A = (  xA )2 + (  yA )2 + ( zA )2
Multiwfn also outputs the contribution of present atom to total molecular dipole moment,
which is evaluated as qAR + A.
Traceless Cartesian form of atomic quadrupole moment tensor is defined as (see Section 1.8.7
of book The Quantum Theory of Atoms in Molecules-From Solid State to DNA and Drug Design).

  xxA

Θ A =   Ayx
  zxA


 xyA
 Ayy
 zyA

3x 2 − r 2
 xzA 
3xy
3xz 
1 

A 
2
2
 yz  = −   3 yx
3y − r
3 yz  wA (r )  (r ) d r
2
 3zx
 zzA 
3zy
3z 2 − r 2 


whose magnitude can be calculated as

Θ A = (2 / 3) [( xxA ) 2 + ( Ayy ) 2 + ( zzA ) 2 ]
Atomic quadrupole moments in Cartesian form can be used to exhibit deviation of electron
distribution from spherical symmetry around nuclei. Specifically, 𝑖𝑖𝐴 <0 (𝑖𝑖𝐴 >0) indicates that the
electron density of atom A is elongated (contracted) along i direction. If the atomic electron density
has exact spherical symmetry, then Θxx = Θyy = Θzz. Noticeably, the Cartesian quadrupole moment
tensor Θ given here is traceless, namely the condition Θxx + Θyy + Θzz = 0 holds.
Standard Cartesian form of atomic quadrupole moment tensor is defined as follows. It is not
outputted by default because it is rarely useful. However, if you hope it to be outputted, you can set
“ispecial” in settings.ini to 1.

  xxA

Θ A =   Ayx
  zxA

 xyA
 Ayy
 zyA

 x2
 xzA 


 Ayz  = −   yx
 zx
 zzA 

xy
y2
zy

xz 

yz  wA (r )  (r )d r
z 2 

Inspired by electronic spatial extent (see Section 3.300.5), I defined atomic electronic spatial
extent 〈𝑟𝐴2 〉, it is expressed as
〈𝑟𝐴2 〉 = ∫ 𝑟 2 𝑤𝐴 (𝐫)𝜌(𝐫)d𝐫 = 〈𝑥𝐴2 〉 + 〈𝑦𝐴2〉 + 〈𝑧𝐴2 〉
whose X component is expressed as follows
〈𝑥𝐴2 〉 = ∫ 𝑥 2 𝑤𝐴 (𝐫)𝜌(𝐫)d𝐫
and similarly for 〈𝑦𝐴2 〉 and 〈𝑧𝐴2 〉.
〈𝑟𝐴2 〉 is a useful metric of overall spatial extent of electron distribution within a fuzzy atom,
while its Cartesian component reveals electronic spatial extent in specific direction.
The atomic quadrupole and octopole moments in spherical harmonic form are also outputted.
The general expression of multipole moments in spherical harmonic form is

QlA,m = −  RlA,m (r ) wA (r )  (r) d r
All of the five components of quadrupole moment in spherical harmonic form correspond to

203

--- page break ---

3 Functions

R2,0 = (3z 2 − r 2 ) / 2
R2,−1 = 3 yz

R2,1 = 3xz

R2,−2 = 3xy

R2,2 = ( 3 / 2)( x 2 − y 2 )

All of the 7 components of octopole moment in spherical harmonic form correspond to

R3,0 = (1 / 2)(5 z 2 − 3r 2 ) z
R3,−1 = 3 / 8(5 z 2 − r 2 ) y

R3,1 = 3 / 8(5 z 2 − r 2 ) x

R3,−2 = 15 xyz

R3,2 = ( 15 / 2)( x 2 − y 2 ) z

R3,−3 = 5 / 8(3x 2 − y 2 ) y

R3,3 = 5 / 8( x 2 − 3 y 2 ) x

The magnitude of multipole moments in spherical harmonic form is calculated as

QlA =

 (Q )

A 2
l ,m

m

At the end of the calculation, the total number of electrons, molecular dipole moment and its
magnitude are outputted. Molecular dipole moment is calculated as the sum of all contributions
from atomic dipole moments and atomic charges (i.e. the sum of all "Contribution to molecular
dipole moment" terms in the output information)

μ mol =  ( qA R A + μ A )
A

In addition, Multiwfn outputs molecular quadrupole and octopole moments in Cartesian form and
spherical harmonic form, they can also be viewed as sum of contributions of atoms. For example,
molecular quadrupole moment of Θxy is expressed as

 xy =

3

RxA RyA Z A −   xywA (r )  (r )d r 


2 A
A


where x, y, z in this context are Cartesian components of r with respect to (0,0,0) position. <r2> of
molecule can be written as
〈𝑟 2 〉 = ∑ ∫ 𝑟 2 𝑤𝐴 (𝐫)𝜌(𝐫)d𝐫
𝐴

where r is radial distance with respect to (0,0,0).
By default, atomic multipole moments and <r2> for all atoms are evaluated, and finally, these
quantities of the whole system are printed. If you only need them for specific atoms, you can use
option -5 to define an atom list, in this case only the quantities of selected atoms will be calculated
and outputted. In addition, via this feature you can calculate the quantities of a molecule in a
molecular complex, or calculate them of a fragment in a molecule, because in this case the
"Molecular dipole and multipole moments" printed at the end of output are only contributed by the
atoms in the defined list. See example in Section 4.15.3 for illustration of use of this feature.
After entering the present function, you will be asked to choose destination of outputting. If
you choose 2 to output result to multipole.txt, a file named atom_moment.txt will also be produced
in the current folder. Based on this file, atomic electric dipole and quadrupole moments can be

204

--- page break ---

3 Functions
visualized in VMD program via a special script, see Section 4.15.5 for detail.
PS 1: If your purpose is only calculating electric dipole/multipole moments and <r2> for the whole system, it is
best to use the function described in Section 3.300.5, it is significantly faster and more accurate since it calculates
them analytically.
PS 2: If “ispecial” in settings.ini is set to 1, then the electron density involved in this function will be replaced
with user-defined function. Via this feature, it is possible to realize some special purpose, such as calculating atomic
dipole moments corresponding to variation of electron density, see #10 and relevant discussions in
http://sobereva.com/wfnbbs/viewtopic.php?id=650.

3.18.4 Atomic overlap matrix and fragment overlap matrix (3, 33)
Subfunction 3 in fuzzy analysis module is used to calculate atomic overlap matrix (AOM) for
orbitals in atomic spaces, the AOM will be outputted to AOM.txt in current folder. The element of
AOM is defined as

Sij ( A) =  i (r) j (r)d r
A

where i and j are orbital indices, integration is performed within fuzzy space of atom A. For
unrestricted wavefunctions, AOMs between  orbitals and between β orbitals are outputted
separately for each atom.
Notice that the highest virtual orbitals will not be taken into account during calculation. For
example, present system has 10 orbitals in total, in which 7, 8, 9, 10 are not occupied, and user has
set the occupation number of orbital 3 to zero via option 26 in main function 6, then the dimension
of each AOM outputted by Multiwfn will be (6,6), corresponding to the overlap integral between
the first 6 orbitals in each atomic space. If you hope to take all orbitals into account, set “ispecial”
in settings.ini to 3.
Since orbitals are orthonormal in the whole space, in principle, summing up AOMs for all
atoms (corresponding to integrating in the whole space) should yield an identity matrix

SUM =  S( A) = I
A

Of course, this condition is not strictly held, because the integration is performed numerically. The
deviation of SUM to identity matrix is a useful metric of integration accuracy

 SUM − I
i, j

Error =

i

i, j

j

N atom

Multiwfn automatically outputs the "Error" value. If it is not small enough, e.g. >0.001, then you
may want to improve the integration accuracy via following ways
(1) Enlarge "radpot" and "sphpot" in settings.ini
(2) Set "radcut" in settings.ini to 0
(3) Choose option -6 to change the default atomic integration grid to the much more expensive
molecular integration grid
(4) If diffuse functions were heavily employed, remove them
Fragment overlap matrix
Fragment overlap matrix (FOM) is simply sum of AOM of the atoms in the fragment. FOM of
one fragment or two fragments can be calculated by subfunction 33 in fuzzy analysis module, the
result is outputted to FOM.txt in current folder. You can directly define the atoms in the fragment(s)

205

--- page break ---

3 Functions
in this subfunction.
When atomic integration grid is used to evaluate AOM, and the number of atoms involved in
the one or two fragments is significantly smaller than total number of atoms, the calculation cost of
FOM is significantly lower than using subfunction 3 to calculate the entire AOM, because atoms
not involved in the fragment(s) will simply be skipped.

3.18.5 Localization index (LI) and delocalization index (DI) (4, 44)
3.18.5.1 Theoretical background
Definition of LI and DI
For open-shell systems, the LI () and DI () are calculated for each spin of electrons
respectively. Below only the expression of LI and DI for  electrons is given. For β electrons, just
replacing  with β, similarly hereinafter. The electrons in atomic space A that can delocalize to
atomic space B is computed as

  ( A → B) = −   XC,tot (r1 , r2 ) d r1 d r2
A B

where ГXC is exchange-correlation density; if you are not familiar with it, please consult the
discussion in part 17 of Section 2.6. The electrons in atomic space B that can delocalize to atomic
space A is

  ( B → A) = −   XC,tot (r1 , r2 ) d r1 d r2
B

A

Clearly, above two terms are identical in value, therefore we define DI between A and B as below,
it measures the total number of  electrons shared by atom A and B

  ( A, B) =   ( A → B) +   ( B → A) = −2   XC,tot (r1, r2 ) d r1 d r2
A B

The LI measures the number of  electrons localized in an atom. Note that this quantity is not
additive.

  ( A) = −   XC,tot (r1 , r2 ) d r1 d r2 =   ( A, A) / 2
A A

The relationship between LI, DI and the population number of electrons in atomic space is
given below, the physical meaning is that the sum of  electrons of atom A that localized in atom A
and that delocalized to other regions is the total number of  electrons in space A.

  ( A) + (1 / 2)    ( A, B)    ( A) +    ( A → B) =
B A

= −  
A

 ,tot
XC

B A

(r1 , r2 ) d r1 d r2 =   (r ) d r = N A


A

Using the approximate expression of ГXC, the DI and LI can be explicitly written as

  ( A, B ) = 2 i j Sij ( A) Sij ( B )
i j

 ( A) =  i j Sij ( A) Sij ( A)


i j

where S is atomic overlap matrix (AOM), see Section 3.18.4 for introduction.
Total DI and LI are the summation of  part and β part

206

--- page break ---

3 Functions

 ( A, B ) =   ( A, B ) +   ( A, B )
 ( A) =  ( A) +  ( A)
Special form of closed-shell cases




Since in closed-shell case  ( A, B ) =  ( A, B ) , one can evaluate total DI as

 ( A, B ) = 2  ( A, B ) = 2  2
m

n

m n
Smn ( A) Smn ( B ) = 2 mn Smn ( A) Smn ( B )
2 2
m n

where m and n denote closed-shell natural orbitals. Similarly, the total LI for closed-shell cases is

 ( A) =  mn Smn ( A)Smn ( A)
m

n

For closed-shell systems, it is argued that the value of total DI is a quantitative measure of the
number of electron pairs shared between two atoms. For example, total (A,B)=1.0 implies a pair of
electron (an  and a  electrons) is shared between atom A and B. (In fact, this is strictly true only
for nonpolar bonds such as H-H bond in H2. In polar bonds, the DI must be lower than formal bond,
because what total DI actually reflects is the effective number of electron pairs shared by two atoms
and thus somewhat reflects covalency. Note that the value of DI is very sensitive the definition of
atomic space employed.
Fragment LI and interfragment DI
Interfragment DI (IFDI) between fragments F and G can be evaluated as

  ( F , G ) = 2 i j Sij ( F ) Sij (G )
i j

→ 2 i j  Sij ( A)  Sij ( B)
i j

AF

BG



→    2 i j Sij ( A) Sij ( B) 
AF BG  i j


→    ( A, B)
AF BG

In Phys. Chem. Chem. Phys., 24, 11486 (2022), it was demonstrated that IFDI between two
terminal fragments in a globally conjugated system is very useful in characterizing extent of global
delocalization, and it IFDI is found to be well positively correlated with rotational barrier between
the two fragments. This is because the stronger the original  conjugation is, the more obvious the
destruction of conjugation will be when the two groups rotate relative to each other, and the higher
the energy will rise.
Fragment LI (FLI) of fragment F can be evaluated as

207

--- page break ---

3 Functions

  ( F ) =  i j Sij ( F ) Sij ( F )
i j

→    i j Sij ( A) Sij ( B)
AF BF i j

→   i j Sij ( A) Sij ( A) +
AF i j

→    ( A) +
AF

  S ( A) S ( B)
 2
 

( B  A )F

i

j

i

j

ij

ij

   ( A, B)

( B  A )F

Special form for single-determinant wavefunctions
For single-determinant wavefunction, because of integer occupation number of orbitals, DI and
LI can be simplified as
occ occ

  ( A, B ) = 2  Sij ( A) Sij ( B )
i j

occ occ

 ( A, B ) = 4  Smn ( A) Smn ( B )
m

n

occ occ

  ( A) =  Sij ( A) Sij ( A)
i j
occ occ

 ( A) = 2 Smn ( A) Smn ( A)
m

n

Relationship between DI and fuzzy bond order
Conventionally, LI and DI are calculated in AIM atomic spaces (also called as AIM basins).
While in fuzzy atomic space analysis module of Multiwfn, they are calculated in fuzzy atomic space,
the physical nature is the same. According to the discussion presented in J. Phys. Chem. A, 109,
9904 (2005) (compare Eq. 13 and Eq. 18), the DI calculated in fuzzy atomic space is just the socalled fuzzy bond order, which was defined by Mayer in Chem. Phys. Lett., 383, 368 (2004).
For closed-shell system, atomic valence can be calculated as the sum of its fuzzy bond orders

V ( A) =   ( A, B)
B A

Separation of  and  contributions
For strictly planar molecules, because overlap integral of σ orbital and π orbital is exactly zero
in atomic space, the contributions from σ and π electrons to DI can be exactly decomposed as DI-σ
and DI-π

  ( A, B ) =   ( A, B ) +   ( A, B )




  ( A, B ) = 2 i j Sij ( A) Sij ( B )
i j





  ( A, B ) = 2 i j Sij ( A) Sij ( B )
i j

Similarly, LI can be decomposed as LI-σ and LI-π. Summing up corresponding off-diagonal
elements in DI-σ and DI-π matrix gives σ-atomic valence and π-atomic valence, respectively. If you
want to compute DI/LI-σ (DI/LI-π), before the DI/LI calculation, you should set the occupation

208

--- page break ---

3 Functions
numbers of all π orbitals (σ orbitals) to zero by subfunction 26 of main function 6.
Covariance and relative fluctuation parameter
In some literatures, especially the ones written by Bernard Silvi, the variance of electronic
fluctuation in atomic space σ2(A) and the covariance of fluctuation of electron pair between two
atomic spaces cov(A,B) are discussed. They are not directly outputted by Multiwfn, because there
is a very simple relationship correlates σ2(A), cov(A,B) and DI(A,B), thus you can calculate them
quite easily, see Chem. Rev., 105, 3911 (2005) for derivation

cov( A, B) = − ( A, B) / 2

 2 ( A) = N A −  ( A) = −  cov( A, B) =   ( A, B) / 2
B A

B A

where NA is the electron population number in A. As mentioned above, the diagonal terms of the DI
matrix outputted by Multiwfn are calculated as the sum of off-diagonal elements in the
corresponding row (or column), hence you can simply obtain σ2 by dividing corresponding diagonal
term of DI matrix by two.
A quantity closely related to σ2 is the relative fluctuation parameter introduced by Bader, which
indicates the electronic fluctuations for a given atomic space relative to its electron population, you
can calculate it manually if you want

F ( A) =  2 ( A) / N A
Alternatively, you can calculate below value to measure the proportion of the electrons localized in
the atomic space

l ( A) =  ( A) / N A
3.18.5.2 Usage
In Multiwfn, before calculating LI and DI, AOM is calculated first automatically, this is the
most time-consuming step. For open-shell systems, the LI and DI for  and β electrons, as well as
for all electrons are outputted respectively. Notice that the diagonal terms of DI matrix are calculated
as the sum of corresponding off-diagonal row (or column) elements. For closed-shell system, as
stated above, they correspond to atomic valence.

3.18.6 Para-delocalization index (PDI) (5)
Para-delocalization index (PDI) is a quantity used to measure aromaticity of six-membered
rings. PDI was first proposed in Chem. Eur. J., 9, 400 (2003), also see Chem. Rev., 105, 3911 (2005)
for more discussion. PDI is essentially the averaged para-delocalization index (para-DI) in sixmembered rings.

PDI =

 (1,4) +  (2,5) +  (3,6)
3

The basic idea behind PDI is that Bader and coworkers reported that DI in benzene is greater

209

--- page break ---

3 Functions
for para-related than for meta-related carbon atoms. Obviously, the larger the PDI, the larger the
delocalization, and the stronger the aromaticity. The main limitation of the definition of PDI is that
it can only be used to study aromaticity of six-membered rings, and it was shown that PDI is
inappropriate for the cases when the ring plane has an out-plane distortion.
In Multiwfn, before calculating PDI, AOM and DI are first calculated automatically. Then you
will be prompted to input the indices of the atoms in the ring that you are interested in, the input
order must be consistent with atom connectivity.
PDI currently is only available for closed-shell systems, although theoretically it may be
possible to be extended to open-shell cases.
Note that for completely planar systems, since DI can be decomposed to  and π parts, PDI
can also be separated as PDI- and PDI-π to individually study  aromaticity and π aromaticity. In
order to calculate PDI- (PDI-π), before enter present module, you should first manually set
occupation number of all MOs except for π () MOs to zero (or you can utilize option 22 in main
function 100 to do this step, which will be much more convenient).

3.18.7 Aromatic fluctuation index (FLU) and FLU-π (6,7)
Aromatic fluctuation index (FLU) was proposed in J. Chem. Phys., 122, 014109 (2005), also
see Chem. Rev., 105, 3911 (2005) for more discussion. Like PDI, FLU is an aromaticity index based
on DI, but can be used to study rings with any number of atoms. The FLU index was constructed by
following the HOMA philosophy (see Section 3.28.6), i.e. measuring divergences (DI differences
for each single pair bonded) from aromatic molecules chosen as a reference. FLU is defined as
below

1 ring  V ( B )    ( A, B ) −  ref ( A, B )  
FLU =  


n A− B  V ( A)  
 ref ( A, B )
 

2

where the summation runs over all adjacent pairs of atoms around the ring, n is equal to the number
of atoms in the ring, ref is the reference DI value, which is precalculated parameter.  is used to
ensure the ratio of atomic valences is greater than one

 1 V ( B )  V ( A)
− 1 V ( B )  V ( A)

=

The first factor in the formula of FLU penalizes those with highly localized electrons, while
the second factor measures the relative divergence with respect to a typical aromatic system.
Obviously, lower FLU corresponds to stronger aromaticity.
The dependence on reference value is one of main weakness of FLU. The default ref in
Multiwfn for C-C, C-N, B-N are 1.468, 1.566 and 1.260 respectively, they are obtained from
calculation of benzene, pyridine and borazine respectively under HF/6-31G* (geometry is optimized
at the same level. Becke's atomic space with modified CSD radii and with sharpness parameter k=3
is used to derive ref). Users can modify or add ref through option -4.
The original paper of FLU also defined FLU-π, which is based on DI-π and π-atomic valence

210

--- page break ---

3 Functions


1  V ( B )    ( A, B ) −  avg  
FLU =  
 
 
n A− B  V ( A)  
 avg
 


2

ring

where π is the average value of the DI-π for the bonded atomic pairs in the ring, and the other
symbols denote the aforementioned quantities calculated using π-orbitals only. The advantage of
FLU-π over FLU is that FLU-π does not rely on predefined reference DI value, while the
disadvantage is that FLU-π can only be exactly calculated for planar molecules.
Akin to FLU, the lower the FLU-π, the stronger aromatic the ring. If FLU-π is equal to zero,
that means DI-π is completely equalized in the ring. The reasonableness to measure aromaticity by
FLU-π is that aromaticity for most aromatic molecules are almost purely contributed by π electrons,
rather than σ electrons.
In fuzzy atomic space analysis module of Multiwfn, PDI, FLU and FLU-π are calculated in
fuzzy atomic spaces. In J. Phys. Chem. A, 110, 5108 (2006), the authors showed that the correlation
between the PDI, FLU and FLU-π calculated in fuzzy atomic space and the ones calculated in AIM
atomic space is excellent.
In Multiwfn, before calculating FLU and FLU-π, AOM will be calculated automatically. If you
are calculating FLU-π, you will be prompted to input the indices of π orbitals, you can find out their
indices by checking isosurface of all orbitals by main function 0. Then DI or DI-π matrix will be
generated. Next, you should input the indices of the atoms in the ring, the input order must be
consistent with atom connectivity. Besides FLU or FLU-π value, the contributions from each bonded
atomic pair are outputted too.
FLU and FLU-π are only available for closed-shell system in Multiwfn. It is not well known
whether FLU and FLU-π are also applicable for open-shell systems.

3.18.8 Condensed linear response kernel (CLRK) (9)
Linear response kernel (LRK) is an important concept defined in DFT framework, which can
be written as


  (r1 ) 
 2E
 = 

  (r1 ) (r2 )  N   (r2 )  N



 (r1 , r2 ) = 

This quantity reflects the impact of the perturbation of external potential at r2 on the electron density
at r1, which may also be regarded as the magnitude coupling between electron at r1 and r2.
In Multiwfn, LRK is evaluated by an approximation form based on second-order perturbation
theory (see Eq.3 of Phys. Chem. Chem. Phys., 14, 3960 (2012))

 i* (r1 ) j (r1 ) *j (r2 ) i (r2 )
i −  j
iocc jvir

 (r1 , r2 )  4  

where φ is molecular orbital, ε stands for MO energy. Note that this approximation form is only
applicable to HF/DFT closed-shell systems, therefore present function only works for HF/DFT
closed-shell systems.
Condensed linear response kernel (CLRK) is calculated as

211

--- page break ---

3 Functions

 A,B =    (r1 , r2 ) d r1 d r2 = 4  
A B

Sij ( A) S ji ( B )

iocc jvir

i −  j

where A and S(A) denote fuzzy atomic space and atomic overlap matrix for atom A, similar for atom
B. In Phys. Chem. Chem. Phys., 15, 2882 (2013), it was shown that CLRK is useful for investigation
of aromaticity and anti-aromaticity.
Present function is used to calculate CLRK between all atomic pairs in current system, and the
result will be outputted as a matrix. Due to evaluation of LRK requires virtual MO information, in
current version .mwfn/.fch/.molden/.gms file must be used as input file.
Note that CLRK can be decomposed to orbital contribution, e.g. for MO i

 A( i,)B = 4 

Sij ( A) S ji ( B)

i −  j

jvir

For instances, assume that you want to evaluate the contribution from MO 3,4,7, then before
calculating CLRK, you should enter main function 6 and use option 26 to set occupation number of
all MOs except for 3,4,7 to zero. (Note that the virtual MOs used to calculate LRK will automatically
still be the original virtual MOs, rather than the ones after modification of MO occupation numbers.)

3.18.9 Para linear response index (PLR) (10)
The definition of para linear response index (PLR) has an analogy to PDI, the only difference
is that DI is replaced by CLRK

PLR( A, B) =

1,4 +  2,5 + 3,6
3

In Phys. Chem. Chem. Phys., 14, 3960 (2012), the authors argued that PLR is as useful as PDI
in quantitatively measuring aromaticity, and it is found that the linear relationship between PLR and
PDI is as high as R2=0.96.
Present function is used to calculate PLR. Multiwfn will first calculate CLRK, and then you
should input the indices of the atoms constituting the ring in question, e.g. 3,5,6,7,9,2. The input
order must be consistent with atom connectivity. Then PLR will be immediately outputted on screen.
PLR is only applicable to HF/DFT closed-shell systems, and currently .mwfn/.fch/.molden/.gms
must be used as input file.
Note that for completely planar systems, PLR can be exactly separated as PLR- and PLR-π
to individually study  aromaticity and π aromaticity. In order to calculate PLR- (PLR-π), before
enter present module, you should first manually set occupation number of all MOs except for π ()
MOs to zero (or you can utilize option 22 in main function 100 to do this step, which will be much
more convenient).

3.18.10 Multi-center delocalization index (11)
n-center multi-center DI is calculated as

 ( A, B, C...H ) = 2n−1 
i

j

k

 S ( A)S ( B)S (C ) S ( H )
ij

q

212

jk

kl

qi

--- page break ---

3 Functions
where i, j, k... only cycle occupied orbitals. The normalized form of multi-center DI is defined as

1/n, and may be compared between rings with different number of members.
Currently this function is only available for single-determinant closed-shell wavefunctions, and
supports up to 10 centers. Note that for relatively large size of systems, calculating multi-center DI
for more than 6 centers may be quite time-consuming.

3.18.11 Information-theoretic aromaticity index (12)
In ACS Omega, 3, 18370 (2018) it is shown that arithmetic mean of some information-theoretic
quantities of the atoms constituting a ring has good linear relationship with other widely accepted
aromaticity indices, such as HOMA and aromatic stabilization energy (ASE). It is thus clear that the
arithmetic mean may be used as index for measuring aromaticity, although this point needs to be
further explored.
The information-theoretic aromaticity index, namely the above-mentioned arithmetic mean can
be calculated via subfunction 12 of fuzzy analysis module. After entering this function, you should
choose the way of defining atomic information-theoretic quantity, three choices are currently
available:



Atomic Shannon entropy: sS ( A) = −  (r ) ln  (r ) wA (r ) d r



Atomic Fisher information: iF ( A) = |  (r ) | /  (r ) wA (r ) d r
2



Atomic GBP entropy: sGBP ( A) = (3 / 2)  (r ){ + ln[t (r ) / tTF (r )]}wA (r ) d r
Essentially, the three quantities correspond to the integral of user-defined functions 50, 51 and 54 in
fuzzy atomic space. In this function, you also need to input the index of the atoms in the ring. Once
calculation of the selected quantity for all atoms in the ring is finished, the average will be shown,
and it can be regarded as an aromaticity index.
Before using this function, you can firstly select the way of defining atomic space. In the
original paper, Hirshfeld partition was employed, while the default partition method of the fuzzy
analysis module is Becke.
Information needed by fuzzy analysis module: GTFs, atom coordinates

3.18.12 Atomic effective volume, free volume, polarizability and C6
coefficient (13)
Theory
Atomic (effective) polarizability of zero frequency, 𝛼 eff (0), in a molecule system, is a quantity
of great important; however, there is no unique way to estimate it and it is not experimentally
observable in general. In the original paper of Tkatchenko-Scheffler (TS) dispersion correction
method, Phys. Rev. Lett., 102, 073005 (2009), the authors suggested a simple way of estimating it
by scaling known free-atom polarizability 𝛼 free (0), they assumed that atomic polarizability of an
element is positively proportional to its atomic volume. Then in a review article Chem. Rev., 117,

213

--- page break ---

3 Functions
4714 (2017), this method is explicitly expressed as

 Aeff (0) =  Afree (0)

VAeff
VAfree

where atomic effective volume Veff in a molecule and atomic free volume Vfree are expressed as

VAeff =   (r )wA (r ) | r − R A |3 d r
VAfree =   Afree (r ) | r − R A |3 d r
in which wA is atomic weighting function of atom A. RA is nuclear position of atom A.  is molecular
electron density, and 𝜌𝐴free is electron density of atom A in its free state. Note that the calculation
level used for evaluating  and free must be exactly the same.
Ideally, the sum of 𝛼 eff (0) of all atoms should be equal to static polarizability of the whole
system. However, since the above method is not rigorous, this condition is obviously impossible to
achieve. In addition, it is worth to note that the choice of atomic weighting function significantly
affects calculation result, but it is unclear which weighting function is the best choice for this
purpose.
In my opinion, it is useful to define percentage contribution of an atom to total polarizability
as

 Aeff (0)
 100%
 Aeff (0)

 A% =

A

This quantity is evidently useful in analyzing major source of molecular polarizability.
According to the TS method, C6 dispersion coefficient of an atom in practical chemical
environment can be easily and approximately evaluated as
2

TS
6, AA

C

 V eff 
=  Afree  C6,freeAA
 VA 

free
where 𝐶6,𝐴𝐴
is the known C6 dispersion coefficient of the atom in free state. Furthermore, C6
between different two atoms can be calculated as

C6,TSAB =

2C6,TSAAC6,TSBB
 Beff (0) TS  Aeff (0) TS
C +
C
 Aeff (0) 6, AA  Beff (0) 6, BB

Finally, intermolecular C6 coefficient can be obtained as follows

C6mol = 

C

Amol1 Bmol 2

TS
6, AB

Usage
TS
Multiwfn is able to calculate Veff, Vfree, 𝛼 eff (0), 𝛼 % , and 𝐶6,𝐴𝐴
for all atoms. In addition, the
mol
𝐶6 between two present systems, in other words, homomolecular 𝐶6mol, is also given. To evaluate
them, the steps are:
(1) Manually generate wavefunction file of each kind of atom in the current system by your
favourite quantum chemistry code.
(2) Boot up Multiwfn and load the wavefunction file of the system.

214
