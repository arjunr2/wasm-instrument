#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>

#include "inst_internal.h"
#include "ir.h"

typedef enum {
    custom_id = 0,
    type_id,
    import_id,
    function_id,
    table_id,
    memory_id,
    global_id,
    export_id,
    start_id,
    element_id,
    code_id,
    data_id,
    datacount_id
} section_id;

static void dump_bytedeque(bytedeque &bdeq, char *outfile) {
    FILE *file = fopen(outfile, "wb");
    if (!file) {
        throw std::runtime_error("Unable to open file-path");
    }
    std::vector<byte> vec(bdeq.begin(), bdeq.end());
    fwrite(vec.data(), 1, vec.size(), file);
    fclose(file);
}

/* Write LimitsType */
inline static void write_limits(bytedeque &bdeq, wasm_limits_t &limits) {
    WR_BYTE((limits.flag << 1) | limits.has_max);
    WR_U32(limits.initial);
    if (limits.has_max) {
        WR_U32(limits.max);
    }
}

/* Write MemoryType */
inline static void write_memtype(bytedeque &bdeq, MemoryDecl &mem) {
    write_limits(bdeq, mem.limits);
}

/* Write TableType */
inline static void write_tabletype(bytedeque &bdeq, TableDecl &table) {
    WR_BYTE(table.reftype);
    write_limits(bdeq, table.limits);
}

/* Write GlobalType */
inline static void write_globaltype(bytedeque &bdeq, GlobalDecl &global) {
    WR_BYTE(global.type);
    WR_BYTE(global.is_mutable);
}

static void write_type_list(bytedeque &bdeq, TypeList &tlist) {
    WR_U32(tlist.size());
    for (auto &type : tlist)
        WR_BYTE(type);
}

static void encode_const_off_expr(bytedeque &bdeq, Opcode_t opc,
                                  uint32_t offset) {
    WR_OPCODE(opc);
    switch (opc) {
    case WASM_OP_I32_CONST: {
        WR_I32(offset);
        break;
    }
    case WASM_OP_I64_CONST: {
        WR_I64(offset);
        break;
    }
    default:
        ERR("Unknown opcode in offset expr (%d): Must be i32/64 const\n", opc);
        throw std::runtime_error("Opcode error");
    }
    WR_OPCODE(WASM_OP_END);
}

// TODO: Only init expr bytes now
static void encode_init_expr(bytedeque &bdeq, bytearr &init_expr) {
    WR_BYTESTR(init_expr);
}

bytedeque WasmModule::encode_type_section() {
    bytedeque bdeq;

    auto &sigs = this->sigs;
    uint32_t num_sigs = sigs.size();
    if (num_sigs == 0) {
        return {};
    }

    WR_U32(num_sigs);
    for (auto &sig : sigs) {
        WR_BYTE(WASM_TYPE_FUNC);
        /* For params */
        write_type_list(bdeq, sig.params);
        write_type_list(bdeq, sig.results);
    }

    return bdeq;
}

bytedeque WasmModule::encode_import_section() {
    bytedeque bdeq;

    auto &imports = this->imports;
    uint32_t total_imports = imports.get_num_imports();
    if (imports.list.size() != total_imports) {
        throw std::runtime_error("Import list size and counts don't match\n");
    }

    if (total_imports == 0) {
        return {};
    }

    WR_U32(total_imports);
    for (auto &import : imports.list) {
        WR_NAME(import.mod_name);
        WR_NAME(import.member_name);
        WR_BYTE(import.kind);
        switch (import.kind) {
        case KIND_FUNC: {
            uint32_t idx = this->getSigIdx((import.desc.func)->sig);
            WR_U32(idx);
            break;
        }
        case KIND_TABLE: {
            write_tabletype(bdeq, *import.desc.table);
            break;
        }
        case KIND_MEMORY: {
            write_memtype(bdeq, *import.desc.mem);
            break;
        }
        case KIND_GLOBAL: {
            write_globaltype(bdeq, *import.desc.global);
            break;
        }
        default:
            ERR("Unknown Import Kind: %d | %d\n", this->getImportIdx(&import),
                import.kind);
            throw std::runtime_error("Unknown Kind");
        }
    }

    return bdeq;
}

bytedeque WasmModule::encode_function_section() {
    bytedeque bdeq;

    auto &imports = this->imports;

    auto &funcs = this->funcs;
    uint32_t num_funcs = funcs.size() - imports.num_funcs;
    if (num_funcs == 0) {
        return {};
    }

    WR_U32(num_funcs);
    for (auto itr = std::next(funcs.begin(), imports.num_funcs);
         itr != funcs.end(); ++itr) {
        uint32_t idx = this->getSigIdx(itr->sig);
        WR_U32(idx);
    }
    return bdeq;
}

