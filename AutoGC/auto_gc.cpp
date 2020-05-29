#include "pch.h"
#include "auto_gc.h"
#include <thread>
#include <cstdlib>
#if  defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif //  WIN32
AutoGC::Heap::Heap()
{

}


namespace AutoGC {
	using std::thread;
	using std::vector;
	class GCThread
	{
	public:
		GCThread(unsigned long sleepTime = 500,unsigned long gcSleepTime = 1) {
			this->sleepTime = gcSleepTime;
			auto ThreadFunc = [this, sleepTime]()->void {
				while (true) {
					this->DoTask();
					if (this->cancel) {
						break;
					}
#if  defined(_WIN32) || defined(_WIN64)
					Sleep(sleepTime);
#else
					_sleep(sleepTime);
#endif
				}
			};
			gc_thread = new thread(ThreadFunc);
		}
		~GCThread() {
			sleepTime = 0;
			cancel = true;
			gc_thread->join();
		}
		void PushTask(std::unordered_map<AutoGC::Object*, AutoGC::Object*> heapObjects) {
			this->tasks.push_back(heapObjects);
		}
	protected:
		friend void ThreadFunc() {}
		void DoTask() {
			for (auto i = tasks.begin(); i < tasks.end(); i++) {
				auto list = *i;
				for (auto j : list) {
					if (j.second != nullptr) {
						j.second->Delete();
						i->erase(j.first);
#if  defined(_WIN32) || defined(_WIN64)
						Sleep(sleepTime);
#else
						_sleep(sleepTime);
#endif
					}
				}
				i->clear();
				tasks.erase(i,i);
			}
		}
		vector< std::unordered_map<AutoGC::Object*, AutoGC::Object*> > tasks;
		thread* gc_thread;
		bool cancel = false;
		unsigned long sleepTime = 0;
	};
};

namespace {
	AutoGC::GCThread * gc_thread = nullptr;
};

void AutoGC::GC_Init(unsigned long sleepTime,unsigned long gcSleepTime)
{
	gc_thread = new GCThread(sleepTime,gcSleepTime);
}

void AutoGC::GC_Shut()
{
	delete gc_thread;
}
AutoGC::Heap::~Heap()
{

	gc_thread->PushTask(this->memObjects);
}
