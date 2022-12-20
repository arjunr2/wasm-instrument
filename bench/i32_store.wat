(module
  (func $test (result i32)
    ;; Store and check value
    (i32.const 6806)
    (i32.const -559038737)
    (i32.store8 offset=4)
    (i32.const 6806)
    (i32.load offset=4)

    ;; Store and check value
    (i32.const 6802)
    (i32.const -559038737)
    (i32.store16 offset=4)
    (i32.const 6802)
    (i32.load offset=4)

    ;; Store and check value
    (i32.const 6810)
    (i32.const -559038737)
    (i32.store offset=4)
    (i32.const 6810)
    (i32.load offset=4)

    (i32.add)
    (i32.add)

    (return)
  )
  (memory (;0;) 2 4)
  (data (;2;) (i32.const 6808) "\0a\00\ef\be\ad\de34434")
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
