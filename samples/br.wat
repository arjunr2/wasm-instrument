(module
  (func $test (param i32 f64 i32) (result f64)
    (local i32 f64)
    (f64.const -25.43)
    (local.tee 4)
    (local.get 1)
    (f64.add)
    (br 0)
    block
      i32.const 2
      i32.const 3
      i32.add
      drop
    end
    (f64.const 32.34)
    (f64.mul)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
