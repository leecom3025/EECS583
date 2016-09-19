
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/ProfileInfo.h"

using namespace llvm;

namespace {
	// fn pass
  struct p1: public FunctionPass {
  static char ID;
  ProfileInfo* PI;
  p1() : FunctionPass(ID) { }

  virtual bool runOnFunction(Function &F) {
    PI = &getAnalysis<ProfileInfo>();

    int countOps = 0;
    float countBBranch = 0;
    float countUBranch = 0;
    float countAlu = 0;
    float countFloat = 0;
    float countMem = 0;
    float countEtc = 0;

    for (Function::iterator b = F.begin(); b != F.end(); b++) {
      float countBranch = 0;
      for (BasicBlock::iterator i = b->begin(); i != b->end(); i++) {
        float count = (PI->getExecutionCount(b) < 0) ? 0 :
          PI->getExecutionCount(b);
        errs() << i->getOpcodeName() << " : " << count << "\n";
        countOps += count;
        switch(i->getOpcode()) {
          /* count branch */
          case Instruction::Br:
          case Instruction::Switch:
          case Instruction::IndirectBr:
            countBranch += count;
            break;

          /* count ALU */
          case Instruction::Add:
          case Instruction::Sub:
          case Instruction::Mul:
          case Instruction::UDiv:
          case Instruction::SDiv:
          case Instruction::URem:
          case Instruction::Shl:
          case Instruction::LShr:
          case Instruction::AShr:
          case Instruction::And:
          case Instruction::Or:
          case Instruction::Xor:
          case Instruction::ICmp:
          case Instruction::SRem:
            countAlu += count;
            break;

          // count floats
          case Instruction::FAdd:
          case Instruction::FSub:
          case Instruction::FMul:
          case Instruction::FDiv:
          case Instruction::FRem:
          case Instruction::FCmp:
            countFloat += count;
            break;

          // count memory
          case Instruction::Alloca:
          case Instruction::Load:
          case Instruction::Store:
          case Instruction::GetElementPtr:
          case Instruction::Fence:
          case Instruction::AtomicCmpXchg:
          case Instruction::AtomicRMW:
            countMem += count;
            break;

          default:
            countEtc += count;
            break;
        } // end switch
      } // end BB forloop

      for (Function::iterator c = F.begin(); c != F.end(); c++) {
        if (countBranch) {
          if (PI->getEdgeWeight(PI->getEdge(b,c))/countBranch > 0.8) {
            countBBranch += countBranch;
          } else {
            countUBranch += countBranch;
          }
        }
      }
    }
    if (countOps == 0.0) {
      countAlu = countFloat = countMem = countBBranch = countUBranch = countEtc = 0;
    }
    errs() << F.getName() << ",";
    errs() << countOps << ",";
    errs() << format("%f,", (countAlu/(float)countOps));
    errs() << format("%f,",(countFloat/(float)countOps));
    errs() << format("%f,",(countMem/(float)countOps));
    errs() << format("%f,",(countBBranch/(float)countOps));
    errs() << format("%f,",(countUBranch/(float)countOps));
    errs() << format("%f\n",(countEtc/(float)countOps)); 

    /* errs() << "   Br: " << countBranch << "\n"; */
    /* errs() << "  ALU: " << countAlu << "\n"; */
    /* errs() << "Float: " << countFloat << "\n"; */
    /* errs() << "  Mem: " << countMem << "\n"; */
    /* errs() << "  Etc: " << countEtc << "\n"; */
 
    return false; // return true if you modified the code
  }

  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<ProfileInfo>();
  }
  };
}

char p1::ID = 0;
static RegisterPass<p1> X("p1", "opcode stats", false, false);

