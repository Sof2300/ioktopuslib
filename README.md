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

| Module           | Purpose                                           |
| ---------------- | ------------------------------------------------- |
| `datastruct/`    | Generic and project-oriented data structures      |
| `device/`        | Device and hardware-related utilities             |
| `events/`        | Event-related infrastructure                      |
| `fs/`            | Filesystem utilities                              |
| `net/`           | Networking utilities                              |
| `sensors/`       | Sensor-related components                         |
| `x86/`           | Desktop/x86 compatibility and development support |
| `TimeUtil.h`     | Time and date utilities                           |
| `SyncedClock.h`  | Extended synchronized clock                       |
| `InterruptMan.h` | Object-oriented interrupt dispatch                |
| `TimeProfiler.h` | Lightweight execution profiling                   |
| `easing.h`       | Mathematical easing/interpolation functions       |
| `mylib.h`        | Common library definitions/helpers                |

---

# Core components

## `TimeUtil`

`TimeUtil.h` provides a collection of time, date and duration utilities designed around the constraints of embedded systems.

It includes functionality for:

* Unix-style timestamps
* millisecond timestamps
* date/time conversion
* duration conversion
* formatting
* timezone handling
* extended time calculations
* handling the limited range of native embedded timing primitives

The goal is to avoid scattering ad-hoc time arithmetic throughout an embedded application.

```cpp
#include "TimeUtil.h"

uint64_t now = getMS();

String date = timestampToDate(now);
```

The implementation is intended to work with the timing facilities available on ESP-class platforms while providing a larger logical time domain where required.

---

# `SyncedClock`

`SyncedClock` provides a logical clock built on top of the microcontroller's local timing source.

A typical embedded application has two very different notions of time:

```text
millis()
   │
   └── local monotonic-ish elapsed time

NTP / server / browser timestamp
   │
   └── externally synchronized wall-clock time
```

`SyncedClock` bridges the two.

It maintains an extended time representation derived from the local clock and can be resynchronized when a trusted external timestamp becomes available.

### Design goals

* Extend the useful range of the underlying millisecond counter
* Maintain a high-resolution local clock between synchronizations
* Detect timing-counter wraparound
* Correct the clock when an external timestamp becomes available
* Notify interested objects when resynchronization occurs

The clock also uses a listener interface:

```cpp
class ClockListener {
public:
    virtual void clockresync(uint64_t diffms) = 0;
};
```

Applications can therefore react to synchronization events without coupling their logic directly to the clock implementation.

### Extended timing

The implementation explicitly tracks the high/low portions of the underlying timing cycle rather than simply relying on the native counter as an absolute timestamp.

This is particularly useful for long-running ESP applications where a raw 32-bit millisecond counter is not sufficient as an application-level clock.

---

# `InterruptMan`

`InterruptMan` provides a small object-oriented wrapper around Arduino hardware interrupts.

Instead of requiring global functions for every interrupt source, interrupt callbacks can be associated with C++ objects through `InterruptCB`.

```cpp
class MyDevice : public InterruptCB {
public:
    void callback() override {
        // Handle hardware event
    }
};
```

An interrupt can then be attached to the object:

```cpp
MyDevice device;

InterruptMan::init();
InterruptMan::attach(4, 0, &device);
```

The interrupt mode parameter supports:

```text
0 → RISING
1 → FALLING
2 → CHANGE
3 → LOW
```

The implementation uses a small static callback table and interrupt trampolines to bridge the Arduino ISR API with object instances.

This keeps interrupt-facing code lightweight and avoids allocating an interrupt object dynamically.

### Resource model

The current implementation intentionally limits the manager to:

```cpp
#define MAXINTERRUPTS 2
```

This is a deliberate embedded-oriented design: predictable memory usage and a fixed resource footprint take precedence over a general-purpose event system.

---

# `TimeProfiler`

`TimeProfiler` provides lightweight instrumentation for measuring execution time inside embedded applications.

Rather than requiring a full profiling framework, it allows named timing points to be recorded and printed.

Conceptually:

```text
operation
   │
   ├── start
   │
   ├── step 1 ─────── Δt
   │
   ├── step 2 ─────── Δt
   │
   └── end   ───────── Δt
```

This is useful when investigating questions such as:

* Which part of a loop is consuming CPU time?
* How long does a filesystem operation take?
* How expensive is a network transaction?
* Is a sensor read blocking?
* How much time is spent between state-machine transitions?

Example:

```cpp
TimeProfiler profiler;

profiler.reset("operation");

profiler.printick("operation", "read");
profiler.printick("operation", "process");
profiler.printick("operation", "write");

profiler.printProfile("operation");
```

The profiler is designed for instrumentation during development without introducing a heavyweight dependency.

---

# `easing.h`

`easing.h` provides a collection of mathematical interpolation functions commonly used for animation, UI transitions, motor control and other time-dependent behaviors.

Supported families include:

