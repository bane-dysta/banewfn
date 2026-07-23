# 3.200 Other functions, part 2 (200)

## Quick navigation

- path: 3  Functions > 3.200 Other functions, part 2 (200)
- pdf pages: 399-427
- category: functions
- main menu / option numbers mentioned in title: 200
- direct child sections: 21
- total descendant sections: 21

## When to consult this file

Use this file for the actual Multiwfn function description: menu number, theory, options, inputs, and detailed behavior.

## Keywords and aliases

3.200, Other, 3.200.1, core-valence, bifurcation, CVB, index, related, quantities, 3.200.2, atomic, bond, dipole, moments, Hilbert, space, 3.200.3, Generate, cube, multiple, orbital, wavefunctions, 3.200.4, moved, Section, 3.28.3, 3.200.5, Plot, radial, distribution

## Direct child sections

- **3.200.1 Calculate core-valence bifurcation (CVB) index and related quantities** (pdf pages 399-403)
- **3.200.2 Calculate atomic and bond dipole moments in Hilbert space** (pdf pages 404-405)
- **3.200.3 Generate cube file for multiple orbital wavefunctions** (pdf pages 406-406)
- **3.200.4 (moved to Section 3.28.3)** (pdf pages 406-406; menu/options: moved to Section 3.28.3)
- **3.200.5 Plot radial distribution function for a real space function** (pdf pages 406-406)
- **3.200.6 Analyze correspondence between orbitals in two wavefunctions** (pdf pages 407-408)
- **3.200.7 (moved to Section 3.27.1)** (pdf pages 409-409; menu/options: moved to Section 3.27.1)
- **3.200.8 (moved to Section 3.27.2)** (pdf pages 409-409; menu/options: moved to Section 3.27.2)
- **3.200.9 Calculate average bond length and average coordinate number** (pdf pages 409-409)
- **3.200.10 Output various kinds of integral between orbitals** (pdf pages 410-410)
- **3.200.11 Calculate center, first/second moments, radius of gyration, and <r^2> of a function** (pdf pages 410-411)
- **3.200.12 Calculate energy index (EI) or bond polarity index (BPI)** (pdf pages 412-412)
- **3.200.13 Evaluate orbital contributions to density difference or other grid data** (pdf pages 413-414)
- **3.200.14 Domain analysis (obtaining properties within isosurfaces of a function)** (pdf pages 415-416)
- **3.200.15 Calculate electron correlation index** (pdf pages 417-417)
- **3.200.16 Generate natural orbitals, natural spin orbitals (NSO) and spin natural orbitals (SNO) based on the density matrix in .fch/.fchk file** (pdf pages 418-418)
- **3.200.17 Calculate Coulomb and exchange integral between two orbitals** (pdf pages 419-420)
- **3.200.18 Calculate bond length/order alternation (BLA/BOA) and angle/dihedral alternation** (pdf pages 421-421)
- **3.200.19 Calculate spatial delocalization index (SDI) for orbitals or a function** (pdf pages 422-422)
- **3.200.20 Bond order density (BOD) and natural adaptive orbital (NAdO) analyses** (pdf pages 423-426)
- **3.200.21 Perform Löwdin orthogonalization between occupied orbitals** (pdf pages 427-427)

## Extracted manual text

3 Functions

An example of using this function to evaluate ESP fitting charge based on user-provided ESP
cube file is given in #4 of http://sobereva.com/wfnbbs/viewtopic.php?pid=1542. This example
illustrates the universality of this module.
Information needed: Atom coordinates

3.200 Other functions, part 2 (200)
3.200.1 Calculate core-valence bifurcation (CVB) index and related
quantities
Note: Chinese version of this section is my blog article “Using Multiwfn to calculate CVB index and measure
strength of hydrogen bonds” (http://sobereva.com/461).

(1) Theory of CVB index
The idea of the so-called core-valence bifurcation (CVB) index was firstly proposed in Theor.
Chem. Acc., 104, 13 (2000), this index was defined based on electron localization function (ELF)
and mainly used to distinguish strength of various kinds of hydrogen bonds (H-bonds). For a Hbond of typical form (D-HA, where D=donor, H=hydrogen, A=acceptor), this index is expressed
as:
CVB index = ELF(C-V) – ELF(DH-A)
where ELF(C-V) corresponds to the ELF bifurcation value between ELF core domain and valence
domain, while the ELF(DH-A) stands for the ELF value at bifurcation point between V(D,H) and
V(A).
In the above-mentioned Theor. Chem. Acc. paper, the authors examined many H-bond dimers
composing of HF and various kinds of monomers, it was found that the CVB index has good linear
relationship with H-bond binding energy. In some succeeding papers, such as Struct. Chem., 16, 203
(2005) and J. Phys. Chem. A, 115, 10078 (2011), this point has been further confirmed, and in the
former it was pointed out at CVB index “is positive in the case of weak complexes and negative in
stronger ones”. In addition, in Chem. Rev., 111, 2597 (2011) the author stated that CVB index “is
positive for weak hydrogen bond, and it decreases if the strength of this interaction increases; usually
this index is negative for strong hydrogen bonds”.
I found the ELF(C-V) and ELF(DH-A) themselves sometimes have better linear relationship
with H-bond binding energy than CVB index, therefore I suggest you also examine this point in
your practical studies when you intend to use CVB index.
(2) Manual evaluation of CVB index
Below I will show how to manually evaluate the two terms involved in the CVB index. HFHF
dimer is taken as example, the wavefunction file is provided as examples\HF_HF.wfn, it was
generated at B3LYP-D3(BJ)/def2-TZVP level, the optimization was also conducted at this level.
The D, H, A atoms in this system correspond to F2, H1, F3, respectively
The ELF(DH-A) term is defined unambiguously in the original paper. The topology analysis
module of Multiwfn is able to locate bifurcation points of ELF, namely (3,-1) critical points of ELF.

378

--- page break ---

3 Functions
Then you can check ELF value of the bifurcation point lying between the hydrogen and acceptor
atom (Section 4.2.2 illustrated how to perform topology analysis for LOL. ELF can be analyzed in
similar way). However, topology analysis of ELF is time-consuming for large system. Considering
the fact that the actual ELF bifurcation point between V(D,H) and V(A) is almost exactly lying on
the straight line linking H and A, it is better to use main function 3 of Multiwfn to plot a ELF curve
map from the H to A and then directly read the value of corresponding minimum. Below is a
screenshot of ELF topology analysis result for the HFHF dimer

The purple and orange spheres are (3,-3) and (3,-1) type of ELF critical points (CPs), respectively.
The (3,-1) CP pointed by the arrow corresponds to the aforementioned ELF bifurcation point
between V(D,H) and V(A), its ELF value was found to be 0.06487, which is just the ELF(DH-A)
of present system.
As can be seen from the above graph, the red linking line basically crosses the center of the
orange sphere, this is why the ELF(DH-A) can also be approximately evaluated based on the ELF
curve map between H1 and F3. The curve map plotted using main function 3 is shown below

The minimum highlighted by the arrow is 0.06482, which is very close to the value 0.06487 obtained
based on the expensive ELF topology analysis. This observation well demonstrates the
reasonableness of employing ELF curve map between H and A to estimate the ELF(DH-A).
As regards ELF(C-V), its definition is fairly ambiguous. Since in the original paper of CVB
index the authors did not explicitly and clearly explain how this quantity should be evaluated,
different papers often employ different rule to calculate it, leading to serious confusion in existing
literatures. For example, in the CVB original paper, namely Theor. Chem. Acc., 104, 13 (2000), it
seems that the ELF(C-V) was determined as maximal value at all ELF minima dissecting core and
valence shell on the ELF curve between D and A atoms. However, in the subsequent paper Struct.
Chem., 16, 203 (2005) written by the same author, I found the ELF(C-V) is seemingly calculated as

379

--- page break ---

3 Functions
the ELF value at one of exactly located ELF bifurcation points connecting core and valence basin
of donor atom (while acceptor atom is seemingly ignored).
In my viewpoint, the best definition of ELF(C-V) should be the ELF value at the minimum
dissecting core and valence shell of donor atom on the ELF curve between D and H. Again taking
the HFHF (H4-F3H1-F2) dimer as example, the ELF curve plotted between F2 and H1 is:

Namely ELF(C-V) = 0.0936. Hence, the CVB index for the HFHF system should be 0.0936 −
0.0648 = 0.0288. This value is very different to the counterpart (-0.006) in Table 2 of Theor. Chem.
Acc., 104, 13 (2000), because the calculation levels are different, the ways of obtaining ELF(C-V)
are different, and the sign of the data in this paper was erroneously reversed.
(3) Calculating CVB index in fully automatic way
In order to simplifying the calculation of CVB index in above mentioned way as much as
possible, Multiwfn provides a function used to calculate this index in fully automatic way. Still
taking the HFHF dimer as example, boot up Multiwfn and input below commands
examples\HF_HF.wfn
200 // Other function, part 2
1 // Calculate CVB index and related quantities
2,1,3 // Index of donor atom, hydrogen and acceptor atom of the H-bond, respectively
The result is
Core-valence bifurcation value at donor, ELF(C-V,D):

0.0936

Distance between corresponding minimum and the hydrogen:

Core-valence bifurcation value at acceptor, ELF(C-V,A):

0.743 Angstrom

0.1408

Distance between corresponding minimum and the hydrogen:

Bifurcation value at H-bond, ELF(DH-A):

1.628 Angstrom

0.0648

Distance between corresponding minimum and the hydrogen:

The CVB index, namely ELF(C-V,D) - ELF(DH-A):

0.614 Angstrom

0.028768

The result is completely identical to that we calculated manually. The outputted ELF(CV,A) is
useless in current context, but some users may be interested in it.
In order to make you better understand how the CVB index is automatically calculated in

380

--- page break ---

3 Functions
Multiwfn, here I explain the implementation detail. After the user inputted index of D, H and A
atoms, the ELF curves corresponding to D-H and H-A are calculated in turn, and then the
ELF(CV,D), ELF(DH-A) and ELF(CV-A) are automatically identified from the curve data, as
illustrated below

(4) Special case: Calculating CVB index for some very strong H-bonds
In principle, the CVB index calculation protocol described above works for most kinds of
systems that have typical H-bond, both intermolecular and intramolecular H-bonds can be analyzed
in the same way. However, for some very strong H-bonds, whose hydrogen is lying at midpoint
between two heavy atoms, such as H2OH+OH2, this protocol is no longer valid because its ELF
curve does not show typical feature, as shown below (since the O-H-O angle in this system is close
to 180, only one plot is needed):

It can be seen that the V(D,H) has bifurcated as V(O) and V(H). In this case you should evaluate
CVB index manually by plotting ELF curve maps, and the ELF(DH-A) in the standard CVB index
expression should be replaced with the ELF value at the local minimum between the V(H) and V(O)
in the curve map.
Below is a more complicated case, F-HO-H, you also need to manually evaluate the CVB
index by plotting ELF curve map. The ELF curve map shown below was plotted between the F and
O (the FHO is almost linear, therefore only one plot is needed), as can be seen the ELF is

381

--- page break ---

3 Functions
unsymmetric with respect to the central hydrogen:

This system can be regarded as having two H-bonds, the H-bond binding energy of O-H...F and
O...H-F must be very different. For the former, CVB index = ELF(B) - ELF(C), while for the latter,
CVB index = ELF(D) - ELF(A). This is because when discussing the system for example as O-H...F,
the O and F behave as donor and acceptor atoms, respectively. Therefore the minimum of point C
between H and F should be regarded as the ELF(DH-A), while the minimum of point B should be
viewed as ELF(C-V,D).
(5) Special case: H-bond acceptor is not a single atom
Acceptor of some H-bonds is not a single atom. For example, the acceptor of HFethylene is
the  region of ethylene. This kind of H-bond is known as -hydrogen bond. In this case, you also
have to manually calculate the CVB index.
The wavefunction file of the HFethylene has been provided as examples\C2H4_HF.wfn, its
geometry is shown below.

For this system, you can obtain ELF(C-V,D) by plotting ELF curve map between F7 and H8
and read the ELF value at minimum, the value will find to be 0.0944.
ELF(DH-A) of this system can be obtained via ELF topology analysis. To do this, we input
below commands in Multiwfn:
2 // Topology analysis
-11 // Select the real space function to be analyzed
9 // ELF
6 // Search critical points by randomly distribute initial guesses within a sphere
4 // Set the sphere center as geometry center of three atoms
1,4,8 // Center of C1, C4 and H8 will be set as the sphere center

382

--- page break ---

3 Functions
0 // Start searching (the sphere radius, the number of starting points can be set by corresponding
options in the interface)
-9 // Return
0 // Visualize topology analysis result
Now you can see below graph. Clearly, the critical point 5 corresponds to the bifurcation point
between V(D,H) and the basin of  electron.

