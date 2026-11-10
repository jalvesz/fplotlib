program test_sdl3
    use iso_c_binding, only: c_int
    use fplotlib, only: dp, figure, plot, clf
    use fplotlib_sdl3, only: open_sdl3, draw_sdl3, poll_sdl3, close_sdl3, show_sdl3
    implicit none
    integer :: status
    logical :: is_open

    interface
        function test_resize() bind(c) result(status)
            import :: c_int
            integer(c_int) :: status
        end function test_resize
        function test_quit() bind(c) result(status)
            import :: c_int
            integer(c_int) :: status
        end function test_quit
        function test_schedule_quit() bind(c) result(status)
            import :: c_int
            integer(c_int) :: status
        end function test_schedule_quit
    end interface

    call figure(figsize=[2.0_dp, 1.0_dp], dpi=50.0_dp)
    call plot([0.0_dp, 1.0_dp], [0.0_dp, 1.0_dp])
    call open_sdl3(status)
    if (status /= 0) error stop "open failed"
    call poll_sdl3(is_open, status)
    if (status /= 0 .or. .not. is_open) error stop "poll failed"
    call open_sdl3(status)
    if (status == 0) error stop "duplicate open succeeded"
    call clf()
    call figure(figsize=[3.0_dp, 2.0_dp], dpi=40.0_dp)
    call plot([0.0_dp, 1.0_dp], [1.0_dp, 0.0_dp])
    call draw_sdl3(status)
    if (status /= 0) error stop "draw failed"
    if (test_resize() /= 0) error stop "resize request failed"
    call poll_sdl3(is_open, status)
    if (status /= 0 .or. .not. is_open) error stop "resize poll failed"
    if (test_quit() /= 0) error stop "quit request failed"
    call poll_sdl3(is_open, status)
    if (status /= 0 .or. is_open) error stop "quit failed"
    call poll_sdl3(is_open, status)
    if (status /= 0 .or. is_open) error stop "closed poll failed"
    call draw_sdl3(status)
    if (status == 0) error stop "draw without window succeeded"
    call open_sdl3(status)
    if (status /= 0) error stop "reopen failed"
    call close_sdl3()
    call close_sdl3()
    if (test_schedule_quit() /= 0) error stop "quit timer failed"
    call show_sdl3(status)
    if (status /= 0) error stop "blocking show failed"
    print *, "SDL3 lifecycle OK"
end program test_sdl3
