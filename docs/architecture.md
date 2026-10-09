# Architecture

```text
SLAM frontend
  → compressed keyframe / submap payload
  → POSIX shared-memory slot
  → transport sender
  → DDS / weak network
  → edge receiver
  → SubmapRegistry
  → loop/inter-robot constraints
  → Graph-SLAM backend
  → versioned global corrections
```

## Same-host data path

`SharedFrameSlot` uses POSIX shared memory plus a named semaphore. The producer writes a payload once into the mapped region. A consumer obtains a read-only span while holding the semaphore, avoiding an intermediate application-level payload copy.

The public implementation is a bounded latest-frame slot, not a production queue. A production pool also needs leases, multiple slots, crash recovery, backpressure and generation-safe buffer reuse.

## Fusion contract

`SubmapRegistry` gives every submap a robot/session/local identity and accepts relative constraints only after both endpoints exist. Duplicate submaps and constraints are rejected, and every accepted operation advances the graph version.

The registry does not optimize poses. GTSAM/g2o/Ceres integration belongs behind a backend adapter.

## Transport boundary

Shared memory is for processes on one host. DDS carries metadata and selected payloads across robots and edge nodes. Pose should prefer latest-value delivery; graph transactions and corrections require reliable, versioned delivery.
