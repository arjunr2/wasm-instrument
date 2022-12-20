(module
  (import "weewasm" "putd" (func $putd (param f64)))
  (func (export "main") (result i32)
    (call $putd (f64.const 1.098765))
    (call $putd (f64.const -1.333333))
    (call $putd (f64.const -333.444444))
    (i32.const 1122)
  )
)
