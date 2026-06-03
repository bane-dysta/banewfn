# 4.A.3 Overview of methods for studying aromaticity

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.3 Overview of methods for studying aromaticity
- pdf pages: 1070-1073
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.3, studying, aromaticity

## Extracted manual text

4 Tutorials and Examples
chemical systems the movement of electron in orbitals and nuclear spins also have contributions to
m, but the magnitude is evidently weaker and thus can often be neglected.
In Multiwfn, the spin population defined in many different ways can be calculated by three
modules, they are briefly discussed below.
(1) Population analysis module (main function 7). In this module, if you select Mulliken or
Löwdin population analysis, alpha, beta and spin population of each basis function, shell and angular
moment orbitals will be outputted. If you select modified Mulliken population analysis (e.g. SCPA),
only the alpha/beta/spin population of each atom will be shown. If you first define a fragment via
option -1 in main function 7, then spin population of the fragment will be printed together. Do not
use these methods when diffuse basis functions are presented in your basis set, otherwise the result
may or may not be reliable.
(2) Fuzzy atomic spaces analysis module (main function 15). After you entered this module,
select option 1 and choose electron spin density, the spin population of each atom will be shown.
They are calculated by integrating electron spin density in fuzzy space of each atom. If you want to
obtain spin population of a fragment, you should first use option -5 to define the atoms to be
calculated.
By default the fuzzy atomic space defined by Becke is employed, so the result can be called
Becke spin population. If before calculation you selected option -1 to switch to Hirshfeld or
Hirshfeld-I fuzzy atomic space, then the result will correspond to Hirshfeld or Hirshfeld-I spin
population. All the Becke, Hirshfeld and Hirshfeld-I methods are reliable in all cases. For more
detail you can consult Section 3.18.
(3) Basin analysis module (main function 17). In this module, you can use AIM method to
calculate spin population. Please consult Section 4.17.1 on how to perform integration of real space
function in AIM atomic basins. If electron spin density is chosen to be the integrand, then the result
will correspond to AIM spin population. In general I do not suggest using this method, because the
computational cost is evidently higher than using population analysis module and fuzzy atomic
spaces analysis module.
Overall, if you only need to calculate atomic spin population, using fuzzy atomic spaces
analysis module is recommended, while SCPA is also a good choice when no basis function is
employed. However, if more detailed information is requested, such as spin population in different
angular moment orbitals, please use or Löwdin Mulliken population analysis.

4.A.3 Overview of methods for studying aromaticity
Aromaticity is a fundamental concept in organic chemistry and wavefunction analysis realm.
Previously I wrote a post to thoroughly discuss the methods for studying aromaticity, see "The
methods for measuring aromaticity and their calculations in Multiwfn" (in Chinese,
http://sobereva.com/176). Multiwfn supports a very large number of methods for investigating
aromaticity, they are summarized in below table and will be briefly introduced in turn. There are
also many other methods, such as induced ring current, ARCS, magnetic susceptibility exaltation,
aromatic stabilization energy (ASE), CiLC; they will not be mentioned since they are not directly
related to the capacities of Multiwfn.
1

Method

Principle

Year

Pop.

Reliab.

Univ.

Ref.

Anti.

Molecular orbital

Hückel

1951

++

0

0

N

Y

1049

/
Y

Cost

Value

0

+

--- page break ---

4 Tutorials and Examples
2

AdNDP

Hückel

2008

+

0

+

N

Y

Y

0

+

3

NICS

Magnet.

1996

+++

++

++

N

Y

Y

+

+++

4

ICSS

Magnet.

2001

0

++

++

N

Y

Y

+++

+

5

HOMA

Geom.

1972

+

+

0

Y

Y

N

−−

+

6

Bird

Geom.

1985

0

?

N

−−

Multi-center BO

Delocal.

1990

+++

−
++

Y

7

−−
+

N

N

Y

8

ELF-σ/π

Delocal.

2004

+

0

N

Y

Y

9

PDI

Delocal.

2003

0

+

−
0

−
0

−
+++

N

N

Y

0

+

10

ATI

Delocal.

2005

−−

+

0

N

N

Y

−

0

11

PLR

Delocal.

2012

−−

+

0

N

N

Y

0

0

12

ΔDI

Delocal.

2003

Y

0

Delocal.

2005

−
0

N

FLU, FLU-π

−
+

N

13

−−
0

Y/N

N

Y

0

−−
+

14

RCP properties



1997

−

0

0

N

N

?

−

0

15

Shannon aromat.



2010

N

Y

N

−

16

EL index

2012

−−

