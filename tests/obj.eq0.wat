(module
  (import "weewasm" "obj.eq" (func $f (param externref externref) (result i32)))
  (func (export "main") (result i32)
    (local $f externref)
    (call $f (local.get $f) (local.get $f))
  )
)
