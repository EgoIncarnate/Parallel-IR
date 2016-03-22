//===-- llvm/Transforms/Utils/SimplifyIndVar.h - Indvar Utils ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines in interface for induction variable simplification. It does
// not define any actual pass or policy, but provides a single function to
// simplify a loop's induction variables based on ScalarEvolution.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_INDVARSIMPLIFYC_H
#define LLVM_TRANSFORMS_UTILS_INDVARSIMPLIFYC_H

#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ScalarEvolutionExpander.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/SimplifyIndVar.h"

namespace llvm {

  struct RewritePhi;
  class IndVarSimplify : public LoopPass {
    LoopInfo                  *LI;
    ScalarEvolution           *SE;
    DominatorTree             *DT;
    TargetLibraryInfo         *TLI;
    const TargetTransformInfo *TTI;

    SmallVector<WeakVH, 16> DeadInsts;
    bool Changed;
  public:

    static char ID; // Pass identification, replacement for typeid
    IndVarSimplify();

    bool runOnLoop(Loop *L, LPPassManager &LPM) override;

    void getAnalysisUsage(AnalysisUsage &AU) const override;

  private:
    void releaseMemory() override;

    bool isValidRewrite(Value *FromVal, Value *ToVal);

    void handleFloatingPointIV(Loop *L, PHINode *PH);
    void rewriteNonIntegerIVs(Loop *L);

    void simplifyAndExtend(Loop *L, SCEVExpander &Rewriter, LoopInfo *LI);

    bool canLoopBeDeleted(Loop *L, SmallVector<RewritePhi, 8> &RewritePhiSet);
    void rewriteLoopExitValues(Loop *L, SCEVExpander &Rewriter);
    void rewriteFirstIterationLoopExitValues(Loop *L);

    Value *linearFunctionTestReplace(Loop *L, const SCEV *BackedgeTakenCount,
                                     PHINode *IndVar, SCEVExpander &Rewriter);

    void sinkUnusedInvariants(Loop *L);

    Value *expandSCEVIfNeeded(SCEVExpander &Rewriter, const SCEV *S, Loop *L,
                              Instruction *InsertPt, Type *Ty);
  };
};
#endif
