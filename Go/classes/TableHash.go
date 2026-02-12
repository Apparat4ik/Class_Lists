package classes

import (
	"bufio"
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"strings"
)

type TableHash struct {
	// Исправление 1: Указываем конкретный тип [string] для дженерика
	buckets           []*ForwardList[string]
	countItems        int
	bucketsThereSmtng int
	loadFactor        float64
}

// NewTableHash создает таблицу.
func NewTableHash(initCap int) *TableHash {
	if initCap < 2 {
		initCap = 10
	}
	ht := &TableHash{
		// Исправление 2: make создает слайс указателей на ForwardList[string]
		buckets:    make([]*ForwardList[string], initCap),
		loadFactor: 0.75,
	}
	for i := range ht.buckets {
		// Исправление 3: Вызываем конструктор с типом [string]() без аргументов
		ht.buckets[i] = NewForwardList[string]()
	}
	return ht
}

func (ht *TableHash) Clear() {
	*ht = *NewTableHash(10)
}

func (ht *TableHash) Size() int   { return ht.countItems }
func (ht *TableHash) Empty() bool { return ht.countItems == 0 }

// --- кодирование пары key/value в одну строку списка ---
func encodeKV(key, value string) string {
	return key + "\t" + value
}

func decodeKV(s string) (key, value string, ok bool) {
	k, v, found := strings.Cut(s, "\t")
	if !found {
		return "", "", false
	}
	return k, v, true
}

// --- hash ---
func (ht *TableHash) hashString(key string) int {
	capacity := len(ht.buckets)
	sum := 0
	for i := 0; i < len(key); i++ {
		sum += int(uint8(key[i]))
	}

	result := (sum % (capacity - 1)) + 1
	if capacity%2 == 0 && result%2 == 0 {
		result++
	}
	return result % capacity
}

func (ht *TableHash) bucketIndex(key string) int {
	return ht.hashString(key)
}

// --- внутренние операции по списку ---

// Исправление 4: bucket теперь *ForwardList[string]
func (ht *TableHash) findInBucket(bucket *ForwardList[string], key string) (value string, found bool) {
	// Исправление 5: Итерация через Head (а не head), Next (а не next) и Key (а не val)
	for n := bucket.Head; n != nil; n = n.Next {
		k, v, ok := decodeKV(n.Key)
		if ok && k == key {
			return v, true
		}
	}
	return "", false
}

func (ht *TableHash) deleteFromBucket(bucket *ForwardList[string], key string) (deleted bool) {
	// Исправление 6: Используем FLNode[string] вместо FNode
	var prev *FLNode[string]
	for cur := bucket.Head; cur != nil; cur = cur.Next {
		k, _, ok := decodeKV(cur.Key)
		if ok && k == key {
			if prev == nil {
				bucket.Head = cur.Next
			} else {
				prev.Next = cur.Next
			}
			return true
		}
		prev = cur
	}
	return false
}

// --- публичное API ---
func (ht *TableHash) Contains(key string) bool {
	if ht.countItems == 0 {
		return false
	}
	idx := ht.bucketIndex(key)
	_, found := ht.findInBucket(ht.buckets[idx], key)
	return found
}

func (ht *TableHash) Add(key, value string) {
	if ht.Contains(key) {
		return
	}

	if float64(ht.countItems)/float64(len(ht.buckets)) >= ht.loadFactor {
		ht.rehash()
	}

	idx := ht.bucketIndex(key)
	// Исправление 7: Проверка через Head
	if ht.buckets[idx].Head == nil {
		ht.bucketsThereSmtng++
	}

	// Исправление 8: Используем PushBack вместо InsertAtTail
	ht.buckets[idx].PushBack(encodeKV(key, value))
	ht.countItems++

	if float64(ht.bucketsThereSmtng)/float64(len(ht.buckets)) >= 0.9 {
		ht.rehash()
	}
}

func (ht *TableHash) Get(key string) (string, bool) {
	idx := ht.bucketIndex(key)
	return ht.findInBucket(ht.buckets[idx], key)
}

