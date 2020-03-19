// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef sw_LRUCache_hpp
#define sw_LRUCache_hpp

#include "System/Math.hpp"

#include <type_traits>
#include <unordered_map>

namespace sw {

template<class Key, class Data>
class LRUCache
{
public:
	LRUCache(int n);

	virtual ~LRUCache();

	Data query(const Key &key) const;
	virtual Data add(const Key &key, const Data &data);

	int getSize() { return size; }
	Key &getKey(int i) { return key[i]; }

protected:
	int size;
	int mask;
	int top;
	int fill;

	Key *key;
	Key **ref;
	Data *data;
};

// An LRU cache which can take a 'snapshot' of its current state. This is useful
// for allowing concurrent read access without requiring a mutex to be locked.
template<class Key, class Data, class Hasher = std::hash<Key>>
class LRUSnapshotCache : public LRUCache<Key, Data>
{
	using LRUBase = LRUCache<Key, Data>;

public:
	LRUSnapshotCache(int n)
	    : LRUBase(n)
	{}
	~LRUSnapshotCache() { clearSnapshot(); }

	Data add(const Key &key, const Data &data) override
	{
		snapshotNeedsUpdate = true;
		return LRUBase::add(key, data);
	}

	void updateSnapshot();
	const Data &querySnapshot(const Key &key) const;

private:
	void clearSnapshot();
	bool snapshotNeedsUpdate = false;
	std::unordered_map<Key, Data, Hasher> snapshot;
};

}  // namespace sw

namespace sw {

template<class Key, class Data>
LRUCache<Key, Data>::LRUCache(int n)
{
	size = ceilPow2(n);
	mask = size - 1;
	top = 0;
	fill = 0;

	key = new Key[size];
	ref = new Key *[size];
	data = new Data[size];

	for(int i = 0; i < size; i++)
	{
		ref[i] = &key[i];
	}
}

template<class Key, class Data>
LRUCache<Key, Data>::~LRUCache()
{
	delete[] key;
	key = nullptr;

	delete[] ref;
	ref = nullptr;

	delete[] data;
	data = nullptr;
}

template<class Key, class Data>
Data LRUCache<Key, Data>::query(const Key &key) const
{
	for(int i = top; i > top - fill; i--)
	{
		int j = i & mask;

		if(key == *ref[j])
		{
			Data hit = data[j];

			if(i != top)
			{
				// Move one up
				int k = (j + 1) & mask;

				Data swapD = data[k];
				data[k] = data[j];
				data[j] = swapD;

				Key *swapK = ref[k];
				ref[k] = ref[j];
				ref[j] = swapK;
			}

			return hit;
		}
	}

	return {};  // Not found
}

template<class Key, class Data>
Data LRUCache<Key, Data>::add(const Key &key, const Data &data)
{
	top = (top + 1) & mask;
	fill = fill + 1 < size ? fill + 1 : size;

	*ref[top] = key;
	this->data[top] = data;

	return data;
}

template<class Key, class Data, class Hasher>
void LRUSnapshotCache<Key, Data, Hasher>::clearSnapshot()
{
	snapshot.clear();
}

template<class Key, class Data, class Hasher>
void LRUSnapshotCache<Key, Data, Hasher>::updateSnapshot()
{
	if(snapshotNeedsUpdate)
	{
		clearSnapshot();

		for(int i = 0; i < LRUBase::size; i++)
		{
			if(LRUBase::data[i])
			{
				snapshot[*LRUBase::ref[i]] = LRUBase::data[i];
			}
		}

		snapshotNeedsUpdate = false;
	}
}

template<class Key, class Data, class Hasher>
const Data &LRUSnapshotCache<Key, Data, Hasher>::querySnapshot(const Key &key) const
{
	auto it = snapshot.find(key);
	static Data null = {};
	return (it != snapshot.end()) ? it->second : null;
}

}  // namespace sw

#endif  // sw_LRUCache_hpp
