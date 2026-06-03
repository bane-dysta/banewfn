# 4.A.1 Study variation of electronic structure along IRC path

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.1 Study variation of electronic structure along IRC path
- pdf pages: 1064-1068
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.1, variation, electronic, structure, along, IRC, path

## Extracted manual text

4 Tutorials and Examples

The van der Waals surface constructed by superposition of atomic spheres is evidently not quite
smooth. Using 0.005 a.u. of electron density isosurface as definition of molecular surface can
produce a map with comparable character, while the color transition and contour lines are much
smoother (after returning to last menu, you can choose option 1, select 1 and then input 0.005 to
switch to this definition).
Note that Multiwfn is also able to plot this kind of map for solid surface, see Section 4 of
http://sobereva.com/589 for illustration.

4.A Special topics and advanced tutorials
The contents in this section involve more than one main functions of Multiwfn, or contain
special usages and skills.

4.A.1 Study variation of electronic structure along IRC path
Note: Chinese version of this section is my blog article “Studying chemical reaction process via curve map of
bond order and anime of ELF/LOL/RDG isosurface” (http://sobereva.com/200), which essentially covers all content
of this section.

In this tutorial, I will briefly show you how to use Multiwfn to study variation of electronic
structure along the IRC path of Diels-Alder adduction. We will study the variation of Mayer bond
order, and will animate the deformation of ELF isosurface. With the similar fashion you can also
easily investigate variation of other properties, such as atomic charges, electron density, aromaticity
and so on.

1043

--- page break ---

4 Tutorials and Examples

Gaussian 09 was used throughout this tutorial. Unless otherwise specified, all calculations will
be performed under Windows 7 64bit system. In this tutorial the files marked by crimson can be
found in "examples\IRC" or "examples" folder.
Before starting this tutorial, you should setup running environment for Gaussian first,
otherwise Gaussian cannot be properly invoked in Windows environment. The setup method is:
Enter “control panel”-“System properties”-“Advanced”, click “Environment variables” button, then
click “New” button in “User variables” frame, input GAUSS_EXEDIR as variable name, input the
install directory of Gaussian as variable value (e.g. D:\study\g09w\, assuming that g09.exe is in this
folder). After that modify "PATH" environment variable to add the install directory of Gaussian into
it.
1 Perform IRC calculation
Run DA_IRC.gjf by Gaussian to produce DA_IRC.out. We will find this IRC path actually
contains 18 and 13 points in the two directions, respectively. B3LYP/6-31+G* is used in this
calculation.
2 Generate wavefunction file for each point of IRC
Write an input file of single point task of Gaussian (DA_SP.gjf), which will be used as
"template" later. The geometry in fact can be arbitrarily filled.
DO NOT write anything here (e.g. %chk)
#p B3LYP/6-31G* nosymm

DA adduction

0 1
C

-0.26156800

1.56679300

0.69509600

C

-0.26156800

1.56679300

-0.69509600

C

0.50031400

-0.43279300

-1.43864500

C

-0.26156800

-1.32826500

-0.70392000

C

-0.26156800

-1.32826500

0.70392000

H

-1.19341600

1.44689700

-1.23752300

H

-1.19341600

1.44689700

1.23752300

H

0.52507600

2.08588200

1.23621300

H

0.38154400

-0.37781200

2.51847500

H

0.38154400

-0.37781200

-2.51847500

H

1.46467600

-0.09643600

-1.07409700

H

-1.04094400

-1.89294000

-1.21418700

H

-1.04094400

-1.89294000

1.21418700

H

1.46467600

-0.09643600

1.07409700

H

0.52507600

2.08588200

-1.23621300

C

0.50031400

-0.43279300

1.43864500

1044

--- page break ---

4 Tutorials and Examples
 blank line
 blank line

Notice that the basis set we used here (6-31G*) is different to the one used in IRC task (631+G*), because Mayer bond order does not work well when diffuse functions are presented. By
the way, ignoring diffuse functions will not lead to detectable change of ELF isosurface. Also note
that the "nosymm" keyword is specified, because if we do not do this Gaussian will automatically
translate and rotate the molecule to put it to standard orientation, which may leads to discontinuity
problem in the animation of ELF (You will see molecule suddenly jumps in certain frames of the
animation).
IRCsplit.exe is a tool used to produce .wfn/.chk file for each point of IRC and SCAN tasks of
Gaussian, IRCsplit.f90 is the corresponding source code, by which you can compile Linux version
of IRCsplit. Boot up IRCsplit.exe by double click its icon and then input
DA_IRC.out //The file of the output file of the IRC task
DA_SP.gjf //The template file used to generate single point input files
2 //Only yield .chk files
C:\DA_IRCchk\DA //The path and prefix of the finally generated .chk files
18,13 // The program detected that in DA_IRC.out there are 18 and 13 points in the two
directions of IRC, respectively. Here we extract all of them, together with the TS point
Now you can find DA_SP0001.gjf, DA_SP0002.gjf ... DA_SP0032.gjf in current folder. Please
manually check one of them to verify the reasonableness of these input files. Note that
DA_SP0014.gjf corresponds to the TS geometry.
Build a new folder "C:\DA_IRCchk" and copy the .gjf files as well as the script runall.bat into
it. Double clicking the icon of runall.bat, which will invoke Gaussian 09 to run all of the .gjf files.
Now you have DA0001.chk, DA0002.chk ... DA0032.chk in "C:\DA_IRCchk" folder. Copy
chk2fch.bat to this folder and run it, then the formchk utility in Gaussian package will be
automatically invoked to convert all .chk files to .fch files.
3 Calculate Mayer bond orders for all IRC points
Mayer bond order of C1-C16 is the one we are particularly interested in, whose formation is
the key process of the DA adduction. Since by default Multiwfn only outputs Mayer bond orders
with value > 0.05, while C1-C16 must be very weak at the initial stage of DA adduction, we need
to set "bndordthres" parameter in the settings.ini file in Multiwfn folder to 0.0, so that all of the
bond orders larger than 0.0 can be outputted.
Write a plain text file (MBObatch.txt) and put it into Multiwfn folder, the content is
9 // Enter bond order analysis module
1 // Calculate Mayer bond order
Note: If you are confused why this file is written in such manner, please read Section 5.2 to study how to run
Multiwfn in silent mode.

Then write a plain text file with .bat suffix (MBObatchrun.bat) and put it into Multiwfn folder,
the content should be
for /f %%i in ('dir C:\DA_IRCchk\*.fch /b') do Multiwfn C:\DA_IRCchk\%%i < MBObatch.txt >
C:\DA_IRCchk\%%~ni.txt

batchrun.bat in fact is a Windows batch script. Double clicking its icon to run it, the .fch files
in "C:\DA_IRCchk\" folder will be sequentially loaded into Multiwfn, and the calculated Mayer
bond orders will be exported to .txt files in the same folder.

1045

--- page break ---

4 Tutorials and Examples
4 Plot Mayer bond order
Now what we should do next is to extract the bond order of C1-C16 from the DA0001.txt,
DA0002.txt ... DA0032.txt. The most convenient way is to utilize "grep" command in Linux. So we
copy all of these .txt files to a folder in Linux system, then in this folder we run
grep "1(C )

16(C )" * > out.txt

The out.txt file now contains C1-C16 bond order of all points in the IRC:
DA0001.txt:#

9:

1(C )

16(C )

0.05929972

DA0002.txt:#

7:

1(C )

16(C )

0.06877306

DA0003.txt:#

7:

1(C )

16(C )

0.07926829

DA0004.txt:#

7:

1(C )

16(C )

0.09089774

DA0005.txt:#

7:

1(C )

16(C )

0.10380144

DA0006.txt:#

7:

1(C )

16(C )

0.11815120

DA0007.txt:#

7:

1(C )

16(C )

0.13417828

DA0008.txt:#

7:

1(C )

16(C )

0.15218555

...
The last column is the values of Mayer bond order of C1-C16, you can plot them by your
favorite program now, you will see
1.1
1.0

C1-C16

0.9

Mayer bond order

0.8
0.7
0.6
0.5
0.4

TS

0.3
0.2
0.1
0.0
0

2

4

6

8

10 12 14 16 18 20 22 24 26 28 30 32
IRC point

Clearly, C1-C16 become stronger and stronger as the reaction proceeds, its Mayer bond order
gradually increases to 1.0 (typical single bond).
With the same method, we also calculate the Mayer bond order of C1-C2 and C4-C5, namely
run below commands
grep "1(C )

2(C )" * > out2.txt

grep "4(C )

5(C )" * > out3.txt

Plot the data in out.txt, out2.txt and out3.txt together, you will see

1046

--- page break ---

4 Tutorials and Examples

TS
2.0
1.8

Mayer bond order

1.6
1.4
1.2
1.0

C1-C16
C1-C2
C4-C5

0.8
0.6
0.4
0.2
0.0
0

2

4

6

8

10 12 14 16 18 20 22 24 26 28 30 32
IRC point

This graph vividly shows that the C1-C2 smoothly becomes to a single bond from a double
bond during the DA adduction, and the reaction increases the double-bond character of C4-C5
significantly.
5 Make animation of ELF isosurface
Next we make animation to study how the ELF isosurface varies during the DA adduction.
Create a plain text file ELFbatch.txt in Multiwfn folder with below content
5 // Generate grid data
9 // ELF
2 // Medium quality grid
2 // Export the grid data to ELF.cub in current folder
Create a script file named ELFbatchrun.bat, whose content is
for /f %%i in ('dir C:\DA_IRCchk\*.fch /b') do (
Multiwfn C:\DA_IRCchk\%%i < ELFbatch.txt
rename ELF.cub %%~ni.cub
)

Run ELFbatchrun.bat, Multiwfn will sequentially load the .fch files in "C:\DA_IRCchk" and
export the corresponding ELF grid data to DA0001.cub, DA0002.cub ... DA0032.cub in current
folder.
We use VMD 1.9.1 program (freely available at http://www.ks.uiuc.edu/Research/vmd/) to
render isosurface for these cube files. Move all of the cube files to VMD folder, and create a plain
text file named isoall.tcl in the VMD folder, the content is
set isoval 0.88
axes location Off
color Display Background white
for {set i 1} {$i<=32} {incr i} {
set name DA[format %04d $i]

1047
