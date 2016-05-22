COMP304 Project 3, Spring 2016, Koç University
Written by Meric Melike Softa and Firtina Kucuk on May, 2016

This project implements a simulation for a custom-made file system in C.
We have a very simple shell interface in kufsMain.c, which reads the user
input for commands and executes them within the kufs file system if applicable.
The supported commands are those given in the problem statement and those
we were asked to implement.

The implementations for display, create and rm commands can be found in kufs.c.
The code is mostly commented and it is stated where we differ from the implementations
in kufs.h that we took as basis. No changes in kufs.h were made.
