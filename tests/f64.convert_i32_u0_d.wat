(module
  (func (export "main") (param i32) (result f64)
    (f64.convert_i32_u (local.get 0))
  )
)