Y

Y

N

−

17

AV1245/AVmin


Delocal.

2017

−

N

N

Y

−−

−
+

−
++

In the table, "+++", "++", "+", "0", "−" and "− −" correspond to very high, high, relatively high,
normal, relatively low and low, respectively. "Y" and "N" stand for "Yes" and "No", respectively.
The meaning of each column are given below.
Principle: The principle behind the method. "Hückel" = Hückel rule; "Magnet." = Magnetic
properties; "Geom." = Molecular geometry; "Delocal." = Electron delocalization character; "" =
Electron density distribution.
Year: The year that the method was first time proposed.
Pop.: Popularity in recent years.
Reliab.: Reliability, measuring if the method is able to faithfully reveal aromaticity.
Univ.: Universality. A method with high universality must be able to be applied to a wide variety of
kinds of systems and situations, such as the rings containing heteroatoms and transition metals, nonequilibrium geometry (e.g. transition state of Diels-Alder adduction), excited state, etc.
Ref.: If the method relies on reference systems. A universal method must avoid this feature.
Anti: If the method is also able to measure anti-aromaticity.
σ/π: If the method can be used to separately discuss σ and π aromaticity.
Cost: The computational cost to apply the method.
Value: The overall value. This is the most important descriptor.
Next, the methods presented in above table will be briefed sequentially, and how to realize
them in Multiwfn will also be mentioned.
1. Molecular orbital (MO): The famous Hückel 4n+2 and 4n rule for determining aromaticity
character was first explicitly presented in J. Am. Chem. Soc., 73, 876 (1951). For a molecule, if there
are totally 4n+2 electrons in π () MOs, and this set of MOs share the similar delocalization pattern,
then the ring involved in these MOs will show π () aromaticity. If there are 4n electrons, then the
ring should possess anti-aromaticity. Note that for Möbius type of molecule, the 4n+2 and 4n rule
are inverted.
In order to use the Hückel rule to determine aromaticity, one should first pick out proper MOs
by visualizing MO isosurfaces, you can use main function 0 for this purpose. If the system is exactly

1050

+

−−
++

--- page break ---

