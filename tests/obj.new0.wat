(module
  (import "weewasm" "obj.new" (func $f (result externref)))
  (func (export "main") (result i32)
    (drop (call $f))
    (i32.const 44)
  )
)
