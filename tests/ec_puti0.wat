(module
  (import "weewasm" "puti" (func $puti (param i32)))
  (func (export "main") (result i32)
    (call $puti (i32.const 46))
    (i32.const 47)
  )
)
