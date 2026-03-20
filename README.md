# parking

A C++17 library that calculates parking bay layouts from WKT geometry inputs and renders them as SVG images. Supports web deployment via WebAssembly (Emscripten) as well as a native console application.

## Building

### Web (WebAssembly)

Requires the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).

```bash
make
```

Output is written to `bin/parking.js` and `bin/parking.wasm`. Open `web/parking.html` in a browser to use the interface (ensure the `bin/` output is accessible from the page).

### Native (Console)

Requires CMake 3.15+ and a C++17-capable compiler.

```bash
cmake -B build -S .
cmake --build build
```

## Testing a pull request locally

To fetch and check out a proposed pull request for local testing, use one of the following approaches.

**Using the GitHub CLI:**

```bash
gh pr checkout <pr-number>
```

**Using plain Git:**

```bash
git fetch origin pull/<pr-number>/head:pr-<pr-number>
git checkout pr-<pr-number>
```

For example, to check out PR #1:

```bash
git fetch origin pull/1/head:pr-1
git checkout pr-1
```

After checking out, build and run the project as described in the [Building](#building) section above.