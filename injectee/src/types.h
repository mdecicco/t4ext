#pragma once
#include <stdint.h>

namespace t4ext {
	typedef int8_t            i8;
	typedef int16_t           i16;
	typedef int32_t           i32;
	typedef int64_t           i64;

	typedef uint8_t           u8;
	typedef uint16_t          u16;
	typedef uint32_t          u32;
	typedef uint64_t          u64;

	typedef float             f32;
	typedef double            f64;

	typedef u8                undefined;
	typedef u16               undefined2;
	typedef u32               undefined4;
	typedef u64               undefined8;
	typedef u32               bool32;
	typedef undefined*        pointer;
};