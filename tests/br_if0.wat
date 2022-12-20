(module
  (func (export "main") (param i32) (result i32)
	(i32.const 46)
	(br_if 0 (local.get 0))
	(drop)
	(i32.const 57)
  )
)