Close the GUI, select option 7, and then input 5 to check properties of the critical point 5, you will
find its ELF value is 0.1241, which is just the ELF(DH-A) of this H-bond. Hence, the CVB index
of this system is 0.0944 - 0.1241 = -0.0297.
In fact, since this system has high symmetry, you can also obtain ELF value of the critical point 5 by simply
plotting ELF curve map between H8 and midpoint of C1-C4.

Information needed: Atom coordinates, GTFs

3.200.2 Calculate atomic and bond dipole moments in Hilbert space
This function is used to calculate atomic and bond dipole moments directly based on basis
functions (viz. in Hilbert space). You can also consult Section 12.3.2 of the book Ideas of Quantum
Chemistry (L. Piela, 2007).
Theory
In the formalism of basis functions, the system dipole moment can be expressed as follows

μ = μ nuc + μele =  Z AR A −  Pi , j i r  j
A

i

j

where Z and R are charge and coordinate of nuclei. P is density matrix,

i r  j

is dipole moment

integral between basis function i and j.
The system dipole moment can be decomposed as the sum of single-atom terms and atomic
pair terms
tot
nuc
pop
dip
pop
dip
μ =  μ tot
A +  μ AB =  (μ A +μ A + μ A ) +  (μ AB + μ AB )
A

A B A

A B A

A

The expression and physical meaning of the five terms are

383

--- page break ---

3 Functions
𝛍nuc
𝐴 : Dipole moment due to nuclear charge

μ nuc
A = Z AR A
pop

𝛍𝐴 : Dipole moment due to the electron population number localized on single atom (Notice
that this is different to the electron population number calculated by Mulliken or similar methods,
because the overlap population numbers have not been absorbed into respective atoms)
loc
μ pop
pAloc =  Pi , j i  j
A = − pA R A

iA jA
dip
𝛍𝐴 : Atomic dipole moment, which reflects the electron dipole moment around an atom. rA is

the coordinate variable with respect to nucleus A

μdip
A = −  Pi , j  i rA  j
iA jA

where rA = r − R A

= − Pi , j i r  j − μ pop
A
iA jA

pop
𝛍𝐴𝐵 : Dipole moment due to the overlap population between atom A and B
pop
AB
AB AB
AB
i, j
i
j
AB
A
B
iA jB
dip
𝛍𝐴𝐵 : Bond dipole moment, which somewhat reflects the electron dipole moment around

μ

= −p R

= 2 P

p

 

R

= (R + R ) / 2

geometry center of corresponding two atoms. Of course, if A and B are not close to each other, then
this term will be very small, and thus inappropriate to be called as bond dipole moment.

μdip
AB = −2  Pi , j  i rAB  j
iA jB

where rAB = r − R AB

= −2 Pi , j i r  j − μ pop
AB
iA jB

By means of Mulliken-type partition, the bond dipole moments can be incorporated into atomic
dipole moments, so that the system dipole moment can be written as the sum of single center terms

