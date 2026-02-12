package classes

import (
	"fmt"
	"testing"
)

// Вспомогательная функция для генерации диапазонов, аналогичная Range(8, 8<<10)
func getRanges() []int {
	return []int{8, 64, 512, 4096, 8192}
}

func BenchmarkMyArray_PushBack(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				// ВАЖНО: Создаем новую структуру внутри цикла, как в C++ коде
				var arr MyArray[int]
				for k := 0; k < size; k++ {
					arr.MPUSHBack(k)
				}
			}
		})
	}
}

func BenchmarkForwardList_PushBack(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				var fl ForwardList[int]
				for k := 0; k < size; k++ {
					fl.PushBack(k)
				}
			}
		})
	}
}

func BenchmarkDoubleList_PushBack(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				dl := NewDoubleList[int]()
				for k := 0; k < size; k++ {
					dl.LPushBack(k)
				}
			}
		})
	}
}

func BenchmarkStack_Push(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				st := NewMyStack[int]()
				for k := 0; k < size; k++ {
					st.Push(k)
				}
			}
		})
	}
}

func BenchmarkQueue_Push(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				q := NewMyQueue[int](5)
				for k := 0; k < size; k++ {
					q.Push(k)
				}
			}
		})
	}
}

func BenchmarkTree_Insert(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				var tree FullBinaryTree[int]
				for k := 0; k < size; k++ {
					tree.Insert(k)
				}
			}
		})
	}
}

func BenchmarkOpenTable_insert(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				otb := NewOpenTable[int, int](10)
				for k := 0; k < size; k++ {
					otb.Insert(k, k)
				}
			}
		})
	}
}

func BenchmarkTableHash_insert(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				tbh := NewTableHash(10)
				for k := 0; k < size; k++ {
					tbh.Add("a", "b")
				}
			}
		})
	}
}

func BenchmarkFullBinaryTree_insert(b *testing.B) {
	for _, size := range getRanges() {
		b.Run(fmt.Sprintf("Size_%d", size), func(b *testing.B) {
			for i := 0; i < b.N; i++ {
				tbh := NewFullBinaryTree[int]()
				for k := 0; k < size; k++ {
					tbh.Insert(k)
				}
			}
		})
	}
}
