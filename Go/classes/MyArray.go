package classes

import (
	"encoding/gob"
	"encoding/json"
	"fmt"
	"os"
)

type ArNode[T any] struct {
	Key T
}

type MyArray[T any] struct {
	data     []ArNode[T]
	size     int
	capacity int
}

func NewMyArray[T any]() *MyArray[T] {
	return &MyArray[T]{
		data:     make([]ArNode[T], 10),
		size:     0,
		capacity: 10,
	}
}

func NewMyArrayWithCapacity[T any](initCap int) *MyArray[T] {
	return &MyArray[T]{
		data:     make([]ArNode[T], initCap),
		size:     0,
		capacity: initCap,
	}
}

func (a *MyArray[T]) Get(index int) (T, error) {
	var zero T
	if index < 0 || index >= a.capacity {
		return zero, fmt.Errorf("index out of array bounds")
	}
	return a.data[index].Key, nil
}

func (a *MyArray[T]) Set(index int, value T) error {
	if index < 0 || index >= a.capacity {
		return fmt.Errorf("index out of array bounds")
	}
	a.data[index].Key = value
	return nil
}

func (a *MyArray[T]) resize() {
	newCap := a.capacity * 2
	if newCap == 0 {
		newCap = 1
	}
	newData := make([]ArNode[T], newCap)

	if a.size > 0 {
		copy(newData, a.data)
	}

	a.data = newData
	a.capacity = newCap
}

func (a *MyArray[T]) MPUSHBack(key T) {
	if a.size >= a.capacity {
		a.resize()
	}
	a.data[a.size].Key = key
	a.size++
}

func (a *MyArray[T]) MPUSHIndex(index int, key T) error {
	if index > a.capacity || index < 0 {
		return fmt.Errorf("array index out of bounds")
	}

	if a.size >= a.capacity {
		a.resize()
	}

	for i := a.size - 1; i >= index; i-- {
		a.data[i+1] = a.data[i]
	}
	a.data[index].Key = key
	a.size++
	return nil
}

func (a *MyArray[T]) MGET(index int) (T, error) {
	var zero T
	if index >= a.capacity || index < 0 {
		return zero, fmt.Errorf("array index out of bounds")
	}
	return a.data[index].Key, nil
}

func (a *MyArray[T]) MDEL(index int) error {
	var zero T
	if index >= a.capacity || index < 0 {
		return fmt.Errorf("array index out of bounds")
	}

	for i := index; i < a.capacity-1; i++ {
		a.data[i] = a.data[i+1]
	}
	a.data[a.capacity-1].Key = zero
	a.size--
	return nil
}

func (a *MyArray[T]) MSWAP(index int, swapkey T) error {
	if index >= a.size || index < 0 {
		return fmt.Errorf("array index out of bounds")
	}
	a.data[index].Key = swapkey
	return nil
}

func (a *MyArray[T]) Print() {
	for i := 0; i < a.capacity; i++ {
		fmt.Printf("%v ", a.data[i].Key)
	}
	fmt.Println()
}

func (a *MyArray[T]) MSize() int {
	return a.size
}

func (a *MyArray[T]) SaveText(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("could not open file for writing: %v", err)
	}
	defer file.Close()

	type SaveData struct {
		Size     int
		Capacity int
		Data     []T
	}

	keys := make([]T, a.capacity)
	for i := 0; i < a.capacity; i++ {
		keys[i] = a.data[i].Key
	}

	saveData := SaveData{
		Size:     a.size,
		Capacity: a.capacity,
		Data:     keys,
	}

	encoder := json.NewEncoder(file)
	if err := encoder.Encode(saveData); err != nil {
		return fmt.Errorf("error encoding  %v", err)
	}

	return nil
}

func (a *MyArray[T]) LoadText(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("could not open file for reading: %v", err)
	}
	defer file.Close()

	type SaveData struct {
		Size     int
		Capacity int
		Data     []T
	}

	var saveData SaveData
	decoder := json.NewDecoder(file)
	if err := decoder.Decode(&saveData); err != nil {
		return fmt.Errorf("error decoding  %v", err)
	}

	a.size = saveData.Size
	a.capacity = saveData.Capacity
	a.data = make([]ArNode[T], a.capacity)

	for i := 0; i < a.capacity && i < len(saveData.Data); i++ {
		a.data[i].Key = saveData.Data[i]
	}

	return nil
}

func (a *MyArray[T]) SaveBinary(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("could not open file for writing: %v", err)
	}
	defer file.Close()

	encoder := gob.NewEncoder(file)

	if err := encoder.Encode(a.size); err != nil {
		return err
	}
	if err := encoder.Encode(a.capacity); err != nil {
		return err
	}

	for i := 0; i < a.capacity; i++ {
		if err := encoder.Encode(a.data[i].Key); err != nil {
			return err
		}
	}

	return nil
}

func (a *MyArray[T]) LoadBinary(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("could not open file for reading: %v", err)
	}
	defer file.Close()

	decoder := gob.NewDecoder(file)

	var newSize, newCapacity int
	if err := decoder.Decode(&newSize); err != nil {
		return err
	}
	if err := decoder.Decode(&newCapacity); err != nil {
		return err
	}

	a.size = newSize
	a.capacity = newCapacity
	a.data = make([]ArNode[T], a.capacity)

	for i := 0; i < a.capacity; i++ {
		if err := decoder.Decode(&a.data[i].Key); err != nil {
			return err
		}
	}

	return nil
}
