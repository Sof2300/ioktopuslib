# ioktopuslib

### Embedded C++ utilities for ESP32 / ESP8266

**ioktopuslib** is a collection of reusable, low-level C++ components developed for real-world ESP32/ESP8266 projects.

It provides the infrastructure that tends to accumulate around embedded applications: extended time handling, synchronized clocks, interrupt dispatch, profiling, interpolation/easing, compatibility layers, data structures, filesystem and networking helpers, device abstractions, and sensor utilities.

The library is intentionally modular. Components can generally be included individually without adopting a larger application framework.

---

## Architecture

```text
                         ┌──────────────────────────┐
                         │       Application        │
                         └────────────┬─────────────┘
                                      │
             ┌────────────────────────┼────────────────────────┐
             │                        │                        │
             ▼                        ▼                        ▼
      ┌─────────────┐          ┌─────────────┐          ┌─────────────┐
      │   device/   │          │   sensors/  │          │    net/     │
      └──────┬──────┘          └──────┬──────┘          └──────┬──────┘
             │                        │                        │
             └────────────────────────┼────────────────────────┘
                                      │
                              ┌───────▼────────┐
                              │   Core Utils   │
                              ├────────────────┤
                              │ TimeUtil       │
                              │ SyncedClock    │
                              │ InterruptMan   │
                              │ TimeProfiler   │
                              │ easing         │
                              └───────┬────────┘
                                      │
                         ┌────────────┴────────────┐
                         │                         │
                    ┌────▼─────┐             ┌────▼─────┐
                    │ ESP32/   │             │   x86/   │
                    │ ESP8266  │             │ testing  │
                    └──────────┘             └──────────┘
```

The repository is organized into several functional areas:
## Root files

- `easing.h`: Common easing functions for animations or smooth transitions.
- `InterruptMan.h`: Helper utilities to manage and abstract hardware interrupts.
- `mylib.h`: Public header that likely aggregates common includes and APIs.
- `SyncedClock.h`: Utilities for keeping a synchronized clock across devices.
- `TimeProfiler.h`: Lightweight timing and profiling helpers.
- `TimeUtil.h`: Time-related utility functions and helpers.
- `TimeUtil.h.bak`: Backup copy of `TimeUtil.h`.

## datastruct/

- `ChunkedVector.h`: Vector-like container optimized for chunked allocation.
- `DataMap.h`: Simple associative map structure for embedded use.
- `GenString.h`: Small, generated/string helper utilities for constrained environments.
- `JSON.h`: JSON serialization/deserialization helpers.
- `JSONLoaders.h`: Helpers to load JSON data into native structures.
- `StateMachine.h`: A lightweight state machine implementation.

## device/

- `Beeper.h`: Simple beeper/buzzer driver.
- `devices.h`: Central header enumerating or registering available device drivers.
- `LedExpress.h`: Helper for driving LEDs with expressive patterns.
- `LoadCellReader.h`: Driver utilities for reading load cell (scale) sensors.
- `Relay.h`: Simple relay control abstraction.
- `RFIDReader.h`: RFID reader driver interfaces.
- `RGBLed.h`: Driver for RGB LED strips or modules.
- `TFTScreen.h`: Abstractions for TFT display modules.

## events/

- `BasicEventEmitter.h`: Minimal event emitter implementation.
- `DefaultEventEmitter.h`: Default configuration or extended emitter implementation.
- `Event.h`: Event type definitions.
- `EventEmitter.h`: Event emitter interface and helpers.

## fs/

- `SimpleFS.h`: Small filesystem abstraction layer.
- `SimpleFSEsp32.h`: `SimpleFS` implementation for ESP32 platforms.
- `SimpleFSEsp8266.h`: `SimpleFS` implementation for ESP8266 platforms.

## net/

- `AsyncWebServerEsp32.h`: Async web server glue for ESP32.
- `BasicServer - Copy.h`: A copy of `BasicServer.h`, likely a working backup.
- `BasicServer.h`: Simple HTTP server abstractions.
- `SimpleWebServer.h`: Lightweight, synchronous web server utilities.
- `WebServerEsp32.h`: Web server bindings for ESP32 platform.
- `WebServerEsp8266.h`: Web server bindings for ESP8266 platform.
- `WifiMan.h`: Wi‑Fi management utilities (connect, scan, retry logic).

## sensors/

- `AsyncHTU21Dmod.h`: Async driver for HTU21D temperature/humidity sensor.
- `Ds18b20Driver.h`: Driver for DS18B20 temperature sensors.
- `FakeDriver.h`: Mock/fake sensor driver for testing.
- `HTUDriver.h`: Driver for HTU-series sensors.
- `Sensor.h`: Sensor base types and interfaces.
- `SensorFactory.h`: Factory helpers to instantiate sensor drivers.

## x86/

- `CurFSx86.cc`: x86 test shim for a current filesystem implementation.
- `CurFSx86.h`: Header for the x86 filesystem shim.
- `CurWebServerx86.cc`: x86 test harness for the webserver.
- `CurWebServerx86.h`: Header for the x86 webserver test harness.
- `devicex86.h`: x86-side device abstractions and stubs.
- `MonoTicker.h`: Monotonic ticker implementation for host builds.
- `monoTimer.h`: Host-side timer utilities.
- `printx86.h`: Printing/IO helpers for the x86 environment.
- `Sensorx86.h`: x86-compatible sensor stubs and helpers.
- `Sensorx86.h.bak`: Backup of the x86 sensor header.
- `SimpleFSx86.h`: SimpleFS shim for x86 testing.
- `WifiManx86.h`: x86 stub for Wi‑Fi management (for simulation).

## webserver/

- `compile.mingw.bat`: Convenience script to compile the webserver on MinGW.
- `main.cpp.ignore`: Example or ignored main used for local testing.
- `README.md`: Notes specific to the `webserver` subproject.
- `stdHelpers.cpp`: Standard helper implementations used by the webserver.
- `stdHelpers.h`: Declarations for the standard helpers.
- `Tracer.h`: Lightweight tracing/debug helper.
- `UrlHelper.cpp`: URL encoding/parsing helper implementations.
- `UrlHelper.h`: URL helper declarations.
- `webserver.cpp`: Core webserver implementation used for host builds.
- `webserver.h`: Public webserver API for host builds.

### webserver/base64/

- `base64.cpp`: Base64 encoding/decoding implementations.
- `base64.h`: Header for base64 helpers.
- `compile-and-run-test`: Simple test runner script for the base64 code.
- `LICENSE`: License file for the base64 implementation.
- `README.md`: Notes and usage examples for the base64 utility.

### webserver/socket/

- `README.md`: Notes describing the socket helper usage.
- `src/Socket.cpp`: Socket implementation for the webserver shim.
- `src/Socket.h`: Socket interface declarations.
---

# Why "ioktopus"?

The library is named after the larger ecosystem of embedded projects in which it evolved.

It is not intended to be a monolithic framework. Think of it more as a toolbox: small pieces of infrastructure that can be pulled into an embedded project when the standard Arduino primitives stop being quite enough.

---

# Project status

This is an actively evolving utility library rather than a frozen, standards-oriented framework.

Some modules are highly reusable and self-contained; others originated from specific embedded applications and may retain project-specific assumptions.

The API should therefore be considered **unstable** until a component is explicitly documented as stable.

---

If you intend to use ioktopuslib as a third-party dependency, check the repository status and licensing terms before redistribution.

---


GitHub:
https://github.com/Sof2300

Repository:
https://github.com/Sof2300/ioktopuslib
