#include "stdio.h"
#include "math.h"
#include <cctype>
#include <memory>
#include <iostream>
#include <fstream>

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

// LLVM stuff
static std::unique_ptr<llvm::LLVMContext> Context;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::unique_ptr<llvm::Module> Module;

// Types
static llvm::Type *ptrTy;
static llvm::Type *arrTy;
static llvm::FunctionType *printfTy;

// alloca instances of runtime
static llvm::AllocaInst *ptrA;
static llvm::AllocaInst *arrA;

// std runtime functions
static llvm::Function *printfFunc;


static llvm::Function *initRuntime() {
  // Create main function
  llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(*Context), {}, false);
  llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", *Module);

  // Define types
  ptrTy = llvm::Type::getInt8Ty(*Context);
  arrTy = llvm::ArrayType::get(ptrTy, 100);

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

  llvm::BasicBlock *StartBB = llvm::BasicBlock::Create(*Context, "start", F);
  Builder->CreateBr(StartBB);
  Builder->SetInsertPoint(StartBB);
  return F;
}

static void initLLVM() {
  Context = std::make_unique<llvm::LLVMContext>();
  Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
  Module = std::make_unique<llvm::Module>("brainfuck", *Context);
}

static void incPtr() {
  llvm::LoadInst *tmp = Builder->CreateLoad(ptrA->getAllocatedType(), ptrA);
  llvm::Value *inc = Builder->CreateAdd(tmp, llvm::ConstantInt::get(ptrTy, 1));
  Builder->CreateStore(inc, ptrA);
}

static void decPtr() {
  llvm::LoadInst *tmp = Builder->CreateLoad(ptrA->getAllocatedType(), ptrA);
  llvm::Value *inc = Builder->CreateSub(tmp, llvm::ConstantInt::get(ptrTy, 1));
  Builder->CreateStore(inc, ptrA);
}

static void incByte() {
  llvm::LoadInst *ptrValue = Builder->CreateLoad(ptrTy, ptrA);
  llvm::Value *gep = Builder->CreateGEP(ptrTy, arrA, ptrValue);
  llvm::Value *add = Builder->CreateAdd(Builder->CreateLoad(ptrTy, gep), llvm::ConstantInt::get(ptrTy, 1));
  Builder->CreateStore(add, gep);
}

static void decByte() {
  llvm::LoadInst *ptrValue = Builder->CreateLoad(ptrTy, ptrA);
  llvm::Value *gep = Builder->CreateGEP(ptrTy, arrA, ptrValue);
  llvm::Value *add = Builder->CreateSub(Builder->CreateLoad(ptrTy, gep), llvm::ConstantInt::get(ptrTy, 1));
  Builder->CreateStore(add, gep);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Specify a filename to compile" << std::endl;
    return 1;
  }
  initLLVM();
  llvm::Function *F = initRuntime();
  std::ifstream ifile(argv[1]);

  // Read file and build IR
  char c;
  while (ifile.get(c)) {
    switch (c)
    {
    case '>':
      incPtr();
      break;
    case '<':
      decPtr();
      break;
    case '+':
      incByte();
      break;
    case '-':
      decByte();
      break;
    default:
      break;
    }
  }

  // Return void
  Builder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Context), 0));

  llvm::verifyFunction(*F);
  llvm::verifyModule(*Module);
  F->print(llvm::errs());

  return 0;
}