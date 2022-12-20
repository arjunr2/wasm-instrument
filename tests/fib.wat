(module
 (export "main" (func $fib))
 (func $fib (param $n i32) (result i32)
  (block
    (br_if 0 (i32.ge_s
      (local.get $n)
      (i32.const 2)
    ))
   (return
    (i32.const 1)
  ))
  (return
   (i32.add
    (call $fib
     (i32.sub
      (local.get $n)
      (i32.const 2)
     )
    )
    (call $fib
     (i32.sub
      (local.get $n)
      (i32.const 1)
     )
    )
   )
  )
 )
)