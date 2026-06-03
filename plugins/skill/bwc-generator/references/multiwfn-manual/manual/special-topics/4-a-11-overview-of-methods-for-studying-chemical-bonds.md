# 4.A.11 Overview of methods for studying chemical bonds

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.11 Overview of methods for studying chemical bonds
- pdf pages: 1098-1104
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.11, studying, chemical, bonds

## Extracted manual text

4 Tutorials and Examples
H

0.00000000

2.14896316

-3.05414223

0.000000

H

0.00000000

-0.00000000

-4.31975533

0.000000

We use Multiwfn to load this .chg file and convert it to N-phenylpyrrole.pqr, then use VMD to
visualize it using exactly the same procedure described in last example. However, this time the lower
and upper limit of color scale should be set to -50 and 50, respectively. The resulting graph is shown
as left part of below map; as a comparison, the corresponding isosurface map of MO36 is shown at
right part.

The more red the atom, the greater its contribution to the orbital. As can be seen, the atomic
coloring introduced in this section well reflects actual orbital distribution. For very large molecules,
the isosurface map may become quite complicated, while the atomic coloring map should be much
clearer.
Of course, the atomic coloring method is also applicable to other kinds of atomic properties
calculated by Multiwfn, such as condensed Fukui function, atomic spin population, atomic transition
charge, source function of atoms, integral of electron energy in atomic space, variation of atomic
charge during electronic transition or intermolecular interaction. More examples can be found in my
blog article http://sobereva.com/425 (in Chinese).