4 Tutorials and Examples
planar, you can directly make use the function introduced in Section 3.100.22 to find out the indices
of all π MOs.
2. AdNDP (Adaptive natural density partitioning): The MO method shown above
commonly is only applicable to the molecule containing only one ring. When there are multiple
rings, such as phenanthrene, the MOs are useless, since MOs in general delocalize over the whole
molecule and thus cannot be used to study local aromaticity of different rings. The AdNDP method,
which was proposed in Phys. Chem. Chem. Phys., 10, 5207 (2008), is able to overcome this difficulty.
AdNDP has been carefully introduced in Section 3.17, and many examples are given in Section 4.14.
3. NICS (Nucleus-independent chemical shift): NICS uses the negative value of magnetic
shielding value at ring center to measure its aromaticity. This is the most popular aromaticity index
nowadays, it was originally proposed in J. Am. Chem. Soc., 118, 6317 (1996) and reviewed in Chem.
Rev., 105, 3842 (2005). There are also a few variants, among them the best one to study  aromaticity
is NICS(1)ZZ, see Org. Lett, 8, 863 (2006) for comparison. For non-planar systems, it is often
difficult to calculate NICS(1)ZZ, in this case you will find the function introduced in Section 3.28.4
extremely useful.
Multiwfn can also scan NICS along a line and thereby plotting one-dimensional NICS curve
map, see Section 3.28.13 for introduction and Section 4.25.13 for examples. Multiwfn can also scan
NICS in a plane and plotting two-dimensional NICS plane map, see Section 3.28.14 for introduction
and Section 4.25.14 for examples. Via the curve and plane maps, considerably richer information
can be obtained compared to simply calculating NICS at specific points, and these analyses are
much more intuitive.
4. ICSS (Iso-chemical shielding surface): The original paper of ICSS is J. Chem. Soc., Perkin
Trans., 2, 1893 (2001). This method analyzes aromaticity by visualizing isosurface of magnetic
shielding value around the molecule. See Section 3.28.3 for introduction and Section 4.25.3 for
examples. The main drawback of this method is that calculating grid data of magnetic shielding
values in a three-dimensional region is fairly time-consuming.
5. HOMA (Harmonic oscillator measure of aromaticity), HOMAc and HOMER: HOMA
measures aromaticity based on bond lengths in the ring of interest. See Section 3.28.6 for
introduction and Section 4.25.6 for example. HOMAc is an improved version of HOMA and more
worth to recommended. HOMA fully fails to characterize aromaticity of T1 state while its variant
HOMER works reasonable for this case. See Section 3.28.7 for introduction of HOMAc and
HOMER.
6. Bird index: The same as above.
7. Multi-center bond order (MCBO): MCBO is an indicator of electron delocalization ability
over a ring and is the aromaticity index I most strongly recommended. Larger MCBO value
corresponds to stronger aromaticity. See Section 3.11.2 for introduction. Some applications of
MCBO in aromaticity studies can be found in J. Phys. Org. Chem., 26, 473 (2013), Phys. Chem.
Chem. Phys., 2, 3381 (2000) and J. Phys. Chem. A, 109, 6606 (2005). It is straightforward to discuss
π and σ aromaticities separately by MCBO, that is before calculating MCBO value, first set
occupation number of all σ and π MOs to zero respectively by subfunction 22 of main function 100.
Note that the definition of MCBO in many literatures differ with that in Multiwfn by a constant
coefficient.
8. ELF-σ/π: The ELF calculated solely based on π orbitals and all other orbitals are referred to
as ELF-π and ELF-σ, respectively. It was argued that the value of bifurcation point of ELF-π (ELF-

1051

--- page break ---

4 Tutorials and Examples
σ) is an indicator of π (σ) aromaticity, some applications can be found in J. Chem. Phys., 120, 1670
(2004), J. Chem. Theory Comput., 1, 83 (2005) and Chem. Rev., 105, 3911 (2005). An example of
calculating ELF-σ/π is given in Section 4.5.3. In addition, Section 4.4.9 presented an example of
studying LOL-π (which is very similar to ELF-π) by plotting plane map. I do not think ELF-σ/π is
a very ideal method for measuring aromaticity, mostly because this method often suffers from
ambiguity (you will recognize this point if you have used this method to study many practical
systems). Also note that the bifurcation values of ELF-σ/π in a lot of literatures are incorrect; if you
try, you will find it is impossible to reproduce their results at all. (So do not always believe literatures
but believe in yourself!)
9. PDI (Para-delocalization index): This aromaticity index is only applicable to sixmembered rings. PDI was first proposed in Chem. Eur. J., 9, 400 (2003) and reviewed in Chem.
Rev., 105, 3911 (2005). Please check Section 3.18.6 for introduction of PDI and Section 4.15.2 for
example of using PDI.
10. ATI (Average two-center indices): ATI was first proposed in J. Phys. Org. Chem., 18, 706
(2005). In fact ATI does not contain any new idea, it simply replaces the delocalization indices
involved in PDI formula with corresponding Mayer bond orders, and according to the discussions
in J. Phys. Chem. A, 109, 9904 (2005), there is no essential difference between Mayer bond order
and delocalization index in physical nature. If you would like to use ATI, you can directly calculate
Mayer bond order by Multiwfn and then manually calculate ATI according to its formula.
11. PLR (Para linear response index): As ATI, PLR is also very akin to PDI. The only
difference between PLR and PDI is that the delocalization indices in PDI are replaced with
corresponding condensed linear response kernels. Original paper of PLR is Phys. Chem. Chem.
Phys., 14, 3960 (2012). You can check Section 3.18.9 for introduction of PLR and Section 4.15.2
for example of using PLR.
12. DI: This method was proposed in Chem. Eur. J., 9, 400 (2003) for measuring aromaticity
of 5-membered systems. Consider below case

The DI is simply defined as the difference of delocalization index (DI) between the formal C=C
bond and the C-C bond. The DI can be either calculated by fuzzy atomic space analysis module or
by basin analysis module (though the definition of atomic spaces are different in these two module,
the results are similar in common). In fact, you can also use Mayer bond order instead of DI. I do
not believe DI is reliable, since aromaticity is an overall property of a system, while the
delocalization over C-X bond is completely ignored in DI.
13. FLU and FLU-π (Aromatic fluctuation index): They were proposed in J. Chem. Phys.,
122, 014109 (2005). See Section 3.18.7 for introduction and Section 4.15.2 for example.
14. RCP properties: In Can. J. Chem., 75, 1174 (1997) it was shown that the density and the
curvature of density perpendicular to the ring plane at ring critical point (RCP) closely relate to
aromaticity of the ring. The larger the density, or the more negative the curvature, the larger the
aromaticity. You can use topology analysis module of Multiwfn to apply this method. Detail
introduction can be found in Section 3.14.6, an example is given in Section 4.2.1.
15. Shannon aromaticity: This method was proposed in Phys. Chem. Chem. Phys., 12, 4742

1052
