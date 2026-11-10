program consumer
    use fplotlib, only: dp, plot
    use fplotlib_sdl3, only: open_sdl3, draw_sdl3, poll_sdl3, close_sdl3
    implicit none
    integer :: status
    logical :: is_open

    call plot([0.0_dp, 1.0_dp], [1.0_dp, 0.0_dp])
    call open_sdl3(status)
    if (status /= 0) error stop "open failed"
    call poll_sdl3(is_open, status)
    if (status /= 0 .or. .not. is_open) error stop "poll failed"
    call draw_sdl3(status)
    if (status /= 0) error stop "draw failed"
    call close_sdl3()
end program consumer
