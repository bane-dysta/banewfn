# 5.5 Make command-line window capable to record more outputs

## Quick navigation

- path: 5  Skills > 5.5 Make command-line window capable to record more outputs
- pdf pages: 1127-1127
- category: skills
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file for operational tricks such as silent mode, batch mode, clipboard handling, or drawing-quality tips.

## Keywords and aliases

5.5, Make, command-line, window, capable, record, more, outputs

## Extracted manual text

5 Skills

After you select "Mark", drag left mouse button from point A to point B

Then press ENTER button, the information highlighted by white rectangle will be copied to
clipboard, you can paste them to anywhere, such as plain text file.
For Mac OS or Linux system running in graphical environment, you can also copy the output
of Multiwfn from console to plain text file by similar manner.

5.5 Make command-line window capable to record more
outputs
Occasionally you may find command-line window cannot record entire outputs of Multiwfn.
For example, you select option 6 in wavefunction modification module to get density matrix for a
relative big wavefunction, however only the last part of the matrix can be found in the commandline window. The solution of the problem is to enlarge buffer size of the window, please follow
below steps.
Boot up Multiwfn, click title of the window by right mouse button, click "Properties", select
"Layout" page, you will find the default buffer size of the window is 300 (see the screenshot below),
that means only up to 300 lines can be recorded in the window, which is obviously too small. Change
the value to a larger value, for example 9999, and then click OK button. After that you will find the
window capable to record much more outputs (If the complete output still cannot be recorded,
enlarge buffer size again).
The buffer size setting is saved permanently in system, you needn't to set this value again at
next time you boot up Multiwfn.
For Linux and Mac OS, you can also find similar option used to set buffer size of terminal.

1106
