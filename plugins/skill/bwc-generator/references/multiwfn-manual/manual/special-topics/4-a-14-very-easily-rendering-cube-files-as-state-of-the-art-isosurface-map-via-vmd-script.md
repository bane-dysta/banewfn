# 4.A.14 Very easily rendering cube files as state-of-the-art isosurface map via VMD script

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.14 Very easily rendering cube files as state-of-the-art isosurface map via VMD script
- pdf pages: 1119-1120
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.14, Very, easily, rendering, cube, as, state-of-the-art, isosurface, map, via, VMD, script

## Extracted manual text

4 Tutorials and Examples
may find error prompts such as "File not found" and "No such file or directory". They are harmless,
simply ignore them)
• Input vmd to boot up VMD, and then input source ESPiso.vmd in the VMD console window
to plot the map

4.A.14 Very easily rendering cube files as state-of-the-art isosurface
map via VMD script
Note: Chinese version of this tutorial is http://sobereva.com/483, it contains more discussions and examples
than this section.

Introduction
Although in most cases, the isosurface map directly drawn by Multiwfn is already satisfactory,
better effect can be gained if you use VMD to render the isosurface. VMD can be freely obtained
via http://www.ks.uiuc.edu/Research/vmd/. In fact, in Section 4.5.5 I already exemplified how to
draw isosurface map based on cube files produced by Multiwfn, however the procedure is somewhat
lengthy, and the effect cannot reach state-of-the-art level. In this section, I will show it is possible
to use VMD script to plot very high-quality isosurface map via very few steps. The methods in this
section only works for Windows platform, but you may also be able to find a way to make the
method work under Linux.
The VMD script is examples\scripts\showcub.vmd. Before using it, you should move it to VMD
folder and add a line source showcub.vmd into the vmd.rc file in VMD folder, so that the four
customized commands in this file will be available after booting up VMD. The commands are
described below.
➢ cub and cubiso: Used for showing single cube file. Example of usage:
cub DD : Draw DD.cub in current folder as isosurface map, both positive and negative parts
are shown as green and blue with default isovalue of 0.05 and -0.05, respectively.
cubiso 0.02 : Changing isovalue of both positive and negative parts to 0.02.
cub DD 0.02 : Equivalent to using cub DD and then cubiso 0.02.
➢ cub2 and cub2iso: Used for showing two cube file simultaneously. Example of usage:
cub2 f+ f- : Draw f+.cub and f-.cub in current folder as green and blue isosurfaces, respectively.
Note that only positive part of the cubes will be shown.
cub2iso 0.02 : Changing isovalue of both the two isosurfaces to 0.02.
cub2 f+ f- 0.02 : Equivalent to using cub2 f+ f- and then cub2iso 0.02.
After showing the isosurfaces in VMD graphical window using above commands, you can use
batch file VMDrender_full.bat or VMDrender_noshadow.bat in examples\scripts folder to invoke
Tachyon render to get better effect, as will be illustrated later. The difference between the two files
is that the shadow effect is enabled in the former while disabled in the latter.
Next I give two practical examples. Before following them, please copy the aforementioned
two .bat files and showcub.vmd to VMD folder, and then properly set up vmd.rc. The VMD version
I am using is 1.9.3.
Example 1: Spin density map of C4H8 singlet biradical
Boot up Multiwfn and input
examples\C4H8.wfn // .wfn file of C4H8 singlet biradical

1098

--- page break ---

4 Tutorials and Examples
5 // Calculate grid data
5 // Spin density
3 // High quality grid
2 // Export the grid data to spindensity.cub in current folder
Now, move the spindensity.cub to VMD folder, boot up VMD and input cub spindensity 0.01
in VMD console window, you will see isosurface map of the cube file with isovalue of 0.01 has
been shown in the graphical window.
In order to get better effect, in VMD we choose "File" - "Render" - "Tachyon", then click "Start
Rendering", you will find vmdscene.dat has appeared in VMD folder. Now double click
VMDrender_full.bat, which will use vmdscene.dat as input file of Tachyon render to yield an image
file named full.bmp in current folder. The resulting graph is shown below, the quality is clearly very
good!

Example 2: Hole-electron map of NH2-biphenyl-NO2
The hole-electron analysis illustrated in Section 4.18.1 is extremely useful in understanding
the nature of electron excitation. Although Multiwfn can directly plot hole and electron distributions
simultaneously in the built-in GUI window, much better effect can be obtained by means of VMD.
I still use the NH2-biphenyl-NO2 analyzed in Section 4.18.1 as example, we will plot isosurface
of both hole and electron of S0→S2 transition via VMD. To do so, boot up Multiwfn and input
examples\excit\D-pi-A.fchk
18 // Electron excitation analysis
1 // Hole-electron analysis
examples\excit\D-pi-A.out
2 // Study excitation between ground state (S0) and the second excited state (S2)
1 // Calculate distribution of hole, electron and so on as well as various indices
3 // High quality grid
After the calculation is finished, choose options 10 and 11 in turn to export grid data of hole
and electron to hole.cub and electron.cub in current folder, respectively. Then move them to VMD
folder, boot up VMD and input cub2 electron hole. You will find no isosurface has been shown, this
is because the default isovalue (0.05) is not suitable for grid data. We use cub2 command to test
different isovalues, finally we find after inputting cub2 0.005 the graphical effect is satisfactory, that
is the isosurfaces can substantially represent distribution character of both hole and electron. The
current graph shown in the VMD graphical window is shown below, green and blue correspond to
electron and hole, respectively.

1099
