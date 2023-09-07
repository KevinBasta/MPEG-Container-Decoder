# MPEG CONTAINER DECODER
"MPEG" is a video container file type. It contains various nested "boxes" or "atoms" that contain different information depending on the type of video format that it contains. The goal of this project is to parse MP4 files according to the ISO/IEC MPEG-4/QuickTime specifications in order to learn about the MPEG container format.

This codebase was originally started under the now called "GIF-Encoder" repository. You can find project history prior to the creation of this repository [here](https://github.com/KevinBasta/GIF-Encoder/commits/main?after=5f10115814d05d87492a712fda2792d9018a0ab9+69&branch=main&qualified_name=refs%2Fheads%2Fmain). 

### This project is in progress.

### Project Status: 
- MPEG-4 parsing, decoding, and processing is currently supported and working
- There are currently no plans to support the AVC/H.264 video format, the AVC files are left in the project because the bit utility functions I made for parsing some of the AVC data types were interesting and took effort to implement. You can find these functions in "AVCMath.c"
