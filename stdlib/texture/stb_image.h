// stb_image.h — STUB for headless/CI builds
// The real stb_image.h (https://github.com/nothings/stb) can be dropped in here
// to enable actual PNG/JPG loading. Until then, all loads return nullptr.
//
// Stub provides:
//   stbi_load()           — always returns nullptr
//   stbi_image_free()     — no-op
//   stbi_failure_reason() — returns a fixed message

#pragma once

#ifndef STBI_NO_STDIO
#define STBI_NO_STDIO
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;

// Load an image from a file path.
// Returns nullptr always in this stub (headless mode).
static inline stbi_uc* stbi_load(const char* /*filename*/,
                                  int* x, int* y, int* channels_in_file,
                                  int /*desired_channels*/)
{
    if (x)                *x = 0;
    if (y)                *y = 0;
    if (channels_in_file) *channels_in_file = 0;
    return (stbi_uc*)0;
}

// Free pixel data returned by stbi_load — no-op in stub.
static inline void stbi_image_free(void* /*retval_from_stbi_load*/) {}

// Return a human-readable reason for the last load failure.
static inline const char* stbi_failure_reason(void)
{
    return "stub: stb_image not available";
}

#ifdef __cplusplus
}
#endif
