(module
  (type (;0;) (func (param i32) (result i32)))
  (import "env" "memory" (memory (;0;) 256 256))
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
        i32.load offset=4
        i32.const 1
        i32.store8 offset=1
        local.get 1
        i32.load offset=4
        i32.const 2
        i32.store8 offset=50
        local.get 1
        i32.load offset=4
        local.tee 2
        i64.const 3
        i64.store offset=96
        local.get 2
        i32.const 4
        i32.store8 offset=108
        local.get 1
        i32.load offset=4
        i32.const 5
        i32.store8 offset=157
        local.get 1
        i32.load offset=4
        i64.const -1
        i64.store offset=200
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
      i32.const 6220
      i32.add
      local.tee 0
      i32.load
      i32.const 7
      i32.store8
      local.get 0
      i32.load
      i32.const 77
      i32.store8 offset=49
      local.get 0
      i32.load
      i64.const 777
      i64.store offset=96
      i32.const 0
      return
    end
    i32.const 0)
  (export "_transition" (func 0)))
