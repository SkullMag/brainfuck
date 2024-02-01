#include "stdio.h"
#include "math.h"
#include <cctype>
#include <memory>
#include <iostream>

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

static std::unique_ptr<llvm::LLVMContext> Context;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::unique_ptr<llvm::Module> Module;
static llvm::AllocaInst *ptrA;
static llvm::AllocaInst *arrA;

int main() {
  Context = std::make_unique<llvm::LLVMContext>();
  Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
  Module = std::make_unique<llvm::Module>("brainfuck", *Context); 

  // Create main function
  std::vector<llvm::Type *> argtypes(0, llvm::Type::getDoubleTy(*Context));
  llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*Context), argtypes, false);
  llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", *Module);

  // Define types
  llvm::Type *ptrTy = llvm::Type::getInt8Ty(*Context);
  llvm::ArrayType *arrTy = llvm::ArrayType::get(ptrTy, 100);

  // Create a basic block for insertion
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*Context, "entry", F);
  Builder->SetInsertPoint(BB);

  // Create ptr variable
  ptrA = Builder->CreateAlloca(ptrTy);
  llvm::Value *ptrV = llvm::ConstantInt::get(ptrTy, 0, false);
  Builder->CreateStore(ptrV, ptrA);

  // Create array variable
  arrA = Builder->CreateAlloca(arrTy, nullptr, "data");
  llvm::Constant *arrC = llvm::ConstantAggregateZero::get(arrTy);
  Builder->CreateStore(arrC, arrA);

  // Return void
  Builder->CreateRetVoid();

  F->print(llvm::errs());

  return 0;
}