#!/bin/bash
POSITIONAL=()
PARAMETERS=""
while [[ $# -gt 0 ]]
do
key="$1"
case $key in
    -licm)
    echo $FILE
    cd ../Opt-LICM/try2/
    make
	cd ../../compiler
    PARAMETERS=$PARAMETERS" -mem2reg -load ../Opt-LICM/try2/lib/LLVMlicm.so -reachdefs -newlicm"
    shift
    ;;
    -gvn)
    cd ../Opt-GVN/
    make
	cd ../compiler
    PARAMETERS=$PARAMETERS" -mem2reg -load ../Opt-GVN/lib/LLVMgvn.so -globalvn"
    shift
    ;;
    -gcse)
    cd ../Opt-GCSE/
    make
	cd ../compiler
    PARAMETERS=$PARAMETERS" -load ../Opt-GCSE/lib/LLVMav.so -available -gcse"
    shift
    ;;
    -lcse)
    cd ../Opt-LCSE/
    make
	cd ../compiler
    PARAMETERS=$PARAMETERS" -mem2reg -load ../Opt-LCSE/lib/LLVMlcse.so -lcse"
    shift
    ;;
    -loopunroll)
    cd ../Opt-LoopUnrolling/
    make
	cd ../compiler
    PARAMETERS=$PARAMETERS" -load ../Opt-LoopUnrolling/lib/LLVMloopunroll.so -loopunroll"
    shift
    ;;
    -constprop)
    cd ../Opt-ConstantPropagation\&Folding/
    make
	cd ../compiler
    PARAMETERS=$PARAMETERS" -mem2reg -load ../Opt-ConstantPropagation&Folding/lib/LLVMconstfold.so -constfold"
    shift
    ;;
    -tailcallelim)
    cd ../Opt-TRE/
    make
	cd ../compiler
    PARAMETERS=$PARAMETERS" -load ../Opt-TRE/lib/LLVMtail.so -tre"
    shift
    ;;
    -dce)
    cd ../Opt-DCE/try2/
    make
	cd ../../compiler
    PARAMETERS=$PARAMETERS" -mem2reg -load ../Opt-DCE/try2/lib/LLVMdce.so -dcelim"
    shift
    ;;
    -indvarsim)
    cd ../Opt-IndVarElim/
    make
	cd ../compiler
    PARAMETERS=$PARAMETERS" -mem2reg -load ../Opt-IndVarElim/lib/LLVMive.so -indvarelim"
    shift
    ;;
    -fninline)
    cd ../Opt-FunctionInlining/
    make
	cd ../compiler
    PARAMETERS=$PARAMETERS" -load ../Opt-FunctionInlining/lib/LLVMfnin.so -fninlining -always-inline -removefn"
    shift
    ;;
    -f)
    FILE="$2"
    shift
    shift
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
echo "opt -S $PARAMETERS $FILE "
opt -S ${PARAMETERS} ${FILE}
