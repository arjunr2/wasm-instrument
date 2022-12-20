(module
  (func $test (result f64)
    (global.get 0)
    (i32.const 42)
    (i32.add)
    (f64.convert_i32_u)

    (global.get 1)
    (f64.const -43.11)
    (f64.add)

    (global.get 2)
    (i32.const 35)
    (i32.add)
    (f64.convert_i32_u)

    (f64.mul)
    (f64.add)

    (return)
  )
  (memory (;0;) 2 4)
  (global (;0;) (mut i32) (i32.const 22))
  (global (;1;) f64 (f64.const 3408))
  (global (;2;) i32 (i32.const 5))
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
