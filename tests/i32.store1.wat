(module
  (memory 1)
  (func (export "main") (param i32) (result i32)
    (i32.store (local.get 0) (i32.const 0x22334455))
    (i32.load (i32.const 500))
  )
)
