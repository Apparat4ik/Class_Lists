package classes

import (
	"bufio"
	"encoding/binary"
	"fmt"
	"io"
	"os"
)

// OTNode represents a key-value node in the hash table
type OTNode[Key comparable, Value any] struct {
	key   Key
	value Value
}

// OpenTable is a hash table implementation with linear probing
type OpenTable[Key comparable, Value any] struct {
	buckets           []OTNode[Key, Value]
	countItems        int
	bucketsThersSmtng int
}

// NewOpenTable creates a new hash table with specified initial capacity
func NewOpenTable[Key comparable, Value any](initCap int) *OpenTable[Key, Value] {
	if initCap <= 0 {
		initCap = 10
	}
	return &OpenTable[Key, Value]{
		buckets:           make([]OTNode[Key, Value], initCap),
		countItems:        0,
		bucketsThersSmtng: 0,
	}
}

// hashInt computes hash for integer keys
func (ot *OpenTable[Key, Value]) hashInt(key int) int {
	hash := 0
	for key > 0 {
		hash ^= (key & 0xFF)
		key >>= 8
	}
	return hash % len(ot.buckets)
}

// hashString computes hash for string keys
func (ot *OpenTable[Key, Value]) hashString(key string) int {
	sum := 0
	for _, c := range key {
		sum += int(uint8(c))
	}

	result := (sum % (len(ot.buckets) - 1)) + 1

	// Make result odd if table size is even
	if len(ot.buckets)%2 == 0 && result%2 == 0 {
		result++
	}

	return result
}

// hash computes the hash based on key type
func (ot *OpenTable[Key, Value]) hash(key Key) int {
	var anyKey any = key
	switch k := anyKey.(type) {
	case int:
		return ot.hashInt(k)
	case string:
		return ot.hashString(k)
	default:
		// For other types, use a simple hash
		s := fmt.Sprintf("%v", key)
		if len(s) == 0 {
			return 0
		}
		return int(s[0]) % len(ot.buckets)
	}
}

// getBucketIndex returns the bucket index for a given key
func (ot *OpenTable[Key, Value]) getBucketIndex(key Key) int {
	return ot.hash(key) % len(ot.buckets)
}

// rehash expands the hash table and rehashes all elements
func (ot *OpenTable[Key, Value]) rehash() {
	oldBuckets := ot.buckets
	ot.buckets = make([]OTNode[Key, Value], len(oldBuckets)*2)
	ot.countItems = 0
	ot.bucketsThersSmtng = 0

	var zeroKey Key
	for i := 0; i < len(oldBuckets); i++ {
		if oldBuckets[i].key != zeroKey {
			ot.Insert(oldBuckets[i].key, oldBuckets[i].value)
		}
	}
}

// Find checks if a key exists in the hash table
func (ot *OpenTable[Key, Value]) Find(key Key) bool {
	if ot.countItems == 0 {
		return false
	}

	var zeroKey Key
	index := ot.getBucketIndex(key)
	start := index

	for ot.buckets[index].key != zeroKey {
		if ot.buckets[index].key == key {
			return true
		}
		index = (index + 1) % len(ot.buckets)
		if index == start {
			break
		}
	}

	return false
}

// Insert adds a key-value pair to the hash table
func (ot *OpenTable[Key, Value]) Insert(key Key, val Value) {
	if ot.Find(key) {
		return // Key already exists
	}

	var zeroKey Key
	index := ot.getBucketIndex(key)
	start := index

	// Linear probing
	for ot.buckets[index].key != zeroKey {
		index = (index + 1) % len(ot.buckets)
		if index == start {
			// Table is full - rehash
			ot.rehash()
			ot.Insert(key, val)
			return
		}
	}

	if ot.buckets[index].key == zeroKey {
		ot.bucketsThersSmtng++
	}
	ot.buckets[index].key = key
	ot.buckets[index].value = val
	ot.countItems++

	// Rehash if load factor > 0.75
	if ot.countItems*4 > len(ot.buckets)*3 {
		ot.rehash()
	}

	// Rehash if bucket usage >= 0.9
	if float64(ot.bucketsThersSmtng)/float64(len(ot.buckets)) >= 0.9 {
		ot.rehash()
	}
}

// Erase removes a key from the hash table
func (ot *OpenTable[Key, Value]) Erase(key Key) {
	if ot.countItems == 0 {
		return
	}

	var zeroKey Key
	var zeroValue Value
	index := ot.getBucketIndex(key)

	for ot.buckets[index].key != key {
		index = (index + 1) % len(ot.buckets)
	}

	ot.buckets[index].key = zeroKey
	ot.buckets[index].value = zeroValue
	ot.countItems--
}

// Size returns the number of elements in the hash table
func (ot *OpenTable[Key, Value]) Size() int {
	return ot.countItems
}

// Empty checks if the hash table is empty
func (ot *OpenTable[Key, Value]) Empty() bool {
	return ot.countItems == 0
}

// GetCap returns the capacity of the hash table
func (ot *OpenTable[Key, Value]) GetCap() int {
	return len(ot.buckets)
}

