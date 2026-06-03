# 5.6 Rapidly load a file into Multiwfn

## Quick navigation

- path: 5  Skills > 5.6 Rapidly load a file into Multiwfn
- pdf pages: 1128-1128
- category: skills
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file for operational tricks such as silent mode, batch mode, clipboard handling, or drawing-quality tips.

## Keywords and aliases

5.6, Rapidly, load, into, Multiwfn

## Extracted manual text

5 Skills

5.6 Rapidly load a file into Multiwfn
Probably sometimes you feel inputting the path of input file is troublesome, especially when
the path is very long. Below I provide you some tricks, which make this step much more easier.
If you want to rapidly load a file into Multiwfn without inputting its path, you can boot up
Multiwfn and then directly drag the icon of the file into the Multiwfn command-line window.
In Windows platform, an even more simple method is directly dragging the file onto the icon
of Multiwfn.exe, then the file will be automatically loaded into Multiwfn. Notice that in this situation,
the "current folder" is the position of the input file.
If directly inputting letter o, the file last time loaded will be loaded again, whose path is
recorded as "lastfile" variable in settings.ini file.
Assume that the file you last time loaded is C:\sob\lover\K-ON\Mio.wfn, and this time you
want to load C:\sob\lover\K-ON\Azusa.wfn, you can simply input ?azusa.wfn, namely the path of
the folder last time involved can be replace with a question mark.

5.7 Make use of cubegen utility in Gaussian package to
reduce computational time of electrostatic potential analyses
When number of your CPU cores is very limited (less than 10), calculation speed of ESP by
internal code of Multiwfn is not as fast as the cubegen utility in Gaussian package. In this
circumstance, you can allow Multiwfn to invoke cubegen to calculate ESP data to reduce the overall

1107
