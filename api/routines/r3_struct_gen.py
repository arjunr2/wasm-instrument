import re

def entry_gen(match):
    secs = match.split('_')
    signed_v = "UNSIGNED" if secs[-1] == 'U' else "SIGNED"
    atom_v = 'true' if 'ATOMIC' in secs else 'false'

    if secs[0] in ['I32', 'I64', 'F32', 'F64']:
        size_ret = int(secs[0][1:])
        for sec in secs[1:]:
            if any(sec.startswith(x) for x in ['RMW', 'LOAD', 'STORE']):
                width = re.split('[^\d]+', sec)[-1]
                size_v = size_ret if width == '' else int(width)
    else:
        dct = {
            'MEMORY_SIZE': 0,
            'MEMORY_GROW': 0,
            'MEMORY_INIT': -1,
            'MEMORY_COPY': -1,
            'MEMORY_FILL': -1,
            'MEMORY_ATOMIC_NOTIFY': 0,
            'MEMORY_ATOMIC_WAIT32': 0,
            'MEMORY_ATOMIC_WAIT64': 0,
            'ATOMIC_FENCE': 0
        }
        if match in dct:
            size_v = dct[match]
        else:
            print(f"!!! Unknown match: {match}")

    return f"[WASM_OP_{match}] = {{ {size_v//8}, {signed_v}, {atom_v} }},"

def main():
    # Open the file for reading
    with open('r3_memops.h', 'r') as file:
        # Read the contents of the file
        contents = file.read()

    # Use regex to find lines starting with "#define WASM_OP_" and capture the word following it
    pattern = r'#define WASM_OP_(\w+)'
    matches = re.findall(pattern, contents)

    h_header = [
        '#ifndef R3_MEMOPS_TABLE_H',
        '#define R3_MEMOPS_TABLE_H',
        '',
        '#include \"wasmops.h\"',
        ''
    ]
    signed_enum = [
        'typedef enum {',
        '\tUNSIGNED,',
        '\tSIGNED',
        '} Signed;',
        ''
    ]
    accentry_struct = [
        'typedef struct {',
        '\tint64_t size;',
        '\tSigned sign;',
        '\tbool atomic;',
        '} MemopProp;',
        ''
    ]
    extern = [
        'extern MemopProp memop_inst_table[];'
    ]

    c_header = [
        '#include <stdbool.h>',
        '#include <stdint.h>',
        '#include \"r3_memops_table.h\"',
    ]
    table = [
        'MemopProp memop_inst_table[] = {'
    ]
    # Print the captured words
    entries = ["\t" + entry_gen(x) for x in matches]
    hfile = h_header + signed_enum + accentry_struct + extern + ['#endif']
    cfile = c_header + table + entries + \
        ['};']

    # Write the captured words to a file
    with open('r3_memops_table.c', 'w') as file:
        file.write('\n'.join(cfile))

    with open('r3_memops_table.h', 'w') as file:
        file.write('\n'.join(hfile))


if __name__ == '__main__':
    main()