# 4.A.13 Plot electrostatic potential colored van der Waals surface map and penetration graph of van der Waals surfaces

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.13 Plot electrostatic potential colored van der Waals surface map and penetration graph of van der Waals surfaces
- pdf pages: 1111-1118
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.13, Plot, electrostatic, potential, colored, van, der, Waals, surface, map, penetration, graph, surfaces

## Extracted manual text

4 Tutorials and Examples
transition can be obtained by fitting NBO orbital densities to density difference between two
electronic states. Since NBO orbitals often have clear feature and chemical meaning, this method is
able to provide deeper insight into the nature of electron transitions. The same module can also be
used to study contribution of any other kind of orbitals (e.g. LMO) to electron transitions, see
Section 3.200.13 for theory and algorithm introduction.
Others
Subfunction 17 of main function 18 is able to provide very valuable insight into the nature of
electron density polarization under an external perturbation (e.g. point charge), and can be used to
study substitution effect, mechanism of electrophilic/nucleophilic reactions, atomic polarizability,
and so on. See Section 3.21.17 of Multiwfn manual for introduction and Section 4.18.17 for example.
Subfunction 15 of main function 18 is able to quickly print all major molecular orbital
transitions in every excited state, this is useful if you want to examine basic characteristics of each
electron excitation in terms of molecular orbitals. See Section 3.21.15 for introduction of this
function.
Do not forget Multiwfn has main function 11, which can plot UV-Vis and ECD spectrum based
on the oscillator/rotatory strengths and excitation energies outputted by quantum chemistry codes.
This module is by far more powerful and flexible than any other plotting tools and able to provide
detailed information about the spectrum. Please check Section 3.13 for introduction and Section
4.11 for abundant examples.
It is also worth mentioning the biorthogonalization method, which may be also useful in
studying the nature of triplet excited state, that is this method can usually describe the triplet excited
state calculated by UKS or UHF method in terms of orbital transition model, so that discussion the
nature of the excitation could be simplified. See Section 3.100.12 for introduction and 4.100.12 for
example.
Finally, note that only for the aforementioned entry 5 (density difference analysis), entry 6
(atom/fragment charge analysis) and entry 10 (excited state wavefunction analysis), arbitrary
electron excitation calculation method could be used as long as they can produce excited state
wavefunction. For example, for the density difference analysis, the difference can be made between
the electron density of the lowest triplet excited state and singlet state calculated by KS-DFT, the
difference can also be made between excited state density produced by EOM-CCSD and the ground
state density yielded by CCSD. While for other kinds of analyses, such as hole-electron analysis,
IFCT analysis, only CIS, TDHF, TDDFT and TDA-DFT can be employed for calculating excited
states.