// GetKey returns the key at the specified index
func (ot *OpenTable[Key, Value]) GetKey(i int) Key {
	return ot.buckets[i].key
}

// GetValue returns the value associated with the given key
func (ot *OpenTable[Key, Value]) GetValue(k Key) Value {
	index := ot.getBucketIndex(k)
	for ot.buckets[index].key != k {
		index = (index + 1) % len(ot.buckets)
	}
	return ot.buckets[index].value
}

// SPRINT prints all key-value pairs in the hash table
func (ot *OpenTable[Key, Value]) SPRINT() {
	var zeroKey Key
	for i := 0; i < len(ot.buckets); i++ {
		if ot.buckets[i].key != zeroKey {
			fmt.Printf("%v %v\n", ot.buckets[i].key, ot.buckets[i].value)
		}
	}
	fmt.Println()
}

// SaveText saves the hash table to a text file
func (ot *OpenTable[Key, Value]) SaveText(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	writer := bufio.NewWriter(file)

	// Write metadata
	fmt.Fprintf(writer, "%d\n", ot.countItems)
	fmt.Fprintf(writer, "%d\n", len(ot.buckets))
	fmt.Fprintf(writer, "%d\n", ot.bucketsThersSmtng)

	// Write key-value pairs
	var zeroKey Key
	for i := 0; i < len(ot.buckets); i++ {
		if ot.buckets[i].key != zeroKey {
			fmt.Fprintf(writer, "%v\n", ot.buckets[i].key)
			fmt.Fprintf(writer, "%v\n", ot.buckets[i].value)
		}
	}

	return writer.Flush()
}

// LoadText loads the hash table from a text file
func (ot *OpenTable[Key, Value]) LoadText(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	var newCountItems, newCapacity, newBucketsThersSmtng int

	scanner.Scan()
	fmt.Sscanf(scanner.Text(), "%d", &newCountItems)
	scanner.Scan()
	fmt.Sscanf(scanner.Text(), "%d", &newCapacity)
	scanner.Scan()
	fmt.Sscanf(scanner.Text(), "%d", &newBucketsThersSmtng)

	// Recreate table with new capacity
	ot.buckets = make([]OTNode[Key, Value], newCapacity)
	ot.countItems = 0
	ot.bucketsThersSmtng = 0

	// Read and insert key-value pairs
	for i := 0; i < newCountItems; i++ {
		var key Key
		var val Value

		scanner.Scan()
		fmt.Sscanf(scanner.Text(), "%v", &key)
		scanner.Scan()
		fmt.Sscanf(scanner.Text(), "%v", &val)

		ot.Insert(key, val)
	}

	return nil
}

// SaveBinary saves the hash table to a binary file
func (ot *OpenTable[Key, Value]) SaveBinary(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	// Write metadata
	binary.Write(file, binary.LittleEndian, int64(ot.countItems))
	binary.Write(file, binary.LittleEndian, int64(len(ot.buckets)))
	binary.Write(file, binary.LittleEndian, int64(ot.bucketsThersSmtng))

	// Write key-value pairs
	var zeroKey Key
	for i := 0; i < len(ot.buckets); i++ {
		if ot.buckets[i].key != zeroKey {
			writeValue(file, ot.buckets[i].key)
			writeValue(file, ot.buckets[i].value)
		}
	}

	return nil
}

// LoadBinary loads the hash table from a binary file
func (ot *OpenTable[Key, Value]) LoadBinary(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var newCountItems, newCapacity, newBucketsThersSmtng int64
	binary.Read(file, binary.LittleEndian, &newCountItems)
	binary.Read(file, binary.LittleEndian, &newCapacity)
	binary.Read(file, binary.LittleEndian, &newBucketsThersSmtng)

	// Recreate table with new capacity
	ot.buckets = make([]OTNode[Key, Value], newCapacity)
	ot.countItems = 0
	ot.bucketsThersSmtng = 0

	// Read and insert key-value pairs
	for i := 0; i < int(newCountItems); i++ {
		var key Key
		var val Value

		readValue(file, &key)
		readValue(file, &val)

		ot.Insert(key, val)
	}

	return nil
}

// Helper function to write values to binary file
func writeValue(w io.Writer, v any) error {
	switch val := v.(type) {
	case int:
		return binary.Write(w, binary.LittleEndian, int64(val))
	case string:
		length := int64(len(val))
		binary.Write(w, binary.LittleEndian, length)
		_, err := w.Write([]byte(val))
		return err
	default:
		return fmt.Errorf("unsupported type for binary write")
	}
}

// Helper function to read values from binary file
func readValue(r io.Reader, v any) error {
	switch val := v.(type) {
	case *int:
		var i int64
		err := binary.Read(r, binary.LittleEndian, &i)
		*val = int(i)
		return err
	case *string:
		var length int64
		binary.Read(r, binary.LittleEndian, &length)
		buf := make([]byte, length)
		_, err := r.Read(buf)
		*val = string(buf)
		return err
	default:
		return fmt.Errorf("unsupported type for binary read")
	}
}
