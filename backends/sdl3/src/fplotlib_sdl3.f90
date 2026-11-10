! One window and the current fplotlib figure; call from the GUI/main thread.
module fplotlib_sdl3
    use iso_c_binding, only: c_char, c_int, c_ptr, c_null_ptr, c_associated, c_null_char
    use iso_fortran_env, only: error_unit
    use fplotlib, only: render_rgb
    implicit none
    private
    public :: open_sdl3, draw_sdl3, poll_sdl3, close_sdl3, show_sdl3

    type(c_ptr), save :: window = c_null_ptr

    interface
        function fp_create(title, pixels, width, height) bind(c, name="fp_window_create") result(handle)
            import :: c_char, c_int, c_ptr
            character(kind=c_char), intent(in) :: title(*), pixels(*)
            integer(c_int), value :: width, height
            type(c_ptr) :: handle
        end function fp_create

        function fp_present(handle, pixels, width, height) bind(c, name="fp_window_present") result(status)
            import :: c_char, c_int, c_ptr
            type(c_ptr), value :: handle
            character(kind=c_char), intent(in) :: pixels(*)
            integer(c_int), value :: width, height
            integer(c_int) :: status
        end function fp_present

        function fp_poll(handle) bind(c, name="fp_window_poll") result(status)
            import :: c_int, c_ptr
            type(c_ptr), value :: handle
            integer(c_int) :: status
        end function fp_poll

        subroutine fp_destroy(handle) bind(c, name="fp_window_destroy")
            import :: c_ptr
            type(c_ptr), value :: handle
        end subroutine fp_destroy

        subroutine fp_pause() bind(c, name="fp_window_pause")
        end subroutine fp_pause
    end interface

contains

    subroutine open_sdl3(status)
        integer, intent(out) :: status
        character(kind=c_char), allocatable :: pixels(:)
        integer :: width, height

        status = 1
        if (c_associated(window)) then
            write (error_unit, '(a)') "fplotlib SDL3: a window is already open"
            return
        end if
        call render_rgb(pixels, width, height)
        window = fp_create(c_char_"fplotlib"//c_null_char, pixels, int(width, c_int), int(height, c_int))
        if (c_associated(window)) status = 0
    end subroutine open_sdl3

    subroutine draw_sdl3(status)
        integer, intent(out) :: status
        character(kind=c_char), allocatable :: pixels(:)
        integer :: width, height

        status = 1
        if (.not. c_associated(window)) then
            write (error_unit, '(a)') "fplotlib SDL3: no window is open"
            return
        end if
        call render_rgb(pixels, width, height)
        status = int(fp_present(window, pixels, int(width, c_int), int(height, c_int)))
    end subroutine draw_sdl3

    subroutine poll_sdl3(is_open, status)
        logical, intent(out) :: is_open
        integer, intent(out) :: status
        integer(c_int) :: result

        is_open = .false.
        status = 0
        if (.not. c_associated(window)) return
        result = fp_poll(window)
        if (result < 0) then
            status = 1
            call close_sdl3()
        else if (result == 0) then
            call close_sdl3()
        else
            is_open = .true.
        end if
    end subroutine poll_sdl3

    subroutine close_sdl3()
        if (c_associated(window)) call fp_destroy(window)
        window = c_null_ptr
    end subroutine close_sdl3

    subroutine show_sdl3(status)
        integer, intent(out) :: status
        logical :: is_open

        call open_sdl3(status)
        if (status /= 0) return
        do
            call poll_sdl3(is_open, status)
            if (status /= 0 .or. .not. is_open) exit
            call fp_pause()
        end do
    end subroutine show_sdl3
end module fplotlib_sdl3
