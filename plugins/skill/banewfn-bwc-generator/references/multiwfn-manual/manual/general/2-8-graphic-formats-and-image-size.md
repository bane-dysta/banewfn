# 2.8 Graphic formats and image size

## Quick navigation

- path: 2  General information > 2.8 Graphic formats and image size
- pdf pages: 70-70
- category: general
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file for setup, prerequisites, file formats, general usage rules, or broad conceptual orientation.

## Keywords and aliases

2.8, Graphic, formats, image, size

## Extracted manual text

2 General information

2.8 Graphic formats and image size
Multiwfn supports a lot of mainstream graphic formats, including:
1 Postscript (ps)
2 Encapsulated postscript (eps)
3 Portable document format (pdf)
4 Windows metafile format (wmf)
5 Graphics interchange format (gif)
6 TIFF (tiff)
7 Portable network graphics (png)
8 Windows bitmap format (bmp)
9 Scalable vector graphics (svg)
The graphic format of the picture exported by Multiwfn is controlled by “graphformat”
parameter in settings.ini, you can set this parameter to the texts in the parentheses listed above, the
default format is “png”.
For curve maps, the height and weight of the image file are controlled by “graph1Dsize”
parameter in settings.ini. “graph2Dsize” is responsible for two-dimension data plotting (color-filled
map, contour map, relief map, etc.). “graph3Dsize“ is responsible for three-dimension data plotting
(isosurface graph, molecular structure graph, etc.).
Tip 1: If the graph is mainly composed of lines, e.g. contour line map and curve map, the best
formats are pdf and svg. However, if you need to embed the resulting graph to Office, commonly
wmf format should be used.
Tip 2: If you want to make background of exported image file transparent, please look this
video illustration: https://youtu.be/E7lAGac3aDM.

2.9 Analysis of periodic systems
Multiwfn is able to deal with periodic systems, details will be given in this section. To analyze
wavefunction for periodic systems, you can use either wavefunction of cluster model produced by
quantum chemistry codes, or use periodic wavefunction produced by CP2K program, as will be
described in Section 2.9.1 and 2.9.2, respectively. There are many analyses in Multiwfn independent
of wavefunction, special attention of applying them to periodic systems will be described in Section
2.9.3.

2.9.1 Wavefunction analysis on wavefunction of cluster model
You can extend primitive cell of the crystal to a large supercell, then extract a cluster from the
supercell. Based on this cluster, you can use any quantum chemistry code to carry out optimization
or single point task and then analyze the resulting wavefunction as usual in Multiwfn. Of course, to
minimize artificial boundary effect due to the finite cluster size, the cluster should be large enough.
If you are not sure what the minimum acceptable size is, you can perform a convergence test for the
result with respect to cluster size. It is important to recognize that electronic structure of boundary

49
