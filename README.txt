COpt : A High Level Domain-Specific Language to Generate Compiler Optimizers

The project files can be found in /lib/Transforms/COpt

This is a language to help compiler writers generate compiler optimizers. A program in COpt once compiled generates a file called Optimiser.cpp in some location in the directory structure which depends on the COpt program. This file can be executed to optimize LLVM IR code.

Pre-requisites : 
  -C++11 
  -LLVM version 4.0.0 (built from source) 
  -For compilation of LLVM libraries : GNU Make 3.79/3.79.1, GCC>=4.8.0, python >=2.7 and zlib >=1.2.3.4 
  -The host platforms allowed for the deployment of this project are :
      1. Operating systems - Linux, Solaris, FreeBSD, NetBSD, MacOS X, Windows 
      2. Architectures - x86, amd64, ARM, PowerPC, V9, x86-64 
      3. Compilers - GCC, Clang, Visual Studio 
      4. LLVM - version 4.0.0 ( other versions may or may not be compatible )

Platform used for development of this project : ubuntu 16.04 LTS, 64-bit

Running a program in the language COpt :

    Compiling : compile /<coptinputfile.txt>
    Executing : run /<llvmirinputfile.ll> -

The following two lines need to be added to the end of .bashrc file - 
    alias compile='./_coptCompiler.sh'
    alias run='./_coptRun.sh -f'

Directory Structure :

      -README This file

      +LLVM Source directories

      /Files pertaining to this project/

      +/lib/Transforms/COpt

      +compiler 
            +inputs 
                  This directory contains .txt files which are input COpt programs 
            +tests 
                  This directory contains .ll files which is the IR that is fed as input to the optimiser and then optimised
            -coptast.c 
                  This file contains code for the creation of the abstract syntax tree 
            -coptast.h 
                  This is the header file for the creation of the abstract syntax tree 
            -_coptCompiler.sh 
                  This is an implementation file which is a shell script that runs on the command compile 
            -coptlexer.l 
                  This is the lexical analyzer for COpt 
            -coptparser.y 
                  This is the parser for COpt 
            -_coptRun.sh 
                  This is an implementation file which is a shell script that runs on the command run 
            -coptsemantic.c 
                  This is the semantic analyzer for COpt 
            -coptsemantic.h 
                  This is the header file for semantic analysis -makefileCompile This make file runs the commands for making the compiler

      +metrictests 
            This directory contains the tests that were used to measure the performance of the generated compiler optimizers

      +Opt-ConstantPropagation&Folding 
            +source 
                Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for constant folding and propagation 
            +tests 
                This directory contains .ll files to test constant folding and propagation

      +Opt-DCE 
            +try1 
                This directory contains unused code 
            +try2 
                +source 
                      Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for dead code elimination 
                +tests 
                      This directory contains .ll files to test dead code elimination

      +Opt-FunctionInlining 
           +source 
                Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for function inlining 
           +tests 
                This directory contains .ll files to test function inlining

      +Opt-GCSE 
            +source 
                Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for global common subexpression elimination 
            +tests 
                This directory contains .ll files to test global common subexpression elimination

      +Opt-GVN 
            +source 
                Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for global value numbering 
            +tests 
                This directory contains .ll files to test global value numbering

      +Opt-IndVarElim 
            +source 
                Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for induction variable simlification 
            +tests 
                This directory contains .ll files to test induction variable simlification

      +Opt-LCSE 
            +source 
                Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for local common subexpression elimination 
            +tests 
                This directory contains .ll files to test local common subexpression elimination

      +Opt-LICM 
            +try1 
                This directory contains unused code 
            +try2 
                +source 
                    Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for loop invariant code motion 
                +tests 
                    This directory contains .ll files to test loop invariant code motion

      +Opt-LoopUnrolling 
            +source 
                  Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for loop unrolling 
            +tests 
                  This directory contains .ll files to test loop unrolling

      +Opt-TRE 
            +source 
                  Optimiser.cpp is generated here. This directory also contains the original LLVM C++ code for tail recursion elimination 
            +tests 
                  This directory contains .ll files to test tail recursion elimination
