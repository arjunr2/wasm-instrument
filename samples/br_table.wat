(module
  (func $test (param i32 i32) (result i32)
    (local i32)
    i32.const 128
    local.set 2
    block  ;; label = @1
      block  ;; label = @2
        block ;; label = @3
          block ;; label = @4
            i32.const 32
            local.get 0
            br_table 3 2 1 0 (;@2;)
            i32.const 5575432
            local.set 2
            br 3 (;@1;)
          end
          i32.const 64
          local.set 2
          br 2
        end
        i32.const 32
        local.set 2
        br 1
      end
      loop  ;; label = @2
        i32.const 16
        local.set 2
        local.get 1
        br_if 1 (;@1;)
        br 0 (;@2;)
      end
      i32.const 8
      local.set 2
    end
    local.get 2
  )
  (export "main" (func $test))
)

