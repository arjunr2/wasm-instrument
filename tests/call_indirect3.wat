(module
  (table funcref (elem $fadd $fsub $fmul $fnone))
  (type $ii_i (func (param i32 i32) (result i32)))
  
  (func $fadd (param i32 i32) (result i32) (i32.add (local.get 0) (local.get 1)))
  (func $fsub (param i32 i32) (result i32) (i32.sub (local.get 0) (local.get 1)))
  (func $fmul (param i32 i32) (result i32) (i32.mul (local.get 0) (local.get 1)))
  (func $fnone (result i32) unreachable)

  (func $main (export "main") (param i32 i32 i32) (result i32)
    (call_indirect (type $ii_i) (local.get 0) (local.get 1) (local.get 2))
  )
)
