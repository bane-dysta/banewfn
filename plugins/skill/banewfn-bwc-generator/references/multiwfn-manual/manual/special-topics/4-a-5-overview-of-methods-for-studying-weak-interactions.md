# 4.A.5 Overview of methods for studying weak interactions

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.5 Overview of methods for studying weak interactions
- pdf pages: 1076-1079
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.5, studying, weak, interactions

## Extracted manual text

4 Tutorials and Examples
condensed version is using main function 22, as introduced in Section 3.25 and illustrated in Section
4.22.1. An additional advantage is that many other important quantities defined in the framework of
conceptual density functional theory can be obtained together without any additional cost, including
Mulliken electronegativity, hardness, electrophilicity and nucleophilicity indexes, softness,
condensed local softness, relative electrophilicity and nucleophilicity and so on, which are also quite
useful for studying reactivity problems.
7 Orbital-weighted Fukui function and orbital-weighted dual descriptor: They are special
form of Fukui function and dual descriptor, the unique advantage of this orbital-weighted form is
able to reasonably deal with systems with degenerate or nearly degenerate frontier molecular
orbitals, such as C60, coronene and cyclo[18]carbon, usually these kinds of system have high point
group symmetry. See Section 3.25.3 for introduction and 4.22.2 for illustrative application.
8 Fukui function and dual descriptor for (quasi-)degenerate HOMO and LUMO case:
The purpose of this special form of Fukui function and dual descriptor is similar with the orbitalweighted form, but it is defined fully based on electron density and hence more physically rigorous.
See Section 3.25.4 for introduction and Section 4.22.3 for example.
9 Orbital overlap distance function. Analysis of this function may be useful for revealing
favorable reactive site, see Section 4.12.8 for example.

