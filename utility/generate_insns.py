import re
from pathlib import Path

def snake2camelinst(string):
    return ''.join([x.capitalize() for x in string.split('_')]) + "Inst"

def _main():
    script_dir = Path(__file__).parent;
    with open(script_dir / ".." / "opcode_table.c") as f:
        print("#include \"inst_internal.h\"\n")
        pattern = re.compile(r"\s*\[WASM_OP_(.*)\]\s*=\s*{(.*)}");
        for line in f.readlines():
            match = pattern.match(line)
            if match:
                insn_name = snake2camelinst(match.group(1))
                args = match.group(2).split(',')
                imm_name = "ImmNoneInst" if len(args) < 2 else \
                    snake2camelinst(args[1].lstrip().rstrip())
                print(f"typedef {imm_name} {insn_name};");

if __name__ == '__main__':
    _main();
