# 4.A.15 Calculating information-theoretic quantities and some relevant quantities

## Quick navigation

- path: 4  Tutorials and Examples > 4.A Special topics and advanced tutorials > 4.A.15 Calculating information-theoretic quantities and some relevant quantities
- pdf pages: 1121-1121
- category: special-topics
- direct child sections: 0
- total descendant sections: 0

## When to consult this file

Use this file when the user is trying to choose between multiple Multiwfn methods, needs an overview article, or wants a higher-level comparison before reading a specific function section.

## Keywords and aliases

4.A.15, Calculating, information-theoretic, quantities, some, relevant

## Extracted manual text

4 Tutorials and Examples

Current effect is already nice, however, the overlapping character between hole and electron
cannot be clearly recognized. In order to improve the effect, we enter "Graphics" - "Representation",
set "Material" to "EdgyGlass", then choose "electron.cub" in "Selected Molecules", also set its
"Material" to "EdgyGlass". Next, choose "File" - "Render" - "Tachyon", then click "Start
Rendering". If this time we directly use VMDrender_full.bat to render the graph, you will find the
graph is too dark. In order to get best effect, we open VMDrender_full.bat using text editor, change
the "-trans_raster3d" to "-trans_vmd", and then add an argument "-shadow_filter_off". Finally we
execute this .bat file to yield full.bmp, which is shown below, the effect is perfect! (Note that I used
Photoshop to increase the brightness of the graph by 20)

4.A.15 Calculating information-theoretic quantities and some relevant
quantities
Prof. Shubin Liu proposed many information-theoretic quantities and applied them to a wide
variety of chemical problems, and got many valuable findings. Multiwfn is able to calculate all
information-theoretic quantities. There is a document "Calculating information-theoretic quantities
and some relevant quantities by Multiwfn" in "Resources" page of Multiwfn website dedicated to
describe how to use Multiwfn to calculate these quantities, please check it.

1100
