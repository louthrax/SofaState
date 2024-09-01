set auto_enable_reverse     off
set autoruncassettes        on
set cassetteplayer_volume   20
set fullspeedwhenloading    on
set master_volume           20
set save_settings_on_exit   off
set sound_driver            sdl
set throttle off

unbind F9
bind F9 "toggle throttle"

unbind F12
bind F12 "reset ; set autoruncassettes off"

after time 300 { set throttle on }
