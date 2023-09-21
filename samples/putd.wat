(module
  (import "weewasm" "putd" (func $putd (param i32)))
  (func $test (param i32) (result i32)
    (local.get 0)
    (call $putd)
    (i32.const 42)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
