set auto_enable_reverse     off
set autoruncassettes        off
set fullspeedwhenloading    on
set maxframeskip            100000
set minframeskip            100000
set mute                    on
set renderer                none
set save_settings_on_exit   off
set sound_driver            null
set throttle                off

set previousMotorRunning    0
set previousTapePosition    0
set stoppedTime             0

set RAM0000Used             0
set RAM4000Used             0
set RAM8000Used             0
set RAMC000Used             0

set activeCondition         0
set memoryChecksStarted     0
set stateCaptured           0
set invalidPSGDirections    0

#set method METHOD_motor_stop
#set method METHOD_1s_after_motor_stop
set method METHOD_last_slot_change_after_motor_stop


proc captureState {} {

    set ::saved_CPU_reg_16 {}
    lappend ::saved_CPU_reg_16 [reg af] [reg af2] [reg bc] [reg bc2] [reg de] [reg de2] [reg hl] [reg hl2] [reg ix] [reg iy] [reg pc] [reg sp]

    set ::saved_CPU_reg_8 {}
    lappend ::saved_CPU_reg_8 [reg i] [reg im] [reg iff]

    set ::saved_Port_A8         [debug read_block {ioports} 0xA8 1]
    set ::saved_VDP_regs        [debug read_block {VDP regs} 0 8]
    set ::saved_PSG_regs        [debug read_block {PSG regs} 0 16]

    set ::saved_physical_VRAM   [debug read_block {physical VRAM} 0 16384]
    set ::saved_Main_RAM        [debug read_block {Main RAM} 0 65536]

    set ::stateCaptured 1
}


proc saveCapturedStateToFile {} {

    if ($::stateCaptured==1) {

        fconfigure stderr -encoding binary

        puts -nonewline stderr [binary format s* $::saved_CPU_reg_16]
        puts -nonewline stderr [binary format c* $::saved_CPU_reg_8]
        puts -nonewline stderr $::saved_Port_A8

        set Buffer {}
        lappend Buffer [getUnsetArea 0x0000 0x3FFF]
        lappend Buffer [getUnsetArea 0x4000 0x7FFF]
        lappend Buffer [getUnsetArea 0x8000 0xBFFF]
        lappend Buffer [getUnsetArea 0xC000 0xF000]
        puts -nonewline stderr [binary format s* $Buffer]

        set Buffer {}
        lappend Buffer [areaUsed 0x0000 0x3FFF]
        lappend Buffer [areaUsed 0x4000 0x7FFF]
        lappend Buffer [areaUsed 0x8000 0xBFFF]
        lappend Buffer [areaUsed 0xC000 0xFFFF]
        lappend Buffer $::invalidPSGDirections
        puts -nonewline stderr [binary format c* $Buffer]

        puts -nonewline stderr $::saved_VDP_regs
        puts -nonewline stderr $::saved_PSG_regs

        puts -nonewline stderr $::saved_physical_VRAM
        puts -nonewline stderr $::saved_Main_RAM
    }
}


proc isMotorRunning {} {

    if {([expr [debug read ioports 0xAA] & 16]==16)} {
        return 0
    } else {
        return 1
    }
}


proc checkEndOfTape {} {

    if ($::previousTapePosition==[cassetteplayer getpos]) {
        incr ::stoppedTime
    } else {
        set ::stoppedTime 0
    }
    set ::previousTapePosition [cassetteplayer getpos]

    if {([cassetteplayer getlength]==[cassetteplayer getpos]) || ($::stoppedTime>30)} {
        if ([isMotorRunning]) {
            captureState
        }
        saveCapturedStateToFile
        exit 0
    }

    after time 1 checkEndOfTape
}


set motorChangeIndex 0

proc checkMotorChange {} {

    incr ::motorChangeIndex

    if { $::previousMotorRunning && ![isMotorRunning] } {

        if ($::motorChangeIndex>2) {
            switch $::method {
                METHOD_motor_stop {
                    captureState
                }

                METHOD_1s_after_motor_stop {
                    after time 1 { captureState }
                }

                METHOD_last_slot_change_after_motor_stop {
                    set ::portWatchpoint [ debug set_watchpoint write_io {0xA8} {} { captureState } ]
                    after time 1 { debug remove_watchpoint $::portWatchpoint }
                }
            }
        }
    }

    if { !$::previousMotorRunning && [isMotorRunning] } {
        incr ::memoryChecksStarted
        if { $::memoryChecksStarted==2 } {
            debug set_watchpoint write_mem {0x0000 0xFFFF} {} { writeMemCallBack }
        }
    }

    set ::previousMotorRunning [isMotorRunning]
}


proc getUnsetArea { start end } {

    for {set i [expr $end-4]} { $i>=[expr $start] } { incr i -1 } {
        if (!$::memWritten([expr $i+0])&&!$::memWritten([expr $i+1])&&!$::memWritten([expr $i+2])&&!$::memWritten([expr $i+3])&&!$::memWritten([expr $i+4])) {
            return $i
        }
    }

    return 0
}


proc areaUsed { start end } {

    for {set i [expr $end-4]} { $i>=[expr $start] } { incr i -1 } {
        if ($::memWritten([expr $i+0])) {
            return 1
        }
    }

    return 0
}


proc writeMemCallBack {} {
    set ::memWritten($::wp_last_address) 1
}


debug set_condition -once { [reg pc]==1 } {
    for {set i 0} {$i < 0x10000} {} {
        debug write {Main RAM} $i 0 ; incr i
    }
}

for {set i 0} {$i < 0x10000} { incr i } {
    set ::memWritten($i) 0
}

type_via_keybuf "POKE -1,(15-PEEK(-1)\\16)*17\r"

if {[string first "RUN'CAS" [cassetteplayer]] != -1} {
    type_via_keybuf "RUN \"CAS:\"\r"
} elseif {[string first "BLOAD'CAS" [cassetteplayer]] != -1} {
    type_via_keybuf "BLOAD \"CAS:\",R\r"
} else {
    type_via_keybuf "CLOAD\rRUN\r"
}

debug set_watchpoint write_io 0xAA {} { checkMotorChange }
debug set_watchpoint write_io 0xAB {} { checkMotorChange }

proc invalidPSGDirectionsDetected {} {
    set ::invalidPSGDirections 1
}

set invalid_psg_directions_callback invalidPSGDirectionsDetected

checkEndOfTape
