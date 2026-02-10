#include <benchmark/benchmark.h>
#include "Classes/MyArray.hpp"
#include "Classes/MyFList.hpp"
#include "Classes/MyDoubleList.hpp"
#include "Classes/MyStack.hpp"
#include "Classes/MyQueue.hpp"
#include "Classes/BinaryTree.hpp"
#include "Classes/OpenTable.hpp"
#include "Classes/TableHash.hpp"


static void BM_MyArray_PushBack(benchmark::State& state) {
    for (auto _ : state) {
        MyArray<int> arr;
        for (int i = 0; i < state.range(0); i++) {
            arr.MPUSH_back(i);
        }
    }
    // Вывод сложности O(N)
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_MyArray_PushBack)->Range(8, 8 << 10)->Complexity();


static void BM_ForwardList_PushBack(benchmark::State& state) {
    for (auto _ : state) {
        ForwardList<int> fl;
        for (int i = 0; i < state.range(0); i++) {
            fl.FPUSH_back(i);
        }
    }
    // Вывод сложности O(N)
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_ForwardList_PushBack)->Range(8, 8 << 10)->Complexity();


static void BM_DoubleList_PushBack(benchmark::State& state) {
    for (auto _ : state) {
        DoubleList<int> dl;
        for (int i = 0; i < state.range(0); i++) {
            dl.LPUSH_back(i);
        }
    }
    // Вывод сложности O(N)
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_DoubleList_PushBack)->Range(8, 8 << 10)->Complexity();


static void BM_Stack_Push(benchmark::State& state) {
    for (auto _ : state) {
        MyStack<int> st;
        for (int i = 0; i < state.range(0); i++) {
            st.SPUSH(i);
        }
    }
    // Вывод сложности O(N)
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Stack_Push)->Range(8, 8 << 10)->Complexity();


static void BM_Queue_Push(benchmark::State& state) {
    for (auto _ : state) {
        MyQueue<int> q;
        for (int i = 0; i < state.range(0); i++) {
            q.QPUSH(i);
        }
    }
    // Вывод сложности O(N)
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Queue_Push)->Range(8, 8 << 10)->Complexity();


static void BM_Tree_Insert(benchmark::State& state) {
    for (auto _ : state) {
        FBTree<int> tree;
        for (int i = 0; i < state.range(0); i++) {
            tree.TINSERT(i);
        }
    }
    // Вывод сложности O(N)
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Tree_Insert)->Range(8, 8 << 10)->Complexity();


static void BM_OpenTable_insert(benchmark::State& state) {
    for (auto _ : state) {
        OpenTable<int, int> otb;
        for (int i = 0; i < state.range(0); i++) {
            otb.insert(i, i);
        }
    }
    // Вывод сложности O(N)
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_OpenTable_insert)->Range(8, 8 << 10)->Complexity();


static void BM_TableHash_insert(benchmark::State& state) {
    for (auto _ : state) {
        TableHash<int, int> tbh;
        for (int i = 0; i < state.range(0); i++) {
            tbh.HTADD(i, i);
        }
    }
    // Вывод сложности O(N)
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_TableHash_insert)->Range(8, 8 << 10)->Complexity();

BENCHMARK_MAIN();