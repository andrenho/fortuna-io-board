`imagegen.py` will generate an image that can be loaded on the Fortuna I/O Board. To create an image, follow this
procedure.

1. Create the image using the `fortuna.pal` palette file (recommended application: Grafx2)
2. Save image as PNG
3. Call `imagegen.py` to convert the PNG image into a Fortuna image

The image format is as this:

F0 34 -- magic number
01    -- image version (determines also compression type)
xx    -- transparent color (or FF for no transparency)
xx xx -- image width, in pixels
xx xx -- image height, in pixels
..... -- the image pixels (2 pixels per byte)