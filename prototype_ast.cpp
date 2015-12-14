#include "prototype_ast.h"

#include <memory>

PrototypeAST::PrototypeAST(IRBuilder<> *builder,
                           std::shared_ptr<Module> module,
                           const std::string &name,
                           std::vector<std::string> args) :
  _builder(builder),
_module(module),
  _name(name),
  _args(std::move(args))
{}

Function* PrototypeAST::codegen() {
    // Make the function type:  double(double,double) etc.
    std::vector<Type *> ints(_args.size(),
                            Type::getInt64Ty(getGlobalContext()));
    FunctionType *FT =
    FunctionType::get(Type::getInt64Ty(getGlobalContext()), ints, false);
    
    Function *F =
    Function::Create(FT, Function::ExternalLinkage, _name, _module.get());
    
    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &arg : F->args())
        arg.setName(_args[Idx++]);
    
    return F;
}

std::string PrototypeAST::name() const {
    return _name;
}
