(module
  (table funcref (elem $main))
  (type $v_i (func (result i32)))
  (func $main (export "main") (param i32) (result i32)
    (call_indirect (type $v_i) (local.get 0))
  )
)
