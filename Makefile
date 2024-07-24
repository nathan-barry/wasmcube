build:
	emcc main.cpp -I/Users/nathanbarry/cpp/emsdk/upstream/emscripten/cash/sysroot/include -s WASM=1 -s USE_SDL=2 -o index.js

run: build
	emrun index.html