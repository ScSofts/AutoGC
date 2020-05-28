// testGC.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../AutoGC/auto_gc.h"
//Test Object
class ObjTest
	:public AutoGC::Object {
public:
	ObjTest() {
		using std::cout;
		cout << "Obj Newed\n";
	}
	~ObjTest() {
		using std::cout;
		cout << "Obj Deleted\n";
	}
	virtual void Delete() override { delete this; }
};
AutoGC::Hanged<ObjTest> func2() {
	AutoGC::Heap heap;
	ObjTest &obj = heap.New<ObjTest>();
	return heap.Escape(obj);
}
void func() {
	AutoGC::Heap heap;
	func2().unReceive();
}
int main()
{
	// Attention: Heap Object shouldn't in main object!
	//AutoGC::Heap heap;
	AutoGC::GC_Init(20);

	func();
	
	AutoGC::GC_Shut();
	return 0;
}
