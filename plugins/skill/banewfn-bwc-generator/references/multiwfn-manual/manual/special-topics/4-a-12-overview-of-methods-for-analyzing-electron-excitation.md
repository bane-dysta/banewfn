# 4.A.12 Overview of methods for analyzing electron excitation

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.12 Overview of methods for analyzing electron excitation
- pdf pages: 1105-1110
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.12, analyzing, electron, excitation

## Extracted manual text

4 Tutorials and Examples
There are three possible ways in Multiwfn:
·Calculate bond dipole moment based on two-center localized molecular orbitals, please
check introduction in Section 3.22 and example in Section 4.19.4.
·Perform ELF basin analysis and check dipole moment of the basin corresponding to the bond
of interest. See Section 4.17.2 for illustration. At the same time, quadrupole moment of the bond
basin can also be obtained.
·Calculating bond dipole moment in Hilbert space. See Section 3.200.2 for introduction
18 Potential acting on one electron in a molecule (PAEM) analysis
The PAEM refers to the total potential acting on an electron at a point. By analyzing PAEM at
proper positions between two atoms, the nature (covalent or non-covalent) can be determined. See
Section 4.3.3 for illustration.

4.A.12 Overview of methods for analyzing electron excitation
In this section, I present a systematic overview of all methods supported by Multiwfn that can
be used for analyzing electron excitation problems.
Note: Chinese version of this section corresponds to my blog article “An overview of electron excitation
analysis methods supported by Multiwfn” (http://sobereva.com/437).

1 Hole-electron analysis
All kinds of excitations can be essentially described as "hole-to-electron" transition, that is,
"hole" is the region where the excited electron leaves, and "electron" is the region where the excited
electron eventually goes. Hole-electron analysis corresponds to subfunction 1 of main function 18,
see Section 3.21.1 for introduction and 4.18.1 for illustration. This analysis is very powerful and
universal and is an almost indispensable analysis method for all kinds of electron excitation
problems. Specifically, the hole-electron analysis has below capacities:
·Displaying isosurfaces of hole and electron. From this picture, one can intuitively understand
how electrons are excited
·Transforming the hole and electron distributions to a form described by Gaussian function,
making them significantly easier to examine visually
·Calculating quantitative indices that measure characteristics of electron excitation, including
the Sr index, which measures the degree of overlap of hole and electron; the D index, which
measures the distance between hole and electron centroids; the  index, which measures the breadth
of hole and electron distributions; the t-index, which measures degree of separation of hole and
electron, and so on.
·Plotting density difference map, which corresponds to subtracting hole from electron
·Calculating contribution of basis functions, atomic orbitals, atoms, molecular fragments and
molecular orbitals to hole and electron, so that the nature of hole and electron can be thoroughly
analyzed. Moreover, amount of hole and electron as well as hole-electron overlapping extent on
various atoms and fragments can be directly displayed as heat map (color-filled matrix map), which
is very convenient for visual horizontal comparison.
·Calculating Coulomb attraction between hole and electron, which is a common definition of
exciton binding energy.
2 Nature transition orbital (NTO) analysis

1084

--- page break ---

4 Tutorials and Examples
When doing electron excitation calculations, it is often found that many orbital transitions have
negligible contribution to electron excitation, this phenomenon makes viewing orbitals to discuss
electron excitation characteristics difficult, and in this case it is necessary to examine multiple
orbitals simultaneously. After transforming the molecular orbitals to NTOs using subfunction 6 of
main function 18, for most cases the electron excitation can be solely described by only one pair of
NTO transition, thus making the discussion much simpler. See Section 3.21.6 for introduction of
NTO analysis and Section 4.18.6 for practical example.
3 Λ index and Δr index
The Λ index proposed in 2008 may be the earliest index to quantitatively examine characteristic
of electron excitations, its intrinsic physical meaning is a measure of degree of overlap between
electron and hole. The Δr proposed in 2013 is another index for characterizing electron excitation
based on the idea of Λ index. The Δr essentially measures the centroid distance between electron
and hole. Λ and Δr are described in detail in Sections 3.21.14 and 3.21.4, and they can be computed
via subfunctions 14 and 4 of main function 18, respectively.
In fact, with the Sr and D indices defined in the hole-electron analysis framework, it is no longer
necessary to use the Δr and Λ indices, since Sr and D are in principle more significant in physical
meaning. However, since Multiwfn is able to calculate Δr and Λ for a large number of selected
excited states simultaneously, if you simply want to roughly examine electron excitation
characteristics for a batch of excited states at once, employing Δr and Λ is still a good choice.
4 IFCT analysis
The full name of IFCT is "interfragment charge transfer", which is a method proposed by me
to estimate amount of electron transfer between atoms or fragments in the process of electron
excitation. The calculation cost is extremely low. This method has been detailed described in Section
3.21.8 and illustrated in Section 4.18.8. Although using difference between fragment charge of
excited state and that of ground state can also study variation of electron population during electron
excitation, one cannot understand details of charge transfer at "who transferred to whom" level,
therefore IFCT analysis has important and irreplaceable practical value for investigating problems
of electron excitation. In particular, when studying transition metal coordinates, exact amount of
MC, LC, LLCT, MLCT and LMCT can be separately evaluated by the IFCT analysis.
As illustrated in Section 4.18.16, Multiwfn is able to very easily calculate IFCT terms for all
excited states for present system, and major terms (with contribution > 5%) can be directly printed
so that you can easily identify major characters of all excited states.
5 Charge-transfer spectrum
The "charge-transfer spectrum (CTS)" has been introduced in Section 3.21.16, and an example
is given in Section 4.18.16. CTS is defined by me on the top of IFCT analysis. The relationship
between CTS and common UV-Vis is similar with that between partial density-of-states and total
density-of-states. CTS decomposes the whole UV-Vis spectrum as subcurves, including
intrafragment electron redistribution curves and interfragment electron-transfer curves. Via the CTS,
the main nature of UV-Vis spectrum can be vividly understood.
6 Analysis based on density difference between excited state and ground state
Density difference analysis is a prevalently used and widely accepted method for studying the
difference in charge distribution between two electron states of a system. Multiwfn supports a
variety of analysis methods based on the density difference between excited state and ground state,

1085

--- page break ---

4 Tutorials and Examples
as shown below:
·Plotting density difference map
First of all, Multiwfn can easily calculate density difference between excited state and ground
state and plot it as curve map, plane map and isosurface map through main functions 3, 4, 5, see
examples in Section 4.3, 4.4 and 4.5, respectively. Moreover, not only the density difference
between excited state and ground state can be drawn, but also the density difference between two
excited states can be easily drawn via Multiwfn, see illustration in Section 4.18.13.
·Smoothing density difference and calculating statistical data of density difference
The original density difference map between excited state and ground state is not easy to
examine, because its positive and negative regions are interlaced and appear to be messy. After
calculating the density difference grid data, one can use subfunction 3 of main function 18 to
transform it to replace the positive and negative parts of the density difference with very smooth
Gaussian functions, then the image will become much more intuitive and easier to analyze. At the
same time, the program outputs various statistical data about the density difference, such as centroid
coordinates of positive and negative parts, charge transfer distance, degree of separation between
positive and negative parts. See introduction in Section 3.21.3 and example in Section 4.18.3.
·Local integral curve and charge displacement curve
If the system under studying is linear or an interface system (such as a dye molecule attached
to TiO2 surface), the local integral curve and charge displacement curve can be plotted along the
direction of the molecular chain or perpendicular to the interface. The local integral curve shows
the integral value of the density difference on each section perpendicular to the chosen direction,
while the charge displacement curve shows the integral of density difference from the beginning
side to the current position. These two kinds of maps are useful to quantitatively study electron
transfer feature along a certain direction. It is easy to draw these two kinds of graphs in Multiwfn,
please check Section 3.16.14 of the manual for introduction and Section 4.13.6 for example.
·Basin integration for density difference
Multiwfn is able to perform basin integration for density difference, so that one can study
variation of number of electron in some featured local regions, see Section 4.17.4 for example.
7 Analyzing difference between excited state and ground state in electron population or
atom/fragment charges
Main function 7 is used to perform population analysis or atomic charge calculation, and if a
fragment is defined by subfunction -1 before evaluating atomic charges, fragment charge will also
be given in the output. See corresponding examples in Section 4.7. After calculating the fragment
charges of excited state and ground state separately, the difference between them can be used to
understand how many electrons were lost or gained at different fragments during the electron
excitation, and thus the influence of electron excitation on the charge distribution can be investigated
at quantitative level.
Although IFCT analysis is able to realize the same purpose, the advantage of using
atomic/fragment charge to discuss this problem is that there is a large room of choice of the method
for evaluating atomic charges, and the charge distribution of excited state can correspond to relaxed
density.
8 Draw transition density isosurface map, plotting heat map of transition density matrix
The transition density matrix (TDM) is very useful for unveiling the underlying nature of an
electron excitation. TDM has two forms:

1086

--- page break ---

4 Tutorials and Examples
(1) The three-dimensional real space form, which can be expressed by drawing isosurface map.
Large value at a point corresponds to large overlap of hole and electron at this place, see Section
3.21.1.1 for detailed introduction and Section 4.18.2.1 for analysis example.
(2) The matrix form in the common sense. This form of TDM can be exhibited as heat map
(namely color-filled matrix map), which may be atom-based or fragment-based. Its diagonal
elements vividly show which atoms or fragments are simultaneously occupied by hole and electron,
while the non-diagonal elements directly reflect the direction and extent of electron transfer between
corresponding atoms or fragments. See Section 3.21.2 for introduction of the TDM heat map and
Section 4.18.2.2 for analysis example.
9 Analyzing heat map of charge transfer matrix
If each atom is defined as a fragment in the aforementioned IFCT analysis, the amount of
charge transfer between various atoms and the amount of charge redistribution within each atom
will constitute a matrix, which is referred to as "atom-atom charge transfer matrix" by me and may
be further contracted to fragment-fragment charge transfer matrix. Both of the matrices can be
drawn as heat maps by subfunction 2 of main function 18, see Section 3.21.2 and 3.21.8 for
introduction as well as Section 4.18.8 for practical example. The information carried by the charge
transfer matrix heat map is very similar to the TDM heat map, and the way of analysis is exactly the
same, but the charge transfer matrix is more strictly defined and has clearer physical meaning.
Moreoever, the charge transfer matrix is completely in agreement with the hole and electron
distributions given by the hole-electron analysis module, therefore I believe the charge transfer
matrix map analysis is a better method than the popular TDM heat map analysis.
10 Analyses on transition dipole moment
For absorption process, the larger the oscillator strength of an electron excitation, the stronger
the corresponding absorption peak. The transition probability between two excited states is mainly
determined by oscillator strength, which is proportional to square of corresponding transition
electric dipole moment. Therefore, it is very meaningful to conduct an in-depth analysis on intrinsic
factors affecting the transition electric dipole moment. Multiwfn provides a number of functions for
decomposing transition dipole moment (including both the electric one and magnetic one), as
described below.
·Drawing transition dipole moment density
The transition dipole moment density is a function that measures the contribution of a point in
the three-dimensional space to the transition dipole moment, and its integral over the whole space
is exactly equal to the transition dipole moment. Obviously, if the transition dipole moment density
is plotted as isosurface map or plane map, contribution of each region to the transition dipole
moment can be vividly exhibited. See Section 3.21.1.1 for introduction and Section 4.18.2.1 for
example.
·Drawing heat map of transition dipole moment matrix
Subfunction 2 of main function 18 can draw heat map of transition dipole moment matrix,
either atom-based or fragment-based. The sum of all matrix elements is exactly the transition dipole
moment of the system, so the diagonal elements in the map show the contribution of atoms or
fragments to the transition dipole moment solely by themselves, while the non-diagonal elements
reflect the atom-atom or fragment-fragment coupling contribution to transition dipole moment.
Clearly, the internal structure of transition dipole moment can be clearly understood via this kind of
heat map. See Section 4.18.2.3 for analysis example.

1087

--- page break ---

4 Tutorials and Examples
·Decomposing transition dipole moment to basis function contributions and atom
contributions
Subfunction 11 of main function 18 can decompose transition dipole moment into contribution
of each atom and each basis function, see Section 3.21.11 for details. Moreover, based on the data
outputted by Multiwfn, via a VMD script one can draw arrows to exhibit contribution vector of
custom fragments to transition dipole moment, so that the contribution of each part of the system to
the transition dipole moment can be intuitively understood, see Section 4.18.11 for example.
·Decomposing transition dipole moment into contributions of orbital transitions
Subfunction 10 of main function 18 can decompose transition dipole moment into contribution
of each orbital transition, and at the same time the program outputs the oscillator strength evaluated
based on the current electron excitation information. Therefore, when many orbitals have significant
participation in electron excitation, this function can be used to immediately identify which orbital
transitions have critical influence on the oscillator strength, so that further discussion can be made.
In addition, one can set configuration coefficients of some orbital transitions to zero in subfunction
-1 of main function 18, and then enter this function again to check influence on the oscillator strength
due to ignoring those orbital transitions. See corresponding introduction in Section 3.21.10 and
example in Section 4.18.10.
·Calculating transition dipole moments between excited states and dipole moment of each
excited state
Transition dipole moments between excited states are important for some studies. For example,
they are needed by the sum-over-states (SOS) method, which can be used to calculate
(hyper)polarizability (see Section 3.27.2); in addition, simulating transient absorption spectrum
needs oscillator strength (f) between excited states, while evaluation of f requires transition dipole
moment between corresponding two excited states. In Multiwfn, subfunction 5 of main function 18
can evaluate transition dipole moments between excited states, and dipole moments of each excited
state can also be directly outputted. See Section 3.21.5 for details about this function.
11 Analyzing excited state wavefunction
Multiwfn is extremely powerful on electronic structure analysis, the analyses can not only be
applied to ground state, but can also be applied to excited state, as long as the input file contains
excited state wavefunction. Note that if the excited states were calculated by
CIS/TDHF/TDDFT/TDA-DFT methods, the input file must record natural orbitals (NOs) of
corresponding excited state. By using Multiwfn, the NOs can be generated based on the excited
state density matrix in .fch file, see Section 3.200.16 for detail; the NOs can also be generated based
on configuration coefficients, as shown in Section 3.21.13.
After loading excited state wavefunction into Multiwfn, one can carry out a variety of
electronic structure analyses. For example, main function 9 can be used to calculate various kinds
of bond orders for excited state, main function 7 can perform population analysis and calculate
atomic charges for excited state, main functions 3,4,5 are able to plot more than one hundred of real
space functions for excited state, AIM analysis can be applied to excited state by main functions 2
and 17, weak interaction of excited state can be visually studied via main function 20, excited state
aromaticity can be investigated via a bunch of methods in Multiwfn (see Section 4.A.3). By
comparing analysis result of excited state and ground state, the impact on electronic structure caused
by electron excitation can be fully shed light on.
12 Orbital composition analysis

1088

--- page break ---

4 Tutorials and Examples
Multiwfn has a very powerful orbital composition analysis module (main function 8), which
supports all orbital composition analysis methods. Via this function, one can study the MOs or NTOs
that mainly involved in electron excitation to make clear the role that played by various atomic
orbitals, atoms and fragments.
13 Examining overlapping extent and centroid distance between orbitals
Subfunction 11 of main function 100 is used to calculate overlapping extent and centroid
distance between two selected orbitals. Evidently, this function is useful for studying electron
excitation. For example, using this function to analyze the MO pair or NTO pair that dominates the
electron excitation, one can investigate charge displacement and separation degree during the
electron excitation.
14 Evaluating atomic transition charges
The atomic charge we commonly say is for a single electronic state, it is essentially determined
by density matrix of this state. It is also possible to calculate charge for each atom using transition
density matrix between two states, these charges are known as atomic transition charges. Just as the
method of calculating atomic charges is not unique, there are many different methods for calculating
the atomic transition charges. Multiwfn can calculate Mulliken atomic transition charges, see
corresponding description in Section 3.21.12. Multiwfn can also calculate atomic transition charges
via electrostatic potential fitting method, J. Phys. Chem. B, 110, 17268 (2006) and some other
literatures called this kind of charges as TrEsp (transition charge from electrostatic potential). Basic
theory and calculation example of TrEsp can be found in Section 4.A.9 of the manual. The main use
of atomic transition charges is quickly calculating electrostatic potential corresponding to transition
density, thereby examining the exciton coupling between molecules, this point is also described in
detail in Section 4.A.9.
15 Investigating contributions of orbital transitions to electron excitation
Computing contribution of an orbital transition to electron excitation is rather simple, see
beginning of Section 3.21 for introduction. For facilitating analysis, when you enter subfunction -1
of main function 18, contribution of ten orbital transitions that have largest contributions to the
selected electron excitation will be directly listed, see Section 3.21.0 for more information.
16 Identifying ghost states
Asymptotic behavior of exchange potential of pure or hybrid DFT functionals with low HF
exchange composition is obviously incorrect. When TDDFT with such exchange-correlation
functional is used to calculate excited states of large conjugate systems, a batch of artificial charge
transfer excited states with low energy tend to occur. The ghost states have no any physical meaning,
their existences not only wastes computation time, but may also cause beginners to mistake a ghost
state as an emission state. The ghost-hunter index proposed in J. Comput. Chem., 38, 2151 (2017)
can be used to diagnose whether an excited state produced by TDDFT calculation is a ghost state.
This index is automatically outputted after performing hole-electron analysis analysis, see Section
3.21.7 for detailed introduction and 4.18.1 for example. If a ghost state is found, the researchers can
avoid these states in their discussions, or try to eliminate these states by using a DFT functional with
higher HF exchange composition or long-range corrected functionals.
17 Evaluating contribution of NBO orbitals to electron transition
As sufficiently exemplified in Section 4.200.13.3, contribution of NBO orbitals to electron

1089
