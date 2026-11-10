program sdl3_demo
    use fplotlib, only: dp, plot, title, xlabel, ylabel, grid
    use fplotlib_sdl3, only: show_sdl3
    implicit none
    integer, parameter :: n = 200
    real(dp) :: x(n), y(n)
    integer :: i, status

    do i = 1, n
        x(i) = 12.0_dp*real(i - 1, dp)/real(n - 1, dp)
        y(i) = sin(x(i))*exp(-0.1_dp*x(i))
    end do
    call plot(x, y, "b-", lw=2.0_dp)
    call title("fplotlib SDL3")
    call xlabel("time")
    call ylabel("amplitude")
    call grid(.true.)
    call show_sdl3(status)
    if (status /= 0) error stop "SDL3 viewer failed"
end program sdl3_demo
