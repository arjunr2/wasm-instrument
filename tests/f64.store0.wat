(module
  (memory 1)
  (func (export "main") (param i32) (result i32)
    (f64.store (i32.const 4002) (f64.const 2.387239457))
    (i32.load8_u (local.get 0))
  )
)
