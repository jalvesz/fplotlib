# SDL3 interactive-viewer experiment

This is the historical static-snapshot experiment. For the opt-in fpm
backend and its application-driven event loop, see
[backends/sdl3](../../backends/sdl3/README.md).

This is an opt-in, standalone fpm package. It does not change fplotlib's
dependency-free build or its existing `show()` behavior. `show_sdl3(status)`
renders the current figure with the existing software rasterizer and opens an
SDL3 window. Close it or press Escape to return. The mouse wheel and `+`/`-`
zoom; arrow keys pan; `0` resets.

The interaction operates on a **static raster snapshot**, not the plot's data
coordinates: ticks do not update and magnification becomes pixelated. Resizing
preserves the figure's aspect ratio. This is a feasibility prototype, not a
Matplotlib-style interactive backend. In particular it has no toolbar, axes
selection, data-coordinate pan/zoom, redraw after updates, nonblocking event
integration, or support for multiple independent windows. The call blocks and
must run on the platform's GUI/main thread. SDL errors are printed to stderr
and reported as a nonzero `status`.

## Build and run

Install a Fortran compiler, C compiler, fpm, pkg-config, and the SDL **3**
development package and runtime (SDL2 is not sufficient). For example, use a
matching conda-forge toolchain and `sdl3`, or the platform's SDL3 development
package. Some distributions, including older Ubuntu releases, require SDL3 to
be installed separately. SDL3 is not fetched by fpm.

From this directory, in a Unix-like shell:

```sh
fpm run --c-flag "$(pkg-config --cflags sdl3)" \
  --link-flag "$(pkg-config --libs-only-L sdl3)"
```

The program must also be able to load SDL3 at runtime. For a non-system
installation, use the platform's library search path (e.g. `LD_LIBRARY_PATH`
on Linux), an rpath, or package the shared library with the application.
On Windows, use a matching MinGW compiler/SDL3 build and arrange for
`SDL3.dll` to be found, typically alongside the executable. macOS packaging,
signing, and distribution also require platform-specific handling. These
platform instructions are a starting point, not verified installers.

For a headless startup/render/cleanup smoke test:

```sh
SDL_VIDEODRIVER=dummy fpm run --c-flag "$(pkg-config --cflags sdl3)" \
  --link-flag "$(pkg-config --libs-only-L sdl3)" -- --smoke
```

`--smoke` posts a quit event after the first frame; it is not an interactive
test. `show_sdl3` is a separate module: an application using this experiment
must build this package and link SDL3 itself. Its C bridge uses SDL's public C
API and needs no SDL_image or OpenGL bindings. It uses fplotlib internals to
avoid encoding and decoding a PNG, so those interfaces may change.

The prototype has been exercised with gfortran and SDL 3.4.20 on Linux:
the dummy driver was used for the smoke test and X11 under Xvfb for opening,
zooming, panning, resetting, resizing and closing a window. Other compilers,
Wayland, macOS and Windows have not been verified.

## Comparison and path to a usable backend

* **SDL3:** one native dependency supplies windows, events and a 2D texture
  renderer. Existing CPU-rendered RGB pixels can be uploaded directly, as in
  this prototype. The C bridge keeps SDL event unions and constants out of the
  Fortran interface; alternatively, the work-in-progress
  [fortran-sdl3](https://github.com/interkosmos/fortran-sdl3) bindings provide
  a broader Fortran interface but still require the native SDL libraries.
* **GLFW + OpenGL:** GLFW handles windows and events, but not drawing the
  image. A comparable viewer also needs an OpenGL context, texture upload,
  shaders and a drawing path, plus GL function loading and careful macOS
  context handling. It is smaller in scope than SDL but not necessarily
  simpler for this CPU-rasterized use case. GLFW also needs a native library
  and platform graphics drivers; it does not eliminate installation work.

SDL3 is the more direct first choice here, not a claim that either option is
universally installable. Both need OS-specific native packages, runtime library
distribution, graphical sessions and main-thread event handling. Neither
should be made a mandatory dependency of fplotlib's file-only use. A real
backend would need a stable pixel-render API, per-figure state and redraws,
data-coordinate transformations for linear/log/polar/3D and multi-axes plots,
HiDPI handling, resize and input tests, and a documented blocking/nonblocking
contract for applications with their own event loop. Packaging and CI should
cover Linux X11/Wayland, macOS and Windows before calling it cross-platform.

References: [SDL releases](https://github.com/libsdl-org/SDL/releases),
[SDL platform build notes](https://github.com/libsdl-org/SDL/tree/main/docs),
[GLFW overview and platform requirements](https://github.com/glfw/glfw#readme),
[fplotlib GUI issue](https://github.com/fortran-lang/fplotlib/issues/34).
