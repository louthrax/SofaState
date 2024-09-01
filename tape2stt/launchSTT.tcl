set auto_enable_reverse     off
set cassetteplayer_volume   20
set master_volume           20
set save_settings_on_exit   off
set sound_driver            sdl
set throttle                off

debug set_watchpoint write_io 0x3E {} { toggle throttle }

unbind F9
bind F9 "toggle throttle"

unbind F12
bind F12 "reset"
