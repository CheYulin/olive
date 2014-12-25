/**
 * The top-down bfs use a bitmask as a immediate representation of the 
 * working set. It is based on a single partition of a graph.
 *
 * Author: Yichao Cheng (onesuperclark@gmail.com)
 * Created on: 2014-12-17
 * Last Modified: 2014-12-18
 */

#include <vector>


/**
 * The following kernel expands from a queue and collects the destination
 * nodes in a O(N) bit mask (array).
 */
__global__
void queue2mask(
    const EdgeId *vertices,
    const Vertex *edges,
    int *levels,
    int *mask,
    VertexId *queue,
    int *queueSize,
    int curLevel)
{
    int tid = THREAD_INDEX;
    if (tid >= *queueSize) return;
    VertexId outNode = queue[tid];
    EdgeId first = vertices[outNode];
    EdgeId last = vertices[outNode + 1];
    for (EdgeId edge = first; edge < last; edge ++) {
        VertexId inNode = edges[edge].localId;
        if (levels[inNode] == INF) {
            levels[inNode] = curLevel + 1;
            mask[inNode] = 1;
        }
    }
}

/**
 * The following kernel compacts the bit mask to a queue.
 */
__global__
void mask2queue(
    size_t n,
    int *mask,
    VertexId *queue,
    int *queueSize)
{
    int tid = THREAD_INDEX;
    if (tid >= n) return;
    if (mask[tid] == 1) {
        mask[tid] = 0;
        size_t offset = atomicAdd(queueSize, 1);
        queue[offset] = tid;
    }
}

/**
 * The following algorithm use a queue/bitmask strategy.
 * 
 * @param partition The graph partition
 * @param nodes     The number of nodes in the graph.
 * @param source    The source node to traverse f
rom.
 * @return a vector containing the BFS level for each node.
 */
std::vector<int> bfs_top_down(const Partition &par, VertexId nodes, VertexId source) {
    GRD<int> levels;
    levels.reserve(nodes);
    levels.allTo(INF);

    GRD<int> mask;
    mask.reserve(nodes);
    mask.allTo(0);

    GRD<VertexId> queue;
    queue.reserve(nodes);

    int *queueSize;
    queueSize = (int *) malloc(sizeof(int));

    int *queueSize_d;
    CUDA_CHECK(cudaMalloc(&queueSize_d, sizeof(int)));

    int curLevel = 0;
    levels.set(source, 0);
    queue.set(0, source);
    *queueSize = 1;
    CUDA_CHECK(H2D(queueSize_d, queueSize, sizeof(int)));

    while (true) {

        CUDA_CHECK(D2H(queueSize, queueSize_d, sizeof(int)));
        // printf("%d queue size: %d\n", curLevel, *queueSize);
        // queue.persist();
        // for (int i = 0; i < *queueSize; i++) {
        //     printf("%d ", queue[i]);
        // }
        // printf("\n");

        // Terminates when the queue is empty
        if (*queueSize == 0) {
            break;
        }

        auto config = util::kernelConfig(*queueSize);
        queue2mask <<< config.first, config.second>>>(
            par.vertices.elemsDevice,
            par.edges.elemsDevice,
            levels.elemsDevice,
            mask.elemsDevice,
            queue.elemsDevice,
            queueSize_d,
            curLevel);

        CUDA_CHECK(cudaThreadSynchronize());

        // Clear the queue before generating it
        *queueSize = 0;
        CUDA_CHECK(H2D(queueSize_d, queueSize, sizeof(int)));

        config = util::kernelConfig(nodes);
        mask2queue <<< config.first, config.second>>>(
            nodes,
            mask.elemsDevice,
            queue.elemsDevice,
            queueSize_d);

        curLevel += 1;
    }

    levels.persist();
    // Return the `levels` array
    return std::vector<int>(levels.elemsHost, levels.elemsHost + nodes);
}
