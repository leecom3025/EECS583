//===- Transforms/LampInstrumentation.h - Instrumentation passes --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines constructor functions for LAMP instrumentation passes.
// Designed, implemented trmason.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_LAMPINSTRUMENTATION_H
#define LLVM_TRANSFORMS_LAMPINSTRUMENTATION_H

namespace llvm {

  class ModulePass;
  class FunctionPass;
  class LoopPass;

  // tmason
  ModulePass* createLdStCallCounter();

  // tmason
  FunctionPass *createLAMPProfilerPass();

  //tmason
  LoopPass *createLAMPLoopProfilerPass();

  //tmason
  ModulePass *createLAMPInitPass();

  //tmason
  //ModulePass *createLAMPAnnotatePass();	// TRM 08/01/08 may not be permanent placement

}

#endif
