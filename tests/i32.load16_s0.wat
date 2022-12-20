(module
  (memory 1)
  (data (i32.const 1104) "x\9901")
  (func (export "main") (param i32) (result i32)
    (i32.load16_s (local.get 0))
  )
)