bytedeque WasmModule::encode_table_section() {
    bytedeque bdeq;

    auto &imports = this->imports;

    auto &tables = this->tables;
    uint32_t num_tables = tables.size() - imports.num_tables;
    if (num_tables == 0) {
        return {};
    }

    WR_U32(num_tables);
    for (auto itr = std::next(tables.begin(), imports.num_tables);
         itr != tables.end(); ++itr) {
        write_tabletype(bdeq, *itr);
    }
    return bdeq;
}

bytedeque WasmModule::encode_memory_section() {
    bytedeque bdeq;

    auto &imports = this->imports;
    auto &mems = this->mems;
    uint32_t num_mems = mems.size() - imports.num_mems;
    if (num_mems == 0) {
        return {};
    }

    WR_U32(num_mems);
    for (auto itr = std::next(mems.begin(), imports.num_mems);
         itr != mems.end(); ++itr) {
        write_memtype(bdeq, *itr);
    }

    return bdeq;
}

bytedeque WasmModule::encode_global_section() {
    bytedeque bdeq;

    auto &imports = this->imports;

    auto &globals = this->globals;
    uint32_t num_globals = globals.size() - imports.num_globals;
    if (num_globals == 0) {
        return {};
    }

    WR_U32(num_globals);
    for (auto itr = std::next(globals.begin(), imports.num_globals);
         itr != globals.end(); ++itr) {
        write_globaltype(bdeq, *itr);
        encode_init_expr(bdeq, itr->init_expr_bytes);
    }
    return bdeq;
}

bytedeque WasmModule::encode_export_section() {
    bytedeque bdeq;

    auto &exports = this->exports;
    uint32_t num_exports = exports.size();
    if (num_exports == 0) {
        return {};
    }

    WR_U32(num_exports);
    for (auto &exp : exports) {
        WR_NAME(exp.name);
        WR_BYTE(exp.kind);
        uint32_t idx;
        switch (exp.kind) {
        case KIND_FUNC:
            idx = this->getFuncIdx(exp.desc.func);
            break;
        case KIND_TABLE:
            idx = this->getTableIdx(exp.desc.table);
            break;
        case KIND_MEMORY:
            idx = this->getMemoryIdx(exp.desc.mem);
            break;
        case KIND_GLOBAL:
            idx = this->getGlobalIdx(exp.desc.global);
            break;
        default:
            ERR("Export kind: %u\n", exp.kind);
            throw std::runtime_error("Invalid export type");
        }
        WR_U32(idx);
    }

    return bdeq;
}

bytedeque WasmModule::encode_start_section() {
    bytedeque bdeq;
    if (this->start_fn) {
        TRACE("NOTE: Start section not fully complete\n");
        uint32_t start_idx = this->getFuncIdx(this->start_fn);
        WR_U32(start_idx);
    }
    return bdeq;
}

bytedeque WasmModule::encode_element_section() {
    bytedeque bdeq;

    auto &elems = this->elems;
    uint32_t num_elems = elems.size();
    if (num_elems == 0) {
        return {};
    }

    WR_U32(num_elems);
    for (auto &elem : elems) {
        WR_U32(elem.flag);
        switch (elem.flag) {
        case 0:
            encode_const_off_expr(bdeq, elem.opcode_offset, elem.table_offset);
            break;
        default: {
            ERR("Invalid element segment flag: %d\n", elem.flag);
            throw std::runtime_error("Flag error");
        }
        }

        WR_U32(elem.funcs.size());
        for (auto &func : elem.funcs) {
            uint32_t idx = this->getFuncIdx(func);
            WR_U32(idx);
        }
    }
    return bdeq;
}

static void encode_locals(bytedeque &bdeq, wasm_localcsv_t &pure_locals) {
    WR_U32(pure_locals.size());
    for (auto &local_cse : pure_locals) {
        WR_U32(local_cse.count);
        WR_BYTE(local_cse.type);
    }
}

#define USE_INSTLIST 1
void WasmModule::encode_expr_to_insts(bytedeque &bdeq, InstList &instlist,
                                      bytearr &code_bytes) {
#if USE_INSTLIST
    for (auto &instruction : instlist) {
        Opcode_t opcode = instruction->getOpcode();
        // TRACE("O: %s\n", opcode_table[opcode].mnemonic);
        /* Write instruction opcode and immediate */
        WR_OPCODE(opcode);
        instruction->encode_imm(*this, bdeq);
    }
#else
    WR_BYTESTR(code_bytes);
#endif
}

bytedeque WasmModule::encode_code(FuncDecl &func) {
    bytedeque bdeq;
    encode_locals(bdeq, func.pure_locals);
    encode_expr_to_insts(bdeq, func.instructions, func.code_bytes);
    return bdeq;
}

bytedeque WasmModule::encode_code_section() {
    bytedeque bdeq;

    auto &imports = this->imports;

    auto &funcs = this->funcs;
    uint32_t num_funcs = funcs.size() - imports.num_funcs;
    if (num_funcs == 0) {
        return {};
    }

    WR_U32(num_funcs);
    for (auto itr = std::next(funcs.begin(), imports.num_funcs);
         itr != funcs.end(); ++itr) {
        /* For each non-import func, generate code and its size */
        auto secdeq = encode_code(*itr);
        WR_SECLEN_PRE();
        bdeq.insert(bdeq.end(), secdeq.begin(), secdeq.end());
    }
    return bdeq;
}

