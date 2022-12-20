(module
  (import "weewasm" "puts" (func $puts (param i32 i32)))
  (memory 1)
  (data (i32.const 1024) "Hello World!\n")
  (func (export "main") (result i32)
    (call $puts (i32.const 1024) (i32.const 13))
    (i32.const 57)
  )
)
