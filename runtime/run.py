import numpy as np
import os
from runtime import Wasmtime
from tqdm.contrib.concurrent import process_map
from argparse import ArgumentParser


_desc = "Run instrumented WebAssembly module"


def _parse(p):
    p.add_argument(
        "-w", "--wasm", nargs='+',
        help="Wasm executable to run; if multiple, runs all.")
    p.add_argument("-i", "--inputs", help="Directory with input files.")
    p.add_argument("-o", "--out", default="data", help="Output directory.")
    p.add_argument(
        "-p", "--prefix", default="lc", help="Instrumentation prefix.")
    p.add_argument(
        "-t", "--threads", default=32, type=int,
        help="Maximum number of threads.")
    return p


def instrument_eval(args):
    """Evaluate instrumentation."""
    wa, s, prefix = args

    # Create wasmtime & load module bytes here for easier multiprocessing;
    # load/inst time is dominated by python overhead from the GIL if using
    # threads, so we use processes instead.
    wasmtime = Wasmtime()
    with open(wa, 'rb') as f:
        module_bytes = f.read()

    module = wasmtime.create_module(module_bytes, argv=[wa, s])
    wasmtime.run_module(module)
    return wasmtime.get_instrumentation(
        module, prefix="__{}".format(prefix), array=True)


def _main(args):
    for wa in args.wasm:
        print(wa)
        files = [os.path.join(args.inputs, x) for x in os.listdir(args.inputs)]
        pmap_args = [(wa, f, args.prefix) for f in files]

        data = np.vstack(
            process_map(instrument_eval, pmap_args, max_workers=args.threads))

        out = os.path.join(
            args.out, os.path.basename(wa).replace(".wasm", ".npz"))
        np.savez_compressed(out, data=data, files=np.array(files))

if __name__ == '__main__':
    args = _parse(ArgumentParser()).parse_args()
    _main(args)
