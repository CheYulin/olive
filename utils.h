/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Yichao Cheng
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/**
 * Utils.
 *
 * Author: Yichao Cheng (onesuperclark@gmail.com)
 * Created on: 2014-10-20
 * Last Modified: 2014-10-23
 */

#ifndef UTILS_H
#define UTILS_H

#include <utility>

#include "cuda_runtime.h"
#include "common.h"
#include "logging.h"

namespace util {

/**
 * Calculates the block number to launch a kernel by specifying
 * the thread number to fulfill the job and per-block thread number.
 *
 * The block number is redundant and can not exceed the limits
 * which is defined by the architecture.
 *
 * @param  threads         How many threads is requires
 * @param  threadsPerBlock How many threads in each block (256 by default)
 * @return                 A pair (block number, thread number per block)
 */
std::pair<int, int> kernelConfig(int threads,
                                 int threadsPerBlock = DEFAULT_THREADS_PER_BLOCK) {
    if (threads == 0) {
        return std::make_pair(0, 0);
    }

    assert(threadsPerBlock <= MAX_THREADS_PER_BLOCK);

    if (threads < threadsPerBlock) threadsPerBlock = threads;
    int blocks =  threads % threadsPerBlock == 0 ?
                  threads / threadsPerBlock :
                  threads / threadsPerBlock + 1;
    if (blocks > MAX_BLOCKS) blocks = MAX_BLOCKS;
    // LOG(INFO) << "The kernel is configured to (" << blocks
    //           << ", " << threadsPerBlock << ")";
    return std::make_pair(blocks, threadsPerBlock);
}


/**
 * Enable peer access from `self` to `other`
 */
void enablePeerAccess(int self, int other) {
    CUDA_CHECK(cudaSetDevice(self));
    int canAccess = 0;
    CUDA_CHECK(cudaDeviceCanAccessPeer(&canAccess, self, other));
    if (canAccess == 1) {
        CUDA_CHECK(cudaDeviceEnablePeerAccess(other, 0));
        LOG(INFO) << self << " enable peer access " << other;
    } else {
        LOG(WARNING) << self << " cannot access peer " << other;
    }
}

/**
 * disable peer access from `self` to `other`
 */
void disablePeerAccess(int self, int other) {
    CUDA_CHECK(cudaSetDevice(self));
    int canAccess = 0;
    CUDA_CHECK(cudaDeviceCanAccessPeer(&canAccess, self, other));
    if (canAccess == 1) {
        CUDA_CHECK(cudaDeviceDisablePeerAccess(other));
        LOG(INFO) << self << " disable peer access " << other;
    } else {
        LOG(WARNING) << self << " cannot access peer " << other;
    }
}

/**
 * Enable all-to-all peer access
 * TODO(onesuper): Temporarily making the following functions a part of utilities.
 */
void enableAllPeerAccess() {
    int numGpus = 0;
    CUDA_CHECK(cudaGetDeviceCount(&numGpus));
    LOG(INFO) << numGpus << " GPU detected";
    for (int i = 0; i < numGpus; i++) {
        for (int j = i + 1; j < numGpus; j++) {
            enablePeerAccess(i, j);
            enablePeerAccess(j, i);
        }
    }
}

void disableAllPeerAccess() {
    int numGpus = 0;
    CUDA_CHECK(cudaGetDeviceCount(&numGpus));
    LOG(INFO) << numGpus << " GPU detected";
    for (int i = 0; i < numGpus; i++) {
        for (int j = i + 1; j < numGpus; j++) {
            disablePeerAccess(i, j);
            disablePeerAccess(j, i);
        }
    }
}

void expectOverlapOnAllDevices() {
    int dev_count;
    cudaDeviceProp prop;
    cudaGetDeviceCount(&dev_count);
    for (int i = 0; i < dev_count; i++) {
        cudaGetDeviceProperties(&prop, i);
        assert(prop.deviceOverlap) ;
    }
}

/**
 * Checks if the string is a numeric number
 * @param  str String to check
 * @return     True If the string represents a numeric number
 */
bool isNumeric(const char *str) {
    assert(str);
    while ((* str) != '\0') {
        if (!isdigit(* str)) {
            return false;
        } else {
            str++;
        }
    }
    return true;
}

/**
 * Get the hash code of any given number very quickly
 * @param  a The number to hash
 * @return   The hash code
 */
size_t hashCode(size_t a) {
    a ^= (a << 13);
    a ^= (a >> 17);
    a ^= (a << 5);
    return a;
}



}  // namespace util

#endif  // UTILS_H
