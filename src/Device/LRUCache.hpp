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

#include <cstring>
#include <type_traits>
#include <unordered_map>

namespace sw
{
	template<class Key, class Data>
	class LRUCache
	{
	public:
		LRUCache(int n);

		virtual ~LRUCache();

		Data *query(const Key &key) const;
		virtual Data *add(const Key &key, Data *data);

		int getSize() {return size;}
		Key &getKey(int i) {return key[i];}

	protected:
		int size;
		int mask;
		int top;
		int fill;

		Key *key;
		Key **ref;
		Data **data;
	};

	template<class Key, class Data>
	class LRUConstCache : public LRUCache<Key, Data>
	{
		using LRUBase = LRUCache<Key, Data>;
	public:
		LRUConstCache(int n) : LRUBase(n) {}
		~LRUConstCache() { clearConstCache(); }

		Data *add(const Key &key, Data *data) override
		{
			constCacheNeedsUpdate = true;
			return LRUBase::add(key, data);
		}

		void updateConstCache();
		Data *queryConstCache(const Key &key) const;

	private:
		void clearConstCache();
		bool constCacheNeedsUpdate = false;
		std::unordered_map<Key, Data*> constCache;
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
		data = new Data*[size];

		for(int i = 0; i < size; i++)
		{
			data[i] = nullptr;

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

		for(int i = 0; i < size; i++)
		{
			if(data[i])
			{
				data[i]->unbind();
				data[i] = nullptr;
			}
		}

		delete[] data;
		data = nullptr;
	}

	template<class Key, class Data>
	Data *LRUCache<Key, Data>::query(const Key &key) const
	{
		for(int i = top; i > top - fill; i--)
		{
			int j = i & mask;

			if(key == *ref[j])
			{
				Data *hit = data[j];

				if(i != top)
				{
					// Move one up
					int k = (j + 1) & mask;

					Data *swapD = data[k];
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
	Data *LRUCache<Key, Data>::add(const Key &key, Data *data)
	{
		top = (top + 1) & mask;
		fill = fill + 1 < size ? fill + 1 : size;

		*ref[top] = key;

		data->bind();

		if(this->data[top])
		{
			this->data[top]->unbind();
		}

		this->data[top] = data;

		return data;
	}

	template<class Key, class Data>
	void LRUConstCache<Key, Data>::clearConstCache()
	{
		auto it = constCache.begin();
		auto itEnd = constCache.end();
		for(; it != itEnd; ++it)
		{
			it->second->unbind();
		}
		constCache.clear();
	}

	template<class Key, class Data>
	void LRUConstCache<Key, Data>::updateConstCache()
	{
		if(constCacheNeedsUpdate)
		{
			clearConstCache();

			for(int i = 0; i < LRUBase::size; i++)
			{
				if(LRUBase::data[i])
				{
					LRUBase::data[i]->bind();
					constCache[*LRUBase::ref[i]] = LRUBase::data[i];
				}
			}

			constCacheNeedsUpdate = false;
		}
	}

	template<class Key, class Data>
	Data *LRUConstCache<Key, Data>::queryConstCache(const Key &key) const
	{
		auto it = constCache.find(key);
		return (it != constCache.end()) ? it->second : nullptr;
	}
}

#endif   // sw_LRUCache_hpp
