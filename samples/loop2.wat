(module
  (func $test (param f64 i32) (result f64)
    (local f64)
    (f64.const 1)
    (local.set 2)

    (loop
      (local.set 2 (f64.add (local.get 0) (local.get 2)))
      (i32.sub (local.get 1) (i32.const 1))
      (local.tee 1)
      (br_if 0)
    )
    (local.get 2)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
