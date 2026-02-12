package classes

import (
	"io"
	"os"
	"testing"
)

func captureOutput(f func()) string {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	f()

	w.Close()
	os.Stdout = old
	out, _ := io.ReadAll(r)
	return string(out)
}

func TestNewTableHash(t *testing.T) {
	ht := NewTableHash(5)
	if ht == nil {
		t.Fatal("NewTableHash returned nil")
	}
	if len(ht.buckets) != 5 {
		t.Errorf("Expected capacity 5, got %d", len(ht.buckets))
	}
	if ht.Size() != 0 {
		t.Error("New table should be empty")
	}

	// Test invalid init cap
	ht2 := NewTableHash(-1)
	if len(ht2.buckets) != 10 {
		t.Error("Should default to capacity 10 for invalid input")
	}
}

func TestAddAndGet(t *testing.T) {
	ht := NewTableHash(10)

	// 1. Add simple
	ht.Add("key1", "val1")
	if ht.Size() != 1 {
		t.Errorf("Size should be 1, got %d", ht.Size())
	}
	if val, ok := ht.Get("key1"); !ok || val != "val1" {
		t.Errorf("Get failed or wrong value: %v, %v", val, ok)
	}

	// 2. Update existing (should NOT update based on your C++ logic "if HT_AT return")
	ht.Add("key1", "newVal")
	if val, _ := ht.Get("key1"); val != "val1" {
		t.Errorf("Add should not overwrite existing key. Got %s", val)
	}

	// 3. Add multiple
	ht.Add("key2", "val2")
	if !ht.Contains("key2") {
		t.Error("Contains returned false for key2")
	}
}

func TestCollisions(t *testing.T) {
	// Force small capacity to ensure collisions or manual hash calculation check
	ht := NewTableHash(2)

	// Добавляем элементы, которые скорее всего попадут в один бакет (или просто заполнят таблицу)
	ht.Add("a", "1")
	ht.Add("b", "2")
	ht.Add("c", "3") // Trigger rehash likely here or next

	v1, ok1 := ht.Get("a")
	v2, ok2 := ht.Get("b")
	v3, ok3 := ht.Get("c")

	if !ok1 || v1 != "1" {
		t.Error("Failed to retrieve 'a'")
	}
	if !ok2 || v2 != "2" {
		t.Error("Failed to retrieve 'b'")
	}
	if !ok3 || v3 != "3" {
		t.Error("Failed to retrieve 'c'")
	}
}

func TestRehashHT(t *testing.T) {
	ht := NewTableHash(2)

	ht.Add("1", "v1")

	ht.Add("2", "v2")

	ht.Add("3", "v3")

	if len(ht.buckets) <= 2 {
		t.Errorf("Rehash did not trigger. Capacity is %d", len(ht.buckets))
	}
	if ht.Size() != 3 {
		t.Errorf("Size incorrect after rehash. Got %d", ht.Size())
	}

	if v, _ := ht.Get("1"); v != "v1" {
		t.Error("Lost data '1' after rehash")
	}
	if v, _ := ht.Get("3"); v != "v3" {
		t.Error("Lost data '3' after rehash")
	}
}

func TestDelete(t *testing.T) {
	ht := NewTableHash(10)
	ht.Add("k1", "v1")
	ht.Add("k2", "v2")
	ht.Add("k3", "v3")

	ht.Delete("k2")
	if ht.Contains("k2") {
		t.Error("k2 should be deleted")
	}
	if ht.Size() != 2 {
		t.Errorf("Size should be 2, got %d", ht.Size())
	}

	ht.Delete("missing")
	if ht.Size() != 2 {
		t.Error("Size changed when deleting missing key")
	}

	htEmpty := NewTableHash(10)
	htEmpty.Delete("any")

	ht.Delete("k1")
	if ht.Contains("k1") {
		t.Error("k1 failed delete")
	}
}

func TestClearAndEmpty(t *testing.T) {
	ht := NewTableHash(10)
	ht.Add("a", "b")

	if ht.Empty() {
		t.Error("Should not be empty")
	}

	ht.Clear()
	if !ht.Empty() {
		t.Error("Should be empty after Clear")
	}
	if ht.Size() != 0 {
		t.Error("Size should be 0")
	}

	if len(ht.buckets) != 10 {
		t.Error("Buckets not reset")
	}
}

func TestPrintHT(t *testing.T) {
	ht := NewTableHash(10)
	ht.Add("key", "val")

	output := captureOutput(func() {
		ht.Print()
	})

	expected := "key val  \n"
	if output != expected {

		if len(output) < 5 {
			t.Errorf("Print output too short: %q", output)
		}
	}
}

func TestSaveLoadText(t *testing.T) {
	filename := "test_db.txt"
	defer os.Remove(filename)

	ht := NewTableHash(10)
	ht.Add("foo", "bar")
	ht.Add("baz", "qux")

	// Save
	if err := ht.SaveText(filename); err != nil {
		t.Fatalf("SaveText failed: %v", err)
	}

	// Load to new table
	ht2 := NewTableHash(10)
	if err := ht2.LoadText(filename); err != nil {
		t.Fatalf("LoadText failed: %v", err)
	}

	if ht2.Size() != 2 {
		t.Errorf("Loaded size wrong: %d", ht2.Size())
	}
	if v, _ := ht2.Get("foo"); v != "bar" {
		t.Error("Data mismatch for foo")
	}
}

func TestSaveLoadBinary(t *testing.T) {
	filename := "test_db.bin"
	defer os.Remove(filename)

	ht := NewTableHash(10)
	ht.Add("binkey", "binval")
	ht.Add("emptyval", "")

	// Save
	if err := ht.SaveBinary(filename); err != nil {
		t.Fatalf("SaveBinary failed: %v", err)
	}

	// Load
	ht2 := NewTableHash(10)
	if err := ht2.LoadBinary(filename); err != nil {
		t.Fatalf("LoadBinary failed: %v", err)
	}

	if ht2.Size() != 2 {
		t.Errorf("Loaded size wrong: %d", ht2.Size())
	}
	if v, _ := ht2.Get("binkey"); v != "binval" {
		t.Error("Data mismatch")
	}
	if v, ok := ht2.Get("emptyval"); !ok || v != "" {
		t.Error("Empty value failed")
	}
}

func TestFileErrors(t *testing.T) {
	ht := NewTableHash(10)

	if err := ht.LoadText("missing_file.txt"); err == nil {
		t.Error("Expected error loading missing text file")
	}
	if err := ht.LoadBinary("missing_file.bin"); err == nil {
		t.Error("Expected error loading missing binary file")
	}

	if err := ht.SaveText("/invalid/path/file.txt"); err == nil {
		t.Error("Expected error saving to invalid path")
	}
}

func TestEdgeCases(t *testing.T) {
	ht := NewTableHash(10)
	ht.Add("key_with_no_tab", "val\twith\ttabs")

	val, ok := ht.Get("key_with_no_tab")
	if !ok || val != "val\twith\ttabs" {
		t.Errorf("Value with tabs broken: %q", val)
	}
}
