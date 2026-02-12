package classes

import (
	"os"
	"testing"
)

// TestNewOpenTable проверяет создание новой хэш-таблицы
func TestNewOpenTable(t *testing.T) {
	tests := []struct {
		name     string
		capacity int
		wantCap  int
	}{
		{"Positive capacity", 20, 20},
		{"Zero capacity", 0, 10},
		{"Negative capacity", -5, 10},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			ht := NewOpenTable[int, string](tt.capacity)
			if ht == nil {
				t.Fatal("NewOpenTable returned nil")
			}
			if ht.GetCap() != tt.wantCap {
				t.Errorf("capacity = %d, want %d", ht.GetCap(), tt.wantCap)
			}
			if !ht.Empty() {
				t.Error("new table should be empty")
			}
		})
	}
}

// TestInsertAndFind проверяет вставку и поиск элементов
func TestInsertAndFind(t *testing.T) {
	t.Run("Insert and find integers", func(t *testing.T) {
		ht := NewOpenTable[int, string](10)

		ht.Insert(1, "one")
		ht.Insert(2, "two")
		ht.Insert(3, "three")

		if ht.Size() != 3 {
			t.Errorf("size = %d, want 3", ht.Size())
		}

		if !ht.Find(1) {
			t.Error("key 1 should exist")
		}
		if !ht.Find(2) {
			t.Error("key 2 should exist")
		}
		if !ht.Find(3) {
			t.Error("key 3 should exist")
		}
		if ht.Find(999) {
			t.Error("key 999 should not exist")
		}
	})

	t.Run("Insert and find strings", func(t *testing.T) {
		ht := NewOpenTable[string, int](10)

		ht.Insert("apple", 100)
		ht.Insert("banana", 200)
		ht.Insert("cherry", 300)

		if ht.Size() != 3 {
			t.Errorf("size = %d, want 3", ht.Size())
		}

		if !ht.Find("apple") {
			t.Error("key 'apple' should exist")
		}
		if ht.Find("orange") {
			t.Error("key 'orange' should not exist")
		}
	})

	t.Run("Insert duplicate key", func(t *testing.T) {
		ht := NewOpenTable[int, string](10)

		ht.Insert(1, "first")
		ht.Insert(1, "second")

		if ht.Size() != 1 {
			t.Errorf("size = %d, want 1 (duplicate should not increase size)", ht.Size())
		}

		val := ht.GetValue(1)
		if val != "first" {
			t.Errorf("value = %s, want 'first' (duplicate should not overwrite)", val)
		}
	})
}

// TestGetValue проверяет получение значений
func TestGetValue(t *testing.T) {
	ht := NewOpenTable[int, string](10)

	testData := map[int]string{
		10: "ten",
		20: "twenty",
		30: "thirty",
	}

	for k, v := range testData {
		ht.Insert(k, v)
	}

	for k, want := range testData {
		got := ht.GetValue(k)
		if got != want {
			t.Errorf("GetValue(%d) = %s, want %s", k, got, want)
		}
	}
}

// TestErase проверяет удаление элементов
func TestErase(t *testing.T) {
	t.Run("Erase existing key", func(t *testing.T) {
		ht := NewOpenTable[int, string](10)

		ht.Insert(1, "one")
		ht.Insert(2, "two")
		ht.Insert(3, "three")

		ht.Erase(2)

		if ht.Size() != 2 {
			t.Errorf("size = %d, want 2", ht.Size())
		}

		if ht.Find(2) {
			t.Error("key 2 should not exist after erase")
		}
		if !ht.Find(1) || !ht.Find(3) {
			t.Error("keys 1 and 3 should still exist")
		}
	})

	t.Run("Erase from empty table", func(t *testing.T) {
		ht := NewOpenTable[int, string](10)
		ht.Erase(999) // не должно паниковать

		if ht.Size() != 0 {
			t.Errorf("size = %d, want 0", ht.Size())
		}
	})
}

// TestRehash проверяет автоматическое расширение таблицы
func TestRehashOP(t *testing.T) {
	ht := NewOpenTable[int, string](4)
	initialCap := ht.GetCap()

	// Вставляем достаточно элементов, чтобы вызвать rehash
	for i := 1; i <= 10; i++ {
		ht.Insert(i, "value")
	}

	newCap := ht.GetCap()
	if newCap <= initialCap {
		t.Errorf("capacity = %d, should be greater than initial %d", newCap, initialCap)
	}

	// Проверяем, что все элементы остались
	for i := 1; i <= 10; i++ {
		if !ht.Find(i) {
			t.Errorf("key %d should exist after rehash", i)
		}
	}

	if ht.Size() != 10 {
		t.Errorf("size = %d, want 10", ht.Size())
	}
}

// TestEmpty проверяет метод Empty
func TestEmpty(t *testing.T) {
	ht := NewOpenTable[int, string](10)

	if !ht.Empty() {
		t.Error("new table should be empty")
	}

	ht.Insert(1, "one")
	if ht.Empty() {
		t.Error("table should not be empty after insert")
	}

	ht.Erase(1)
	if !ht.Empty() {
		t.Error("table should be empty after erasing all elements")
	}
}