bytedeque WasmModule::encode_data_section() {
    bytedeque bdeq;

    auto &datas = this->datas;
    uint32_t num_datas = datas.size();

    if (num_datas == 0) {
        return {};
    }
    WR_U32(num_datas);
    for (auto &data : datas) {
        WR_U32(data.flag);
        switch (data.flag) {
        case 0:
            encode_const_off_expr(bdeq, data.opcode_offset, data.mem_offset);
            break;
        case 1:
            break;
        case 2: {
            WR_U32(this->getMemoryIdx(data.mem));
            encode_const_off_expr(bdeq, data.opcode_offset, data.mem_offset);
            break;
        }
        default: {
            ERR("Invalid data segment flag: %d\n", data.flag);
            throw std::runtime_error("Flag error");
        }
        }
        /* Write raw bytes for the data*/
        WR_U32(data.bytes.size());
        WR_BYTESTR(data.bytes);
    }

    return bdeq;
}

bytedeque WasmModule::encode_datacount_section() {
    bytedeque bdeq;
    if (this->has_datacount) {
        WR_U32(this->datas.size());
    }
    return bdeq;
}

bytedeque WasmModule::encode_custom_section(CustomDecl &custom) {
    bytedeque bdeq;

    if (custom.name.empty() && custom.bytes.empty()) {
        return {};
    }
    WR_NAME(custom.name);

    if (custom.name == "name") {
        DebugNameDecl &debug = custom.debug;
        /* We have to write non-function subsections and function subsection
         * (id: 1) in the correct order for faithful validation */
        auto subsec = debug.subsections.begin();
        /* Non-function subsections before function subsection: Just write
         * section info */
        for (; (subsec != debug.subsections.end()) && (subsec->id < 1);
             ++subsec) {
            WR_BYTE(subsec->id);
            WR_U32(subsec->bytes.size());
            WR_BYTESTR(subsec->bytes);
        }
        /* Function subsection */
        if (!debug.func_assoc.empty()) {
            bytedeque secdeq;
            {
                bytedeque bdeq;
                WR_U32(debug.func_assoc.size());
                for (auto &name_asc : debug.func_assoc) {
                    uint32_t idx = this->getFuncIdx(name_asc.func);
                    WR_U32(idx);
                    WR_NAME(name_asc.name);
                }
                secdeq = bdeq;
            }
            WR_SECLEN_PRE();
            WR_SECBYTE_PRE(0x01);
            bdeq.insert(bdeq.end(), secdeq.begin(), secdeq.end());
        }
        /* Non-function subsections after function subsection: Just write
         * section info */
        for (; subsec != debug.subsections.end(); ++subsec) {
            WR_BYTE(subsec->id);
            WR_U32(subsec->bytes.size());
            WR_BYTESTR(subsec->bytes);
        }
    }
    /* Non-name sections: Just write bytes */
    else {
        WR_BYTESTR(custom.bytes);
    }
    return bdeq;
}

/* Main Encoder routine */
bytedeque WasmModule::encode_module(char *outfile) {
    bytedeque bdeq;
    DEF_TIME_VAR();

    TRACE("<=== Encoding module ===>\n");
    WR_U32_RAW(this->magic);
    WR_U32_RAW(this->version);

#define ENCODE_CALL(sec, ...)                                                  \
    {                                                                          \
        TIME_SECTION(                                                          \
            1, "Time for " #sec,                                               \
            auto secdeq = this->encode_##sec##_section(__VA_ARGS__);           \
            uint32_t section_size =                                            \
                secdeq.size(); /* Push section information */                  \
            if (section_size) {                                                \
                WR_SECLEN_PRE();                                               \
                WR_SECBYTE_PRE(sec##_id);                                      \
                bdeq.insert(bdeq.end(), secdeq.begin(), secdeq.end());         \
            })                                                                 \
    }
    ENCODE_CALL(type);
    ENCODE_CALL(import);
    ENCODE_CALL(function);
    ENCODE_CALL(table);
    ENCODE_CALL(memory);
    ENCODE_CALL(global);
    ENCODE_CALL(export);
    ENCODE_CALL(start);
    ENCODE_CALL(element);
    ENCODE_CALL(datacount);
    ENCODE_CALL(code);
    ENCODE_CALL(data);
    /* Write all the customs at the end */
    for (auto &custom : this->customs) {
        ENCODE_CALL(custom, custom);
    }

    if (outfile != NULL) {
        TIME_SECTION(1, "Time for file write", dump_bytedeque(bdeq, outfile););
        printf("Wrote module to \"%s\"\n", outfile);
    }
    return bdeq;
}
