(module
  (table 5 funcref)
  (type $v_i (func (result i32)))
  (func (export "main") (param i32) (result i32)
    (call_indirect (type $v_i) (local.get 0))
  )
)