// TestGetKey проверяет метод GetKey
func TestGetKey(t *testing.T) {
	ht := NewOpenTable[int, string](10)

	ht.Insert(42, "answer")

	// Находим индекс, где хранится ключ 42
	found := false
	for i := 0; i < ht.GetCap(); i++ {
		key := ht.GetKey(i)
		if key == 42 {
			found = true
			break
		}
	}

	if !found {
		t.Error("inserted key should be found via GetKey")
	}
}

// TestSaveLoadText проверяет сохранение и загрузку в текстовый файл
func TestSaveLoadTextOP(t *testing.T) {
	filename := "test_table.txt"
	defer os.Remove(filename)

	// Создаем и заполняем таблицу
	original := NewOpenTable[int, string](10)
	testData := map[int]string{
		1: "one",
		2: "two",
		3: "three",
		4: "four",
		5: "five",
	}

	for k, v := range testData {
		original.Insert(k, v)
	}

	// Сохраняем
	err := original.SaveText(filename)
	if err != nil {
		t.Fatalf("SaveText failed: %v", err)
	}

	// Загружаем в новую таблицу
	loaded := NewOpenTable[int, string](10)
	err = loaded.LoadText(filename)
	if err != nil {
		t.Fatalf("LoadText failed: %v", err)
	}

	// Проверяем
	if loaded.Size() != original.Size() {
		t.Errorf("loaded size = %d, want %d", loaded.Size(), original.Size())
	}

	for k, want := range testData {
		if !loaded.Find(k) {
			t.Errorf("key %d not found after loading", k)
			continue
		}
		got := loaded.GetValue(k)
		if got != want {
			t.Errorf("GetValue(%d) = %s, want %s", k, got, want)
		}
	}
}

// TestSaveLoadBinary проверяет сохранение и загрузку в бинарный файл
func TestSaveLoadBinaryOP(t *testing.T) {
	filename := "test_table.bin"
	defer os.Remove(filename)

	// Создаем и заполняем таблицу
	original := NewOpenTable[string, int](10)
	testData := map[string]int{
		"alpha":   100,
		"beta":    200,
		"gamma":   300,
		"delta":   400,
		"epsilon": 500,
	}

	for k, v := range testData {
		original.Insert(k, v)
	}

	// Сохраняем
	err := original.SaveBinary(filename)
	if err != nil {
		t.Fatalf("SaveBinary failed: %v", err)
	}

	// Загружаем в новую таблицу
	loaded := NewOpenTable[string, int](10)
	err = loaded.LoadBinary(filename)
	if err != nil {
		t.Fatalf("LoadBinary failed: %v", err)
	}

	// Проверяем
	if loaded.Size() != original.Size() {
		t.Errorf("loaded size = %d, want %d", loaded.Size(), original.Size())
	}

	for k, want := range testData {
		if !loaded.Find(k) {
			t.Errorf("key %s not found after loading", k)
			continue
		}
		got := loaded.GetValue(k)
		if got != want {
			t.Errorf("GetValue(%s) = %d, want %d", k, got, want)
		}
	}
}

// TestLoadNonExistentFile проверяет загрузку из несуществующего файла
func TestLoadNonExistentFile(t *testing.T) {
	ht := NewOpenTable[int, string](10)

	err := ht.LoadText("nonexistent.txt")
	if err == nil {
		t.Error("LoadText should return error for nonexistent file")
	}

	err = ht.LoadBinary("nonexistent.bin")
	if err == nil {
		t.Error("LoadBinary should return error for nonexistent file")
	}
}

// TestFindInEmptyTable проверяет поиск в пустой таблице
func TestFindInEmptyTable(t *testing.T) {
	ht := NewOpenTable[int, string](10)

	if ht.Find(1) {
		t.Error("Find should return false for empty table")
	}
}

// TestSPRINT проверяет метод печати (визуальный тест)
func TestSPRINT(t *testing.T) {
	ht := NewOpenTable[int, string](10)

	ht.Insert(1, "one")
	ht.Insert(2, "two")

	// Просто проверяем, что не паникует
	ht.SPRINT()
}

// TestHashCollisions проверяет работу с коллизиями
func TestHashCollisions(t *testing.T) {
	ht := NewOpenTable[int, string](5)

	// Вставляем достаточно элементов, чтобы гарантировать коллизии
	for i := 1; i <= 15; i++ {
		ht.Insert(i, "value")
	}

	// Проверяем, что все элементы доступны
	for i := 1; i <= 15; i++ {
		if !ht.Find(i) {
			t.Errorf("key %d not found (collision handling failed)", i)
		}
	}
}

// TestMultipleRehashes проверяет несколько последовательных rehash
func TestMultipleRehashes(t *testing.T) {
	ht := NewOpenTable[int, int](2)

	// Вставляем много элементов, чтобы вызвать несколько rehash
	n := 100
	for i := 1; i <= n; i++ {
		ht.Insert(i, i*10)
	}

	if ht.Size() != n {
		t.Errorf("size = %d, want %d", ht.Size(), n)
	}

	// Проверяем все значения
	for i := 1; i <= n; i++ {
		if !ht.Find(i) {
			t.Errorf("key %d not found after multiple rehashes", i)
		}
		if got := ht.GetValue(i); got != i*10 {
			t.Errorf("GetValue(%d) = %d, want %d", i, got, i*10)
		}
	}
}
