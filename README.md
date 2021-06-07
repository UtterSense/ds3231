# ds3231 - Shared Library
INSTRUCTIONS FOR USE
--------------------

The source code in this repo may be used for building a shared library for
i)  Linux system
    The ANDROID_APP define in ds3231.h is commented out so as to ignore Android specific code.
    We can reference the ds3231.c/ds3231.h files from any other application with its
    makefile to create the .so file (and calling executable if required).
        

ii) Android system 
    The Android.mk makefile included in the repo can be used within the AOSP tree
    (or use similar for NDK build). 
    We create another shared library which is a jni wrapper for the ds3231 code.
    The ANDROID_APP define is included ib ds3231.h to ensure Android/JNI specific code is compiled.


   
 
