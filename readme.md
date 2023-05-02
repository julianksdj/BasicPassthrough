# Basic Passthrough
 
 This plugin performs the FFT of the input, then the IFFT of it and copies the result to the output.
 
 The purpose of this project is to provide a template of a JUCE plugin to perform any operations in the frequency domain.
 Using this project the programmer can quickly embbed any frequency domain processing code inside the `processFFT` function.
 
 For the FFT and IFFT operations, fftw3 library is used. fftw3 binary and header files are included inside the project.
 
 FFTW is a C subroutine library for computing the discrete Fourier transform (DFT) in one or more dimensions, of arbitrary input size, and of both real and complex data. The FFTW package was developed at MIT by Matteo Frigo and Steven G. Johnson. More info: https://www.fftw.org/
 
 For more complex processing, you can use the following project, which additionally performs an overlap-add operation: https://github.com/julianksdj/OverlapAdd
