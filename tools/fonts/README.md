`fontgen.py` will generate Fortuna fonts out of images. The font needs to be organized in a grid of 32x8, and the 
encoding is CP437.

A good strategy to create a Fortuna font from an existing (TTF or FON font) is:

1. Install the font
2. Modify `cp437-chart.html` to use the desired font
3. Open the HTML and take a screenshot
4. Adjust the screenshot so that the grid is correct
5. Generate the font using `fontgen.py`. The resulting H file can be imported into the project.