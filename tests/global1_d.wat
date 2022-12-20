(module
  (global $g f64 (f64.const -77.5))
  (func (export "main") (result f64)
    (global.get $g)
  )
)
