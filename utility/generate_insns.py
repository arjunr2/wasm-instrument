import re
import json
from pathlib import Path

def snake2camelinst(string):
    return ''.join([x.capitalize() for x in string.split('_')]) + "Inst"

def generate_class_def(insn_op, insn_name, imm_name, insn_format):
    #print(f"typedef {imm_name} {insn_name};");
    prototype = insn_format[imm_name]
    if not prototype:
        constructor = f"{insn_name}() : {imm_name}({insn_op}) {{ }}"
    else:
        params_types = ','.join(prototype)
        params_notypes = ','.join([x.split()[-1] for x in prototype])
        constructor = f"{insn_name}({params_types})"\
                        f" : {imm_name}({insn_op}, {params_notypes}) {{ }}"
    lines = [
        f"class {insn_name}: public {imm_name} {{",
        f"\tpublic:",
        f"\t" + constructor,
        f"}};\n"
        ]
    print('\n'.join(lines))


def _main():
    script_dir = Path(__file__).parent;
    with open(script_dir / "insn_format.json") as f:
        insn_format = json.load(f)
    print(insn_format)

    with open(script_dir / ".." / "opcode_table.c") as f:
        print("#include \"inst_internal.h\"\n")
        pattern = re.compile(r"\s*\[WASM_OP_(.*)\]\s*=\s*{(.*)}");
        for line in f.readlines():
            match = pattern.match(line)
            if match:
                insn_op = "WASM_OP_" + match.group(1)
                insn_name = snake2camelinst(match.group(1))
                args = match.group(2).split(',')
                imm_name = "ImmNoneInst" if len(args) < 2 else \
                    snake2camelinst(args[1].lstrip().rstrip())
                generate_class_def(insn_op, insn_name, imm_name, insn_format)

if __name__ == '__main__':
    _main();
