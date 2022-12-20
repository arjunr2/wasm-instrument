(module
  (func $test (result f64)
    ;; Store and check value
    (i32.const 6806)
    (f64.const -354.445)
    (f64.store offset=4)
    (i32.const 6806)
    (f64.load offset=4)

    (return)
  )
  (memory (;0;) 2 4)
  (data (;2;) (i32.const 6808) "\0a\00\ef\be\ad\de34434")
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
