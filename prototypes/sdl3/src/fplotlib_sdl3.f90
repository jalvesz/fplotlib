module fplotlib_sdl3
    use iso_c_binding, only: c_char, c_int
    use fplotlib_backend_png, only: png_renderer_t
    use fplotlib_draw, only: render_figure
    use fplotlib_state, only: fig_dpi
    implicit none
    private
    public :: show_sdl3

    interface
        function sdl3_view(pixels, width, height, smoke) bind(c, name="fplotlib_sdl3_view") result(status)
            import :: c_char, c_int
            character(kind=c_char), intent(in) :: pixels(*)
            integer(c_int), value :: width, height, smoke
            integer(c_int) :: status
        end function sdl3_view
    end interface

contains

    subroutine show_sdl3(status, smoke)
        integer, intent(out) :: status
        logical, intent(in), optional :: smoke
        type(png_renderer_t) :: renderer
        character(kind=c_char), allocatable :: pixels(:)
        integer :: i
        integer(c_int) :: smoke_flag

        renderer%keep_pixels = .true.
        call renderer%set_dpi(fig_dpi)
        call render_figure(renderer)
        allocate(pixels(len(renderer%rgb)))
        do i = 1, size(pixels)
            pixels(i) = achar(iachar(renderer%rgb(i:i)), kind=c_char)
        end do
        smoke_flag = 0_c_int
        if (present(smoke)) then
            if (smoke) smoke_flag = 1_c_int
        end if
        status = sdl3_view(pixels, int(renderer%pw, c_int), int(renderer%ph, c_int), smoke_flag)
    end subroutine show_sdl3
end module fplotlib_sdl3
