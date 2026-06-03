# 4.A.6 Calculate odd electron density

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.6 Calculate odd electron density
- pdf pages: 1080-1083
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.6, odd, electron, density

## Extracted manual text

4 Tutorials and Examples
of NOCV pairs, each pair has corresponding energy contribution to orbital interaction energy and
has corresponding density which can be visualized to easily understand nature, therefore ETSNOCV analysis provides very deep insight into orbital interaction. See Section 3.26 for detailed
introduction of this analysis and Section 4.23 for examples of applying ETS-NOCV on studying
various kinds of interactions. Although orbital interaction commonly is not the dominant physical
component of weak interactions, ETS-NOCV is still useful in some cases. For example, Section
4.23.4 illustrates how to utilize ETS-NOCV to investigate hydrogen-bond interaction.
(19) Multiwfn can calculate contribution of atoms to dispersion energy and calculate dispersion
density. It is very convenient to use, the calculation is particularly fast, and it also supports periodic
systems. One can visually display which atoms contribute significantly to the dispersion effect in
the present system by coloring atoms and drawing isosurface map. One can also discuss problems
related to dispersion effect by obtaining the difference in the dispersion energy contributed by atoms
in two different systems, such as which atoms have significant changes in their dispersion
contribution during a conformational change process, which atoms make the main contribution to
physical adsorption (when dispersion effect dominates), and so on. See Section 3.24.4 for
introduction of this function and Section 3.21.4 for analysis examples.
There are also other possibly ways to study weak interactions, but they are not directly relevant
to Multiwfn. These methods include: NBO E(2) and NBO deletion analyses, rehybridization
analysis (specific for H-bond, based on natural population analysis), variation of bond length and
vibrational frequency, Mayer energy decomposition analysis (Phys. Chem. Chem. Phys., 8, 4630
(2006)), magnetically induced current (Phys. Chem. Chem. Phys., 13, 20500 (2011)), interacting
quantum atoms (IQA, see J. Phys. Chem. A, 117, 8969 (2013) for example), SAPT analysis
(supported by PSI4, Molpro etc. see WIREs Comput. Mol. Sci., 2, 254 (2012)).

4.A.6 Calculate odd electron density
Odd electron refers to unpaired electron. The so-called odd electron density (OED) is defined
for representing distribution of odd electrons, the idea originates from Chem. Phys. Lett., 372, 508
(2003), and further explicitly expressed as the function form in Theor. Chem. Acc., 130, 711 (2011)
and J. Phys. Chem. C, 116, 19729 (2012). OED is useful in graphically exhibiting distribution of
unpaired electrons especially when spin density cannot be obtained (for example, excited state
calculated by TDDFT). In this section, I will introduce OED and show how to plot it by using
Multiwfn in combination with .wfn file produced by Gaussian.
Theory of OED
Spatial (spinless) natural orbitals are yielded by diagonalizing total density matrix and have
occupation number between 0.0 and 2.0. The OED contributed by the kth natural orbital is defined
as

kodd (r ) = min(2 − nk , nk ) k (r)
where k(r) and nk are probability density and occupation number of natural orbital k, respectively.
Clearly, for nk<1, the prefactor directly corresponds to occupation number, while for nk1, the
prefactor corresponds to the complement to achieve a closed shell. The min(2-nk, nk) term, which

1059

--- page break ---

4 Tutorials and Examples
measures the deviation from current orbital occupation number to closed-shell limit, is regarded as
the effective number of unpaired electrons expressed by the orbital.
The OED is defined as the sum of OED for all natural orbitals, namely

 odd (r) =  kodd (r)
k

The total number of odd electrons is

N odd =  min(2 − nk , nk )
k

Calculation of OED for a closed-shell system, OC-BH3
Although the OED was originally proposed to exhibit distribution of unpaired electrons, in my
personal viewpoint, this function may be also useful to reveal the regions where electron correlation
is significant, because the deviation of orbital occupation number to 0.0 and 2.0 is caused by electron
correlation effect.
As an example, we calculate OED for a typical closed-shell system OC-BH3 at CCSD/def2SVP level (while at HF/DFT level, this quantity is obviously zero everywhere). The Gaussian input
file can be found in examples\COBH3_CCSD.gjf, note that density out=wfn keywords are used. The
resulting file examples\COBH3_CCSD.wfn contains all CCSD natural orbitals.
We first calculate total OED. Boot up Multiwfn and input
examples\COBH3_CCSD.wfn
6 // Modify wavefunction
26 // Modify occupation number
0 // Select all orbitals
odd // Taking min(2-nk, nk) as occupation number for all orbitals
Now from screen you can see
Sum of occupation numbers of selected orbitals:

0.628552

