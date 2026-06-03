# 3.10 Orbital composition analysis (8)

## Quick navigation

- path: 3  Functions > 3.10 Orbital composition analysis (8)
- pdf pages: 134-139
- category: functions
- main menu / option numbers mentioned in title: 8
- direct child sections: 8
- total descendant sections: 8

## When to consult this file

Use this file for the actual Multiwfn function description: menu number, theory, options, inputs, and detailed behavior.

## Keywords and aliases

3.10, Orbital, composition, 3.10.1, Output, basis, shell, atom, specific, Mulliken, Stout-Politzer, SCPA, approaches, 3.10.2, Define, fragment, 3.10.3, inter-fragment, 3.10.4, natural, atomic, approach, 3.10.5, contributions, Hirshfeld, or, Hirshfeld-I, method, 3.10.6, Becke

## Direct child sections

- **3.10.1 Output basis function, shell and atom composition in a specific orbital by Mulliken, Stout-Politzer and SCPA approaches (1, 2, 3)** (pdf pages 134-134; menu/options: 1, 2, 3)
- **3.10.2 Define fragment 1 and 2 (-1, -2)** (pdf pages 135-135; menu/options: -1, -2)
- **3.10.3 Output composition of fragment 1 and inter-fragment composition by Mulliken, Stout-Politzer and SCPA approaches (4, 5, 6)** (pdf pages 136-136; menu/options: 4, 5, 6)
- **3.10.4 Orbital composition analysis by natural atomic orbital approach (7)** (pdf pages 136-137; menu/options: 7)
- **3.10.5 Calculate atom and fragment contributions by Hirshfeld or Hirshfeld-I method (8,10)** (pdf pages 138-138; menu/options: 8,10)
- **3.10.6 Calculate atom and fragment contributions by Becke method (9)** (pdf pages 139-139; menu/options: 9)
- **3.10.7 Calculate atom and fragment contributions by AIM method (11)** (pdf pages 139-139; menu/options: 11)
- **3.10.100 Evaluate oxidation state by LOBA and mLOBA method (100)** (pdf pages 139-139; menu/options: 100)

## Extracted manual text

3 Functions

