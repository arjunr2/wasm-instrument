(module
  (import "weewasm" "putd" (func $putd (param f64)))
  (func (export "main") (result i32)
    (call $putd (f64.const 1.098765))
    (i32.const 6677)
  )
)
