# 4.A.10 Intuitively exhibiting atomic properties by coloring atoms

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.10 Intuitively exhibiting atomic properties by coloring atoms
- pdf pages: 1096-1097
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.10, Intuitively, exhibiting, atomic, properties, coloring, atoms

## Extracted manual text

4 Tutorials and Examples
coordinate in this file should be consistent with optimized dimer coordinate.
(5) Load the dimer .chg file into Multiwfn, enter main function 7 and select option -2, then
input atom list of the two monomers in turn, the exciton coupling energy will be printed immediately.

4.A.10 Intuitively exhibiting atomic properties by coloring atoms
There is a very useful way of intuitively exhibiting atomic properties calculated by Multiwfn,
namely coloring atoms in VMD program (http://www.ks.uiuc.edu/Research/vmd/), here I illustrate
how to do that via two examples. More detailed discussions and examples can be found in my blog
article "Using Multiwfn+VMD to exhibit atomic charges, spin populations, charge transfer and
condensed Fukui function via coloring atoms" (http://sobereva.com/425).
(1) Coloring atoms according to atomic charges
First, I illustrate how to use this manner to vividly represent atomic charges of polyyne. This
system was also involved in Section 4.13.6.
The first step is calculating atomic charges. Boot up Multiwfn and input below commands:
examples\polyyne.wfn
7 // Population analysis
11 // ADCH charge (this type of charge is generally recommended)
1
y // Export atomic coordinates and atomic charges to polyyne.chg in current folder
Now reboot Multiwfn, then input
polyyne.chg
100 // Other functions (Part 1)
2 // Export new file
1 // The format of the new file is .pqr
polyyne.pqr
Now we have polyyne.pqr in current folder. The .pqr format is very similar to the popular .pdb
format, the major difference is that in the .pqr format the last two columns are specific for recording
atomic charges and atomic radii, respectively. In current file, the atomic charges correspond to the
polyyne ADCH charges, while the atomic radii correspond to Bondi van der Waals radii.
The .pqr file can be recognized by VMD. We boot up VMD, then drag the polyyne.pqr into
VMD main window to load it. After that, we modify plotting settings:
(a) Use white background: Inputting color Display Background white in VMD console
window
(b) Modifying drawing style and coloring setting: Enter "Graphics" - "Representation", set the
drawing method as "CPK", set the coloring method as "Charge". Then choose "Trajectory" tab, input
-0.4 and 0.4 in the "Color Scale Data Range" text boxes and press ENTER button.
(c) Changing color transition style: Enter "Graphics" - "Colors", choose "Color Scale" tab,
change the default RWB to BWR (Blue-White-Red)
(e) Choose "Display" - "Orthographic" to use orthographic perspective.
Now you will see below graph in VMD OpenGL window. The atoms at both ends are hydrogen,
all the other atoms are carbon.

1075

--- page break ---

4 Tutorials and Examples

In above graph, the red and blue colors reflect that the atom has positive and negative charge,
respectively. The deeper red (blue) the more positive (negative) the charge. As can be seen, since
carbon has larger electronegativity than hydrogen, the two hydrogens have evident positive charge
and the carbons bonded to them have evident negative charge. The white color indicates that the
charge of the carbons in the middle region of the molecule is close to zero.
The polyyne is a highly conjugated system, it is expected that external field could significantly
polarize its charge distribution. To study this problem, we use the same procedure to plot the map
based on examples\polyyne_field.wfn, which was generated under 0.03 a.u. external electric field
along molecular axis. The resulting graph is shown below, the direction of the external electric field
is from the right side to the left side.

It can be seen that the atomic charge distribution is no longer symmetric. Since the source of
the field is at right side, large amounts of electrons transferred from left to right, as a result, the net
charge of the carbon atoms at left side become positive, while the ones at right side become negative.
(2) Coloring atoms according to atomic contribution to molecular orbitals
The atomic coloring method is not only able to be employed to exhibit atomic charges, but can
also be used to exhibit other atomic properties. As an example, I illustrate how to represent atomic
contributions to molecular orbitals by coloring atoms, examples\N-phenylpyrrole.fch is taken as
example molecule.
First, we calculate orbital composition of an orbital. Boot up Multiwfn and input
examples\N-phenylpyrrole.fch
8 // Orbital composition
3 // SCPA method
36 // Select MO 36 as example
Then we copy all atomic contributions from the Multiwfn window to a text file using the
method described in Section 5.4.
0 // Return
-10 // Return to main menu
100 // Other functions (Part 1)
2 // Export new file
2 // The format of the new file is xyz, because .xyz is very similar to .chg
N-phenylpyrrole.chg // Name of the new file
Now manually modify the N-phenylpyrrole.chg by your favorable text editor (Ultraedit is
recommended), delete the first two lines, and copy the orbital composition to the last column using
column mode, then save the file. Finally, the content of the N-phenylpyrrole.chg should be
C

-0.00000000

1.12162908

1.82507914

0.396870

C

-0.00000000

0.71310006

3.13457424

0.099898

C

-0.00000000

-0.71310006

3.13457424

0.099898

...[ignored]

1076
