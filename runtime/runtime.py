"""Runtime wrapper."""

import numpy as np

from beartype.typing import NamedTuple, Union
from wasmtime import (
    Store, Module, Linker, WasiConfig, Engine, Instance, Global)


class WasmtimeModule(NamedTuple):
    """Wasmtime module state with WASI."""

    store: Store
    inst: Instance


class Wasmtime:
    """Wasmtime instrumentation executor."""

    def __init__(self):
        self.engine = Engine()
        self.linker = Linker(self.engine)
        self.linker.define_wasi()

    def create_module(
        self, wasm: bytes, dir: tuple[str, str] = (".", "."),
        argv: list[str] = []
    ) -> WasmtimeModule:
        """Create module using wasmtime."""
        config = WasiConfig()
        config.argv = argv
        config.preopen_dir(dir[0], dir[1])

        store = Store(self.engine)
        store.set_wasi(config)
        inst = self.linker.instantiate(store, Module(self.engine, wasm))

        return WasmtimeModule(store, inst)

    def run_module(self, module: WasmtimeModule) -> None:
        """Run wasmtime module."""
        main = module.inst.exports(module.store)["_start"]
        main(module.store)

    def get_instrumentation(
        self, module: WasmtimeModule, prefix="__lc", array=True
    ) -> Union[dict, np.ndarray]:
        """Extract instrumentation from module."""
        values = {
            k: v.value(module.store)
            for k, v in module.inst.exports(module.store)._extern_map.items()
            if k.startswith(prefix) and isinstance(v, Global)
        }
        if array:
            return np.array([values[k] for k in sorted(values.keys())])
        else:
            return values