This value is total number of odd electrons, and it also corresponds to integral of OED over the
whole space. It may be employed as a metric of electron correlation. Then input
q // Return
-1 // Return to main menu
Then we plot isosurface map of electron density in usual way by inputting the following
commands. Since the current orbital occupation numbers have been transformed to min(2-nk, nk),
the resulting map will correspond to OED map
5 // Calculate grid data
1 // Electron density
2 // Medium quality grid
-1 // Visualize isosurface
Then set isovalue to 0.005 a.u., the OED map shown in the GUI window will be

1060

--- page break ---

4 Tutorials and Examples

As you can see from the above graph, electron correlation effect is most prominent in the
multiple bond region of CO. It is widely known that electron correlation of multiple bonds is much
stronger than single bonds.
If you want to check contribution of each natural orbital to OED, you can enter main function
0, choose “Orbital info.” - “Show all” in menu bar, then in the console window you can see
[Ignored...]
Orb:

8 Ene(au/eV):

0.000000

0.0000 Occ: 0.044509 Type:A+B

Orb:

9 Ene(au/eV):

0.000000

0.0000 Occ: 0.047150 Type:A+B

Orb:

10 Ene(au/eV):

0.000000

0.0000 Occ: 0.052490 Type:A+B

Orb:

11 Ene(au/eV):

0.000000

0.0000 Occ: 0.052490 Type:A+B

Orb:

12 Ene(au/eV):

0.000000

0.0000 Occ: 0.054139 Type:A+B

Orb:

13 Ene(au/eV):

0.000000

0.0000 Occ: 0.054139 Type:A+B

Orb:

14 Ene(au/eV):

0.000000

0.0000 Occ: 0.023915 Type:A+B

[Ignored...]

The value after “Occ” is just the min(2-nk, nk) in aforementioned formula. In the GUI window of
main function 0 you can visualize the orbitals having large “Occ” to examine which orbitals are
closely related to electron correlation effect.
It is also possible to evaluate atom contribution to OED. After return to main menu, input
following commands
15 // Fuzzy atomic space analysis
1 // Perform integration in fuzzy atomic spaces for a real space function
1 // Electron density (corresponds to OED currently)
Then you can see
Atomic space

Value

% of sum

% of sum abs

1(C )

0.17435408

27.739003

27.739003

2(O )

0.20206854

32.148257

32.148257

3(B )

0.14039803

22.336737

22.336737

4(H )

0.03724382

5.925335

5.925335

5(H )

0.03724381

5.925334

5.925334

6(H )

0.03724381

5.925334

5.925334

Summing up above values:

0.62855211

Summing up absolute value of above values:

1061

0.62855211

--- page break ---

4 Tutorials and Examples
Clearly, O contributes most to OED, followed by C and then B. By default Becke partition of atomic
spaces is adopted, you can also change to other atomic partition methods via option -1.
It is worth to note that one can also plot OED contributed by specific orbitals. For example, we
want to plot the OED only contributed by natural orbitals from 10 to 13. After inputting odd in
subfunction 26 of main function 6, we then need to clean the occupation numbers of all other orbitals,
namely in subfunction 26 we then also need to input
1-9 // Select orbitals 1 to 9
0 // Set occupation number to 0
14-57 // Select orbitals 14 to 57
0 // Set occupation number to 0
After that you can return to main menu and plot electron density as usual.
Calculation of OED for an open-shell system: C4H8 biradical
In order to illustrate the value of OED in representing distribution of unpaired electrons of
biradicals, next we will plot OED for a typical biradical system C4H8 at unrestricted M06-2X level.
In this case, unrestricted open-shell calculation is needed and guess=mix keyword should be used
to achieve symmetry-broken state. In addition, pop=no out=wfn must be specified so that spatial
natural orbitals will be generated by mixing alpha and beta density matrices and subsequent
diagonalization, then will be exported to .wfn file. The natural orbitals obtained by such an
unrestricted DFT calculation are sometimes referred to as unrestricted natural orbitals (UNO). The
Gaussian input file for producing the .wfn file is examples\C4H8-UNO.gjf, and the resulting .wfn
file is examples\C4H8-UNO.wfn. Please use this file to plot OED like above example, the isosurface
map with isovalue of 0.02 a.u. should look like as follows. It is seen that its distribution character is
quite similar to spin density, though the alpha and beta spins cannot be distinguished by sign.

CASSCF method is very commonly adopted for calculating biradical systems. OED can also
be plotted for CASSCF wavefunction, you just need to generate a wavefunction file containing
natural orbitals produced by CASSCF calculation.
OED can also be used to represent distribution of unpaired electrons of excited states calculated
by TDDFT method, see my blog article "Using Multiwfn to calculate odd electron density to study
distribution of unpaired electrons of excited states" (http://sobereva.com/583, in Chinese) for
detailed illustration and discussion. It is worth to note that spin density is not available for TDDFT
excited state wavefunction, so the OED is the only way to characterize the unpaired electron
distribution.

1062