* Linear
* Sine
* Quadratic
* Cubic
* Quartic
* Quintic
* Exponential
* Circular
* Back
* Elastic
* Bounce

Each family provides the usual variants:

```text
Ease In
Ease Out
Ease In/Out
```

The API can be used through a function pointer:

```cpp
easingFunction easing = getEasingFunction(EaseInOutCubic);

double value = easing(t);
```

where `t` is normally normalized to:

```text
0.0 ≤ t ≤ 1.0
```

This makes easing functions particularly convenient when implementing embedded UI animations without pulling in a larger graphics framework.

For example:

```text
Linear

0 ───────────────────────────── 1


Ease In

0 ───────────────────────╮
                         ╰───── 1


Ease Out

0 ─────╮
       ╰─────────────────────── 1


Ease In/Out

0 ───────╮          ╭────────── 1
         ╰──────────╯
```

---

# Compatibility layer

A recurring design goal of ioktopuslib is to isolate platform-specific functionality.

Several components use compatibility abstractions so that the same higher-level utility can be used in more than one environment.

The repository contains an `x86/` area alongside ESP-oriented code, allowing selected components to be developed or tested outside the microcontroller environment.

This is particularly useful for utilities that contain substantial logic but only a small amount of platform-specific code.

---

# Embedded design principles

ioktopuslib is built around a few practical assumptions.

### Predictable resources

Embedded applications often benefit more from predictable resource usage than from maximum abstraction.

Where appropriate, components therefore use:

* static storage
* fixed-size tables
* lightweight objects
* minimal runtime machinery

### Small components

There is no requirement to use the entire library.

For example:

```cpp
#include "easing.h"
```

does not imply that an application needs to use the clock, networking or filesystem components.

### Long-running devices

Several utilities are designed with continuously running devices in mind rather than short-lived Arduino sketches.

This is particularly relevant to:

* extended time handling
* clock synchronization
* rollover handling
* resource-constrained callbacks
* lightweight profiling

### Hardware-aware abstractions

The library does not attempt to hide the embedded platform completely.

Instead, it provides small abstractions around recurring problems while retaining access to the underlying Arduino/ESP environment.

---

# Typical use cases

ioktopuslib is intended for embedded applications such as:

* ESP32 control systems
* ESP8266/ESP32 IoT devices
* sensor monitoring
* industrial or agricultural controllers
* network-connected machines
* embedded user interfaces
* hardware state machines
* data-logging systems
* long-running autonomous devices
* hardware prototypes that have grown beyond a simple Arduino sketch

A typical application might combine the components like this:

```text
                 ┌─────────────────────┐
                 │     ESP32 device    │
                 └──────────┬──────────┘
                            │
       ┌────────────────────┼────────────────────┐
       │                    │                    │
       ▼                    ▼                    ▼
   Sensors              Network              Inputs
       │                    │                    │
       └────────────────────┼────────────────────┘
                            │
                     Application logic
                            │
          ┌─────────────────┼─────────────────┐
          ▼                 ▼                 ▼
     SyncedClock       TimeProfiler      InterruptMan
          │
          ▼
       TimeUtil
```

---

# Installation

Clone the repository into your Arduino libraries directory:

```bash
git clone https://github.com/Sof2300/ioktopuslib.git
```

Alternatively, download the repository as a ZIP and install it using the Arduino IDE library manager's local ZIP installation mechanism.

The repository contains an Arduino `library.properties` file and is structured as a collection of header-based utilities.

---

# Including components

Components can be included individually:

```cpp
#include "TimeUtil.h"
#include "SyncedClock.h"
#include "InterruptMan.h"
#include "TimeProfiler.h"
#include "easing.h"
```

For components located in subdirectories:

```cpp
#include "datastruct/..."
#include "device/..."
#include "events/..."
#include "fs/..."
#include "net/..."
#include "sensors/..."
```

The exact dependencies are component-specific.

---

# Platform

Primary targets:

* ESP32
* ESP8266
* Arduino-compatible embedded environments

Some components additionally provide compatibility support for x86 development environments.

The codebase is written in C++ and makes use of Arduino/ESP facilities where appropriate.

---

# Repository structure

```text
ioktopuslib/
│
├── datastruct/
│   └── Generic data structures
│
├── device/
│   └── Hardware/device utilities
│
├── events/
│   └── Event infrastructure
│
├── fs/
│   └── Filesystem utilities
│
├── net/
│   └── Networking utilities
│
├── sensors/
│   └── Sensor-related utilities
│
├── x86/
│   └── Desktop/x86 compatibility
│
├── InterruptMan.h
│   └── Object-oriented interrupt dispatch
│
├── SyncedClock.h
│   └── Extended synchronized clock
│
├── TimeProfiler.h
│   └── Lightweight execution profiler
│
├── TimeUtil.h
│   └── Time/date/duration utilities
│
├── easing.h
│   └── Mathematical easing functions
│
├── mylib.h
│   └── Common library definitions
│
└── library.properties
```

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