3.10 Orbital composition analysis (8)
Notice that the word “orbital” here is not restricted to molecular orbital, for example, if the
input file carries natural bond orbitals (NBO), then what will be analyzed is NBOs. There is an
excellent paper compared various orbital composition analysis approaches, see Acta Chim. Sinica,
69, 2393 (2011) (in Chinese, http://sioc-journal.cn/Jwk_hxxb/CN/abstract/abstract340458.shtml).
No matter which orbital composition analysis method you choose, if you request Multiwfn to
print composition of various atoms in an orbital, in the output you can find a value "Orbital
delocalization index" (ODI). The lower the value, the stronger the orbital delocalization. When you
intend to quantitatively compare extent of spatial delocalization of various orbitals, you will find
this index quite useful. This ODI is detailedly described and illustrated in Section 4.8.5.

3.10.1 Output basis function, shell and atom composition in a specific
orbital by Mulliken, Stout-Politzer and SCPA approaches (1, 2, 3)
Mulliken, SCPA and Stout-Politzer methods support decomposing orbital to basis function,
shell and atom compositions. Actually I have introduced the theories in Sections 3.9.5, 3.9.6 and
3.9.7, i,a100% is just the composition of basis function a in orbital i, if we sum up all the
compositions of basis functions that within a shell we will get shell composition, and if we sum up
all the compositions of shells that attributed to the same atom, we will get atom composition.
These approaches rely on basis expansion, in current Multiwfn version you must
use .mwfn, .fch, .molden or .gms as input file.
When you entered “Orbital composition analysis” submenu from main menu, select which
method you want to use for decomposition, and then input the index of orbital, the result will be
printed on screen immediately, you can also input -1 to print basic information of all orbitals to find
which one you are interested in. By default, only those terms with composition larger than 0.5%
will be printed, this threshold can be adjusted by “compthres” in settings.ini.
If the basis functions stored in .mwfn/.fch/.molden file are spherical harmonic type, then the
label of basis functions printed will look like D+1, F-3 rather than XX, XYY. The labels of spherical
harmonic basis functions used in Multiwfn are completely identical to Gaussian program, the
conversion relationship is:
D 0=-0.5*XX-0.5*YY+ZZ
D+1=XZ
D-1=YZ
D+2=√3/2*(XX-YY)
D-2=XY

F 0=-3/2/√5*(XXZ+YYZ)+ZZZ
F+1=-√(3/8)*XXX-√(3/40)*XYY+√(6/5)*XZZ
F-1=-√(3/40)*XXY-√(3/8)*YYY+√(6/5)*YZZ
F+2=√3/2*(XXZ-YYZ)
F-2=XYZ
F+3=√(5/8)*XXX-3/√8*XYY
F-3=3/√8*XXY-√(5/8)*YYY

113

--- page break ---

3 Functions

G 0=ZZZZ+3/8*(XXXX+YYYY)-3*√(3/35)*(XXZZ+YYZZ-1/4*XXYY)
G+1=2*√(5/14)*XZZZ-3/2*√(5/14)*XXXZ-3/2/√14*XYYZ
G-1=2*√(5/14)*YZZZ-3/2*√(5/14)*YYYZ-3/2/√14*XXYZ
G+2=3*√(3/28)*(XXZZ-YYZZ)-√5/4*(XXXX-YYYY)
G-2=3/√7*XYZZ-√(5/28)*(XXXY+XYYY)
G+3=√(5/8)*XXXZ-3/√8*XYYZ
G-3=-√(5/8)*YYYZ+3/√8*XXYZ
G+4=√35/8*(XXXX+YYYY)-3/4*√3*XXYY
G-4=√5/2*(XXXY-XYYY)

H 0=ZZZZZ-5/√21*(XXZZZ+YYZZZ)+5/8*(XXXXZ+YYYYZ)+√(15/7)/4*XXYYZ
H+1= √ (5/3)*XZZZZ-3* √ (5/28)*XXXZZ-3/ √ 28*XYYZZ+ √ 15/8*XXXXX+ √ (5/3)/8*XYYYY+ √
(5/7)/4*XXXYY
H-1= √ (5/3)*YZZZZ-3* √ (5/28)*YYYZZ-3/ √ 28*XXYZZ+ √ 15/8*YYYYY+ √ (5/3)/8*XXXXY+ √
(5/7)/4*XXYYY
H+2=√5/2*(XXZZZ-YYZZZ)-√(35/3)/4*(XXXXZ-YYYYZ)
H-2=√(5/3)*XYZZZ-√(5/12)*(XXXYZ+XYYYZ)
H+3=√(5/6)*XXXZZ-√(3/2)*XYYZZ-√(35/2)/8*(XXXXX-XYYYY)+√(5/6)/4*XXXYY
H-3=-√(5/6)*YYYZZ+√(3/2)*XXYZZ-√(35/2)/8*(XXXXY-YYYYY)-√(5/6)/4*XXYYY
H+4=√35/8*(XXXXZ+YYYYZ)-3/4*√3*XXYYZ
H-4=√5/2*(XXXYZ-XYYYZ)
H+5=3/8*√(7/2)*XXXXX+5/8*√(7/2)*XYYYY-5/4*√(3/2)*XXXYY
H-5=3/8*√(7/2)*YYYYY+5/8*√(7/2)*XXXXY-5/4*√(3/2)*XXYYY

An example is given in Section 4.8.1.
Information needed: Basis functions

3.10.2 Define fragment 1 and 2 (-1, -2)
Before doing composition analysis for fragments by Mulliken, Stout-Politzer and SCPA
approaches, you have to define fragment in advance. If what you are interested in is only
composition of one fragment rather than the composition between two fragments (cross term
composition), you only need to define fragment 1. The content of fragment can be chosen to basis
functions, shells, atoms or mixture of them, whatever you choose, only the indices of corresponding
basis functions are recorded eventually. Notice that the "fragment" I referred here has no any
relationship with the "fragment" involved in Section 3.1, the fragment defined here does not disturb
wavefunction at all.
All supported commands in the interface of defining fragment are self-explanatory, so I will
not reiterate them but only give an examples, that is define fragment as all P-shells of atom 3: First,
type command all, information of all basis functions is listed, find out the shells that attributed to
center 3 and contain X, Y and Z type of basis functions (viz. PX, PY and PZ). Assume that the
indices of such shells are 3, 6 and 7, then input s 3,6,7 to add them into fragment. If you want to

114

--- page break ---

3 Functions
verify your operation, input all again and check if asterisks have appeared in the leftmost of
corresponding rows, the marked basis functions are those that have been included in the fragment.
Finally, input the letter q to save current fragment and return to last menu, the indices of basis
functions in the fragment will be printed at the same time.
By default, fragments do not have any content. Each time you enter the fragment definition
interface, the status of fragment is identical to that when you leave the interface last time. So, if you
have defined the fragment earlier and you want to completely redefine it, do not forget to use “clean”
command to empty the fragment first.

3.10.3 Output composition of fragment 1 and inter-fragment
composition by Mulliken, Stout-Politzer and SCPA approaches (4, 5, 6)
After you defined fragment 1, the fragment composition analysis based on Mulliken, StoutPolitzer and SCPA approaches is available. The fragment composition is the sum of all basis function
compositions within the fragment, in this function the fragment compositions of all orbitals are
printed on screen at the same time. If the analysis method you chose is Mulliken (subfunction 4) or
Stout-Politzer (subfunction 5), below component terms are outputted together with total
composition:
c^2 term: The sum of square of coefficients of basis functions within fragment 1, namely

 C 100% .
2
a ,i

afrag1

Int.cross:

The

a ,i

b ,i a ,b

sum

  C C S 100% .

afrag1 bfrag 2

of

internal

cross

terms

in

fragment

1,

namely

Ext.cross: Fragment 1 part of the total cross term between fragment 1 and all other atoms,
namely

  w 2C C S 100% .

afrag1 bfrag1

a ,b

a ,i

b ,i a ,b

It is clear that total composition of fragment 1 equals to c^2 term + Int.cross + Ext.cross.
If the fragment 2 is also defined (you must have already defined fragment 1), in subfunction 5
(Mulliken) or subfunction 5 (Stout-Politzer) the cross term between fragment 1 and fragment 2 in
each orbital, namely

  2C C S 100% will be outputted too. “Frag1 part” and

afrag1 bfrag 2

a ,i

b ,i a ,b

“Frag2 part” correspond to the components of cross term attributed to fragment 1 and fragment 2
respectively, for Mulliken analysis the two terms are of course exactly equal due to the “equal
partition”.

3.10.4 Orbital composition analysis by natural atomic orbital approach
(7)
This function is used to calculate orbital composition based on natural atomic orbitals (NAOs).
This idea was proposed in my paper Acta Chim. Sinica, 69, 2393 (2011) http://sioc-

115

--- page break ---

3 Functions
journal.cn/Jwk_hxxb/CN/abstract/abstract340458.shtml.
Theory
The first step of the famous natural bond orbital (NBO) analysis is converting original basis
functions to NAOs based on density matrix. Resulting NAOs can be classified into three categories:
• Core-type NAOs, describing inner core densities, their occupation numbers are almost equal
to integer
• Valence-type NAOs, describing valence densities, generally they have high occupation
numbers
• Rydberg-type NAOs, mainly displaying characteristics of polarization and delocalization of
electrons, the occupation numbers of them are very low
Core and valence NAOs are collectively called as minimal set, they have strong physical
meaning and have one-to-one correspondence with "actual" atomic orbitals, so they are what we
should be most concerned. Occupied MOs are almost exclusively contributed by minimal set NAOs.
Rydberg NAOs do not have clear physical interpretation, their contributions can be ignored in
occupied MOs, however they often have great contribution to virtual orbitals.
Since NAOs is an orthonormal set, if we have MO coefficient matrix in NAO basis, we can get
contribution from a NAO to specific MO by simply squaring corresponding expansion coefficient
and then multiplying it by 100%. Composition of an atom can be calculated as sum of composition
of minimal set NAOs in this center.
This orbital composition calculation method based on NAOs has great basis set stability as
Hirshfeld approach, it is especially suitable for analyzing composition of occupied orbitals.
However for virtual orbitals, since contribution from Rydberg NAOs is often large, this method no
longer works well.
Input file
The MO coefficient matrix in NAO basis cannot be generated by Multiwfn itself, you need to
provide an output file of NBO program containing this matrix as Multiwfn input file. By default,
NBO program does not output this matrix, so you need to manually add NAOMO keyword between
$NBO ... $END field in NBO input file. The NBO program we referred here may be stand-alone
NBO program (also known as GENNBO), or NBO module embedded in quantum chemistry
software, such as L607 in Gaussian.
Options
After loading proper input file and enter present function, you will find following options in
the interface:
-1 Define fragment: This option is used to define fragment, which is needed by fragment
contribution analysis (option 1). All commands are self-explanatory.
0 Show composition of an orbital: Print contribution from NAOs, shells and atoms to a
specific MO. At the meantime, contributions from core, valence and Rydberg type of NAOs are
reported respectively.
1 Show fragment contribution to a batch of orbitals: Print contribution from the fragment
defined by option -1 to specific orbitals.
2 Select output mode: This option controls which set of terms will be printed by option 0,
there are four modes:
(0) Show all terms

116

--- page break ---

3 Functions
(1) Show non-Rydberg terms
(2) Show the terms whose contributions are larger than specific criterion
(3) Show non-Rydberg terms whose contributions are larger than specific criterion (default)
3 Switch spin type: You can find this option if the current system is open shell. You can select
the spin of the MOs to be analyzed.
An example is given in Section 4.8.2.
Information needed: MO coefficients in NAO basis

3.10.5 Calculate atom and fragment contributions by Hirshfeld or
Hirshfeld-I method (8,10)
Hirshfeld and Hirshfeld-I weighting function (see Sections 3.9.1 and 3.9.13, respectively) can
also be used for decomposing orbital to atom and fragment compositions, the composition of atom
A in orbital i is

  (r)w (r) d r  100% . The composition of a fragment is simply the sum of
2
i

A

the compositions of the atoms that belongs to the fragment. These methods have great basis set
stability and are always more reliable and reasonable than Mulliken and MMPA. In fact the
Hirshfeld partition is already good enough, the more sophisticated and computationally demanding
Hirshfeld-I partition is not necessary.
If you choose to use Hirshfeld partition, you will be prompted to select the way to generate
atomic densities for constructing Hirshfeld weighting function, I strongly suggest using the built-in
atomic densities rather than using atomic .wfn files, since the former is much more convenient. If
you choose to use Hirshfeld-I partition, regular HI iterations will be performed first to yield
converged atomic weighting functions (if you are confused by the operations, please consult the
example of computing HI charges in Section 4.7.4 and the implementation details of Hirshfeld-I
introduced in Section 3.9.13).
Before calculating orbital composition, data initialization is automatically carried out. Once it
is finished, you can input the orbital index that you are interested in. Because numerical quadrature
always introduces some errors, so the sum of all atom compositions is not exactly equals to 100%,
the deviation might be relatively significant in rare cases, so Multiwfn normalizes results
automatically and prints them under the title “After normalization”.
If you want to view composition of an atom in specific range of orbitals at the same time,
choose option -2, then input the atom index and the index range of orbitals.
If you wish to study contribution of a fragment to orbitals, use -9 to define a fragment first,
then when you input an orbital index, the contribution of the fragment will be outputted along with
the contributions of all atoms. Also, you can choose -3 to calculate the contribution from the
fragment you defined to a range of orbitals.
If selecting option -4, program will calculate composition of every atom in every orbitals and
then export all of them to orbcomp.txt in current folder.
An example is given in Section 4.8.3.

117

--- page break ---

3 Functions
Information needed: Atom coordinates and GTFs

3.10.6 Calculate atom and fragment contributions by Becke method (9)
This function is very similar to the function introduced in Section 3.10.5, the only difference
is that Becke partition is used instead of Hirshfeld partition. For most cases, their results are in
qualitative agreement with each other. Using Becke partition instead of Hirshfeld partition has a
prominent advantage, namely the atomic wavefunction files are not needed, since the Becke atomic
space can be simply constructed based on atomic radius. For more detail about Becke partition, see
Section 3.18.0. An example is given in Section 4.8.3.
Information needed: Atom coordinates and GTFs

3.10.7 Calculate atom and fragment contributions by AIM method (11)
Multiwfn is also able to compute orbital composition based on atoms-in-molecules (AIM)
partition of molecular space. In this partition method, each atomic basin corresponds to space of an
atom, see Section 3.20 on detail about the concept of basin and AIM partition. To calculate orbital
composition under AIM partition, you should use subfunction 11 of basin analysis module (main
function 17), see Section 4.8.6 for example.
Usually I do not recommend calculating orbital composition in this way, because the cost is
significantly higher than other ways while the result is not better.
Information needed: Atom coordinates and GTFs

3.10.100 Evaluate oxidation state by LOBA and mLOBA method (100)
This function is an implementation the localized orbital bonding analysis (LOBA) method
proposed in Phys. Chem. Chem. Phys., 11, 11297 (2009), and the modified LOBA (mLOBA)
proposed by me (to be published).
Theory
LOBA is a method used to evaluate atomic oxidation state based on orbital composition of
localized MOs (LMOs). The idea is very simple: if an atom has nuclear charge of Z, and its
compositions in N occupied LMOs are larger than a given threshold (e.g. 50%. In this case the
electrons in these LMOs can be approximately viewed as completely attributed to the atom. If a
LMO is doubly occupied, it should be counted twice), then the oxidation state of the atom will be
Z−N.
The idea of LOBA can also be extended to define oxidation state of a fragment, namely if the
sum of nuclear charge in a fragment is Z, and the fragment contribution to N LMOs are larger than
a certain threshold, then the fragment oxidation state will be Z−N.
mLOBA employs a different way of determining attribution of electrons of LMOs. In this
method, electrons in each LMO are assigned to the atom with maximal contribution to it. This not

118
