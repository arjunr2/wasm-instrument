(module
  (memory 1)
  (func (export "main") (param i32) (result i32)
    (f64.load offset=60000 (local.get 0))
    (drop)
    (i32.const 143)
  )
)
