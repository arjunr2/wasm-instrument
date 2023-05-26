import sys
import numpy as np
from runtime import Wasmtime


wasmtime = Wasmtime()


with open(sys.argv[1], 'rb') as f:
    module_bytes = f.read()

module = wasmtime.create_module(module_bytes, argv=sys.argv[1:])
wasmtime.run_module(module)

data = wasmtime.get_instrumentation(module, prefix="__lc", array=True)
print(len(data), data[data > 0])
import pdb
pdb.set_trace()
