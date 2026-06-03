# 3.3 Outputting all properties at a point (1)

## Quick navigation

- path: 3  Functions > 3.3 Outputting all properties at a point (1)
- pdf pages: 80-80
- category: functions
- main menu / option numbers mentioned in title: 1
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file for the actual Multiwfn function description: menu number, theory, options, inputs, and detailed behavior.

## Keywords and aliases

3.3, properties, at, point

## Extracted manual text

3 Functions
ENTER button, the bond length, angle, and dihedral will be returned.
• Batch plotting orbitals: Via this tool you can very conveniently save a lot of selected orbitals
to respective image file in current folder, see https://youtu.be/SHwrQhqBHZ0 for video illustration.
• Select fragment: After selecting it and input an atom index, the whole fragment where the
atom attributes will be highlighted, and the indices of all atoms in the fragment will be returned.
This is useful when you perform some analyses, in which you need to input atom indices in the
fragment of interest.
• Get atom indices of a given element: Input an element name, program will return indices of
all atoms corresponding to this element.
• Print XYZ coordinates in Angstrom: X, Y, Z coordinates of all atoms with Å as unit will be
shown in console window.
• Print fractional coordinates: Fractional coordinates of all atoms will be shown in console
window. This option is available only when cell information is available.
• Export all internal coordinates: This option exports all bonds, angles, and dihedrals of present
system to int_coord.txt in current folder. If you want to use this function with noGUI version of
Multiwfn, after entering main menu, you can input geomparm, then all internal coordinates can be
exported to specific file.
More functions and illustration of the use of the main function 0 are described in Section 4.0.
Information needed: GTFs (only for viewing orbitals), atom coordinates, grid data (only for
viewing isosurface of grid data)

3.3 Outputting all properties at a point (1)
Input coordinate of a point or index of an atom, then values of all real space functions supported
by Multiwfn at the point or corresponding nuclear position will be printed on screen, as well as each
component of gradient and Hessian matrix of the specified function (default is electron density). If
the specified function is electron, Multiwfn also outputs stress tensor, its eigenvalues and
eigenvectors, as well as stress tensor stiffness and stress tensor polarizability.
The function to be studied can be specified by f?, for example f9 select ELF, which is real space
function 9. You can input allf to list all available functions. The orbital of which the wavefunction
value will be outputted can be selected by command o?, for example o4 choose the 4th orbital.
If the input file merely contains atom coordinates (such as pdb file), then only limited functions
based on electron density will be outputted, the electron density used is promolecular density
constructed from fitted free atom density, see the introduction of real space function 14 in Section
2.6.
If input d, value of a real space function at a given point can be decomposed into orbital
contributions. The contribution from orbital i is evaluated as follows: First set occupation number
of all orbitals to zero except for orbital i, and then calculate real space function as usual. Note that
sum of contribution of all orbitals may be different to the result when all orbitals are simultaneously
into account, since many real space functions contain nonlinear operators, such as ELF.
Electrostatic potential is the most expensive one among all of the real space functions supported

59