4.A.13 Plot electrostatic potential colored van der Waals surface map
and penetration graph of van der Waals surfaces
Note 1: I strongly suggest looking at this video tutorial https://youtu.be/QFpDf_GimA0, which clearly and
sufficiently illustrates most content in this section.
Note 2: Average local ionization energy (ALIE) on molecular surface can also be plotted via VMD script, see
Section 4.12.2 for example.
Note 3: Chinese version of this tutorial is my blog article “Using Multiwfn+VMD to rapidly plot electrostatic
potential colored molecular van der Waals surface map and penetration map between molecules”
(http://sobereva.com/443), which contains more discussion and examples than this section.

1 Foreword

1090

--- page break ---

4 Tutorials and Examples
In the tutorial "Plotting electrostatic potential colored molecular surface map with ESP surface
extrema via Multiwfn and VMD" (http://sobereva.com/multiwfn/res/plotESPsurf.pdf), I detailedly
described how to plot electrostatic potential (ESP) colored molecular van der Waals (vdW) surface,
this kind of map is very important and frequently involved in literatures. However, there are a large
number of steps in the tutorial. In order to make plotting this kind of map as easy as possible, here
I introduce a script-based method to draw similar graph, and meantime I will introduce how to plot
penetration graph of vdW surfaces, which is very useful for discussing intermolecular interactions.
However, I still suggest you also read the aforementioned tutorial after reading the content in the
present section, so that you can understand more details and are able to manually improve the effect
of the obtained graph.
The vdW surface mentioned in this section corresponds to isosurface of electron density of
0.001 a.u.
2 Preparation
VMD program is needed in the present plotting, it can be freely downloaded from
http://www.ks.uiuc.edu/Research/vmd/, the version I used here is 1.9.3. Here I assume that you are
using Windows system, however the method described below is also suitable for Linux system, see
Part 9 of this section.
All files utilized below have been given in "examples\drawESP" folder, they are briefly
introduced here:
• .bat files: Batch process files of Windows system. They are used to invoke Multiwfn to
calculate needed data for plotting graphs in VMD. The content of the files is very easy to understand
and can be easily modified. If you do not know how to run Multiwfn in silent mode, please check
Section 5.2
• .txt files: Input stream files of Multiwfn that involved in the .bat files.
• .vmd files: VMD plotting scripts.
Before plotting, you should do below things:
(1) Move all .bat and .txt files to the folder containing Multiwfn executable file
(2) Modify the VMD path in the .bat files to actual path of VMD in your machine
(3) Copy all .vmd files to VMD folder
(4) Add below content to the end of the vmd.rc file in VMD folder:
proc iso {} {source ESPiso.vmd}
proc iso2 {} {source ESPiso2.vmd}
proc pt {} {source ESPpt.vmd}
proc pt2 {} {source ESPpt2.vmd}
proc ext {} {source ESPext.vmd}

These defined shortcut commands. For example, simply inputting iso is equivalent to inputting
source ESPiso.vmd.
3 Plotting ESP colored vdW surface for single molecule
Here we take acetamide as example. Move the CH3CONH2.fch in "examples" folder to the
folder containing Multiwfn executable file, modify the file name to 1.fch. Double click the
ESPpt.bat, Multiwfn will be invoked to carry out quantitative molecular surface analysis (main
function 12) for the 1.fch, once the calculation has done, the exported mol1.pdb and vtx1.pdb will
be automatically moved to the VMD folder. Then boot up VMD and input command pt in VMD

1091

--- page break ---

4 Tutorials and Examples
console window, then the ESPpt.vmd will be activated to load the mol1.pdb and vtx1.pdb to draw
below map:

The default lower and upper limits of color scale is -50 and 50 kcal/mol, respectively, and the
default color transition is BWR (Blue-White-Red), therefore in above map the white area
corresponds to the region having almost vanished ESP value, while the red and blue points have
evidently positive and negative ESP, respectively. You can manually change the default setting by
modifying the ESPpt.vmd, the settings can also be changed in VMD GUI interface, see the
plotESPsurf.pdf tutorial for detail.
In above map, the ESP colored vdW surface is represented in terms of surface vertices, the
graph can also be drawn in an alternative way, namely mapping ESP on electron density isosurface,
we do this now. Double click the ESPiso.bat, then Multiwfn will be invoked to calculate and export
cube file of electron density and ESP, the resulting density1.cub and ESP1.cub will be automatically
moved to the VMD folder. Then boot up VMD and input command iso in VMD console window,
then the ESPiso.vmd will be activated to load the two cube files to draw below map. Notice that in
order to gain slightly better effect, I used the built-in Tachyon render to obtain below graph, namely
selecting "File" - "Render", change to "Tachyon (internal, in-memory rendering)" and click "Start
Rendering" button (The resulting file is in .tga format, you need to use advanced image viewer to
view it, such as IrfanView, which is freely available at https://www.irfanview.com).

It is quite worth to explain the “ESPrhoiso” parameter. It can be set either by argument of running command
(as you can see the “-ESPrhoiso 0.001” in the ESPiso.bat) or by corresponding parameter in settings.ini. If ESPrhoiso
is set to a value larger than 0, for example, 0.001, then during calculating ESP grid data by Multiwfn's own code,
ESP will be evaluated only for the grids around isosurface of electron density of 0.001 a.u., while ESP of other grids
will be automatically set to 0. This trick never affects the quality of the resulting ESP colored vdW surface map,

1092

--- page break ---

4 Tutorials and Examples
while computational cost is significantly reduced since the ESP evaluation for irrelevant grids is fully ignored.

4 Show ESP extrema on molecular surface simultaneously
It is possible to append the ESP surface extrema on the graph. To do this, double click
ESPext.bat, it will do all things that ESPpt.bat do, but it also outputs surfanalysis.pdb and moves it
to the VMD folder. This file records all surface extrema. Then boot up VMD and input command pt
or iso to draw corresponding map first, and then input ext, then ESPext.vmd will be activated to load
the surfanalysis.pdb and render the surface extrema as small spheres. The combination of pt+ext
and iso+ext are shown at left and right sides of below graph, respectively. Note that in order to make
ESP extrema at backside visible, I have changed the material of electron density isosurface to
"Transparent" (namely enter "Graphics" - "Representation", switch to "density1.cub", change
"Material" to "Transparent". If you want to make this as default setting, modify the ESPiso.vmd and
change the "$id EdgyGlass" to "id Transparent")

In above graph, the orange and cyan spheres correspond to the positions of maxima and minima of
ESP on the vdW surface. You can also manually mark ESP values on the extrema by an image editor,
see the plotESPsurf.pdf tutorial on how to do this. An easy way to get ESP value of an extreme is:
Clicking "0" on keyboard to enter query mode, click center of a sphere, then its index will be shown
on console window. Assume that the index is 3, you should input the following command in VMD
console window
[atomselect top "index 3"] get beta
then the ESP value will be shown. See the first line of surfanalysis.pdb for the unit of the printed
ESP.
As I have mentioned in Section 4.12.1, even for a neutral system, there may be some surface
minima (maxima) with positive (negative) value, which are often chemically insignificant and can
be ignored. If you do not want to plot them on the graph, you can replace the content of ESPext.txt
by that of examples\drawESP\ESPext_noinsig.txt. The additional four lines in this file with respect
to ESPext.txt is used to remove these insignificant extrema.
5 Plotting ESP colored penetration map of vdW surface of monomers
Here I use water tetrameter to illustrate how to plot this kind of map. The files used in this
instance are provided in "examples\water_tetramer\fch" folder. The Gaussian input files of the four
water molecules are 1/2/3/4.gjf, respectively, their coordinates were directly extracted from the
optimized tetramer coordinate, which can be found in complex.gjf. Run these .gjf files by Gaussian,
you will obtain the 1/2/3/4.fch. Notice that nosymm keyword has been employed, otherwise the
Cartesian coordinates of the monomers will be no longer consistent with those in complex, because
without this keyword Gaussian will automatically put the systems to standard orientation.

1093

--- page break ---

4 Tutorials and Examples
Copy the 1/2/3/4.fch files to the folder containing Multiwfn executable file, run ESPpt.bat, then
Multiwfn will be invoked to calculate the four .fch files in turn, the resulting mol1/2/3/4.pdb and
vtx1/2/3/4.pdb will be automatically moved to the VMD folder. Then boot up VMD and input pt2
to activate the ESPpt2.vmd script, you will immediately see left part of below graph. If you run
ESPiso.bat and then input iso2 in VMD instead, then the ESPiso2.vmd will be activated to draw the
right part of below graph based on the exported density1/2/3/4.cub and ESP1/2/3/4.cub.

From above map, the inter-penetration between the vdW surfaces of the four monomers due to
formation of hydrogen bonds can be clearly seen. In addition, the mapped colors show that the
tetramer was formed in ESP positive-negative complementary way, revealing the electrostatic nature
of the hydrogen bonds.
As an exercise, please try to plot the ESP colored vdW surface penetration map of GuanineCytosine dimer via above two ways, the .fch file of the two monomers can be downloaded at
http://sobereva.com/multiwfn/extrafiles/GC_fch.rar. Notice that before plotting, you should
manually delete the .pdb and .cub files generated for previous systems in VMD folder.
6 Hint: On the adjustment of material
For some systems, the ESP colored map plotted via iso command is not quite ideal. For
example, the below map looks messy

In this case, you can enter "Graphics" - "Materials", choose "EdgyGlass", which is the material

1094

--- page break ---

4 Tutorials and Examples
currently used for representing the surface. Then adjust its each setting, especially the "Opacity". If
we change the setting to below case, you will find the difference of ESP on the vdW surface now
can be distinguished more clearly.

7 Hint: Plotting ESP mapped vdW surface for very huge systems
Sometimes we need to plot ESP mapped surface for systems consisting of several hundreds of
atoms, in this case even single point calculation using DFT with 6-31G* is very expensive or
computationally infeasible. To plot the map for this case, below is my suggested steps:
• Perform single point task or optimization task via Grimme's xtb code
(https://github.com/grimme-lab/xtb/). The xtb conducts all calculations based on GFN-xTB theory,
which can be regarded as a semi-empirical version of DFT. --molden argument should be employed
to make xtb output Molden input file (molden.input). Since xtb is extremely fast, even for a system
composed of several hundreds of atoms, single point task can be finished within no more than 1
minutes on a personal computer.
• Load the molden.input into Multiwfn, then use corresponding option in subfunction 2 of main
function 100 to export .fch file (e.g. xtb.fch).
• Make sure that "cubegenpath" in settings.ini has been properly set. Ensure that you have
defined GAUSS_MEMDEF environment variable, see Section 5.7 for detail.
• load the xtb.fch into Multiwfn, using main function 5 to calculate grid data and export cube
file for electron density and ESP in turn, the exported density.cub and totesp.cub should be manually
renamed to density1.cub and ESP1.cub, respectively. Notice that "High quality grid" should be
employed since the system is large. The calculation of ESP is relatively time-consuming, for
example, using a common Intel 4 cores CPU, the time cost for a system with 336 atoms takes half
an hour.
• Move the density1.cub and ESP1.cub, as well as the aforementioned ESPiso.vmd in
examples\drawESP\ to VMD folder.
• Boot up VMD and input source ESPiso.vmd in VMD console window.
Now you can see the ESP colored vdW surface map. I suggest you also properly adjust material
setting, as mentioned earlier. Below is a system containing 336 atoms.

1095

--- page break ---

4 Tutorials and Examples

Even if you only have a personal computer with 4 cores, you can obtain above map starting
from a structure file with total time cost of no more than 1 hour. While if you have a server with
dozens of cores, the graph can be obtained within 10 minutes.
It is worth to note although the xtb code is extremely fast, the ESP quality corresponding to the
wavefunction generated by xtb is usually satisfactory. According to my test, no evident difference
can be found between the ESP colored molecular surface graph generated based on the xtb
wavefunction and the high-quality B3LYP/def2-TZVP wavefunction.
More information about this topic can be found in my article "Rapidly plotting electrostatic
potential colored van der Waals surface for huge system" (in Chinese, http://sobereva.com/481).
8 Other worth noting points
Color bar of ESP can be appended on the map, see illustration close to the end of this video:
https://youtu.be/QFpDf_GimA0.
I strongly suggest readers to check the content of the .bat, .txt and .vmd files to make clear how
they work. As you can see, the ESPpt.bat and ESPiso.bat can at most deal with four .fch file
(1/2/3/4.fch), you can also extend them to more molecules. In the ESPpt2.vmd and ESPiso2.vmd,
the variable "nsystem" was set to 4, namely at most mol4.pdb&vtx4.pdb and
density4.cub&ESP4.cub will be loaded and plotted, clearly if you want to use the plotting scripts to
simultaneously plot more monomers, the "nsystem" should be enlarged.
It is noteworthy that the value 0.15 in the ESPpt.txt is the grid spacing in quantitative molecular
surface analysis; if you increase it, the surface vertices will become sparser, and calculation cost
will be reduced. The default commands in the ESPiso.txt correspond to using high quality grid for
electron density while use low quality grid for ESP (for saving computational cost purpose), this
combination is suitable for most systems, but for extremely large systems, you may need to modify
the file so that better quality grids are employed for electron density and ESP, respectively, otherwise
the resulting isosurface may be unsmooth and the mapped color may be blurred.
For better graphical effect, users are suggested to manually change lower and upper limits of
color scale, so that the variation of ESP on vdW surface can be represented by color as clear as
possible. For charged systems, the default color scale should always be changed, otherwise the vdW
surface will be monochromatic. For these systems, you should load the input file, enter main
function 12, select option 1 to carry out quantitative molecular surface analysis for ESP, copy the
global minimal and maximal values of ESP to the text box for setting color scale, as shown below,

1096

--- page break ---

4 Tutorials and Examples
and then press ENTER button to make the setting take effect.
Note: If the map was plotted by ESPiso.bat, you should take the ESP values in a.u. and then set them as the
color scale. However, if the map was plotted by ESPpt.bat, you should open the vtx1.pdb in VMD folder by text
editor, the first few lines clearly indicate the unit used in this file, you should read ESP values in that unit from the
Multiwfn console window and the set the color scale.

If you prefer to use eV rather than a.u. as ESP unit when using iso or iso2 commands to plot
the ESP map, you should use ESPiso_eV.bat and ESPiso_eV.txt in “examples\drawESP” folder
instead of the ESPiso.bat and ESPiso.txt respectively in aforementioned procedure, and you should
manually edit the ESPiso.vmd and ESPiso2.vmd files, remove the # sign before the lines “set
colorlow -0.8” and “set colorhigh 0.8”. In this case, the ESP data in .cub file will be in eV, and the
default lower and upper limits of color scale will be -0.8 and 0.8 eV, respectively. The color bar
drawn according to aforementioned YouTube tutorial video, will also be in eV.
In the case of extremely charged systems, such as DNA, when ESPpt.bat is used, the B-factor
column may be unable to correctly record mapped ESP values because their magnitude is too large.
In this situation, you should use ESPpt_pqr.bat, ESPpt_pqr.txt, ESPpt_pqr.vmd and ESPext_pqr.vmd
in "examples\drawESP" folder instead of the ESPpt.bat, ESPpt.txt, ESPpt.vmd and ESPext.vmd
mentioned above, respectively, in this case the "Charge" column of .pqr file will be used instead of
B-factor column of .pqr file to record the data, the former can record a much larger range of data
and the unit is always in a.u. Also note that the ESPext.txt and ESPext.bat are no longer needed,
since extrema1.pqr is also exported and moved to VMD folder when you use ESPpt_pqr.bat.
9 On the plotting ESP colored vdW map under Linux
The method described above can also be used in Linux (and may be also MacOS) environment.
In "examples\drawESP" folder, you can find ESPiso.sh, ESPpt.sh and ESPext.sh, they are
corresponding Linux scripts of the .bat file mentioned above.
For example, you want to use ESPiso.sh to draw ESP colored vdW surface for cosplay.fchk,
what you need to do is
• Install Multiwfn by strictly following Section 2.1.2. Install VMD in usual way
• Copy ESPiso.sh, ESPiso.txt and ESPiso.vmd from "examples\drawESP" to a working
directory
• Copy cosplay.fchk to the working directory
• Edit the ESPiso.sh, change the 1.fchk to cosplay.fchk
• Enter the working directory, run chmod +x ./ESPiso.sh, and then run ./ESPiso.sh. (Then you

1097
