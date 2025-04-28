#include "views.h"

#define ADD_SCOPE(startitr)                                                    \
    ({                                                                         \
        ScopeBlock scopeblock(startitr, scope_depth++);                        \
        if (scopeblock.get_scope_type() == LOOP) {                             \
            scopeblock.set_loop_depth(++loop_depth);                           \
        } else {                                                               \
            scopeblock.set_loop_depth(loop_depth);                             \
        }                                                                      \
        static_scopes.push_back(scopeblock);                                   \
        ScopeBlock *scp = &static_scopes.back();                               \
        /* Add as subscope to all parent scopes */                             \
        for (auto &par : dynamic_scopes) {                                     \
            par->subscopes.push_back(scp);                                     \
        }                                                                      \
        dynamic_scopes.back()->outer_subscopes.push_back(scp);                 \
        /* */                                                                  \
        dynamic_scopes.push_back(&static_scopes.back());                       \
    })

#define REMOVE_SCOPE(enditr)                                                   \
    ({                                                                         \
        ScopeBlock *s = dynamic_scopes.back();                                 \
        s->set_end(enditr);                                                    \
        scope_depth--;                                                         \
        if (s->get_scope_type() == LOOP) {                                     \
            loop_depth--;                                                      \
        }                                                                      \
        dynamic_scopes.pop_back();                                             \
    })

ScopeList WasmModule::gen_scopes_from_instructions(FuncDecl *func) {
    ScopeList static_scopes;
    if (this->isImport(func)) {
        return static_scopes;
    }

    std::list<ScopeBlock *> dynamic_scopes;

    InstList &instructions = func->instructions;

    int scope_depth = 0;
    int loop_depth = 0;
    /* First instruction starts scope */
    ScopeBlock scopeblock(instructions.begin(), scope_depth++);
    scopeblock.set_loop_depth(loop_depth);
    static_scopes.push_back(scopeblock);
    dynamic_scopes.push_back(&static_scopes.back());

    auto last_institr = instructions.begin();
    for (auto institr = instructions.begin(); institr != instructions.end();
         ++institr) {
        InstBasePtr inst = *institr;
        switch (inst->getOpcode()) {
        /* Scope begin instructions */
        case WASM_OP_LOOP:
        case WASM_OP_BLOCK:
        case WASM_OP_IF: {
            ADD_SCOPE(institr);
            break;
        }
        /* Scope end instructions */
        case WASM_OP_END: {
            REMOVE_SCOPE(institr);
            break;
        }
        /* Scope end + start instructions */
        case WASM_OP_ELSE: {
            REMOVE_SCOPE(institr);
            ADD_SCOPE(institr);
            break;
        }
        default: {
        }
        }
        last_institr = institr;
    }

    return static_scopes;
}

ScopeBlock WasmModule::gen_func_static_scope_hierarchy(FuncDecl *func) {
    ScopeList static_scopes;
    if (this->isImport(func)) {
        return ScopeBlock();
    }

    std::list<ScopeBlock *> dynamic_scopes;

    InstList &instructions = func->instructions;

    int scope_depth = 0;
    int loop_depth = 0;
    /* First instruction starts scope */
    ScopeBlock scopeblock(instructions.begin(), scope_depth++);
    scopeblock.set_loop_depth(loop_depth);
    static_scopes.push_back(scopeblock);
    dynamic_scopes.push_back(&static_scopes.back());

    auto last_institr = instructions.begin();
    for (auto institr = instructions.begin(); institr != instructions.end();
         ++institr) {
        InstBasePtr inst = *institr;
        switch (inst->getOpcode()) {
        /* Scope begin instructions */
        case WASM_OP_LOOP:
        case WASM_OP_BLOCK:
        case WASM_OP_IF: {
            ADD_SCOPE(institr);
            break;
        }
        /* Scope end instructions */
        case WASM_OP_END: {
            REMOVE_SCOPE(institr);
            break;
        }
        /* Scope end + start instructions */
        case WASM_OP_ELSE: {
            REMOVE_SCOPE(institr);
            ADD_SCOPE(institr);
            break;
        }
        default: {
        }
        }
        last_institr = institr;
    }

    return static_scopes.front();
}
