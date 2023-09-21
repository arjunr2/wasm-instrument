(module
  (func $test (result i32)
    (i32.const 6806)
    (i32.load offset=4)
    (return)
  )
  (memory (;0;) 2 4)
  (data (;2;) (i32.const 6808) "\0a\00\ef\be\ad\de34434")
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
