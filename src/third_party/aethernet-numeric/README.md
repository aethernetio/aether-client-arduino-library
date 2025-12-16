# Æthernet Numeric Types

Specialized numeric formats for constrained systems without FPU or with limited bandwidth and memory.
Used across the Æthernet C++ client to efficiently represent durations, fixed-point values, and tiered integers.

---

## Table of Contents

1. [Overview](#overview)
2. [TieredInt](#tieredint)
3. [Fixed](#fixed)
4. [Exponent](#exponent)
5. [Combined Types](#combined-types)
6. [Usage Examples](#usage-examples)
7. [Integration Notes](#integration-notes)
8. [Running Tests](#running-tests)

---

## Overview

The **Æthernet C++ client** is designed for restricted devices with limited memory, bandwidth, and no floating point unit.
To address these constraints, we've implemented specialized numeric formats that maintain precision, range, and compactness without sacrificing runtime performance.

All formats are **header-only**, compile-time defined, and interoperable.

---

## TieredInt

This integer format efficiently compresses frequently used numbers into fewer bytes.
Numbers below **200** (adjustable via template parameters) occupy a single byte, while those under **4000** require only two bytes.

Compression and decompression occur only during serialization and deserialization, keeping runtime overhead minimal.

Although other variable-length encoding methods exist (e.g., Google-optimized varint or UTF-8–style encodings), Æthernet’s `TieredInt` offers **compile-time control** over tier boundaries, improving both **packing ratio** and **predictability**.

Traditional varint:

* 1 byte → values < 128
* 2 bytes → values < 16,384

Æthernet `TieredInt` (example configuration):

* 1 byte → values < 251
* 2 bytes → values < 4000
* Tiers are **compile-time configurable**

```cpp
TieredInt<1, 251>;
```

**Parameters**

* **First parameter:** minimum tier size (1, 2, or 4 bytes)
* **Subsequent parameters:** maximum values per tier

**Examples**

```cpp
TieredInt<2, 4000>;                 // min tier = 2 bytes, max tier = 4 bytes
TieredInt<2, 4000, 100000000>;      // min tier = 2 bytes, max tier = 8 bytes
```

All standard C++ numeric limits and type traits are supported.

---

## Fixed

Microcontrollers often lack an **FPU** or have limited floating-point performance.
Æthernet provides a **fixed-point module** that behaves like a floating-point type but is deterministic and overflow-safe.

Unlike many libraries, you do **not** need to manually define integer/fraction bit widths — they’re automatically derived from a **range** at compile time.

```cpp
AE_FIXED(uint8_t, 123.5) f(3.14f);
```

Values can also be initialized directly from floating-point literals:

```cpp
AE_FIXED(uint8_t, 10.0) f1(3.14f);
AE_FIXED(uint8_t, 10.0) f2(9.0f);
auto f3 = f1 + f2;  // inferred result range [0..20]
```

The library infers result types for arithmetic to prevent overflow.

It also supports **out-of-range fixed-point positioning**, e.g.:

```cpp
AE_FIXED(uint8_t, 60000.0) big(3000.0f);
AE_FIXED(uint8_t, 0.0001) tiny(0.00001f);
```

While other libraries often restrict fractional bits to [0..8], this approach enables **natural semantics** (e.g., expressing time directly in seconds rather than milliseconds).

---

## Exponent

When values span several orders of magnitude, **relative precision** is often more meaningful than absolute precision (e.g., function durations from microseconds to seconds).

`AE_EXPONENT_FIXED` provides a compact exponential encoding with deterministic resolution. The type exposes a `Fixed`-like interface at runtime and uses a compact serialized form.

**Example — store values from `0.001` to `60.0` in one byte:**

```cpp
using E = AE_EXPONENT_FIXED(uint16_t, uint8_t, 0.001, 60.0);
```

Here:

* `Fixed<uint16_t, 60>` — runtime type
* `uint8_t` — serialization type

Ideal for durations between **1 ms** and **60 s**.

---

## Combined Types

You can combine `TieredInt`, `Fixed`, and `Exponent` for optimal encoding.

**Example — exponential number with single-byte encoding for small values:**

```cpp
using P = TieredInt<1, 254>;                 // serialization type (tiered)
using E = AE_EXPONENT(uint32_t, P, 1.0, 90000.0);
```

This represents a duration range **1 ms → 90 s**, with relative resolution ≈ **2%**, meaning:

* Precision ≈ **0.022 ms** at 1 ms
* Precision ≈ **2 s** at 90 s

The first parameter (`uint32_t`) defines the base type used by the `Fixed` runtime representation; its range is derived from the exponent’s upper bound.
In this configuration, it yields an effectively **uniform absolute resolution** across the full range.

> **Units note:** The library is unit-agnostic; the numeric range you specify determines semantics. In the example above, `1.0 … 90000.0` are interpreted as **milliseconds**.

---

## Usage Examples

```cpp
#include "aethernet/numeric.hpp"

// Duration encoded exponentially, serialized via TieredInt
using Duration = AE_EXPONENT(uint32_t, TieredInt<1, 254>, 1.0, 90000.0);

Duration t1 = 12.5f;   // 12.5 seconds
Duration t2 = 0.003f;  // 3 milliseconds

auto sum = t1 + t2;    // type-safe, overflow-aware arithmetic

auto bytes = serialize(sum);         // serialize to bytes
auto restored = deserialize<Duration>(bytes);
```

---

## Integration Notes

* **Header-only**, zero external deps (beyond the C++ standard library)
* C++17 or newer
* Interoperable with Æthernet message packing
* Designed for deterministic, low-overhead serialization on MCUs and embedded systems
* Typical code size per instantiated type is minimal

---

## Running Tests

Build and run the unit tests with CMake.

```bash
# 1) Clone the repository
git clone https://github.com/aethernetio/aethernet-numeric.git

# 2) Enter the repo and update submodules
cd aethernet-numeric
git submodule update --init --recursive

# 3) Create a build directory
mkdir build
cd build

# 4) Generate build files with tests enabled
cmake -DAE_BUILD_TESTS=On ..

# 5) Build the project and run tests
cmake --build .
ctest .
```

---

© Æthernet Inc. — Part of the Æthernet Core Client Library
[https://aethernet.io](https://aethernet.io)
