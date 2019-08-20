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

#include "Common/Math.hpp"

#include <cstring>
#include <type_traits>

namespace sw
{
	template<class Key, class Data>
	class LRUCache
	{
	public:
		LRUCache(int n);

		~LRUCache();

		Data query(const Key &key) const;
		Data add(const Key &key, const Data &data);

		int getSize() {return size;}
		Key &getKey(int i) {return key[i];}

	private:
		int size;
		int mask;
		int top;
		int fill;

		Key *key;
		Key **ref;
		Data *data;
	};

	// Helper class for clearing the memory of objects at construction.
	// Useful as the first base class of cache keys which may contain padding bytes or bits otherwise left uninitialized.
	template<class T>
	struct Memset
	{
		Memset(T *object, int val)
		{
			static_assert(std::is_base_of<Memset<T>, T>::value, "Memset<T> must only clear the memory of a type of which it is a base class");

			// GCC 8+ warns that
			// "‘void* memset(void*, int, size_t)’ clearing an object of non-trivial type ‘T’;
			//  use assignment or value-initialization instead [-Werror=class-memaccess]"
			// This is benign iff it happens before any of the base or member constructrs are called.
			#if defined(__GNUC__) && (__GNUC__ >= 8)
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wclass-memaccess"
			#endif

			memset(object, 0, sizeof(T));

			#if defined(__GNUC__) && (__GNUC__ >= 8)
			#pragma GCC diagnostic pop
			#endif
		}
	};

	// Traits-like helper class for checking if objects can be compared using memcmp().
	// Useful for statically asserting if a cache key can implement operator==() with memcmp().
	template<typename T>
	struct is_memcmparable
	{
		// std::is_trivially_copyable is not available in older GCC versions.
		#if !defined(__GNUC__) || __GNUC__ > 5
			static const bool value = std::is_trivially_copyable<T>::value;
		#else
			// At least check it doesn't have virtual methods.
			static const bool value = !std::is_polymorphic<T>::value;
		#endif
	};
}

namespace sw
{
	template<class Key, class Data>
	LRUCache<Key, Data>::LRUCache(int n)
	{
		size = ceilPow2(n);
		mask = size - 1;
		top = 0;
		fill = 0;

		key = new Key[size];
		ref = new Key*[size];
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

		return nullptr;   // Not found
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
}

#endif   // sw_LRUCache_hpp
