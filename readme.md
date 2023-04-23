# Basic Passthrough
 
 This plugin performs the FFT of the input, then the IFFT of it and copies the result to the output.
 
 The purpose of this project is to provide a template of a JUCE plugin to perform any operations in the frequency domain.
 Using this project the programmer can quickly embbed any frequency domain processing code inside the `processFFT` function.
 
 For the FFT and IFFT operations, fftw3 library is used. fftw3 binary and header files are included inside the project.
