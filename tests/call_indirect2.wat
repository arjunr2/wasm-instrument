(module
  (table funcref (elem $f11 $f22 $f33 $f44 $main))
  (type $v_i (func (result i32)))
  
  (func $f11 (result i32) (i32.const 11))
  (func $f22 (result i32) (i32.const 22))
  (func $f33 (result i32) (i32.const 33))
  (func $f44 (result i32) (i32.const 44))
  
  (func $main (export "main") (param i32) (result i32)
    (call_indirect (type $v_i) (local.get 0))
  )
)
