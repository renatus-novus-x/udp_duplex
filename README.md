[![windows](https://github.com/renatus-novus-x/udp_duplex/workflows/windows/badge.svg)](https://github.com/renatus-novus-x/udp_duplex/actions?query=workflow%3Awindows)
[![macos](https://github.com/renatus-novus-x/udp_duplex/workflows/macos/badge.svg)](https://github.com/renatus-novus-x/udp_duplex/actions?query=workflow%3Amacos)
[![ubuntu](https://github.com/renatus-novus-x/udp_duplex/workflows/ubuntu/badge.svg)](https://github.com/renatus-novus-x/udp_duplex/actions?query=workflow%3Aubuntu)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/renatus-novus-x/udp_duplex)
# udp_duplex

Cross-platform UDP ping-pong sample for:

- Windows (MSVC / MinGW)
- Linux
- macOS
- X68000 via elf2x68k (m68k-xelf-gcc + libsocket)

The same `src/udp_duplex.c` is used for all platforms.

## Usage

On two machines (or one machine + X68000), run:

Machine A (e.g., 192.168.0.10):

```bash
udp_duplex 50000 192.168.0.11 50001
```

Machine B (e.g., 192.168.0.11):

```bash
udp_duplex 50001 192.168.0.10 50000
```

Each side sends one line per second:

- `hello 0`
- `hello 1`
- `hello 2`
- ...

and prints everything it receives.

This is intended as a minimal starting point before replacing the
`"hello <counter>"` payload with ADPCM / PCM audio frames for VoIP experiments.

## Download

- [udp_duplex.exe (windows)](https://raw.githubusercontent.com/renatus-novus-x/udp_duplex/main/bin/windows/udp_duplex.exe)
- [udp_duplex     (macos)](https://raw.githubusercontent.com/renatus-novus-x/udp_duplex/main/bin/macos/udp_duplex)
- [udp_duplex     (ubuntu)](https://raw.githubusercontent.com/renatus-novus-x/udp_duplex/main/bin/ubuntu/udp_duplex)
- [udp_duplex.x   (x68000)](https://raw.githubusercontent.com/renatus-novus-x/udp_duplex/main/bin/x68000/udp_duplex.x)
