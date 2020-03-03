# RLE-Image-Compression
Program that can compress image with run-length-encoding algorithme

# Compilation
To compile the project and obtain the binary excutable, simply run
<br>$make

# Compilation
To run the program, simply run 
<br>$make run
<br>by default, it open the program with the file can_bottom2.ppm of the img folder
<br> Or 
<br>$./exec XXX
<br> where XXX is the path to ppm image file
<br> Or
<br>$make run FILE=XXX
<br> where XXX is the path to ppm image file
<br>Once launched, right click on the picture to open the menu, several option are possible
<br> such as "quit", "save"

# Compatibility
 Make, glut.h are needed to run makefile,
 the program does not run on windows environnement, (due to glut.h dependencies that I'm to lazy to fix).
 <br> Seriously, who use windows?
 
