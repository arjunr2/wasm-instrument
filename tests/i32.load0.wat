(module
  (memory 1)
  (func (export "main") (param i32) (result i32)
    (i32.load offset=0 (local.get 0))
  )
)
