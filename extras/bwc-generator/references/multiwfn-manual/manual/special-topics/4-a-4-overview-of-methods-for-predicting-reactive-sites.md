# 4.A.4 Overview of methods for predicting reactive sites

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.4 Overview of methods for predicting reactive sites
- pdf pages: 1074-1075
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.4, predicting, reactive, sites

## Extracted manual text

4 Tutorials and Examples
(2010), which measures aromaticity based on electron density at bond critical points (BCP) in the
ring. See Section 3.14.6 for introduction and the example given in Section 4.2.1.
16. EL index: The idea of EL index is quite similar to HOMA, the most prominent difference
is that the bond lengths in HOMA formula are replaced with electron density ellipticity at BCPs in
the ring. For more detail see the original paper Struct. Chem., 23, 1173 (2012). Electron density
ellipticity at BCPs can be directly calculated by topology analysis module of Multiwfn. Since the
ellipticity at BCP is usually unclear for strongly polar bonds, EL index may be unreliable for the
ring containing heteroatoms. In addition, EL index shares the same drawback of HOMA, that is
reference system is need. If reference system cannot be obtained, such as the case of metal clusters,
this method does not work.
17. Aromaticity indices defined based on information-theoretic quantities: It was
demonstrated in ACS Omega, 3, 18370 (2018) that the average of information-theoretic quantities
of the atoms constituting a ring is closely related to aromaticity. This method is supported as
subfunction 12 of main function 15, see Section 3.18.11 for details.
18. AV1245 and AVmin: AV1245 can be viewed as an approximation of MCBO. AVmin is
closely related to AV1245, it is able to reveal bottleneck of electron delocalization and thus
aromaticity of a selected path. See Section 3.11.10 for introduction and Section 4.9.11 for example.

4.A.4 Overview of methods for predicting reactive sites
There are numerous methods able to predict reactive site of electrophilic, nucleophilic and
radical reactions, and almost all of them are supported by Multiwfn. In this section, I will summary
and briefly introduce the methods available in Multiwfn. The interested reader is highly
recommended to take a look at Acta Phys.-Chim. Sinica, 30, 628 (2014), in which various methods
for predicting electrophilic sites are carefully introduced and thoroughly compared. You may also
find the slideshow "Predicting reactive sites" in "Related resources and posts" Section of Multiwfn
website useful.
1 Electrostatic potential (ESP). If you are not familiar with ESP, please consult corresponding
introduction in Section 2.6. Since electrophile (nucleophile) locally carries positive (negative)
charge, and thus tends to be attracted to the region where ESP is negative (positive), the position
and value of minima (maxima) of ESP on molecular vdW surface is often used to reveal favorable
site of electrophilic (nucleophilic) attack. ESP analysis can be realized via quantitative molecular
surface analysis module, see Section 4.12 for detailed introduction and Section 4.12.1 for example.
There are also alternative ways to study ESP; as illustrated in Section 4.12.3, the average of ESP on
local vdW surface corresponding to each atom is also very useful, and this approach is more reliable
and robust than analyzing ESP extrema on vdW surface. For planar system, one can also calculate
and compare the ESP value above 1.6Å (approximately equal to vdW radius of carbon) of molecular
plane from different atoms to examine their reactivities; to do this, you need to use main function 1,
which directly outputs various real space function values at given points.
However, as shown in my paper Acta Phys.-Chim. Sin., 30, 628 (2014), ESP is usually not a
reliable property for predicting reactive sites.
2 Average local ionization energy (ALIE) and local electron affinity (LEA). If you are not
familiar with ALIE, please read corresponding introduction in Section 2.6. ALIE can be studied in
ways analogous to ESP. The most common way to predict reactive sites in terms of ALIE is

1053

--- page break ---

4 Tutorials and Examples
analyzing minima of ALIE on vdW surface, see Section 4.12.2 for example. Also, you can study
average of ALIE on local vdW surface or evaluate ALIE above 1.6Å of molecular plane for planar
system.
ALIE analysis is applicable to electrophilic and radical attacks, but it is useless for nucleophilic
attack. However, the local electron affinity (LEA) defined in similar way may be useful for this
purpose, see J. Mol. Model., 9, 342 (2003). LEA is supported in Multiwfn as user-defined function
27, see corresponding description in Section 2.7 for detail. The best way of analyzing LEA should
be plotting LEA mapped molecular surface map, as explicitly illustrated in Section 4.12.13.
3 Atomic charges. It is easy to understand that favorable electrophilic and nucleophilic
reactive sites should carry negative and positive atomic charges respectively, so that they can attract
electrophile and nucleophile to attack them. Multiwfn supports a lot of methods to calculate atomic
charges, see Section 3.9 for introduction and Section 4.7 for some instances. Among the available
atomic charges, the best one for predicting reactive site purpose may be Hirshfeld, interested readers
are suggested to consult J. Phys. Chem. A, 118, 3698 (2014) and especially Theor. Chem. Acc., 138,
124 (2019), the latter very nicely demonstrated reliability and value of Hirshfeld charge in predicting
both electrophile and nucleophile reactive sites. Do not use Mulliken charges, which may be the
worst one, though it is the most popular charge model.
4 Frontier molecular orbital (FMO) theory. Atom with larger contribution to HOMO
(LUMO) is more likely to be the preferential site of electrophilic (nucleophilic) attack. Multiwfn
supports many kinds of methods to calculate molecular orbital composition, see Section 3.10 for
introduction and Section 4.8 for examples. Commonly I suggest using Becke or Hirshfeld method.
Mulliken method works equally well if no diffuse functions are presented. NAO method is also a
good choice, but not suitable for analyzing virtual MOs. Besides, you can also directly visualize the
isosurface of MOs by main function 0 to discuss their compositions.
5 Fukui function and condensed Fukui function. The Fukui function proposed in J. Am.
Chem. Soc., 106, 4049 (1984) by Parr is the most prevalently used method for predicting reactive
sites nowadays. Please consult Section 4.5.4 for introduction and illustration. Fukui function is a
real space function, which is commonly studied by means of visualization of isosurface. In order to
faciliate quantitative comparison between difference sites, one can calculate condensed Fukui
function based on atomic charges, please consult Section 4.7.3. In addition, as illustrated in Section
4.12.4, distribution of Fukui function can also be characterized by means of local quantitative
molecular surface analysis technique. Furthermore, Multiwfn is able to evaluate contribution of
various kinds of orbitals (MO, NBO, NAO, etc.) to Fukui function to characterize it in terms of
orbital perspective, see Section 4.200.13.1 for example and Section 3.200.13 for introduction of the
algorithm.
6 Dual descriptor, Descriptor potential and condensed dual descriptor. As demonstrated
in Acta Phys.-Chim. Sinica, 30, 628 (2014), the dual descriptor proposed in J. Phys. Chem. A, 109,
205 (2005) may be the most robust method for predicting reactive sites, at least for electrophilic
reaction. Like Fukui function, dual descriptor also has condensed version. Dual descriptor and the
condensed version are introduced in Section 4.5.4 and 4.7.3, respectively.
Dual descriptor potential (DDP) is more rigorous than dual descriptor in predicting reactive
sites but much more expensive to evaluate. If your system is not large, using DDP instead of dual
descriptor is highly recommended. See Section 4.22.4 on how to calculate DDP.
Note that the easiest way of calculating Fukui function, dual descriptor as well as their

1054
