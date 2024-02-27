#pragma once

#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <Bitmask.h>

template<typename T>
class RingStack {
	size_t mCapacity;
	size_t mSize;
	size_t mStartIndex;
	std::vector<T> mData;
public:
	RingStack(size_t capacity) :mCapacity(capacity), mSize(0), mStartIndex(0), mData(mCapacity) {}
	void pop() {
		mSize = std::max(mSize - 1, size_t(0));
	}
	void push(const T& val) {
		if (mSize == mCapacity) {
			mData[mStartIndex] = val;
			mStartIndex = (mStartIndex + 1) % mCapacity;
		}
		else {
			size_t index = (mStartIndex + mSize) % mCapacity;
			mData[index] = val;
			mSize++;
		}
	}
	size_t size() { return mSize; }
	const T& last() const { 
		size_t index = (mStartIndex + mSize - 1) % mCapacity;
		return mData[index]; 
	}
	T& last() { 
		size_t index = (mStartIndex + mSize - 1) % mCapacity;
		return mData[index]; 
	}
	void restore() {
		mSize = std::min(mSize + 1, mCapacity);
	}
	T& operator[](size_t index) {
		index = (mStartIndex + mSize - 1) % mCapacity;
		return mData[index];
	}
	const T& operator[](size_t index) const {
		index = (mStartIndex + mSize - 1) % mCapacity;
		return mData[index];
	}
	void clear() {
		mSize = 0;
	}
};

template<typename T>
struct WriteInfo {
	T oldValue;
	T newValue;
	WriteInfo() {}
	WriteInfo(T oldValue, T newValue) :oldValue(oldValue), newValue(newValue) {}
	/*WriteInfo operator=(const WriteInfo& right) {
		newValue = right.newValue;
		return *this;
	}*/
};

template<typename T>
class UndoStack {
	T *data;
	size_t size;

	RingStack<std::map<size_t, WriteInfo<T>>> previousStates;		//Shows changes required to return to previous state
	std::vector<std::map<size_t, WriteInfo<T>>> redoStates;
	bool lastStateUnfinished;

	void propagateLastState() {
		if (previousStates.size() <= 0)
			return;
		if (previousStates.last().size() > 0) {
			for (auto& it : previousStates.last()) {
				data[it.first] = it.second.newValue;
			}
		} 
		lastStateUnfinished = false;
	}
public:
	UndoStack(T *data, size_t size, size_t maxUndo) 
		:data(data), size(size), previousStates(maxUndo), lastStateUnfinished(false){}

	int lowestIndex() { 
		if (previousStates.size())
			return getLastState().begin()->first;
		else
			return -1;
	}
	int highestIndex() {
		if (previousStates.size() > 0 && getLastState().size() > 0)
			return getLastState().rbegin()->first;
		else
			return -1;
	}


	void modify(size_t element, T value) {
		try {
			//Store new and old values until propagated with propagateLastState()
			previousStates.last()[element] = WriteInfo<T>(data[element], value);
		} catch (out_of_range) {
			printf("UndoStack::modify -- Out of range exception\n");
		}
	}

	void modify(size_t element, T value, Bitmask mask) {
		try {
			//Store new and old values until propagated with propagateLastState()
			if(!mask.test(data[element]))
				previousStates.last()[element] = WriteInfo<T>(data[element], value);
		}
		catch (out_of_range) {
			printf("UndoStack::modify -- Out of range exception\n");
		}
	}

	const std::map<size_t, WriteInfo<T>>& getLastState() const {
		return previousStates.last();
	}

	void startNewState() {
		redoStates.clear();
		propagateLastState();
		if (previousStates.size() == 0 || previousStates.last().size() > 0) {
			previousStates.push(map<size_t, WriteInfo<T>>());
			lastStateUnfinished = true;
		}
	}

	void undo(std::map<size_t, T>* changes) {
		if (previousStates.size() > 0 && previousStates.last().size() == 0) {
			previousStates.pop();
			lastStateUnfinished = false;
		}
		if (previousStates.size() > 0 && previousStates.last().size() > 0) {
			if(lastStateUnfinished)
				propagateLastState();
			//Build redo information and apply undo
			redoStates.push_back(previousStates.last());
			for (const auto &it: previousStates.last()) {
				data[it.first] = it.second.oldValue;
				(*changes)[it.first] = it.second.oldValue;
			}
			previousStates.pop();
		}
	}
	void redo(std::map<size_t, T>* changes) {
		if (redoStates.size() > 0) {
			previousStates.push(redoStates.back());
			for (const auto &it : redoStates.back()) {
				//previousStates.last()[it.first] = data[it.first];
				data[it.first] = it.second.newValue;
				(*changes)[it.first] = it.second.newValue;
			}
			lastStateUnfinished = false;
			redoStates.pop_back();
		}
	}

};


template<typename T>
class UndoStackRef {
	RingStack<std::map<size_t, WriteInfo<T>>> previousStates;		//Shows changes required to return to previous state
	std::vector<std::map<size_t, WriteInfo<T>>> redoStates;
public:
	UndoStackRef(size_t maxUndo)
		:previousStates(maxUndo){}

	void modify(size_t element, T value, const T* data, Bitmask mask) {
		try {
			//Store new and old values until propagated with propagateLastState()
			if (!mask.test(data[element])) {
				auto pos = previousStates.last().find(element);
				if (pos == previousStates.last().end())
					previousStates.last()[element] = WriteInfo<T>(data[element], value);
				else
					pos->second.newValue = value;
			}
		}
		catch (out_of_range) {
			printf("UndoStack::modify -- Out of range exception\n");
		}
	}

	void startNewState() {
		redoStates.clear();
		if (previousStates.size() == 0 || previousStates.last().size() > 0) {
			previousStates.push(map<size_t, WriteInfo<T>>());
		}
	}

	const std::map<size_t, WriteInfo<T>>& getLastState() const {
		return previousStates.last();
	}

	int lowestIndex() {
		if (previousStates.size())
			return getLastState().begin()->first;
		else
			return -1;
	}
	int highestIndex() {
		if (previousStates.size() > 0 && getLastState().size() > 0)
			return getLastState().rbegin()->first;
		else
			return -1;
	}

	void undo(std::map<size_t, T>* changes) {
		if (previousStates.size() > 0 && previousStates.last().size() == 0) {
			previousStates.pop();
		}
		if (previousStates.size() > 0 && previousStates.last().size() > 0) {
			//Build redo information and apply undo
			redoStates.push_back(previousStates.last());
			for (const auto &it : previousStates.last()) {
				(*changes)[it.first] = it.second.oldValue;
			}
			previousStates.pop();
		}
	}
	void redo(std::map<size_t, T>* changes) {
		if (redoStates.size() > 0) {
			previousStates.push(redoStates.back());
			for (const auto &it : redoStates.back()) {
				(*changes)[it.first] = it.second.newValue;
			}
			redoStates.pop_back();
		}
	}
};

