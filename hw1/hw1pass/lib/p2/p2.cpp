
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
      PI = &getAnalysis<ProfileInfo>();
      LLP = &getAnalysis<LAMPLoadProfile>();

      /*
       *  std::map<unsigned int, Instruction*> IdToInstMap; // Inst* -> InstId
       *  std::map<Instruction*, unsigned int> InstToIdMap; // InstID -> Inst*
       *  std::map<std::pair<Instruction*, Instruction*>*, unsigned int> 
       *      DepToTimesMap
       */
      float ld_total = 0;
      for (Module::iterator f = M.begin(); f != M.end(); f++) {
        for (Function::iterator b = f->begin(); b != f->end(); b++) {
          for (BasicBlock::iterator i = b->begin(); i != b->end(); i++) {
            /* errs() << b << ": " << LLP->InstToIdMap[b] << "\n"; */
            if (i->getOpcode() == Instruction::Load) { 
              float t = PI->getExecutionCount(b);
              ld_total += t;
            }
          }
        }
      }

      unsigned int MaxLoadId = 0;
      std::map<unsigned int, unsigned int> DepFrac;
      std::map<std::pair<Instruction*, Instruction*>*, unsigned int>::iterator d;
      for (d = LLP->DepToTimesMap.begin(); d != LLP->DepToTimesMap.end(); d++) {
        Instruction *instr1 = d->first->first;
        Instruction *instr2 = d->first->second;
        /* errs() << instr1->getOpcodeName() << " depends " <<
         * instr2->getOpcodeName() << "\n"; */
        if(isa<LoadInst>(instr1) && isa<StoreInst>(instr2)) {
          unsigned int loadid = LLP->InstToIdMap[instr1];
          unsigned int count = d->second;

          DepFrac[loadid] += count; 

          if (loadid > MaxLoadId) 
            MaxLoadId = loadid;

          /* errs() << "< Ld[" << loadid << "], St[" << */  
          /*   LLP->InstToIdMap[instr2] << "] > : " << count << "\n"; */
        }
      }

      /* errs() << "=============================================\n"; */

      for (unsigned int i = 0; i < MaxLoadId + 1; i++) {
        float percent = DepFrac[i]/ld_total;
        errs() << i << format(",%f\n", percent);
      }

      return false;
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

