#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"

namespace rubinius {

  Module* Module::create(STATE) {
    Module* mod = state->new_object<Module>(G(module));

    mod->name(state, (Symbol*)Qnil);
    mod->superclass(state, (Module*)Qnil);

    mod->setup(state);

    return mod;
  }

  Module* Module::allocate(STATE, Object* self) {
    Module* module = Module::create(state);

    module->klass(state, (Class*)self);

    return module;
  }

  void Module::setup(STATE) {
    constants(state, LookupTable::create(state));
    method_table(state, MethodTable::create(state));
  }

  void Module::setup(STATE, const char* str, Module* under) {
    setup(state, state->symbol(str), under);
  }

  void Module::setup(STATE, Symbol* name, Module* under) {
    if(!under) under = G(object);

    this->constants(state, LookupTable::create(state));
    this->method_table(state, MethodTable::create(state));
    this->name(state, name);
    under->set_const(state, name, this);
  }

  void Module::set_name(STATE, Module* under, Symbol* name) {
    if(under == G(object)) {
      this->name(state, name);
    } else {
      String* cur = under->name()->to_str(state);
      String* str_name = cur->add(state, "::")->append(state,
          name->to_str(state));

      this->name(state, str_name->to_sym(state));
    }
  }

  void Module::set_const(STATE, Object* sym, Object* val) {
    bool found;
    Object* obj = constants_->fetch(state, sym, &found);

    if(found) {
      LookupTableAssociation* assoc = as<LookupTableAssociation>(obj);
      assoc->value(state, val);
    } else {
      LookupTableAssociation* assoc =
        LookupTableAssociation::create(state, sym, val);

      constants_->store(state, sym, assoc);
    }
  }

  void Module::set_const(STATE, const char* name, Object* val) {
    set_const(state, state->symbol(name), val);
  }

  LookupTableAssociation* Module::get_const_association(STATE, Symbol* sym, bool* found) {
    Object* assoc = constants_->fetch(state, sym, found);
    if(*found) {
      return as<LookupTableAssociation>(assoc);
    } else {
      return reinterpret_cast<LookupTableAssociation*>(Qnil);
    }
  }

  Object* Module::get_const(STATE, Symbol* sym, bool* found) {
    Object* assoc = constants_->fetch(state, sym, found);
    if(*found) {
      return as<LookupTableAssociation>(assoc)->value();
    } else {
      return assoc;
    }
  }

  Object* Module::get_const(STATE, Symbol* sym) {
    bool found;
    return get_const(state, sym, &found);
  }

  Object* Module::get_const(STATE, const char* sym) {
    return get_const(state, state->symbol(sym));
  }

  void Module::Info::show(STATE, Object* self, int level) {
    Module* mod = as<Module>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); mod->name()->show(state, level);
    indent_attribute(level, "superclass"); class_info(state, mod->superclass(), true);
    indent_attribute(level, "constants"); mod->constants()->show(state, level);
    indent_attribute(level, "method_table"); mod->method_table()->show(state, level);
    close_body(level);
  }

  IncludedModule* IncludedModule::create(STATE) {
    IncludedModule* imod;
    imod = state->new_object<IncludedModule>(G(included_module));

    imod->name(state, state->symbol("<included module>"));
    imod->superclass(state, (Module*)Qnil);

    return imod;
  }

  IncludedModule* IncludedModule::allocate(STATE, Object* self) {
    IncludedModule* imod = IncludedModule::create(state);

    imod->klass(state, (Class*)self);

    return imod;
  }

}
