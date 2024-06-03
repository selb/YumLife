/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2002-March-29    Jason Rohrer
 * Added Fortify inclusion.
 *
 * 2002-October-18    Jason Rohrer
 * Moved common include out of header and into platform-specific cpp files,
 * since MemoryTrack uses a mutex lock.
 *
 * 2011-March-9    Jason Rohrer
 * Removed Fortify inclusion.
 *
 * 2024-June-03 Marek Schwann
 * std::mutex better
 */

#pragma once
#include <mutex>

using MutexLock = std::mutex;
