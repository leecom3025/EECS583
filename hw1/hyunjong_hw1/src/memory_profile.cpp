
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/ADT/IndexedMap.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/ProfileInfo.h"
#include "LAMP/LAMPLoadProfile.h"
#include "LAMP/LAMPProfiling.h"
#include <map>
#include <sys/stat.h>

using namespace llvm;

namespace {
  struct p2: public ModulePass {
    static char ID;
    LAMPLoadProfile *LLP;
    ProfileInfo *PI;
    p2() : ModulePass(ID) {} 

    virtual bool runOnModule(Module &M) {
      std::map<unsigned int, unsigned int> DepFrac;
      std::map<std::pair<Instruction*, Instruction*>*, unsigned int>::iterator d;

      PI = &getAnalysis<ProfileInfo>();
      LLP = &getAnalysis<LAMPLoadProfile>();

      // get < loadid : # of deps >
      for (d = LLP->DepToTimesMap.begin(); d != LLP->DepToTimesMap.end(); d++) {
        Instruction *instr1 = d->first->first;
        Instruction *instr2 = d->first->second;
        if(isa<LoadInst>(instr1) && isa<StoreInst>(instr2)) {
          unsigned int loadid = LLP->InstToIdMap[instr1];
          unsigned int count = d->second;
          /* if (DepFrac[loadid]) */ 
          /*   errs() << "load id " << loadid << " incremented!\n"; */
          DepFrac[loadid] += count; 
        }
      }

      // get < loaid : # of deps / # of counts > 
      for (Module::iterator f = M.begin(); f != M.end(); f++) {
        for (Function::iterator b = f->begin(); b != f->end(); b++) {
          for (BasicBlock::iterator i = b->begin(); i != b->end(); i++) {
            if (i->getOpcode() == Instruction::Load) { 
              unsigned int loadid = LLP->InstToIdMap[i];
              float count = PI->getExecutionCount(b);
              float depCount = DepFrac[loadid];
              float percent = depCount / count;

              if (!count || !depCount)
                percent = 0;

              errs() << loadid << "," << format("%f", percent) << "\n";
            }
          }
        }
      }

      return true;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const {
      // fix for required assertion...
      AU.addRequired<LAMPLoadProfile>();
      AU.addRequired<ProfileInfo>();
    }
  };
}

char p2::ID = 0;
static RegisterPass<p2> C("p2", "mem profile...", false, false);