4.A.11 Overview of methods for studying chemical bonds
Note: Chinese version of this Section is my blog article “An overview of chemical bond analysis methods
supported by Multiwfn” (http://sobereva.com/471), which also contains extended discussion.

In this section, I present an overview of all methods that may be used to study chemical bonds.
You will find Multiwfn is indispensably useful in characterizing and unraveling nature of the bonds.
Most analyses can be applied to both ground state and excited states (see Section 4.18.13 for more
information about this point)
1 AIM (Atoms-in-molecules) analysis
In the framework of AIM, the bond critical point (BCP) is the most representative point of a

1077

--- page break ---

4 Tutorials and Examples
bond, hence character of chemical bonds can be characterized by various properties at
corresponding BCPs, for example:
·Electron density and potential energy at BCP, namely (BCP) and V(BCP), are often used
to discuss bonding strength. For the same kind of bond, they are usually positively and negatively
correlated to bonding strength, respectively.
·Laplacian of electron density at BCP, namely 2(BCP), is often used to judge whether or
not a bond mainly shows covalent character. Negative and positive values imply that the major
nature of the bond is covalent and non-covalent, respectively. But notice that this criterion is often
wrong (e.g. CO has positive 2(BCP) but it is evidently a polar covalent bond)
·In Angew. Chem. Int. Ed. Engl., 23, 627 (1984) it was argued that negative and positive
values of energy density at BCP, i.e. H(BCP), implying the bond has covalent and non-covalent
nature, respectively. But this criterion is not always true; for example, the Ca-O in CaO is typical
ionic bond, but its H(BCP) is negative.
·The V(BCP)/G(BCP) was proposed in J. Chem. Phys., 117, 5529 (2002), where G(BCP)
denotes Lagrangian kinetic energy density at BCP. It was argued that <0, >1 but <2, >2 of this
quantity respectively imply that the bonding mainly belongs to close-shell interaction, intermediate
(mixed) interaction and covalent interaction.
·The eta index was proposed in J. Phys. Chem. A, 114, 552 (2010) and further studied in
Angew. Chem. Int. Ed., 53, 2766 (2014), it is defined as |1(r)|/3(r), where 1 and 3 are the smallest
and largest eigenvalues of Hessian matrix of electron density, respectively. It was argued that if eta
index at BCP is smaller than 1, then the bonding should be closed-shell interaction; while if it is
larger than 1, the interaction should has covalent nature, and the more positive the value, the stronger
the covalent character. However I found this argument is not always true, for example this quantity
of both Ni-C and C-O bonds in Ni(CO)4 is less than 1, but undoubtedly they should be attributed to
polar covalent bonds.
·Bond degree (BD) was proposed in J. Chem. Phys., 117, 5529 (2002) and defined as
H(BCP)/(BCP). The physical meaning of BD is energy density of unit electron at BCP. For
covalent interaction (usually H(BCP)<0), the more negative the BD, the stronger the bonding; while
for non-covalent interaction (usually H(BCP)>0), the more positive the BD, the weaker the
interaction.
·Bond ellipticity was proposed in J. Am. Chem. Soc., 105, 5061 (1983) and defined as (r)=
[1(r)/2(r)]-1. At BCP the 1 and 2 must be negative and exhibit the curvature of electron density
perpendicular to the bond. The larger the value deviates to 0, the stronger the tendency that the
electron density has unsymmetric distribution in the plane perpendicular to the bond at the BCP.
·The source function with BCP as reference point has been employed to study chemical bonds,
see Struct. & Bond., 147, 193 (2010) for comprehensive review and Section 4.17.5 for analysis
example.
There are some other research papers utilized BCP properties to discuss chemical bonds, for
example J. Am. Chem. Soc., 120, 13429 (1998) and J. Comput. Chem., 39, 1697 (2018). It is worth
to note that properties at certain critical points can also be used to estimate metallicity of crystals,
see J. Am. Chem. Soc., 124, 14721 (2002), Chem. Phys. Lett., 471, 174 (2009) and J. Phys.: Condens.
Matter, 14, 10251 (2002).
The bond path is also a very important concept in the AIM framework, it rigorously reveals the
main interaction path connecting various atoms. It is important to understand the fact that chemical

1078

--- page break ---

4 Tutorials and Examples
bond must be accompanied by a bond path and BCP, while present of bond path and BCP does not
imply existence of chemical bond.
AIM topology analysis has been systematically introduced in Section 3.14 and illustrated in
Section 4.12.1, all above mentioned quantities can be easily and rapidly evaluated by Multiwfn. It
is noteworthy that in Multiwfn many real space functions at critical point (or specific point) can be
decomposed as contributions from various orbitals (usually MOs or LMOs), see Section 4.2.4 for
example; moreover, any real space function can be plotted along bond paths, see Section 4.2.3 for
example. These useful features can often provide much deeper insight into the bonding.
The above mentioned real space functions can also be plotted as curve map, plane map or
isosurface map so that one can visually study their distribution, see Section 4.3, 4.4 and 4.5 for
practical example, respectively. The contour line map and isosurface map of 2(r) is particularly
useful and frequently employed.
2 Bond order and delocalization index analysis
Bond order is a very useful and straightforward way of characterizing chemical bonds.
Multiwfn supports a lot of bond order definitions, please check Section 3.11 for detailed introduction.
Different bond orders have different characters and physical meanings. For example, Laplacian
bond order (LBO) measures covalent component of a bond and usually has good relationship with
bond dissociation energy (BDE), while Mayer bond order essentially reflects the number of
electrons shared by two interacting atoms. The bond analysis module of Multiwfn is also able to do
more things than just calculating the value of bond order. For example, Multiwfn can decompose
some bond orders as contributions from various orbitals, the Wiberg bond order can be decomposed
as contributions from atomic orbital pairs. Many detailed analysis examples of bond order are given
in Section 4.8.
It is worth to mention that the delocalization index (DI) is essentially equivalent to Mayer bond
order and fuzzy bond order in physical nature. The difference arises from how the atomic spaces are
defined. For nonpolar bonds, the DI is usually very close to Mayer and Fuzzy bond orders, but they
quantitatively differ for polar bonds. For DI, the AIM atomic basins are employed as atomic spaces.
The DI can be calculated via basin analysis module, see Section 3.18.5 for detailed introduction of
DI and Section 4.17.1 for DI analysis example. Commonly I do not suggest employing DI, because
its cost is by far higher than evaluation of Mayer and Fuzzy bond orders.
For the same kind of chemical bond, e.g. C-O bond in different transition metal coordinates,
the Mayer bond order and LBO are positively correlated with bonding strength. For different kinds
of chemical bonds, Mayer bond order should not be used to compare bonding strength, for example,
the bond in N2 and that in P2 has evidently different BDEs, but they have basically the same Mayer
bond order because both of them are typical triple bonds. In contrast, the LBO is able to faithfully
reflect that the bond in P2 is much weaker than N2. Please check original paper of LBO (J. Phys.
Chem. A, 117, 3100 (2013)) for more discussions, comparisons and examples.
Plotting variation of bond order versus reaction coordinate is an absolutely very good idea to
shed light on the underlying change of electronic structure in a chemical reaction, see Section 4.A.1
on how to easily realize this.
3 Bond order density and natural adaptive orbital analysis
The concept of bond order density (BOD) and natural adaptive orbital (NAdO) has been
introduced in detail in Section 3.200.20, they are fairly useful if you want to graphically discuss
bond order of a given covalent bond. The BOD is a real space function representing everywhere

1079

--- page break ---

4 Tutorials and Examples
contribution to bond order (strictly speaking, delocalization index in the present context), while
NAdO unveils nature of bond order in terms of orbitals. See Section 4.200.20 for application
examples, you will find this method particularly useful in many situations.
4 Orbital localization analysis
Molecular orbitals (MOs) commonly are unable to be used to study bonding character because
they are highly delocalized and do not directly correspond to chemical bonds. The orbital
localization is a very powerful technique, it can transform the MOs to localized molecular orbitals
(LMOs), which are highly localized and have very close relationship with bonding. Via LMOs,
numerous useful information about chemical bonds can be extracted, such as bond polarity, bond
multiplicity, bond type, the atomic orbitals that participate in the bonding and so on. Please check
Section 3.22 for detailed introduction of LMOs and follow the LMO analysis examples in Section
4.19.
5 AdNDP analysis
The purpose of the adaptive natural density partitioning (AdNDP) method is somewhat similar
to orbital localization method, the advantage of AdNDP is that it is also able to derive orbitals with
semi-delocalization character from the complicated multi-electron wavefunction. If the AdNDP
analysis has been properly performed, then the resulting orbitals will faithfully reveal all multicenter bonds in current systems. The drawback of AdNDP analysis is that user must manually pick
out orbitals from candidate list, this process is slightly troublesome and requires the user has
adequate chemical intuition. When there is no multi-center bond, using orbital localization is much
preferred over AdNDP because it is fully automatic, fast and free of subjectivity; while if you suspect
that present system may have evident multi-center bonds and you want to study them, commonly
AdNDP is the only choice. The AdNDP method is detailedly introduced in Section 3.17, related
examples are provided in Section 4.14.
6 Analysis of ELF and relevant real space functions
ELF is a very important real space function, it is able to reveal localization and delocalization
of electrons in chemical systems. Brief introduction of ELF can be found in Section 2.6. In Multiwfn
ELF can be analyzed in many different ways, as shown below
·Visualizing study. In Multiwfn, the ELF can be drawn as curve map by main function 3,
drawn as plane map by main function 4 and plotted as isosurface map via main function 5, see
Sections 4.3, 4.4 and 4.5 for examples, respectively. From the graphs one can easily identify which
region contains evident covalent interaction (i.e. evident share of electrons) and judge the nature of
a given chemical bond. In addition, the bond multiplicity can be inferred from the shape of ELF
isosurface around the bond. Multiwfn is also capable of studying ELF- and ELF-, so that 
interaction and  interaction can be studied separately, see Sections 4.5.3 and 4.100.22 for example.
Note that there are a lot of real functions having analogous distribution feature as ELF, though
their underlying ideas may be not very similar to ELF. Multiwfn supports most of them and they
can also be plotted in the exactly the same way as ELF. These real space functions include LOL,
SCI, SEDD, RoSE, PS-FID. The LOL is introduced in Section 2.6 and sometimes preferred over
ELF because of its clearer graphical effect; introduction of other real space functions can be found
in Section 2.7.
The negative part between two atoms in 2 map is able to reveal the region where electrons
concentrate due to formation of covalent bond, this point is similar with ELF. In J. Phys. Chem.,

1080

--- page break ---

4 Tutorials and Examples
100, 15398 (1996), Bader believes that 2 and ELF are homeomorphic and their similarities and
differences are able to provide complementary information in understanding chemical bonds.
However, notice that for bonds involving very heavy atom, 2 map often fails to reveal covalent
character. For example, 2 in the interacting region of Re-Re bond is entirely positive.
By using Multiwfn and shell script as well as third-part software, anime of ELF or other
functions during a chemical process (often represented as trajectory resulting from intrinsic reaction
coordinate or rigid scan tasks) can be easily generated, such an anime is able to very vividly exhibit
variation of characters of chemical bonds, see Section 4.A.1 on how to make the anime.
·Basin analysis of ELF (or similar functions): This kind of analysis can be carried out via
basin analysis module (main function 17), see Section 4.17.2 for example. All ELF basins
collectively make up the whole space, each ELF basin corresponds to a region with featured
electronic structure. For example, the ELF basins may correspond to covalent bond, lone pair, core
region, etc. By analyzing character of bond basins, one can acquire many information about the
bonds, such as average number of electrons that occurs in the bonding region, degree of electron
localization in the bonding region, dipole moment of the bonding region. Contribution of each atom
to the electron population in the bonding region can also be obtained, as illustrated in Section 4.17.7.
·Topology analysis of ELF (or similar functions): This kind of analysis allows one to obtain
accurate position of ELF maximum (also known as ELF attractor) and (3,-1) type of ELF critical
point (also known as ELF bifurcation point), the former displays the most representative point of a
ELF basin, while the value of the latter somewhat reflects the extent of electron sharing (degree of
delocalization) between two ELF basins. Topology analysis of ELF can be realized via main
function 2, see Section 4.2.2 for example. For practical studies based on topology analysis of ELF
and LOL, see Nature, 371, 683 (1994) and J. Comput. Chem., 30, 1093 (2009). Tracing variation of
ELF attractors is particularly useful for understanding the change in electronic structure and bonding
character during a chemical process, illustrative examples of such analysis are RSC Adv., 5, 62248
(2015), Chem. Phys., 501, 128 (2018) and Comput. Theor. Chem., 1154, 17 (2019).
Note that basin analysis is also able to give positions of ELF attractors, the procedure is even more simple than
using topology analysis module, however the accuracy of the positions given by basin analysis module is not as good
as topology analysis module, since basin analysis is carried out based on even-distributed grids.

7 IRI analysis
Compared to ELF/LOL, the unique advantage of the interaction region indicator (IRI) defined
by Tian Lu is able to clearly reveal all kinds of interactions in a chemical system, including both
covalent and non-covalent ones. In the original paper of IRI it is demonstrated that IRI can even
perfectly visually represent variation of bonding during a whole chemical reaction. See Section
3.23.8 for introduction of IRI, related analysis examples are given in Section 4.20.4.
In the same paper of IRI, its variant named IRI- was also proposed, it is shown that it can well
distinguish type and strength of  interaction on different chemical bonds, many examples can be
found in its original paper.
A very detailed document showing how to perform IRI and IRI- analyses is given in
http://sobereva.com/multiwfn/res/IRI_tutorial.zip. Note that DORI is another function that has
similar capacity with IRI, however its graphical effect is obviously not as good as IRI and its
definition is too complicated compared to IRI.
8 Analysis of valence electron density
As clearly illustrated in my paper Acta Phys. -Chim. Sin., 34, 503 (2018) DOI:
10.3866/PKU.WHXB201709252, visualizing electron density of valence electrons is a very useful,

1081

--- page break ---

4 Tutorials and Examples
powerful and intuitive way of revealing electron structure and studying character of chemical bonds,
please carefully read this paper. Moreover, basin analysis can be applied to valence electron density
to unveil more information of chemical interest. See Section 4.6.2 for example on how to carry out
this kind of analysis.
9 Electron density difference analysis
Formation of chemical bond always leads to significant electron reorganization (polarization
and charge transfer), in particular, formation of covalent bond must be accompanied with the
phenomenon that electrons concentrate to the bonding region. Plotting electron density difference
(EDD) map is one of the best ways to reveal this point, EDD can be very easily plotted as curve
map, plane map and isosurface map in Multiwfn via main functions 3, 4 and 5, respectively. EDD
can be defined in different ways, if you want to study the bond formed between two fragments, you
should study the EDD between the whole system and the two fragments, see Section 4.5.5 for
example; if you want to study the reorganization of electron density due to forming bonds between
various atoms in the system, you should study deformation density, which is defined as the
difference between the electron density of the whole system and all atom in their isolated states, see
Section 4.4.7 for example.
Do not forget that Multiwfn also provides advanced technique for analyzing EDD, for example,
basin analysis can be applied to EDD, see Section 4.17.4 for example. Also one can plot charge
displacement curve to better quantitatively study electron reorganization along specific direction,
see Section 4.13.6 for example.
It is worth to note that plotting difference map of ELF between whole system and its fragments
is also valuable, see illustration in Section 4.4.8.
10 Analysis of g function and IBSI index
The real space function g is defined in the framework of IGM theory, see Section 3.23.5 for
introduction. g is capable of revealing all kinds of interactions, including both chemical bonding
and weak interaction, as well as both covalent and non-covalent ones. Moreover, the magnitude of
g in the bonding region is often positively correlated with bonding strength, therefore one can
easily examine bonding strength in different regions by inspecting colors in color-filled map or by
properly adjusting isovalue in isosurface map. In addition, the isosurface of g can be mapped by
sign(2) function via various colors, this makes the isosurface map informative. Please check
Sections 4.20.10 and 4.20.11 for IGM examples; although the examples focus on studying weak
interactions, the same procedure can also be migrated to chemical bond analysis.
The intrinsic bond strength index (IBSI) is defined based on integral of g over the whole space.
In J. Phys. Chem. A, 124, 1850 (2020) is was shown that it has the ability to measure bonding
strength and distinguish type of bonds to a certain extent, see Section 3.11.9 for introduction and
Section 4.9.6 for example.
11 Quantifying amount of charge transfer due to bonding
Formation of chemical bond between two different fragments must result in detectable charge
transfer (CT) between the two fragments. The amount of CT can be obtained as difference between
the fragment charge in actual system and the net charge of the fragment in its isolated state. The
fragment charge is defined as sum of charges of the atoms in the fragment. In the population analysis
module of Multiwfn, if a fragment has been defined, the fragment charge will be directly outputted
when calculating atomic charges, see Section 4.7.1 for example.

1082

--- page break ---

4 Tutorials and Examples
12 Charge decomposition analysis (CDA)
Using fragment charge we can easily discuss the total amount of CT, however, in order to
examine details of charge transfer due to bonding, the CDA must be employed. CDA is able to
explicitly show electron donation and back-donation between each pair of user-defined fragments
at resolution of orbital interactions, also it provides clear information about how the MOs of the
entire system is composed of MOs of individual fragments. See Section 3.19 for introduction and
Section 4.16 for example.
13 Extended Transition State - Natural Orbitals for Chemical Valence (ETS-NOCV)
This popular method was proposed in J. Chem. Theory Comput., 5, 962 (2009), it focuses on
deciphering orbital interaction between fragments. The key advantage of this analysis is that it is
able to transform variation of electron density due to orbital interaction to a set of NOCV pairs, each
pair has corresponding energy contribution to orbital interaction energy and has corresponding
density which can be visualized to easily understand nature, therefore ETS-NOCV analysis provides
very deep insight into orbital interaction. See Section 3.26 for detailed introduction of this analysis
and Section 4.23 for examples of applying ETS-NOCV on studying various kinds of interactions.
14 Density-of-states (DOS) analysis
The partial DOS (PDOS) curve map is useful for intuitively exhibiting bonding and antibonding due to interaction between user-defined fragments (may be defined as a batch of atoms,
shells or atomic orbitals) at various energy range, see Section 3.12 for introduction and Section
4.10.1 for example.
15 Energy decomposition analysis
Energy decomposition analysis is used to decompose the bond energy to different physical
components to provide deeper insight into the bonding nature. The “simple energy decomposition”
supported by Multiwfn can be applied to chemical bonds, please check Section 4.100.8 for example.
You need Gaussian to use this function.
16 Studying bond polarity
It is often interesting to study polarity of a bond, there are several possible ways, as shown
below. The results often differ significantly, since the concept itself cannot be uniquely defined.
·Calculating respective contribution from the two bonding atoms to the LMO corresponding
to the bond (A and B), then the ionicity of the bond can be evaluated as |A-B|. Clearly the larger
this value, the higher the bond polarity. To obtain A and B, you should first perform orbital
localization, then find the LMO corresponding to the bond in main function 0, and finally use main
function 8 to evaluate composition of the LMO via proper method.
·Firstly evaluating respective contribution from the two bonding atoms to the population
number of the ELF basin corresponding to the bond, as illustrated in Section 4.17.7, then take the
difference of the two contribution values to estimate the bond polarity.
·Calculating bond polarity index. See Section 3.200.12 for introduction and 4.200.12 for
example.
· It is worth to note that Laplacian bond order only reflects covalent component of a bond,
while Mayer bond order may be regarded as total bond order. Therefore, in certain cases, the
difference between Laplacian and Mayer bond orders may be used to reveal bond polarity.
17 Studying bond dipole moment

1083
