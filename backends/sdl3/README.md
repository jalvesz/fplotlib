# Optional SDL3 window backend

This companion fpm package displays the current fplotlib figure in an SDL3
window. fplotlib renders on the CPU using the same rasterizer as PNG; SDL3
only creates the window, handles close/resize events and presents RGB pixels
with its software renderer. SDL types are not exposed to Fortran. The core
fpm package remains pure Fortran and its existing `show()` still writes SVG.

## Build

Install fpm, a Fortran compiler, a compatible C compiler, and the SDL **3**
development package and runtime (SDL2 does not work). fpm does not download
the native SDL library. For a checkout of this repository, on a Unix-like
system with pkg-config:

```sh
cd backends/sdl3
fpm run --example sdl3-demo --c-flag "$(pkg-config --cflags sdl3)" \
  --link-flag "$(pkg-config --libs-only-L sdl3)"
```

In another fpm application, use a local checkout:

```toml
[dependencies]
fplotlib-sdl3 = { path = "../fplotlib/backends/sdl3" }
```

Adjust the path to the checkout; the package pulls in the core from that
checkout. Pass SDL's include and library search flags to the **application's**
fpm build as above. fpm does not select a subdirectory of a git dependency,
so a direct git dependency on this companion package is not supported. The
SDL3 runtime must also be on the platform's library search path. On Windows,
use matching Fortran/C and SDL3 toolchains and place SDL3.dll where the
executable can load it. macOS applications may need application packaging,
signing and framework-specific setup. These instructions are not a promise
of prebuilt binaries; platform setup and testing are still needed.

## Usage

```fortran
use fplotlib
use fplotlib_sdl3, only: show_sdl3
integer :: status
call plot([0.0_dp, 1.0_dp], [0.0_dp, 1.0_dp])
call show_sdl3(status)  ! blocks until the window closes or Escape is pressed
if (status /= 0) error stop "SDL3 failed"
```

For applications that drive their own loop:

```fortran
use fplotlib_sdl3, only: open_sdl3, poll_sdl3, draw_sdl3, close_sdl3
! Plot first, then:
call open_sdl3(status)
if (status /= 0) error stop "SDL3 open failed"
do while (simulation_running)
    call poll_sdl3(is_open, status)
    if (status /= 0) error stop "SDL3 event processing failed"
    if (.not. is_open) exit
    call advance_simulation()
    ! Update the current fplotlib figure here when it changes.
    call draw_sdl3(status)
    if (status /= 0) error stop "SDL3 draw failed"
end do
call close_sdl3()
```

`open_sdl3` draws the current figure; `draw_sdl3` redraws the **current**
figure explicitly after changes. `poll_sdl3` drains queued events and reports
whether the window remains open; it destroys the window after a close request
or error. `close_sdl3` is safe to call again. All status values are zero for
success and nonzero for failure; SDL errors are printed to standard error.
Only one backend window is supported, and all calls must be on the platform's
GUI/main thread. The backend drains SDL's process-wide event queue and manages
one SDL video-subsystem reference, so it is **not** suitable for embedding
alongside an application that handles SDL events itself. Call `poll_sdl3`
frequently; long computation between polls prevents window responsiveness.

Resize and HiDPI events scale the most recent image to fit while preserving
its aspect ratio. They do not rerender at a new resolution. There is no
data-coordinate zoom, pan, inspection, multiple windows, automatic dirty
tracking or mouse interaction yet. Figure edits do not appear until
`draw_sdl3` is called; selecting a different current figure changes what is
drawn. Blocking `show_sdl3` is a static view.

The public `render_rgb(pixels, width, height)` in the core returns an
allocatable `character(kind=c_char)` array of RGB24 bytes, interleaved by
pixel, left to right and top to bottom, with no row padding. Alpha is
composited onto white. This is intentionally RGB rather than a new RGBA
renderer: it reuses the existing animation/PNG raster path without
encode/decode or duplicate graphics logic.

## Testing

For a headless lifecycle test:

```sh
SDL_VIDEODRIVER=dummy fpm test --c-flag "$(pkg-config --cflags sdl3)" \
  --link-flag "$(pkg-config --libs-only-L sdl3)"
```

The lifecycle and a resize/keyboard-close of the demo have also been exercised
with gfortran on Linux X11 under Xvfb. This is not a substitute for testing on
each desktop: Wayland, Windows, macOS, HiDPI, and additional Fortran compilers
need verification. Future work includes data-coordinate interactions,
accurate resize rerendering, multiple figures and event-loop integration that
does not consume another SDL application's events.