pop + μAdip )
μ =  (μ nuc
A +μ A

A
pop
where 𝛍′𝐴 is the dipole moment due to the Mulliken population number of atom A
pop
Mul
Mul
A
A
A
A
i, j
i
j
B iB jB
dip
and 𝛍′𝐴 is the atomic overall dipole moment of atom A
dip
Mul
A
i, j
i A
j
i, j
i
j
A
B iB jB
B iB jB

μ

μ

= −p R

= − P

p

=  P

 

 r  = − P  r  − μ

Note that the B index in above formulae runs over all atoms.
Usage
The input file must contain basis function information (e.g. .mwfn, .fch, .molden and .gms).
After you enter present function, you can choose option 1 to output information of a specific
dip

atom, including: Atomic local population number, 𝑝𝐴loc ; atomic dipole moment, 𝛍𝐴 ; contribution
to system dipole moment due to nuclear charge, 𝛍nuc
𝐴 ; contribution to system dipole moment due to
dip

pop

dip

pop

electron, 𝛍𝐴 + 𝛍𝐴 ; contribution to system dipole moment, 𝛍nuc
𝐴 + 𝛍𝐴 + 𝛍𝐴 .
You can also choose option 2 to output information between specific atomic pair, including:
pop

dip

bond population number, 𝛍𝐴𝐵 ; bond dipole moment, 𝛍𝐴𝐵 ; contribution to system dipole moment,

384

--- page break ---

3 Functions
pop

dip

𝛍𝐴𝐵 + 𝛍𝐴𝐵 .
If choose 3, atomic overall dipole moment and related information of selected atoms will be
outputted, including: Atomic Mulliken population number, 𝑝𝐴Mul ; atomic overall dipole moment,
dip

𝛍′𝐴 ; contribution to system dipole moment due to nuclear charge, 𝛍nuc
𝐴 ; contribution to system
dip
pop
dipole moment due to electron, 𝛍′𝐴 + 𝛍′𝐴 ; contribution to system dipole moment, 𝛍nuc
𝐴 +
dip
pop
𝛍′𝐴 + 𝛍′𝐴 .
If you choose option 10, then X/Y/Z components of electron dipole moment matrix will be
outputted to dipmatx.txt, dipmaty.txt and dipmatz.txt in current folder, respectively. For example, the
(i, j) element of Z component of electron dipole moment matrix corresponds to

− Pi , j i z  j
i

j

Information needed: Atom coordinates, basis functions

3.200.3 Generate cube file for multiple orbital wavefunctions
By this function, grid data of multiple orbital wavefunctions can be calculated and then
exported to a single cube file or separate cube files at the same time.
After you entered this function, you need to first select the orbitals you are interested in (e.g.
3,5,9-17), then define grid setting, then choose the scheme to export the grid data. If you select
scheme 1, then the grid data will be exported as separate files, for example orb000003.cub,
orb000005.cub, orb000009.cub, etc. The number in the filename corresponds to orbital index. If
you select scheme 2, then grid data of all orbitals you selected will be collectively exported to
orbital.cub in current folder. Lots of visualization programs, including VMD and Multiwfn, support
the cube file containing multiple sets of grid data.
For restricted and unrestricted single-determinant wavefunctions, in this function you can
select orbital based on HOMO and LUMO. For example, h-3 means HOMO-3, l+2 corresponds to
LUMO+2. See prompt on screen for more examples.
Information needed: Atom coordinates, GTFs

3.200.4 (moved to Section 3.28.3)
3.200.5 Plot radial distribution function for a real space function
This function is used to plot radial distribution function (RDF) for a real space function

RDF ( r ) =  f ( r, )r 2 d 
where r is radial distance from sphere center, and  denotes angular coordinate in a sphere layer.
The integration curve of RDF can also be plotted
r'

r'

rlow

rlow

I (r ') =  RDF (r )d r = 

 f ( r , ) r d  d r
2

Clearly, if rlow is set to 0 (viz. sphere center), then I() will be the integral of f over the whole space.

385

--- page break ---

3 Functions
In present function, one can choose the real space function to be studied, set the position of
sphere center, set the lower and upper limit to be calculated and plotted, set the number of points in
radial and angular parts. The larger the number of points, the more accurate the integration curve.
After the parameters have been properly set, selecting option 0 to start the calculation, then you
will see a new menu, in which you can plot RDF and its integration curves, save the graph or export
the corresponding original data. In this menu you can also find an option used to export spherically
averaged function (f sph), it correlates with RDF via below relationship

f sph ( r ) =

RDF ( r )
4 r 2

An example is given in Section 4.200.5.
Information needed: Atom coordinates, GTFs

3.200.6 Analyze correspondence between orbitals in two wavefunctions
Theory
This function is primarily used to analyze correspondence between the orbitals in two
wavefunctions. The two sets of orbitals can be produced under different basis sets, by different
theoretical methods, at different external environments, in different electronic states, or at slightly
different geometries. The two sets of orbitals can also be different types, for example the first set of
orbitals are canonical MOs produced by Hartree-Fock calculation, while the second set of orbitals
are natural orbitals produced by post-HF calculation.
The orbitals {i} in present wavefunction (the wavefunction loaded when Multiwfn boots up)
can be represented as linear combination of the orbitals {j} in another wavefunction (the
wavefunction you specified after entering present module), i.e.

i =  Ci , j j

where Ci , j = i j 

j

  (r) (r) d r
i

j

Once we have the overlap integral, we immediately know how j is associated with i. The
contribution from orbital j to orbital i is simply the square of overlap integral, namely <i|j>2100%.
The present function is able to compute the C matrix as well as the contributions, so that you can
easily make clear the relationship between the two sets of orbitals.
Usage
After you enter this function, first you need to input the orbital range to be considered for
present wavefunction (istart1~iend1), then input the path of the second wavefunction and the orbital
range to be considered (istart2~iend2). After that the overlap matrix between istart1~iend1 and
istart2~iend2 will be calculated by Becke's multi-center numerical integration scheme. Then you
will see the five largest contributions from istart2~iend2 to each orbital in istart1~iend1.
If you want to obtain all coefficients (as well as the corresponding contributions) of
istart2~iend2 in a specific orbital among istart1~iend1, you can then directly input the index of the
orbital.
If an orbital (i) in present wavefunction can be exactly expanded as linear combination of
istart2~iend2, then the normalization condition must be satisfied:

386

--- page break ---

3 Functions
iend 2

 i j 100% = 100%
2

j =istart 2

From the Multiwfn output you can find the maximum deviation to normalization condition. If the
value is zero, that means all orbitals in istart1~iend1 can be exactly represented by the orbitals in
istart2~iend2.
Note that the atomic coordinate of present wavefunction and that of the second wavefunction
are not necessarily identical, the two wavefunctions can even correspond to different molecules.
However, if the difference of the distribution scope of the atomic coordinates in the two
wavefunctions is large, the integration accuracy must be low and the result is not reliable.
Commonly the default integration grid is fine enough, i.e. 30 radial points, 302 angular points
with "radcut=15". If you wish to improve the accuracy, you should set "iautointgrid" in settings.ini
to 0, then you can define "radpot", "sphpot" and "radcut" in settings.ini; increasing their values will
result in better integration accuracy.
The computational cost of this function directly depends on the number of orbitals in
consideration; so if your system contains very large number of orbitals, do not choose all orbitals at
once.
Special usage: Evaluating overlap integrals and superpositions between two sets of
orbitals
Present function can also be used to evaluate overlap between orbitals, the orbitals may come
from the same wavefunction, or come from two different wavefunctions.
After entering the interface of present function, if you want to obtain all overlap integrals (i.e.
all <i|j>) between the above mentioned orbitals istart1~iend1 in the first wavefunction and orbitals
istart2~iend2 in the second wavefunction, simply input -1, then these integrals will be outputted to
convmat.txt in current folder.
If what you need is not common overlap integral between orbital wavefunctions but overlap
integral between norm of orbital wavefunctions, which is useful for measuring orbital superposition
and expressed as ∫|𝜑𝑖 (𝐫)||𝜑𝑗 (𝐫)|d𝐫, you should input -2 in the interface of present function, then
all these integrals between the orbitals istart1~iend1 and istart2~iend2 will be outputted to
Snormmat.txt in current folder. Similarly, if what you need is ∫|𝜑𝑖 (𝐫)|2 |𝜑𝑗 (𝐫)|2 d𝐫, you should input
-3 in the interface, then the result will be outputted to Snorm2mat.txt.
In fact, the present function can somewhat equivalently realize the functions introduced in
Sections 3.100.5, 3.100.11 and 3.100.15, but the output format and main purpose are different.
Some usage examples of this function are given in Section 4.200.6.
Information needed: Atom coordinates, GTFs

387

--- page break ---

3 Functions

3.200.7 (moved to Section 3.27.1)
3.200.8 (moved to Section 3.27.2)
3.200.9 Calculate average bond length and average coordinate number
This function is used to calculate average bond length between two elements and average
coordinate number. This function is particularly useful for analyzing structure character of atom
clusters, for example the Ge12Au cluster shown below (the structure file is provided as
examples\Ge12Au.pdb). By using this function, we can immediately obtain the average Ge-Ge bond
length and average Au-Ge bond length, as well as average coordinate number of Ge due to Ge-Au
or Ge-Ge bonds, or of Au due to Ge-Au bonds. A nice application of this kind of analysis on Al
clusters can be found in J. Chem. Phys., 111, 1890 (1999).

The average bond length is defined as follows

R =

1
 Rij
nb i j

where Rij is the distance between atom i and j, only the terms smaller than or equal to a given distance
cutoff (e.g. 2.2 Å) will be regarded as bonds and thus be taken into the summation. nb is the total
number of bonds.
The average coordinate number is calculated as follows

CN =

1
 Ni
n i

where Ni is the number of bonds surrounding the atom i, n is the total number of atoms.
After you entered this function, you need to input two elements, for example Ge,Au, and input
a distance cutoff, for example 3.2, then the Ge-Au contacts  3.2 Å will be regarded as Ge-Au bonds
and the average bond length will be calculated, the minimum and maximum bond lengths will also
be outputted. After that, if you select y, the average coordinate number of Ge due to Ge-Au bonds
will be shown.
Information needed: Atom coordinates

388

--- page break ---

3 Functions

3.200.10 Output various kinds of integral between orbitals
This function is used to calculate electric/magnetic dipole moment integral, velocity integral,
kinetic energy integral and overlap integral between orbitals, advanced users may recognize the
significance of these data. In the case of a range of orbitals, the results are exported to orbint.txt in
current folder, the first and second columns correspond to the index of the two orbitals; In the case
of a pair of orbitals, the result is directly printed on screen.
The electric dipole moment integral vector between two orbitals is defined as

μ ij = μ ji =  i | −r |  j 
The magnetic dipole moment integral vector between two orbitals is calculated as (more detail
can be found in Section 3.21.1.1. The negative sign is ignored)

M ij = i   i | r   |  j 
The velocity integral vector between two orbitals is evaluated as (the negative sign is ignored)

v ij = i  i |  |  j 
It is noteworthy that, due to the Hermitian of the operators, we have

M ii = 0

M ij = M ji = − M ji

v ii = 0

v ij = v ji = − v ji

Note that the imaginary sign is not explicitly shown in the output.
The kinetic energy and overlap integrals between two orbitals are respectively evaluated as

Kij = − 12  i  2  j

Sij =  i  j

If you need to calculate Coulomb or exchange integral between two orbitals, you should use
the function described in Section 3.200.17.
Information needed: Atom coordinates, GTFs

3.200.11 Calculate center, first/second moments, radius of gyration, and
<r^2> of a function
This function is used to calculate various quantities characterizing distribution of a selected
real space function.
Theory
The center of a real space function f is defined as

389

--- page break ---

3 Functions

rc =

 r  f (r ) d r
 f (r ) d r

where the integral is performed over the whole space.
The first moment is a vector and is evaluated as

 x 
 x


μ =  y =   y  f (r ) d r
 
 
  z 
 z 
where x, y, z are the Cartesian coordinate components relative to rc.
The second moment is a matrix and defined as

  xx

Θ =   yx
  zx


 xy
 yy
 zy

 x2
 xz 


 yz  =   yx
 zx
 zz 


xy
y2
zy

xz 

yz  f (r ) d r
z 2 

If its eigenvalues {} are sorted from low to high, then the anisotropy of  can be calculated as

 3 − ( 1 +  2 ) / 2 . The radius of gyration is calculated as

1 +  2 +  3

 f (r ) d r

.

Spatial extent 〈𝑟 2 〉 = ∫(𝑥 2 + 𝑦 2 + 𝑧 2 )𝑓(𝐫)d𝐫 is simply the trace of the second moment matrix,
or the sum of its three eigenvalues. If f(r) is chosen to be electron density, then <r2> corresponds to
the well-known electronic spatial extent (ESE). Note that ESE and electric dipole/multipole
moments can be evaluated analytically and much more efficiently by a specific function in Multiwfn,
see Section 3.300.5.
Usage
This function employs Becke's multicenter integration method for evaluating above mentioned
quantities. The accuracy is fully determined by radial points and angular points, which can be set
by "radpot" and "sphpot" in settings.ini, respectively.
Option 1 calculates and outputs all aforementioned quantities. The real space function to be
studied can be selected by option 3. The center (rc) defaults to (0,0,0), and it can be manually set by
option 4. Option 2 is used evaluate the center of the selected real space function, which can be
directly taken as the rc for the subsequent calculation of option 1 (evidently, if rc is set to be the
center of the selected function, the calculated first moment will be zero).
When using option 1, if the real space function to be studied is chosen as electron density, then
the nuclear contribution of quadrupole moment and molecular quadrupole moment tensors will also
be outputted. In fact, the latter can be straightforwardly obtained by subtracting the former by the
second moment of electron density.
If the real space function of interest has both positive and negative parts with comparable
magnitude (e.g. orbital wavefunction with evident positive and negative phases), option 5 is usually
recommended to use instead of option 2 for evaluating the distribution center of the selected real
space function, because option 5 uses absolute function value in the evaluation, therefore
cancellation effect can be avoided. In addition, for such kind of real space function, in order to
calculate their aforementioned statistical quantities, it is suggested to choose option -1 once before
choosing option 1, in this case the absolute function value will be used in the evaluation.

390

--- page break ---

3 Functions

A brief example is given here. To evaluate the first and second moments of spin density
(relative to the center of spin density), after loading a wavefunction file, you should input
200 // Other function (Part 2)
11 // The present function
3 // Select a real space function
5 // Spin density
2 // Calculate center of spin density
y // Take the calculated center for evaluating various data in option 1
1 // Evaluate various data for spin density
Then the data will be shown on screen.
See my blog article “Using Multiwfn to exhibit excess electrons and calculate their radius of
gyration” (http://sobereva.com/658, in Chinese) for more illustration of using this module.
Information needed: Atom coordinates, GTFs

3.200.12 Calculate energy index (EI) or bond polarity index (BPI)
This function is used to calculate energy index (EI) and bond polarity index (BPI), which were
defined in J. Phys. Chem., 94, 5602 (1990) and further discussed in J. Phys. Chem., 96, 157 (1992).
The EI for atom A in a molecule is defined as follows
val

EI A =

  
i i

i, A

 

i, A

i
val

i

i

where i,A denotes composition of atom A in MO i. i and i are occupation and energy of MO i,
respectively. The summation runs over valence MOs. In fact, the denominator is simply the number
of valence electrons of atom A, and the numerator corresponds to total energy of its valence electrons.
Therefore, EIA can be regarded as average energy per valence electron of atom A. In the original
paper of EI, Mulliken method was used to compute the atomic contribution to MOs, thus this method
is also employed in present implementation of EI, though other methods such as Hirshfeld partition
should work equally well or even better. (Note that since Mulliken method is used, which is
incompatible with diffuse functions, the use of diffuse basis functions must be avoided!)
The BPI between atoms A and B in a molecule is defined as
ref
BPI AB = ( EI A − EI ref
A ) − ( EI B − EI B )

where EIref is reference EI value derived from calculation of homonuclear species. For example, you
ref
study BPICN for H3C-NH2, then EICref is computed as EIC in ethane, and EIN
is computed as EIN in
H2N-NH2. The larger magnitude of BPIAB implies higher bond polarity of the A-B bond.
Group electronegativity is evaluated as negative of EIX for corresponding radical, X is the
attaching atom. For example, to obtain group electronegativity for -CH3 group, you should calculate

-EIC for CH3 radical.

391

--- page break ---

3 Functions
This function of Multiwfn is used to calculate EI for specific atom in present system, all the R,
RO and U types of HF/DFT wavefunction are supported. Multiwfn automatically detects the number
of inner-core electrons and determines which MOs are the valence ones and thus should be taken
into account.
An example is given in Section 4.200.12.
Information needed: Atom coordinates, basis functions

3.200.13 Evaluate orbital contributions to density difference or other
grid data
Theory
This function is mainly designed to evaluate contribution of each of selected orbitals to a given
density difference, , so that you can clearly understand which orbital(s) are main contributor(s)
of change in electron density distribution. A similar idea has been employed in J. Mol. Model., 24,
25 (2017) to study contribution of various NBO orbitals to Fukui function (a special kind of electron
density, see Section 4.5.4) to better unravel its chemical meaning. Below, the theory and algorithm
used in the present function are outlined.
 is able to be approximately represented as linear combination of probability density of
orbitals, which is norm of corresponding orbital wavefunction

 (r )   pi |  i (r ) |2
i

What we need to obtain is the optimal value of {p} for expanding the . The pi can be regarded as
contribution of orbital i to the . The {p} could be derived via least-squares method by minimizing
the difference between  and

 p |  (r) | over the whole space, at the meantime the sum of
2

i

i

i

{p} could be constrained to a given value P via Lagrangian multiplier technique. The error function
to be minimized in the actual implementation is
2





F =    (r ) −  pi |  i (r ) |2  d r +    pi − P 
i


 i

In Multiwfn, the integral is treated as numerical integration based on evenly distributed grids, that
is
2





F = V    (r ) −  pi |  i (r ) |2  +    pi − P 
 
i

 i

where  is index of grid point, V is grid volume.
Clearly, below conditions should be satisfied to determine the optimal {p}

F
= 0 i = {1, 2, 3 }
pi

p =P
i

i

392

--- page break ---

3 Functions
more explicitly,

F
=0 
pi

 p  |  (r ) | |  (r ) | +  =  |  (r ) |  (r )
2

j

2

i

2

j

i

j

obviously, the working equation to determine {p} should be

 A1,1


 AN ,1

 1

1  p1   B1 
   
  =  
1  pN   BN 
   
0     P 

A1, N

1

AN , N
1

Ai , j = Aj ,i =  | i (r ) |2 |  j (r ) |2


Bi =  | i (r ) |2  (r )


The fitting error reported by Multiwfn is estimated using the following formula

definition1:   (r) −  pi |  i (r) |2 d r
i

2



definition 2 :    (r ) −  pi | i (r ) |2  d r
i


In principle, this algorithm works for any kind of  and orbital. For example, the  may
corresponds to Fukui function, density variation during electron excitation and so on. The orbital
could be localized molecular orbital (LMO), NBO, MO, etc. The contribution p can be positive or
negative, the sign reflects phase of participation of orbital density in the . Notice that the choice
of orbital range is highly arbitrary while evidently affects the result. For example, the range can
include all orbitals of a certain type, or only include occupied ones.

Usage
Below is the common procedure to derive contribution of a set of orbitals to  via the present
function
(1) Use Multiwfn or other codes to generate cube file of . The procedure of calculating grid
data of  has been substantially illustrated in many sections of present manual, for example,
Section 4.5.4 (Fukui function and dual descriptor) and Section 4.18.3 ( corresponding to electron
excitation).
(2) Load a file containing orbitals of interest into Multiwfn, then enter subfunction 13 of main
function 200.
(3) Input the path a cube file containing  to make Multiwfn load it.
(4) Use option 1 to set constraint on the sum of contributions, namely the P value in above
equations. The P is default to 1.0.
(5) Select option 0, then set the range of orbitals to be taken into account. After calculation,
contribution (p) of all chosen orbitals will be shown on screen.
(6) In the post-processing menu, you can choose to visualize isosurface of , fitted density

 p |  (r) | or their difference  −  p |  (r) | so that you can visually examine the
2

2

i

i

i

i

i

393

i

--- page break ---

3 Functions
fitting quality. The fitted density can also be exported as cube file.
In Multiwfn, there are two modes to deal with the grid data of orbitals during construction of
the A matrix and B vector. You can switch the mode via option 2.
• Memory based (default): Grid data of density of all chosen orbitals are automatically
calculated first and recorded in memory, in this case the calculation is fairly fast however the
requirement on available memory is very high when large range of orbitals is selected and the grid
quality is relatively high. This mode is strongly recommended to use if Multiwfn does not crash due
to insufficient memory.
• Cube file based: Grid data of density of all chosen orbitals are automatically calculated and
saved as individual cube files in current folder as rho_xxxxx.cub, where xxxxx is orbital index. The
file will be loaded when corresponding orbital is used to construct the A matrix and B vector. The
speed of this mode is by far slower than the "memory based" mode, but the advantage is that memory
consumption is almost negligible.
Note that the grid setting of the automatically calculated orbital densities is set by the program
to exactly identical to that of the loaded  grid data.
It is important to note that the present function is general, flexible and not necessarily limited
to study orbital contributions to . For example, if the provided cube file contains grid data of 
rather than , then the present function will yield contribution of selected orbitals to  (of course,
before doing this, the P should be properly set to make the resulting contribution values meaningful.
If you are not sure how to set it, you can simply remove the constraint)
Practical analysis examples of this function are given in Section 4.200.13.

3.200.14 Domain analysis (obtaining properties within isosurfaces of a
function)
This function is used to integrate specific real space functions in domains. The domains refer
to individual spatial regions enclosed by isosurfaces of a specific real space function. For example,
you can use this function to integrate electron density within various domains defined by isosurfaces
of reduced density gradient (RDG) to study strength of weak interactions at different places. If this
module is flexibly utilized, many special analyses can be realized. For example, visualizing and
obtaining volume of molecular cavity (see Section 4.200.14.2 for example).
Basic usage
Below is basic procedure of using this module:
(1) After entering the domain analysis module, use options 2 and 3 to set the way to define the
domains. For example, you selected RDG by option 2 and inputted <0.5 in option 3, then the regions
where RDG is smaller than 0.5 will be identified as different domains.
Note that periodicity can be taken into account during identification of domains. To enable it,
choose option “4 Toggle considering periodicity during domain analysis” to set its status to “Yes”.
(2) Choose option 1 and properly define grid, then Multiwfn starts to calculate the grid data for
the real space function you selected and identifies domains that satisfied the criterion you set.

394

--- page break ---

3 Functions
NOTE: If you already have grid data in memory, for example you just calculated it via main
function 5 or directly load a .cub file when Multiwfn boots up, you can also choose option -1 to
directly use the grid data instead of calculating a new grid data. In this case, option 2 is evidently
meaningless.
(3) Once calculation in last step is finished, Multiwfn prints total number of grids in each
domain. In very simple case, from this information you may directly infer which domains are those
you want to study, while for general cases, you need to use option "3 Visualize domains" to visualize
domains in a GUI window, in which you can select domain at the right-bottom list and check its
profile, each green point on the graph corresponds to a grid in the domain.
Once you found the domains of interest, and you want to integrate a real space function in a
domain, you can choose option "1 Perform integration for a domain", then you will be asked to input
index of the domain, and then select the integrand. The integrand can be (1) An arbitrary real space
function supported by Multiwfn (2) The grid data currently stored in memory (3) The grid data
recorded in a .cub file (you will be asked to input its path. The grid distribution in this file must be
exactly identical to that of present grid data). After performing the integration, the integral value,
domain volume and average/maximum/minimum value of the integrand in the domain will be
outputted. In addition, minimum and maximum X/Y/Z of grids belonging to the domain, as well as
span distance in X/Y/Z will also be outputted. You can also select option "2 Perform integration for
all domains" to obtain integral values for all domains at once. (Hint: If what you need is just domain
volume, you can choose the user-defined function as integrand, which by default is 1.0 everywhere
and thus integrating this function does not take any computational time).
In addition, some regions that you are interested in may be identified as separated domains, to
study the property of the regions more conveniently, you can choose option "-1 Merge specific
domains" to merge selected domains as a single domain, so that you do not need to manually sum
up their integral values.
Via option 12 in post-process menu, you can export X, Y, Z coordinates along with value of
grid data of all grids in specific domain to domain.txt in current folder.
Visualizing domains by third-part tool
If you wish to visualize domains in third-part program such as VMD, there are two ways:
 Select "10 Export a domain as domain.cub file in current folder" and input index of the domain
of interest, then Multiwfn will export the domain as domain.cub, in which the grid point
belonging and not belonging to the domain have value of 1 and 0, respectively (value of
boundary grids are also outputted as 0 to guarantee that the isosurfaces always look closed).
After that, you can load the cube file into visualization program to visualize isosurface using
isovalue of 0.5.
 Select "11 Export boundary grids of a domain to domain.pdb file in current folder" and input
index of the domain of interest, then the resulting .pdb file will contain particles, each one
corresponds to a boundary grid. You can directly drag this file into VMD and render the
particles as spheres to visualize domain.
Special usage: Studying interactions
In the post-processing menu there is an option "5 Calculate q_bind index for a domain", this is
used to calculate the qbind index defined in J. Phys. Chem. A, 115, 12983 (2011), in which it was
demonstrated that for hydrogen-bond dimer, the scan curve of qbind index well mimics to actual
potential energy curve. This index for a domain is defined as:

395

--- page break ---

3 Functions

qatt = 

2 ( r )0

qrep = 

2 ( r )0

 n (r ) d r

attractive effect

 n (r ) d r

repulsive effect

qbind = −( qatt − qrep )
where 2(r) is the second largest eigenvalue of electron density Hessian matrix at r, its sign can be
utilized to discriminate interaction type. The paper showed that n = 4/3 gives best correlation
between qbind and actual potential curve. In Multiwfn the n can be manually set. Note that the paper
used isosurface of RDG = 0.6 when calculating this index. More negative of qbind may imply more
stable interaction.
It is noteworthy that in the post-processing menu there is a very flexible option "Perform
integration for subregion of some domains according to range of sign(lambda)*rho", which may be
useful in studying interactions. You can first select a batch of domains, and then define which
subregions of the domains will be integrated by setting range of sign(2) (if you are not familiar
with it, check Section 3.23.1), the real space function as integrand can be arbitrarily chosen. After
calculation, integral of the selected real space function and volume of the subregion of each
considered domain will be outputted in turn; in addition, the result for the areas with positive and
negative 2 in the subregions is also individually printed. Via this option, you can realize special
aim, for example, obtaining integral of kinetic energy density within the subregion where sign(2)
is between -0.015 and 0.015 a.u. for the RDG < 0.6 domain corresponding to an intermolecular
interaction region.
About integration accuracy
The method used to integrate domains in present module is even-grid integration method. In
other words, the integration value of a real space function for a domain is simply the sum of the real
space function value of the grids constituting the domain multiplied by grid volume. Therefore, the
accuracy of integration result is directly affected by the quality of grid you set.
Illustrative application examples of present module are given in Section 4.200.14.

3.200.15 Calculate electron correlation index
The total, dynamic and nondynamic electron correlation indices proposed by Matito et al. in
Phys. Chem. Chem. Phys., 18, 24015 (2016) are useful indicator of measuring magnitude of electron
correlation in present system.
Dynamic and nondynamic electron correlation indices (ID and IND) are defined as

I D = 14 {[i (1 − i )]1/ 2 − 2i (1 − i )}
i

I ND =

1
2

 (1 −  )
i

i

i

Where i denotes index of natural spin orbital,  is corresponding occupation number. Note that in
some cases,  may be marginally larger than 1.0 or negative, Multiwfn automatically set it to 1.0
and 0.0 respectively to make the calculation feasible.
Total electron correlation index defined is

396

--- page break ---

3 Functions

I T = I D + I ND = 14  i (1 − i )
i

Present function is used to calculate all the three electron correlation indices. Any wavefunction
file carrying occupation number of natural orbitals may be used as input file, e.g. .mwfn, .wfn, .wfx
and .molden files. An example is given in Section 4.A.6.
Note that Matito et al. also proposed local version of the three functions to characterize electron
correlation in local regions, Multiwfn is also able to study them, see Section 4.A.6 for example.

3.200.16 Generate natural orbitals, natural spin orbitals (NSO) and
spin natural orbitals (SNO) based on the density matrix in .fch/.fchk file
In .fch (or .fchk) file, density matrix is always recorded. For example, if you carried out a MP2
task for an open-shell system with Gaussian keywords "# MP2/cc-pVTZ density", then the
resulting .fch file will have below four fields recording corresponding type of density matrix:
Total SCF Density, Spin SCF Density, Total MP2 Density, Spin MP2 Density

While for a closed-shell system, if the keyword used is "# TD PBE1PBE/6-311G* density", then
the resulting .fch file will contain below type of density matrix:
Total SCF Density, Total SCF Density, Total CI Rho(1) Density, Total CI Density

If you do not know which kinds of density matrix are recorded in the .fch file, simply search
"Density" in the file.
Various kinds of natural orbitals can be obtained via diagonalization of proper type of density
matrix:
Natural orbitals (NOs): Diagonalizing total density matrix. The occupation is from 0.0 to 2.0.
This type of NOs is also known as spatial NOs, and specifically, unrestricted natural orbital (UNO)
for unrestricted wavefunctions
Alpha and beta natural orbitals (collectively known as natural spin orbitals, NSOs):
Diagonalizing alpha and beta density matrix, respectively. The occupation is from 0.0 to 1.0.
Spin natural orbitals (SNOs): Diagonalizing spin density matrix (i.e. Difference between
alpha and beta density matrix). The occupation is from -1.0 to 1.0. SNO with positive (negative)
occupation represent distribution of unpaired alpha (beta) electrons.
Using the present function, you can obtain any set of above mentioned types of NOs. For
example, you want to obtain SNOs of triplet water at CCSD/cc-pVDZ level, you can run below
Gaussian input file:
%chk=C:\CCSD_water_m3.chk
#p CCSD/cc-pVDZ density

test

0 3
O 0.00000000

0.00000000

0.11930801

H 0.00000000

0.75895306

-0.47723204

397

--- page break ---

3 Functions
H 0.00000000

-0.75895306

-0.47723204

Convert the .chk file to .fch, then boot up Multiwfn and input
C:\CCSD_water_m3.fch
200
16
CC // Meaning that we want to analyze coupled-cluster density matrix. You can also input
SCF here to analyze Hartree-Fock density matrix
3 // Generate SNOs (if the system is closed-shell, this selection will not occur, since only NOs
can be generated in this case)
Now the basis function information in memory has been updated to SNOs. If then you want to
visualize SNOs, or to perform real space function analysis (e.g. analyzing orbital composition of
SNOs via Hirshfeld partition), you should choose y to export wavefunction information to new.mwfn
in current folder, and then program will automatically load it. After that, all subsequent analyses
will correspond to SNOs.
One of my blog articles detailedly discussed and presented analysis example of SNOs: "The
way of generating natural orbitals based on fch file in Multiwfn and analysis instances about excited
state wavefunctions and spin natural orbitals" (in Chinese) http://sobereva.com/403.
Note: Once .mwfn file containing SNOs is loaded into Multiwfn, the system will be regarded
as open-shell and there will be the same number of alpha and beta orbitals, only the former
corresponds to SNOs, while the latter are completely meaningless and you should simply ignore
them.
This function works well for .fch/.fchk files produced by Gaussian and PSI4, and may or may
not be compatible with other programs.
If this function is used in combination with PSI4, you can analyze wavefunction as high as
CCSD(T) level, please check Section 4.A.8 for detail.
The example in Section 4.18.9 utilized this function to generate natural orbitals for transition
density matrix.
Information needed: .fch/.fchk file

3.200.17 Calculate Coulomb and exchange integral between two
orbitals
Theory and implementation
Coulomb (ii|jj) and exchange integral (ij|ji) are the two most important integrals in quantum
chemistry. This function is used to calculate them between two selected orbitals i and j, their
expressions are:

398

--- page break ---

3 Functions

(ii | jj ) =  
(ij | ji ) =  

i (r1 )i (r1 ) j (r2 ) j (r2 )
r12

i (r1 ) j (r1 ) j (r2 )i (r2 )
r12

d r1 d r2
d r1 d r2

This function is applicable for any kind of orbital, such as molecular orbitals, localized
molecular orbitals, natural transition orbitals and so on.
Currently, the integral is calculated based on uniform grid (i.e. evenly placed grid):

 2j (rl )
l  k rl − rk

(ii | jj ) = (d x d yd z ) 2 i2 (rk )
k

i (rl ) j (rl )
rl − rk
l k

(ij | ji ) = (d x d yd z ) 2 i (rk ) j (rk )
k

where k and l are indices of grid; dx, dy and dz are grid spacing in X, Y and Z directions, respectively.
This function is fairly time-consuming. For a given system, the smaller the spacing, the higher
the computational cost and better the accuracy. If you do not know if the grid spacing currently
employed is small enough, you can make a convergence test, namely gradually decreasing the
spacing and check when the value is converged.
There are two kinds of input files could be used:
• A file containing orbital wavefunction. If you use such as .mwfn, .fch or .molden as input file
when Multiwfn boots up, after entering present function, you will be asked to input two orbital
indices and choose a grid setting, then grid data of wavefunction will be automatically calculated
for them.
• Two cube files containing orbital wavefunction. You should input the cube file of the first
orbital when Multiwfn boots up, and after entering present function, input cube file of another orbital.
The cube file can be generated by any quantum chemistry code (also including main function 5 of
Multiwfn).
In the interface of present function, you can set truncation value for Coulomb (J) integral and
exchange integral (K) respectively prior to the calculation. The innermost summation of Coulomb
and exchange integral will be ignored if i2 (rk )   J and |  i (rk ) j (rk ) |  K , respectively.
Clearly, if the truncation values are properly set, the cost could be significantly reduced while
keeping accuracy almost unchanged. Commonly using the default value is suggested.
Note that if you need to calculate one-electron orbital integrals, you should use the function
described in Section 3.200.10.
Example
Here I use water molecule as example to illustrate calculation of the two kinds of integrals.
Boot up Multiwfn and input
examples\H2O_iijj.fch // Containing molecular orbitals at HF/6-31G* level
200 // Other functions (Part 2)
17 // Calculate Coulomb and exchange integrals between two orbitals
4,10 // The two orbitals are selected to be MO4 and MO10
1 // Low quality grid (corresponding to grid spacing of 0.2 Bohr)
1 // Calculate Coulomb integral with default truncation level. The result is 0.615700

399

--- page break ---

3 Functions
3 // Calculate exchange integral with default truncation level. The result is 0.122246
The exact value of (ii|jj) and (ij|ji) computed by analytic integral are 0.623256 and 0.129893,
respectively, clearly accuracy of our values calculated based on numerical integration is basically
satisfactory. If you employ better grid, for example spacing of 0.1 Bohr (corresponding to "medium
quality grid"), the accuracy will be further noticeably improved (0.62143 and 0.12793, respectively),
but the cost will be eight times higher, note that the cost is inversely proportional to cube of the grid
spacing.

3.200.18 Calculate bond length/order alternation (BLA/BOA) and
angle/dihedral alternation
Theory
In conjugated polymers, the atom and bond properties in the conjugation chain show alternant
character. The bond length alternation (BLA) is an important quantity in the study of this kind of
systems. To calculate BLA, the atom sequence in the conjugated chain should be given. For example,
the atom sequence is given as 3-5-6-9-10-12, the bond 1 is thus 3-5, the bond 2 is 5-6, etc. The BLA
in this case is calculated as
BLA = (R5-6+R9-10)/2 − (R3-5+R6-9+R10-12)/3
where R is bond length. More generally, the BLA is defined as below (see Eq. 7.6 of Handbook of
Thiophene-based Materials: Applications in Organic Electronics and Photonics)
BLA = average length of even bonds − average length of odd bonds
Smaller magnitude of BLA implies better electron conjugation along the selected path. This quantity
has been frequently employed in literatures, see J. Chem. Phys., 136, 094904 (2012) for research
example and http://photonicswiki.org/index.php?title=Structure-Property_Relationships for a
comprehensive review about the relationship between BLA and various molecular properties.
Bond order is a quantity closely related to bond length, thus the bond order alternation (BOA)
is also a quantity as useful as BLA. The only difference between BOA and BLA is that the bond
length in the latter is replaced with bond order. Compared to the BLA, the BOA exhibits the bond
alternation character from electronic structure aspect rather than simply from geometric aspect. As
fully introduced in Section 3.11, there is no unique definition of bond order. The Mayer bond order
is very suitable for evaluating BOA since it is quite general, cheap and its magnitude is close to
formal bond order.
The bond angle alternation and dihedral alternation are also frequently studied, Multiwfn is
able to calculate variation of bond angles and dihedrals along the chain.
Usage
To use this function, an atom sequence must be defined, this is quite easy. After entering the
present function, you will be asked to input the indices of the atoms that make up the sequence, the
order is completely arbitrary. Then you need to input index of the beginning atom and ending atom
in the sequence. After that, based on this information and interatomic connectivity, Multiwfn
automatically identifies the actual atom sequence and prints it on the screen, you are suggested to
briefly check it to ensure the sequence is correct. Then, for each bond in the atom sequence,
Multiwfn prints its index, corresponding atom indices, bond length and Mayer bond order, then
outputs BLA and BOA values. The bond data are also exported to current folder as bondalter.txt so

400

--- page break ---

3 Functions
that you can import it to data plotting tools such as Origin to plot "bond length vs. bond index" and
"bond order vs. bond index" curve maps. Finally, if you want to study bond angle and dihedral
alternation along the sequence, you can also let Multiwfn to output them.
The present function is also able to be used to study above mentioned properties for a closed
path (e.g. a ring), the index of ending atom in this case should be identical to the beginning atom.
If your input file contains both atom information and basis function information, such
as .mwfn, .fch and .molden files, both bond lengths and bond orders will be outputted, as stated
above. However, if your input file only contains atom information, such as .xyz, .mol2 and .pdb
files, then bond order information will not be calculated and printed.
If the input file contains interatomic connectivity, such as .mol and .mol2 format, the
connectivity matrix will be directly loaded from it. For other file formats, the connectivity is guessed
based on atom coordinate and atomic radii. If present function does not properly work, using .mol
or .mol2 file with correct connectivity as input file is recommended.
Since Mayer bond order is incompatible with diffuse functions, employing diffuse functions
must be avoided when generating wavefunction.
An example of calculating BLA/BOA and plotting "bond length/order vs. bond index" map is
given in Section 4.200.18.
Information needed: Atom coordinates, basis function (optional)

3.200.19 Calculate spatial delocalization index (SDI) for orbitals or a
function
Introduction
Spatial delocalization index (SDI) is defined by Tian Lu to measure extent of spatial
delocalization of a real space function f, it is expressed as

SDI =

1

| f

norm

(r ) |n d r

f norm (r ) =

f (r )

 | f (r ) | d r

where fnorm is a normalized function. Normalization makes comparison of spatial delocalization
extent feasible when the functions to be compared do not normalize to the same value. In standard
definition of SDI, n = 2.
The larger the SDI, the more even distribution of the function in the whole 3D space. If the
function distribution tends to aggregate in some local regions, then SDI must be small.
The larger the the exponent factor n, the stronger the ability of SDI value to distinguish spatial
delocalization extent. If n = 1, then SDI will always be 1.0 for all functions.
A key application of SDI is determining spatial delocalization extent of orbitals. In this case,
SDI of orbital i can be written as

SDIi =

1

 |  (r ) | d r
n

i (r ) =|  i (r ) |2

i

where  is orbital wavefunction. Via SDI, one can easily and quantitatively characterize

401

--- page break ---

3 Functions
delocalization character of orbitals. It is applicable to any kind of orbitals, such as molecular orbitals,
natural transition orbitals, and so on.
Usage
There are three ways to use this function to calculate SDI:
(1) Calculate SDI for a real space function: You will be asked to select a real space function
from menu, then SDI will be calculated.
(2) Calculate SDI for density of orbital wavefunctions: You will be asked to input indices of
the orbitals for which SDI will be calculated. The input file of course should contain wavefunction
information, see Section 2.5. Multiwfn will print SDI for all selected orbitals.
(3) Calculate SDI based on the grid data in memory: To use this mode, a grid data file (e.g. .cub
file) containing values of f at evenly distributed grids should be loaded when Multiwfn boots up.
This mode is useful if f cannot be directly calculated by Multiwfn.
For cases (1) and (2), Becke’s multi-center integration algorithm is used to evaluate SDI, while
for case (3), SDI is evaluated based on uniform grids.
In this function, option -1 is used to adjust the exponent factor n. Commonly it does not need
to be adjusted.
Please check Section 4.200.19 for examples of using this function.

3.200.20 Bond order density (BOD) and natural adaptive orbital
(NAdO) analyses
1 Preface
The concept of delocalization index (DI) has been detailedly introduced in Section 3.18.5. The
DI between two regions is closely related to the electronic correlation between the two regions.
Essentially, Mayer bond order and fuzzy bond order are DI calculated based on atomic spaces
defined in terms of Hilbert partition and fuzzy partition.
DI is a value. If it can be visualized, then it will be quite helpful in understanding its nature and
interatomic interaction. In J. Phys. Chem. A, 124, 339 (2020), the author proposed a real space
function named bond order density (BOD), its integral over the whole space is just DI, therefore
BOD directly reveals local contribution to DI. Clearly BOD must be a useful function in
characterizing chemical bonds. The natural adaptive orbital (NAdO) is a kind of orbital closely
related to BOD, it can exhibit source of DI in terms of an orbital picture. I also generalized the idea
of BOD/NAdO, allowing them be able to study interaction between basins or between specific
fragments. Below I detailedly describe all details about BOD and NAdO.
Note that the NAdO has no any relationship with the adaptive natural density partitioning (AdNDP) orbital
introduced in Section 3.17!

2 Theory of BOD
In order to fully understand underlying idea of BOA, it is crucial to first familiar yourself with
some related concepts.
• nth-order cumulant density

402

--- page break ---

3 Functions
The nth-order cumulant density 𝜌C𝑛 (𝐫1 , 𝐫2 ⋯ 𝐫𝑛 ) was detailedly introduced in Comput. Theor.
Chem., 1003, 71 (2013), it represents the part of nth-order reduced density 𝜌𝑛 (𝐫1 , 𝐫2 ⋯ 𝐫𝑛 ) that
cannot be expressed in terms of lower orders of reduced density, and thus provides an appropriate
measure of the n-electrons correlation existing in the system. Explicit expression of 𝜌C1 , 𝜌C2, and 𝜌C3
are given below (expressions of others orders can be found in the Comput. Theor. Chem. paper).

 C1 (r ) =  (r )
 C2 (r1 , r2 ) =  (r1 )  (r2 ) −  2 (r1 , r2 )
 C3 (r1 , r2 , r3 ) =  (r1 )  (r2 )  (r3 ) + (1 / 2)  3 (r1, r2 , r3 )
− (1 / 2)[  (r1 )  2 (r2 , r3 ) +  (r2 )  2 (r1, r3 ) +  (r3 )  2 (r1, r2 )]
where 𝜌2 (𝐫1 , 𝐫2 ) corresponds to the pair density  introduced in Section 2.6.
𝜌C𝑛 has an important feature

rn −1 ) =  Cn (r1

Cn−1 (r1

rn ) d rn

as a consequence,

   (r
n
C

1

d rn = N

rn ) d r1

where N is the total number of electrons.
• n-center population and DI
n-center population is defined as follows

N ( A, B

n) =  

A B

  (r
n

n
C

1

rn ) d r1 d r2

d rn

The subscript of the integral denotes the integration region, usually it corresponds to atomic space.
After properly normalization, the n-center population can be named as n-center delocalization index
to quantify multi-center delocalization extent.
It is important to note that 𝜌C2 just corresponds to the negative of the well-known exchangecorrelation density XC, whose integral directly defines DI ():

 ( A, B) = −2   XC (r1 , r2 ) d r1 d r2  2   C2 (r1, r2 ) d r1 d r2
A B

A B

Extensive introduction of DI can be found in Section 3.18.5. Evidently, (A,B) essentially
corresponds to the 2-center population (only differs by a factor of 2).
• Definition of BOD
The one-electron function BOD between regions A and B is defined as

BOD AB (r ) = 2  AB (r )
where

 AB (r ) =   C3 (r1, r2 , r3 ) d r2 d r3
A B

For closed-shell cases, the working equations for single-determinant wavefunctions (and thus
without explicit representation of Coulomb correlation in the wavefunction) are

403

--- page break ---

3 Functions
occ occ

 AB (r ) = i (r ) DiAB
, j  j (r )
i

D

AB

j

= S( A)S( B ) + S( B )S( A)

where i and j are doubly occupied spatial orbitals,  is orbital wavefunction. In Multiwfn, the
following definitions of S can be adopted in the calculation:
• Atomic overlap matrix (AOM)
• Basin overlap matrix (BOM)
• Fragment overlap matrix (FOM), which is defined as sum of AOM of involved atoms
There are important relationships correlating the BOD with DI and localization index (LI, )

 BOD (r) d r =  ( A, B)
(1 / 2)  BOD (r ) d r =  ( A)
AB

AA

Obviously, BOD is able to reveal contribution of every spatial position to DI and LI.
For unrestricted open-shell single-determinant wavefunctions,  and  spins should be
separately taken into account:

BOD AB (r ) = BODAB (r ) + BOD AB (r )
The working equation of  spin is
occ occ

BODAB (r ) = 2  AB (r ) = i (r ) Di, j, AB j (r )
i j

 , AB

D





= S ( A)S ( B ) + S ( B )S ( A)

Si, j ( A) =  i (r ) j (r ) d r
A

i, j  

Relevant relationships:

 BOD (r) d r =  ( A, B)
(1 / 2)  BOD (r ) d r =   ( A)




AB

AA

In fact, the relationships can be easy demonstrated, given that (with consideration of the orbital
orthogonality condition)
occ occ

occ

i j

i


 , AB

 , AB
 BOD AB (r) d r =  Di , j  i (r) j (r) d r =  Di ,i
occ

Di, j, AB =   Si,k ( A) Sk, j ( B ) + Si,k ( B ) Sk, j ( A) 
k

we have (note that S is a symmetric matrix)

404

--- page break ---

3 Functions
occ occ






 BOD AB (r) d r =   Si ,k ( A) Sk ,i ( B) + Si ,k ( B) Sk ,i ( A) 
i k

occ occ

=   Si,k ( A) Si,k ( B ) + Si,k ( A) Si,k ( B ) 
i k

occ occ

= 2 Si,k ( A) Si,k ( B )
i k

which corresponds to the expression of   given in Section 3.18.5.
In principle the BOD can be applied to multiconfiguration wavefunctions, however currently
Multiwfn only supports BOD analysis for single-determinant wavefunctions.
3 Natural adaptive orbital (NAdO)
The BOD of  spin can also be expressed in terms of natural adaptive orbitals (NAdOs, ) of
 spin:

 AB
(r ) =  i (r)nAB,ii (r)
i

,𝑖
The 𝑛𝐴𝐵
is eigenvalue of NAdO i in  spin. Evidently, if the eigenvalues are viewed as occupation
numbers, the BOD will be equivalent to the electron density calculated based on the NAdOs.
The NAdOs between regions A and B can be easily constructed. First, diagonalizing DAB to
obtain eigenvalue matrix n and eigenvector matrix U

U −1D AB U = n
MOs can then be transformed to NAdOs via the unitary transformation matrix U
MO
CNAdO = Cocc
U

MO
NAdO
where 𝐂occ
and 𝐂occ
are coefficient matrices of occupied MOs and NAdOs in basis functions,
respectively, and different columns correspond to different orbitals. Assume that there are m
occupied MOs, then both of them have m columns.

Note that for unrestricted wavefunctions, the  and  NAdOs are generated in above way
separately based on  and  occupied MOs, respectively.
Although NAdO is not an eigenfunction of Fock/KS operator, its energy can still be
meaningfully evaluated as expectation of Fock/KS operator.
5 Usage of BOD/NAdO analysis module
This module corresponds to subfunction 20 of main function 200. As shown in the interface, it
can do three kind of analysis:
(1) Interatomic interaction analysis based on atomic overlap matrix (AOM): AOM will be
loaded from a file, which can be generated by fuzzy atomic space analysis module or basin analysis
module (in the case of AIM partition). Then you will be asked to input two atomic indices.
(2) Interbasin interaction analysis based on basin overlap matrix (BOM): BOM will be loaded
from a file, which can be generated by basin analysis module (any kind of basin can be used). Then
you will be asked to input two basin indices.
(3) Interfragment interaction analysis based on fragment overlap matrix (FOM), which can be

405

--- page break ---

3 Functions
provided by two ways, corresponding options 3 and 4, respectively
• Way 1: Provide a file containing AOMs (exactly the same as case (1)), and then input indices
of the atoms in the two fragments. Then FOM will be generated based on the AOM.
• Way 2: Provide a file directly containing FOM of the two fragments. This file can be directly
generated by subfunction 33 of main function 15, see Section 3.18.4 for details. If only small portion
of atoms is involved in the two fragments, and you found computational cost for generating AOM
using subfunction 3 of main function 15 is too high, then it is suggested to provide FOM in this way,
because computational cost of generating the two FOMs by subfunction 33 of main function 15 is
significantly lower in this case.
Then NAdOs will be generated and exported to NAdOs.mwfn in current folder, in which the
originally occupied orbitals in the inputted wavefunction file now have been replaced with NAdOs,
whose occupation numbers correspond to NAdO eigenvalues, and hence the sum of the occupation
numbers just equals to DI. The unoccupied orbitals in the NAdOs.mwfn are still the original ones.
Next, if you want to directly examine BOD and NAdOs, you should select "y" to load the
NAdOs.mwfn, then you can for example, visualize NAdOs via main function 0 or perform orbital
composition analysis via main function 8. Note that as mentioned above, electron density
corresponds to BOD currently, therefore, for example, if you want to plot isosurface of BOD, you
can use main function 5 to calculate and plot electron density, the resulting map will correspond to
BOD isosurface.
By default energies of NAdOs are not calculated but simply set to zero. If you hope to obtain
energies, you should choose option “-1 Toggle if calculating energies for NAdOs” after entering the
BOD/NAdO function, then you can choose one of two ways to provide Fock matrix F: (1) Generate
it based on energies and coefficient matrix of MOs via F=SCEC-1 relationship (2) Input path of a
file containing F, then the matrix will be loaded, see Appendix 7 of this manual for details. After
that, energies of NAdOs will be evaluated during generation of NAdOs and recorded to
NAdOs.mwfn.
Examples of using BOD and NAdO to analyze practical chemical systems are given in Section
4.200.20.
Information needed: Atom coordinates, basis function.

3.200.21 Perform Löwdin orthogonalization between occupied orbitals
Sometimes, occupied orbitals are not orthonormal with each other. For example, the
wavefunction combined from multiple monomer wavefunctions via the function described in
Section 3.100.19 is an instance. The present function performs Löwdin orthogonalization between
occupied orbitals, so that they form an orthonormal set. Coefficient matrix and density matrix in
memory will be updated in this function.
This function only supports restricted closed-shell and unrestricted open-shell form of singledeterminant wavefunction. For the latter case, Löwdin orthogonalization is performed between
alpha occupied orbitals and between beta occupied orbitals respectively.

406
