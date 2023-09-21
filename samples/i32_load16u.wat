(module
  (func $test (result i32)
    (i32.const 6806)
    (i32.load16_u offset=8)
    (return)
  )
  (memory (;0;) 2 4)
  (data (;2;) (i32.const 6808) "\0a\00\ef\be\ad3\de4\7d4au\02\43")
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
