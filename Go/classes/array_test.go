package classes

import (
	"os"
	"testing"
)

func assertArrayEquals(t *testing.T, arr *MyArray[int], expected []int) {
	if arr.MSize() != len(expected) {
		t.Errorf("Expected size %d, got %d", len(expected), arr.MSize())
	}
	for i, v := range expected {
		val, err := arr.Get(i)
		if err != nil {
			t.Errorf("Unexpected error getting index %d: %v", i, err)
		}
		if val != v {
			t.Errorf("At index %d: expected %d, got %d", i, v, val)
		}
	}
}

func TestNewMyArray(t *testing.T) {
	arr := NewMyArray[int]()
	if arr.MSize() != 0 {
		t.Errorf("New array should have size 0")
	}
	if arr.capacity != 10 {
		t.Errorf("New array should have default capacity 10")
	}

	arrCap := NewMyArrayWithCapacity[int](5)
	if arrCap.capacity != 5 {
		t.Errorf("Expected capacity 5, got %d", arrCap.capacity)
	}
}

func TestMPUSHBack_And_Resize(t *testing.T) {
	arr := NewMyArrayWithCapacity[int](2)

	arr.MPUSHBack(1)
	arr.MPUSHBack(2)

	if arr.capacity != 2 {
		t.Errorf("Capacity should still be 2")
	}

	arr.MPUSHBack(3)

	if arr.capacity != 4 {
		t.Errorf("Capacity should have doubled to 4, got %d", arr.capacity)
	}

	assertArrayEquals(t, arr, []int{1, 2, 3})
}

func TestGet_Set_Errors(t *testing.T) {
	arr := NewMyArrayWithCapacity[int](5)
	arr.MPUSHBack(10)

	val, err := arr.Get(0)
	if err != nil || val != 10 {
		t.Errorf("Get(0) failed")
	}

	val, err = arr.MGET(0)
	if err != nil || val != 10 {
		t.Errorf("MGET(0) failed")
	}

	_, err = arr.Get(-1)
	if err == nil {
		t.Error("Expected error for Get(-1)")
	}

	_, err = arr.Get(100)
	if err == nil {
		t.Error("Expected error for Get(100)")
	}

	err = arr.Set(0, 99)
	if err != nil {
		t.Errorf("Set failed: %v", err)
	}
	val, _ = arr.Get(0)
	if val != 99 {
		t.Errorf("Set did not update value")
	}

	err = arr.Set(100, 50)
	if err == nil {
		t.Error("Expected error for Set(100)")
	}
}

func TestMPUSHIndex(t *testing.T) {
	arr := NewMyArrayWithCapacity[int](5)
	arr.MPUSHBack(1)
	arr.MPUSHBack(3)

	err := arr.MPUSHIndex(1, 2)
	if err != nil {
		t.Errorf("MPUSHIndex failed: %v", err)
	}

	assertArrayEquals(t, arr, []int{1, 2, 3})

	smallArr := NewMyArrayWithCapacity[int](1)
	smallArr.MPUSHBack(10)
	err = smallArr.MPUSHIndex(0, 5)
	if err != nil {
		t.Errorf("MPUSHIndex with resize failed: %v", err)
	}
	assertArrayEquals(t, smallArr, []int{5, 10})

	err = arr.MPUSHIndex(-1, 0)
	if err == nil {
		t.Error("Expected error for MPUSHIndex(-1)")
	}
	err = arr.MPUSHIndex(100, 0)
	if err == nil {
		t.Error("Expected error for MPUSHIndex(100)")
	}
}

func TestMDEL(t *testing.T) {
	arr := NewMyArray[int]()
	arr.MPUSHBack(1)
	arr.MPUSHBack(2)
	arr.MPUSHBack(3)

	err := arr.MDEL(1)
	if err != nil {
		t.Errorf("MDEL failed: %v", err)
	}

	assertArrayEquals(t, arr, []int{1, 3})

	err = arr.MDEL(10)
	if err == nil {
		t.Error("Expected error for MDEL out of bounds")
	}
}

func TestMSWAP(t *testing.T) {
	arr := NewMyArray[int]()
	arr.MPUSHBack(10)

	err := arr.MSWAP(0, 20)
	if err != nil {
		t.Errorf("MSWAP failed: %v", err)
	}

	val, _ := arr.Get(0)
	if val != 20 {
		t.Errorf("MSWAP did not update value")
	}

	err = arr.MSWAP(5, 50)
	if err == nil {
		t.Error("Expected error for MSWAP out of bounds")
	}
}

func TestFileIO_Text(t *testing.T) {
	filename := "test_data.txt"
	defer os.Remove(filename)

	arr := NewMyArray[int]()
	arr.MPUSHBack(100)
	arr.MPUSHBack(200)

	err := arr.SaveText(filename)
	if err != nil {
		t.Fatalf("SaveText failed: %v", err)
	}

	newArr := NewMyArray[int]()
	err = newArr.LoadText(filename)
	if err != nil {
		t.Fatalf("LoadText failed: %v", err)
	}

	assertArrayEquals(t, newArr, []int{100, 200})

	err = newArr.LoadText("non_existent_file.txt")
	if err == nil {
		t.Error("Expected error loading non-existent file")
	}
}

func TestFileIO_Binary(t *testing.T) {
	filename := "test_data.bin"
	defer os.Remove(filename)

	arr := NewMyArray[int]()
	arr.MPUSHBack(555)
	arr.MPUSHBack(777)

	err := arr.SaveBinary(filename)
	if err != nil {
		t.Fatalf("SaveBinary failed: %v", err)
	}

	newArr := NewMyArray[int]()
	err = newArr.LoadBinary(filename)
	if err != nil {
		t.Fatalf("LoadBinary failed: %v", err)
	}

	assertArrayEquals(t, newArr, []int{555, 777})

	err = newArr.LoadBinary("non_existent.bin")
	if err == nil {
		t.Error("Expected error loading non-existent binary file")
	}
}

func TestFileIO_SaveErrors(t *testing.T) {
	dirname := "bad_path_dir"
	os.Mkdir(dirname, 0755)
	defer os.Remove(dirname)

	arr := NewMyArray[int]()

	err := arr.SaveText(dirname)
	if err == nil {
		t.Error("Expected error saving text to directory path")
	}

	err = arr.SaveBinary(dirname)
	if err == nil {
		t.Error("Expected error saving binary to directory path")
	}
}

func TestPrint(t *testing.T) {
	arr := NewMyArray[int]()
	arr.MPUSHBack(1)
	arr.MPUSHBack(2)

	func() {
		defer func() {
			if r := recover(); r != nil {
				t.Errorf("Print panicked: %v", r)
			}
		}()
		arr.Print()
	}()
}

func TestResizeZeroCheck(t *testing.T) {
	arr := NewMyArrayWithCapacity[int](0)

	arr.MPUSHBack(42)

	if arr.capacity < 1 {
		t.Error("Capacity should have increased to at least 1")
	}
	val, _ := arr.Get(0)
	if val != 42 {
		t.Error("Value not inserted correctly after zero-capacity resize")
	}
}
