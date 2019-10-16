(module
  (type $t0 (func (result i32)))
  (type $t1 (func))
  (type $t2 (func (param i32 i32 i32) (result i32)))
  (type $t3 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)))
  (import "env" "io_get_stdout" (func $io_get_stdout (type $t0)))
  (func $__wasm_call_ctors (type $t1))
  (func $_start (type $t1)
    call $io_get_stdout
    drop)
  (func $cwa_main (type $t1)
    call $io_get_stdout
    drop)
  (func $memset (type $t2) (param $p0 i32) (param $p1 i32) (param $p2 i32) (result i32)
    (local $l3 i32) (local $l4 i32)
    i32.const 0
    local.set $l3
    block $B0
      block $B1
        loop $L2
          local.get $l3
          local.get $p2
          i32.ge_u
          br_if $B1
          local.get $p0
          local.get $l3
          i32.add
          local.tee $l4
          i32.eqz
          br_if $B0
          local.get $l4
          local.get $p1
          i32.store8
          local.get $l3
          i32.const 1
          i32.add
          local.set $l3
          br $L2
        end
      end
      local.get $p0
      return
    end
    unreachable
    unreachable)
  (func $resume (type $t1)
    unreachable
    unreachable)
  (func $Transition (type $t3) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 i32) (param $p8 i32) (param $p9 i32) (param $p10 i32) (param $p11 i32) (param $p12 i32) (param $p13 i32) (param $p14 i32) (param $p15 i32) (param $p16 i32) (param $p17 i32) (param $p18 i32) (param $p19 i32) (param $p20 i32) (param $p21 i32) (param $p22 i32) (param $p23 i32) (param $p24 i32) (param $p25 i32) (param $p26 i32) (param $p27 i32) (param $p28 i32) (param $p29 i32) (param $p30 i32) (param $p31 i32) (param $p32 i32) (param $p33 i32) (param $p34 i32) (param $p35 i32)
    block $B0
      local.get $p34
      i32.const 2
      i32.lt_u
      br_if $B0
      block $B1
        local.get $p33
        i32.load8_u offset=1
        i32.const 1
        i32.ne
        br_if $B1
        i32.const -1
        local.set $p33
        loop $L2
          local.get $p33
          i32.const 1
          i32.add
          local.tee $p33
          i32.const 32
          i32.lt_u
          br_if $L2
        end
      end
      local.get $p0
      i32.const 0
      i32.store offset=40
      local.get $p0
      i64.const 0
      i64.store offset=32 align=4
      local.get $p0
      local.get $p1
      i32.store8
      local.get $p0
      local.get $p2
      i32.store8 offset=1
      local.get $p0
      local.get $p3
      i32.store8 offset=2
      local.get $p0
      local.get $p4
      i32.store8 offset=3
      local.get $p0
      local.get $p5
      i32.store8 offset=4
      local.get $p0
      local.get $p6
      i32.store8 offset=5
      local.get $p0
      local.get $p7
      i32.store8 offset=6
      local.get $p0
      local.get $p8
      i32.store8 offset=7
      local.get $p0
      local.get $p9
      i32.store8 offset=8
      local.get $p0
      local.get $p10
      i32.store8 offset=9
      local.get $p0
      local.get $p11
      i32.store8 offset=10
      local.get $p0
      local.get $p12
      i32.store8 offset=11
      local.get $p0
      local.get $p13
      i32.store8 offset=12
      local.get $p0
      local.get $p14
      i32.store8 offset=13
      local.get $p0
      local.get $p15
      i32.store8 offset=14
      local.get $p0
      local.get $p16
      i32.store8 offset=15
      local.get $p0
      local.get $p17
      i32.store8 offset=16
      local.get $p0
      local.get $p18
      i32.store8 offset=17
      local.get $p0
      local.get $p19
      i32.store8 offset=18
      local.get $p0
      local.get $p20
      i32.store8 offset=19
      local.get $p0
      local.get $p21
      i32.store8 offset=20
      local.get $p0
      local.get $p22
      i32.store8 offset=21
      local.get $p0
      local.get $p23
      i32.store8 offset=22
      local.get $p0
      local.get $p24
      i32.store8 offset=23
      local.get $p0
      local.get $p25
      i32.store8 offset=24
      local.get $p0
      local.get $p26
      i32.store8 offset=25
      local.get $p0
      local.get $p27
      i32.store8 offset=26
      local.get $p0
      local.get $p28
      i32.store8 offset=27
      local.get $p0
      local.get $p29
      i32.store8 offset=28
      local.get $p0
      local.get $p30
      i32.store8 offset=29
      local.get $p0
      local.get $p31
      i32.store8 offset=30
      local.get $p0
      local.get $p32
      i32.store8 offset=31
      return
    end
    unreachable
    unreachable)
  (table $T0 1 1 funcref)
  (memory $memory 16)
  (global $g0 (mut i32) (i32.const 65536))
  (global $__heap_base i32 (i32.const 65536))
  (global $__data_end i32 (i32.const 65536))
  (global $__dso_handle i32 (i32.const 65536))
  (export "memory" (memory 0))
  (export "__wasm_call_ctors" (func $__wasm_call_ctors))
  (export "__heap_base" (global 1))
  (export "__data_end" (global 2))
  (export "__dso_handle" (global 3))
  (export "_start" (func $_start))
  (export "cwa_main" (func $cwa_main))
  (export "memset" (func $memset))
  (export "resume" (func $resume))
  (export "Transition" (func $Transition)))
