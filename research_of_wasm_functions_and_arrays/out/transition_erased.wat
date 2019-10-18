(module
  (type (;0;) (func (param i32) (result i32)))
  (memory $memory 256)(export "memory" (memory 0))
  (func (;0;) (type 0) (param i32) (result i32)
    (local i32 i32 i32)
    local.get 0
    i32.const -64
    i32.sub
    local.tee 3
    i32.load
    local.set 1
    block  ;; label = @1
      block  ;; label = @2
        block  ;; label = @3
          local.get 0
          i32.load offset=68
          local.tee 2
          br_table 1 (;@2;) 0 (;@3;) 2 (;@1;)
        end
        local.get 1
        i32.const 2
        i32.store
        local.get 1
        i32.const 1
        i32.store8 offset=9
        local.get 1
        i32.const 2
        i32.store8 offset=58
        local.get 1
        i64.const 3
        i64.store offset=104
        local.get 1
        i32.const 4
        i32.store8 offset=116
        local.get 1
        i32.const 5
        i32.store8 offset=165
        local.get 1
        i64.const -1
        i64.store offset=208
        local.get 0
        i32.load offset=68
        local.set 2
        br 1 (;@1;)
      end
      i32.const 1
      return
    end
    block  ;; label = @1
      block  ;; label = @2
        block  ;; label = @3
          local.get 2
          i32.const 2
          i32.sub
          br_table 0 (;@3;) 1 (;@2;) 2 (;@1;)
        end
        local.get 0
        i32.load8_u offset=72
        local.get 0
        i32.const 32
        i32.add
        i32.add
        local.get 0
        i32.load8_s offset=73
        i32.store8
        i32.const 0
        return
      end
      local.get 3
      local.get 1
      i32.const 6216
      i32.add
      local.tee 0
      i32.store
      local.get 0
      i32.const 1
      i32.store
      local.get 1
      i32.const 6224
      i32.add
      i32.const 7
      i32.store8
      local.get 1
      i32.const 6273
      i32.add
      i32.const 77
      i32.store8
      local.get 1
      i32.const 6320
      i32.add
      i64.const 777
      i64.store
      i32.const 0
      return
    end
    i32.const 0)
  (export "transition" (func 0)))
