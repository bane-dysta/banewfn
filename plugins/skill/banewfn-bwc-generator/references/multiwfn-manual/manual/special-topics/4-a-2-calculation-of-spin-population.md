# 4.A.2 Calculation of spin population

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.2 Calculation of spin population
- pdf pages: 1069-1069
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.2, spin, population

## Extracted manual text

4 Tutorials and Examples
puts "Processing $name.cub..."
mol default style CPK
mol new $name.cub
#translate by -0.100000 0.20000 0.000000
#scale to 0.30
rotate y by 50
rotate z by 90
rotate x by -30
rotate y by -20
mol addrep top
mol modstyle 1 top Isosurface $isoval 0 0 0 1 1
mol modcolor 1 top ColorID 3
render snapshot $name.bmp
mol delete top
}

This file essentially is a VMD script, in which the command set isoval 0.88 means the isosurface of
0.88 will be plotted, the default viewpoint is adjusted by scale, rotate and translate commands. for
{set i 1} {$i<=32} {incr i} means the file from DA0001.cub to DA0032.cub will be processed.
Now boot up VMD, and input the command source isoall.tcl in its command line window, then
you will have DA0001.bmp, DA0002.bmp ... DA0032.bmp.
There are numerous programs that can convert single-frame graphic files to animation, such as
Atani, FFmpeg, Videomach, etc. Here we use ImageMagick tool in Linux to do this, and we choose
gif as the animation format, since gif animation can be directly embedded into webpages.
Copy all of the .bmp files to Linux system, and run below command in the corresponding folder:
convert -delay 12 -colors 100 -monitor *.bmp ELF_IRC.gif

in which -delay controls the time interval between each frame in the animation, and -colors
determines the number of colors used, the larger the value, the more smoothly the color changes,
but the larger the animation file. You can run convert --help to study more arguments of this tool.
If the the resultant ELF_IRC.gif cannot be properly displayed on your system, use your
webpage explorer or advanced image explorers (e.g. IrfanView) to open it. The deformation of ELF
isosurface in this animation very intuitively exhibits how the new bonds are formed and how the
characteristic of existing bonds changed.

4.A.2 Calculation of spin population
As there are many ways to calculate atomic charges (see Section 3.9 for introduction and 4.7
for examples), there are various ways to calculate spin population. Spin population is defined as the
population number of alpha electrons minus that of beta electrons. Spin population is a key quantity
for characterizing electronic structure of open-shell systems, i.e. radicals and antiferromagnetic
systems. From spin population we can clearly know where the spin electrons are mainly distributed.
Moreover, we can discuss contribution from different regions (atomic orbitals, atoms or fragments)
to the total magnetic dipole moment m due to electron spin. If spin population of a region is x, then
its contribution to m will be xμB, where the Bohr magneton μB=eћ/(2me) (e: electron charge, me:
mass of electron) represents the magnetic moment produced by a single electron. Note that in

1048
