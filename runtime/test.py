import sys
import numpy as np
import wasmtime
from wasmtime import Store, Module, Instance, Linker, WasiConfig, Engine


engine = Engine()
linker = Linker(engine)
linker.define_wasi()

with open(sys.argv[1], 'rb') as f:
    module_bytes = f.read()
module = Module(engine, module_bytes)

config = WasiConfig()
config.argv = sys.argv[1:]
config.preopen_dir(".", ".")

store = Store(linker.engine)
store.set_wasi(config)
instance = linker.instantiate(store, module)

main = instance.exports(store)["_start"]
main(store)


def __bugfix(x):
    return x.value if isinstance(x, wasmtime.Val) else x


exported = {
    k: __bugfix(v.value(store))
    for k, v in instance.exports(store)._extern_map.items()
    if k.startswith('_lc')
}

data = np.array(sorted([exported[k] for k in sorted(exported.keys())]))
print(len(data), data[data > 0])