4.A.5 Overview of methods for studying weak interactions
There are a lots of ways to characterize weak interactions, and most of them are supported by
Multiwfn, here I give you a brief summary. If you can read Chinese, I suggest reading my blog
article "An overview of the weak interaction analysis methods supported by Multiwfn"
(http://sobereva.com/252), in which this topic is discussed more deeply and extensively.
(1) AIM topology analysis is a very popular method for studying both strong and weak
interactions. Its use in weak interaction analysis is partially illustrated in Section 4.2.1.
(2) NCI analysis proposed in 2010 may be viewed as an visualization extension of AIM
analysis, this method rapidly became quite popular after it was proposed. The example of using NCI
analysis is given in Sections 3.23.1, 4.20.1 and 4.20.2. NCI analysis is also able to be employed to
study weak interaction in dynamic environment such as molecular dynamic simulation, this is
known as averaged NCI (aNCI) analysis, see the introduction in Section 3.23.2 and the accompanied
example in Section 4.20.3. Integrating domain of NCI is a useful way to discuss weak interactions
quantitatively, examples are provided in Section 4.200.14.
IRI and DORI analysis is closely related to the NCI method. Advantage of IRI and DORI is
that all kinds of interactions can be simultaneously visualized, including both chemical bonds and
weak interactions, as illustrated in Sections 4.20.4 and 4.20.5. Relatively speaking, the IRI is
evidently preferred over DORI, since the graphical effect of IRI is much better and computational
cost is lower. A detailed comparison between IRI, DORI and NCI can be found in original paper of
IRI: Chemistry−Methods, 1, 231 (2021).
(3) IGM analysis. A key advantage of IGM analysis compared to NCI analysis is that this
method is able to visually study intrafragment and interfragment interaction regions separately by
properly defining fragments. Contributions by atoms and atomic pairs can be quantified as atomic
g index and atomic pair g index defined in the IGM framework, respectively, and atoms can be
colored according to the atomic g indices to vividly exhibit the role played by various atoms. Three

1055

--- page break ---

4 Tutorials and Examples
forms of IGM are supported, namely the original IGM, and the IGMH and mIGM proposed by me,
see Sections 3.23.5, 3.23.6, 3.23.10 for introduction, respectively, as well as see Sections 4.20.10,
4.20.11, 4.20.12 for examples, respectively. Graphical effect of IGMH is much better than IGM, but
the computational cost is evidently higher. mIGM has similar graphical effect as IGMH, while the
cost is the same as the original IGM, so the original IGM in my opinion is no longer useful.
I also extended IGM analysis to the case of molecular dynamics simulation and proposed a
new form of IGM, namely averaged IGM (aIGM), which is able to represent averaged interaction
between specific fragments in the simulation trajectory. A variant of aIGM is amIGM, the latter has
significantly better graphical effect, so amIGM should always be used instead of aIGM. See Section
3.23.11 for introduction of amIGM and 4.20.13 for example.
(4) Electrostatic potential (ESP) analysis. ESP has been introduced in Section 2.6, this is an
extremely important real space function for studying electrostatic dominated weak interactions.
There are many different ways to carry out ESP analysis:
·Visually studying ESP color-mapped molecular vdW surface, this analysis can be used to
quickly figure out potential electrostatic interaction sites and qualitatively study interaction strength.
See the end of Section 4.12.1 and J. Mol. Model., 13, 291 (2007) for example.
·Studying ESP minima and maxima on molecular vdW surface. This can done by quantitative
molecular surface analysis module, see Section 4.12.1 for example and Section 3.15 for more details.
The value of these ESP extrema on vdW surface strongly correlate with electrostatic interaction
energies, and you can find many papers have used this method, for example Phys. Chem. Chem.
Phys., 15, 14377 (2013), J. Mol. Model., 13, 305 (2007), Int. J. Quantum. Chem., 107, 3046 (2007),
Phys. Chem. Chem. Phys., 12, 7748 (2010), J. Mol. Model., 14, 659 (2008), J. Mol. Model., 18, 541
(2012), J. Mol. Model., 15, 723 (2009), Chapter 6 of book Practical Aspects of Computational
Chemistry (2009).
·Studying area and averaged ESP value corresponding to characteristic region, such as hole, -hole and lone pair, see Section 4.12.10 for example.
·Superposition analysis of ESP contour map. This method was proposed by Tian Lu in J. Mol.
Model., 19, 5387 (2013), it is quite vivid, easy-to-use and powerful. It was demonstrated that
stability of complex configurations can be fairly well predicted by this method. Section 4.4.4
showed how to plot ESP contour map.
·In J. Phys. Chem. A, 118, 1697 (2014), the authors showed that by making use of ESP at
nuclear positions the electrostatic dominated intermolecular interaction energies can be very
accurately predicted. See Section 4.1.2 for introduction and example.
(5) van der Waals (vdW) potential. The vdW potential has same importance as ESP, especially
for the case that the interaction is dominated by vdW interaction rather than electrostatic interaction.
Multiwfn is able to easily perform vdW potential analysis in various forms. See Section 3.23.7 for
introduction and Section 4.20.6 for example. An in-depth introduction and discussion of vdW
potential can be found in my research paper J. Mol. Model., 26, 315 (2020)
(6) Atomic charge analysis. Atomic charge is a very simple and intuitive model for describing
charge distribution and can be used to analyze the strength of electrostatic interaction between
different sites. The functions for calculating atomic charges are introduced in Section 3.9, and some
practical examples are given in Section 3.7.
(7) Hirshfeld and Becke surface analysis. This kind of analysis is extremely useful for revealing
weak interaction in molecular crystals, but can also be applied to molecular clusters, see examples

1056

--- page break ---

4 Tutorials and Examples
in Sections 4.12.5 and 4.12.6 as well as theory introduction in Section 3.15.5.
(8) Bond order and delocalization index (DI) analysis. Commonly weak interactions are
dominated by electrostatic and/or vdW interactions, so bond order and DI analysis, which mainly
reflect covalent character are often not useful in these cases. However, for "strong" weak interactions,
such as low-barrier hydrogen bonds (LBHB) and charge-assisted halogen bonds, covalent
contribution may be not negligible, and thus bond order and DI analysis can be applied. Bond order
calculations are illustrated in Section 4.9. In Multiwfn, DI can be calculated based on fuzzy atomic
space or AIM basin, the former is equivalent to fuzzy bond order, while the latter can be evaluated
in basin analysis module, see example in Section 4.17.1.
(9) ELF analysis. In Theor. Chem. Acc., 104, 13 (2000), Fuster and Silvi defined CVB index
based on ELF to distinguish strength of H-bonds. J. Phys. Chem. A, 115, 10078 (2011) employed
this method to study a large amount of resonance-assisted hydrogen bonds and find this index is in
good correlation with other H-bond strength indices. CVB index can be easily calculated in
Multiwfn, see Section 3.200.1 for detail. There are also other papers using ELF to study H-bonds,
e.g. Chem. Rev., 111, 2597 (2011).
(10) Charge variation analysis. Weak interactions often accompanied by charge transfer and
polarization, therefore studying how the electrons are transferred between or within molecules, as
well as how the electron density is polarized due to the presence another molecule are important.
There are many available ways to investigate these points:
·Plotting difference map of electron density between complex and monomers. This is the
most straightforward and intuitive way to study variation of electron density. The procedure is
illustrated in Section 4.5.5.
·Plotting charge displacement curve. After generating grid data of density difference, in order
to quantitatively study the charge variation in a direction, you can plot charge displacement curve,
see Section 3.16.14 for introduction and Section 4.13.6 for example.
·Variation of atomic charges of monomers in their isolated states and in complex state can
quantitatively and clearly show how the electrons are transferred between different atoms/fragments
due to the interaction.
·After generating grid data of electron density difference between complex and monomers,
you can use basin analysis module to integrate basin of density difference to study amount of
electron variation in various characteristic regions (e.g. the region corresponding to σ-hole). You
can consult the example in Section 4.17.4.
·Charge decomposition analysis (CDA). CDA is used to reveal underlying details of charge
transfer, the amount of donation and back-donation of electrons between two fragments due to
various complex MOs can be studied. In addition, the CDA module of Multiwfn can tell you how
the fragment MOs are mixed and hence yield complex MOs. CDA is commonly applied to strong
interaction, but it may be also useful for exploring weak interactions. The theory of CDA is
introduced in Section 3.19, practical examples are given in Section 4.16.
·Multiwfn has a function dedicated to analyze charge transfer in electron excitation based on
electron density difference, many important quantities characterizing the transfer can be obtained,
see Section 3.21.3 for introduction and 4.18.3 for example. Based on the grid data of electron density
difference between complex and monomers, this function may be also useful for studying charge
transfer due to weak interaction.
(11) Mutual penetration distance of vdW surfaces. For the same kind of weak interaction,

1057

--- page break ---

4 Tutorials and Examples
generally the larger the penetration of vdW surface, the stronger the interaction strength. For a noncovalently interacting atomic pair AB, the difference between the distance of A-B and the sum of
their non-bonded radii is termed as mutual penetration distance. The non-bonded atomic radius is
the closest distance between a nucleus and the molecular vdW surface, and can be obtained by
option 10 in post-processing interface of quantitative molecular surface analysis module of
Multiwfn.
(12) Energy decomposition analysis is a kind of important approaches for characterizing the
nature of weak interactions, physical components of total interaction energy can be separately
obtained. Multiwfn is capable of performing simple energy decomposition analysis in combination
with Gaussian to provide deeper insight into weak interactions, see Section 4.100.8 for example.
Multiwfn can also perform energy decomposition analysis based on molecular forcefield, this
function is very useful, flexible can be used to evaluate/decompose the weak interaction energy for
very large systems, see Section 3.24.1 for introduction and Section 4.21.1 for example.
(13) LOLIPOP index is useful for measuring π-π stacking ability, see the introduction in
Section 3.100.14 and the example in Section 4.100.14.
(14) Source function analysis is defined in the framework of AIM theory. Gatti et al. suggested
using source function to study both strong and weak interactions. Introduction of source function
can be found in Section 2.6, and tutorial of performing source function analysis is given in Section
4.17.5. A thorough review is Struct. & Bond., 147, 193 (2010), in which H-bond analysis is involved.
(15) Atomic multipole moment analysis. The definition of atomic multipole moment can be
found in Section 3.18.3. Atomic multipole moment measures the anisotropy distribution of electron
density around an atom, which has important impact on interatomic electrostatic interactions. See
Section 7.4.3 of the Bader's book Atoms in molecules-A quantum theory for illustrative examples.
In Multiwfn, atomic multipole moment can be calculated by both fuzzy space analysis module and
basin analysis module, for the latter case see Section 4.17.1 for example.
(16) Orbital overlap. For weak interactions involving orbital interaction, you can use Multiwfn
to study orbital overlap, which is closely related to orbital interaction strength. The example Section
in 4.100.15 illustrated how to calculate intermolecular orbital overlap integral. Section 4.0.2
exemplified how to visualize overlap degree of two NBO orbitals, high (low) overlap degree
commonly implies large (small) second-order perturbation energy E(2) between the two NBOs.
(17) As demonstrated in J. Mol. Model., 19, 2035 (2013), interaction energy of halogen-bond
complexes is well correlated with the properties of (3,-1) critical point of Laplacian of electron
density at σ-hole location. The topology analysis of Laplacian of electron density can be
conveniently realized in main function 2. Section 4.2.2 showed how to perform topology analysis
for LOL, you can use the same method to analyze Laplacian of electron density.
(18) The cubic electrophilicity index defined in conceptual density functional theory
framework has close relationship with strength of weak interaction energy. In J. Phys. Chem. A, 124,
2090 (2020) it is shown that condensed form of cubic at halogen atom in halogen bond dimers has
nice linear relationship with binding energy, therefore this quantity may be useful in predicting
strength of interaction and revealing interaction nature in some cases. This quantity can be
calculated in a fully automatic way via main function 22, see Section 3.25 for detail.
(18) ETS-NOCV. This popular method was proposed in J. Chem. Theory Comput., 5, 962
(2009), it focuses on deciphering orbital interaction between fragments. The key advantage of this
analysis is that it is able to transform variation of electron density due to orbital interaction to a set

1058
