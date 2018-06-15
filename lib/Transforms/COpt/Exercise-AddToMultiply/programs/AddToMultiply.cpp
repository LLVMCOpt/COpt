#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
/**/ //#include "llvm/IR/LegacyPassManager.h"
/**/ //#include "llvm/IR/InstrTypes.h"
/**/ //#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
/**/ //#include "llvm/Transforms/Utils/BasicBlockUtils.h"
using namespace llvm;

namespace {
  struct AddToMultiply : public FunctionPass {								//FunctionPass -> Pass.h
    static char ID;
    AddToMultiply() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
    /**/ errs() << "In here -la la \n "; int ch=0; F.dump();
      for (auto &B : F) {				/**/B.dump();									//F -> Function.h
        for (auto &I : B) {				/**/I.dump();
          if (auto *op = dyn_cast<BinaryOperator>(&I)) {
          	/**/ errs() <<"OP - "<< *op<<"\n";
            // Insert at the point where the instruction `op` appears.
            IRBuilder<> builder(op);

            // Make a multiply with the same operands as `op`.
            Value *lhs = op->getOperand(0);
            Value *rhs = op->getOperand(1);
            Value *mul = builder.CreateMul(lhs, rhs);
			/**/ errs() <<"lhs - "<< *lhs<<"\n";
			/**/ errs() <<"rhs - "<< *rhs<<"\n";
			/**/ errs() <<"mul - "<< *mul<<"\n";
			
            // Everywhere the old instruction was used as an operand, use our
            // new multiply instruction instead.
            for (auto &U : op->uses()) {
              User *user = U.getUser();  // A User is anything with operands.
              user->setOperand(U.getOperandNo(), mul);
            }

            // We modified the code.
            /**/ch = 1; //return true;
          }
        }
      }
	/**/	if (ch==1) return true;
      return false;
    }
  };
}

char AddToMultiply::ID = 0;
static RegisterPass<AddToMultiply> X("atm", "Add to Multiply Pass");

