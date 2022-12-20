(module
  (import "weewasm" "puti" (func $puti (param i32)))
  (func (export "main") (result i32)
    (call $puti (i32.const 46))
    (call $puti (i32.const 93))
    (call $puti (i32.const 1111))
    (i32.const 666)
  )
)
