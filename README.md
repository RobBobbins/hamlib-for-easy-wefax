# Hamlib for Easy WEFAX

The Hamlib library used by Easy WEFAX for radio control (CAT), and how it is built for Android.

Hamlib, the Ham Radio Control Library, is free software under the GNU Lesser General Public License, version 2.1 or later ([upstream/COPYING.LIB](upstream/COPYING.LIB)). Easy WEFAX uses it as its own separate, unchanged library, `libhamlib.so`. This repository is the source for that library.

## Source

`upstream/` is Hamlib **4.7.2**, unpacked from the release file `hamlib-4.7.2.tar.gz` (https://github.com/Hamlib/Hamlib/releases/tag/4.7.2, SHA-256 `AE1FCF2DBC80EA0786EA8F047B09399C3F7737D1930442F61A031708ED33E88F`). **No file in `upstream/` has been changed.**

## Android build files

| Path | What it is |
|------|------------|
| `CMakeLists.txt` | Builds `libhamlib.so` with the Android NDK (CMake and Ninja) |
| `android/include/hamlib/config.h` | Settings written by Hamlib's own `configure` for `aarch64-linux-android` (Android 8.0, API 26) |
| `android/sources.cmake` | The library's 398 source files, read from Hamlib's own Makefiles after `configure` |
| `android/include/android_glob_compat.h` | Used only for `src/microham.c`: `glob()` exists on Android 9 and newer only, so on Android 8 it finds nothing (Android apps cannot list `/dev` anyway) |

How `config.h` and `sources.cmake` were made (Git Bash on Windows, NDK 28.2.13676358):

```bash
CC="clang --target=aarch64-linux-android26" ../../upstream/configure --host=aarch64-linux-android \
  --disable-static --enable-shared --without-libusb --without-readline --without-indi \
  --without-xml-support --without-cxx-binding --disable-winradio
```

then the `*_la_SOURCES` lists of `lib/`, `security/`, `src/` and every rig, rotator and amplifier backend were printed with `make`, plus the two C++ files of `rotators/androidsensor`.

## Building

From Easy WEFAX, Gradle builds this folder for arm64 (phones) and x86_64 (the emulator). On its own:

```bash
cmake -S . -B build/arm64 -G Ninja -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-26 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=Release
cmake --build build/arm64
```

Result 2026-10-20: `libhamlib.so` 12.6 MB stripped for arm64, needing only Android system libraries (libm, libandroid, liblog, libdl, libc). In the Android emulator, Easy WEFAX listed Hamlib's radios, opened the Hamlib Dummy rig, set 12748.1 kHz USB and read it back.

## Replacing Hamlib

Anyone may build their own `libhamlib.so` from this source, or a newer Hamlib, and put it in place of the one inside Easy WEFAX. Easy WEFAX calls Hamlib only through its public functions (`rig_init`, `rig_open`, `rig_set_freq`, `rig_get_freq`, `rig_set_mode`, `rig_get_mode`, `rig_set_conf`, `rig_list_foreach` and similar).
