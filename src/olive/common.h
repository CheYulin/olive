/**
 * Common defines, including constants, macros and types.
 *
 * Author: Yichao Cheng (onesuperclark@gmail.com)
 * Created on: 2014-10-20
 * Last Modified: 2014-10-23
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <inttypes.h>

<<<<<<< HEAD


/**
 * A wrapper that asserts the success of CUDA calls
 * TODO(onesuper): replace it with a method which throws an exception
 * 
 */
#define CUDA_CHECK(cuda_call)                                           \
    do {                                                                \
        cudaError_t err = cuda_call;                                    \
        if (err != cudaSuccess) {                                       \
        fprintf(stderr, "CUDA Error in file '%s' in line %i : %s.\n",   \
                __FILE__, __LINE__, cudaGetErrorString(err));           \
        assert(false);                                                  \
        }                                                               \
    } while (0)
=======
#include "cuda.h"
>>>>>>> 95039a5bd2b30e4a6e9779d3cf8ff2c44aa7564b


/** One word equals 64 bit. */
typedef uint64_t Word;


/** Defines type of the number space for vertex id. */
typedef uint32_t VertexId;


/** Defines the number space for edge id. */
typedef uint32_t EdgeId;


/** Defines the type for the partition identifier. */
typedef uint32_t PartitionId;

/**
 * Constants for kernel configuration.
 */
const int DEFAULT_THREADS_PER_BLOCK = 256;

/**
 * Machine-specified limits.
 */
const int MAX_BLOCKS = 65535;

/**
 * Machine-specified limits. 1024 for sm3.5, 512 for sm2.0
 */
const int MAX_THREADS_PER_BLOCK = 1024;

/**
 * Machine-specified limits.
 */
const int MAX_THREADS = MAX_THREADS_PER_BLOCK * MAX_BLOCKS;


/**
 * Constants for thread identification. Only one dimension is used.
 */
#define BLOCK_INDEX blockIdx.x

#define THREAD_INDEX (threadIdx.x + blockDim.x * blockIdx.x)

/**
 * A wrapper that asserts the success of CUDA calls
 * TODO(onesuper): replace it with a method which throws an exception
 *
 */
#define CUDA_CHECK(cuda_call)                                           \
    do {                                                                \
        cudaError_t err = cuda_call;                                    \
        if (err != cudaSuccess) {                                       \
        fprintf(stderr, "CUDA Error in file '%s' in line %i : %s.\n",   \
                __FILE__, __LINE__, cudaGetErrorString(err));           \
        assert(false);                                                  \
        }                                                               \
    } while (0);

/** Generic success and failure */
typedef enum {
    SUCCESS = 0,
    FAILURE,
} Error;


/** Different levels of memory location  */
typedef enum {
    CPU_ONLY = 0,   // Normal C allocations
    PINNED,         // Zero-copy memory
    MAPPED,         // Allocate on host side and maps the allocation into
                    // CUDA address space. The device pointer to the memory
                    // is obtained by calling cudaHostGetDevicePointer().
    MANAGED,        // Unified memory supported in CUDA 6.0
    GPU_ONLY,       // Normal CUDA allocations
} MemoryLevel;


#endif  // COMMON_H