func (ht *TableHash) Delete(key string) {
	if ht.countItems == 0 {
		return
	}

	idx := ht.bucketIndex(key)
	b := ht.buckets[idx]
	if b.Head == nil {
		return
	}

	deleted := ht.deleteFromBucket(b, key)
	if !deleted {
		return
	}

	ht.countItems--
	if b.Head == nil {
		ht.bucketsThereSmtng--
	}
}

func (ht *TableHash) Print() {
	for i := range ht.buckets {
		// Исправление 9: Доступ через Head, Key, Next
		for n := ht.buckets[i].Head; n != nil; n = n.Next {
			k, v, ok := decodeKV(n.Key)
			if ok {
				fmt.Printf("%s %s  ", k, v)
			}
		}
	}
	fmt.Println()
}

// --- rehash ---
func (ht *TableHash) rehash() {
	old := ht.buckets
	newCap := len(old) * 2
	if newCap < 2 {
		newCap = 10
	}

	ht.buckets = make([]*ForwardList[string], newCap)
	for i := range ht.buckets {
		ht.buckets[i] = NewForwardList[string]()
	}
	ht.countItems = 0
	ht.bucketsThereSmtng = 0

	for i := range old {
		for n := old[i].Head; n != nil; n = n.Next {
			k, v, ok := decodeKV(n.Key)
			if ok {
				ht.Add(k, v)
			}
		}
	}
}

// --- Text save/load ---
func (ht *TableHash) SaveText(filename string) error {
	f, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	if _, err := fmt.Fprintln(w, ht.countItems); err != nil {
		return err
	}

	for i := range ht.buckets {
		for n := ht.buckets[i].Head; n != nil; n = n.Next {
			k, v, ok := decodeKV(n.Key)
			if !ok {
				continue
			}
			if _, err := fmt.Fprintln(w, k, v); err != nil {
				return err
			}
		}
	}
	return w.Flush()
}

func (ht *TableHash) LoadText(filename string) error {
	f, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer f.Close()

	ht.Clear()

	r := bufio.NewReader(f)
	var n int
	if _, err := fmt.Fscanln(r, &n); err != nil {
		return err
	}

	for i := 0; i < n; i++ {
		var k, v string
		if _, err := fmt.Fscanln(r, &k, &v); err != nil {
			return err
		}
		ht.Add(k, v)
	}
	return nil
}

// --- Binary save/load ---
func writeU64(w io.Writer, x uint64) error {
	return binary.Write(w, binary.LittleEndian, x)
}
func readU64(r io.Reader, x *uint64) error {
	return binary.Read(r, binary.LittleEndian, x)
}

func writeString(w io.Writer, s string) error {
	l := uint32(len(s))
	if err := binary.Write(w, binary.LittleEndian, l); err != nil {
		return err
	}
	if l == 0 {
		return nil
	}
	_, err := w.Write([]byte(s))
	return err
}

func readString(r io.Reader) (string, error) {
	var l uint32
	if err := binary.Read(r, binary.LittleEndian, &l); err != nil {
		return "", err
	}
	if l == 0 {
		return "", nil
	}
	buf := make([]byte, l)
	if _, err := io.ReadFull(r, buf); err != nil {
		return "", err
	}
	return string(buf), nil
}

func (ht *TableHash) SaveBinary(filename string) error {
	f, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer f.Close()

	if err := writeU64(f, uint64(ht.countItems)); err != nil {
		return err
	}

	for i := range ht.buckets {
		for n := ht.buckets[i].Head; n != nil; n = n.Next {
			k, v, ok := decodeKV(n.Key)
			if !ok {
				continue
			}
			if err := writeString(f, k); err != nil {
				return err
			}
			if err := writeString(f, v); err != nil {
				return err
			}
		}
	}
	return nil
}

func (ht *TableHash) LoadBinary(filename string) error {
	f, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer f.Close()

	ht.Clear()

	var n uint64
	if err := readU64(f, &n); err != nil {
		return err
	}

	for i := uint64(0); i < n; i++ {
		k, err := readString(f)
		if err != nil {
			return err
		}
		v, err := readString(f)
		if err != nil {
			return err
		}
		ht.Add(k, v)
	}
	return nil
}
