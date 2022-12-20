(module
  (memory 1)
  (func (export "main") (param i32) (result i32)
    (i32.store16 (i32.const 1000) (i32.const 0x22334455))
    (i32.load (local.get 0))
  )
)
