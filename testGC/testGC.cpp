// testGC.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <Windows.h>
#include <iostream>
#include "../AutoGC/auto_gc.h"
//Test Object
class ObjTest
	:public AutoGC::Object {
public:
	ObjTest() {
		p = new int[20];
	}
	~ObjTest() {
		using std::cout;
		cout << "Obj Deleted\n";
		delete[] p;
	}
	virtual void Delete() override { delete this; }
private :int *p = nullptr;
};
AutoGC::Hanged<ObjTest> func2() {
	AutoGC::Heap heap;
	try {
		ObjTest &obj = heap.New<ObjTest>();
		return heap.Escape(obj);
	}
	catch (AutoGC::NULLPointerException e) {
		throw e;
	}
}
void func() {
	AutoGC::Heap heap;
	try {
		for (int i = 0; i < 200000; i++)
			heap.New<ObjTest>();
	}
	catch (AutoGC::NULLPointerException e) {
		std::cout << "NULL!\n";
		return;
	}
}
int main()
{
	// Attention: Heap Object shouldn't in main object!
	//AutoGC::Heap heap;
	AutoGC::GC_Init(20,1);

	func();

	for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 1, 1, 1, 1, 1, }) {
		std::cout << "Hello World!" << i << std::endl;
		Sleep(20);
	}

	AutoGC::GC_Shut();
	return 0;
}
