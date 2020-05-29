#pragma once
#include <exception>
#include <unordered_map>
#include <type_traits>
//Declarations
namespace AutoGC {
	//Use Hanged to function return value
	template <class MemType>
	class Hanged;
	
	//All Objects in the heaps must be based on Object
	class Object;
	
	//The heap of a function
	//Attention: This object Must NOT in the main function!
	class Heap;
	
	//This is just for check the Template of functions
	template <class Class, class Base>
	using is_based_on=std::is_base_of<Base,Class>;
	
	//Must be called in the main function
	void GC_Init(unsigned long sleepTime = 500,unsigned long gcSleepTime = 1);
	void GC_Shut();
	class NULLPointerException :public std::exception {};
};


template <class MemType>
class AutoGC::Hanged {
protected:
	Hanged() = delete;
	Hanged(MemType & MemObject);
protected:
	MemType &HangObject;
public:
	
	//If this return value is unnessary,call this method
	void unReceive();
	
	//receive the object to another heap
	MemType & receive(AutoGC::Heap& heap);

private:
	friend class AutoGC::Heap;

	//MemType Must be an AutoGC::Object!
	static_assert(
		AutoGC::is_based_on<MemType,AutoGC::Object>::value,
		"Error:MemType Must be an AutoGC::Object!\nIn " __FILE__ ",Class Hanged");
};

class AutoGC::Heap {
public:
	
	Heap();
	~Heap();
	
	//Heap object must NOT in heap!
	void* operator new(size_t) = delete;
	void operator delete(void*) = delete;
	
	//Attention: Do NOT change this map!
	std::unordered_map<AutoGC::Object*, AutoGC::Object*> memObjects;
	
	//Use this method to allocate an object in the heap
	template <class MemType>
	MemType & New();
	
	//Use this to create a return value of funciton
	template <class MemType>
	Hanged<MemType> Escape(MemType & memObject);
	
};

class AutoGC::Object {
public:
	virtual void Delete() = 0;//{delete this;}
};

namespace AutoGC {
	namespace Objects {
		class Int :
			public Object {
		public:
			Int(int value = 0) { this->value = new int; this->get() = value; };
			~Int() { delete this->value; };
			int &get() { return *value; }
			virtual void Delete() override { delete this; }
			Int& operator= (int value) { *this->value = value; return *this; };
		private:
			int *value;
		};

		class Long :
			public Object {
		public:
			Long(long value = 0) { this->value = new long; this->get() = value; };
			~Long() { delete this->value; };
			long &get() { return *value; }
			virtual void Delete() override { delete this; }
			Long& operator= (int value) { *this->value = value; return *this; };
		private:
			long *value;
		};

		class Double :
			public Object {
		public:
			Double(double value = 0) { this->value = new double; this->get() = value; };
			~Double() { delete this->value; };
			double &get() { return *value; }
			virtual void Delete() override { delete this; }
			Double& operator= (double value) { *this->value = value; return *this; };
		private:
			double *value;
		};

		class Float :
			public Object {
		public:
			Float(float value = 0) { this->value = new float; this->get() = value; };
			~Float() { delete this->value; };
			float &get() { return *value; }
			virtual void Delete() override { delete this; }
			Float& operator= (float value) { *this->value = value; return *this; };
		private:
			float *value;
		};
	};
};



template<class MemType>
inline AutoGC::Hanged<MemType>::Hanged(MemType & MemObject) :
	HangObject(MemObject)
{
}

template<class MemType>
void AutoGC::Hanged<MemType>::unReceive()
{
	((Object*)&HangObject)->Delete();
}

template<class MemType>
MemType & AutoGC::Hanged<MemType>::receive(AutoGC::Heap& heap)
{
	heap.memObjects.insert(std::make_pair(&this->HangObject, &this->HangObject));
	//.insert(std::make_pair(&this->HangObject, &this->HangObject));
	return this->HangObject;
}

template<class MemType>
inline MemType & AutoGC::Heap::New()
{
	static_assert(
		AutoGC::is_based_on<MemType, AutoGC::Object>::value,
		"Error:MemType Must be an AutoGC::Heap!\n");
	MemType *obj = new MemType;
	if (obj == nullptr)
		throw NULLPointerException();
	this->memObjects.insert(std::make_pair(obj, obj));
	return *obj;
}

template<class MemType>
AutoGC::Hanged<MemType> AutoGC::Heap::Escape(MemType & memObject)
{
	static_assert(
		AutoGC::is_based_on<MemType, AutoGC::Object>::value,
		"Error:MemType Must be an AutoGC::Heap!\n");
	this->memObjects[&memObject] = nullptr;
	return Hanged<MemType>(memObject);
}
