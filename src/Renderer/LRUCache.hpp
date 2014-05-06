// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_LRUCache_hpp
#define sw_LRUCache_hpp

#include "Common/Math.hpp"

namespace sw
{
	template<class Key, class Data>
	class LRUCache
	{
	public:
		LRUCache(int n);

		~LRUCache();

		Data *query(const Key &key) const;
		Data *add(const Key &key, Data *data);
		
		int getSize() {return size;}
		Key &getKey(int i) {return key[i];}

	private:
		int size;
		int mask;
		int top;
		int fill;

		Key *key;
		Key **ref;
		Data **data;
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
			data[i] = 0;

			ref[i] = &key[i];
		}
	}

	template<class Key, class Data>
	LRUCache<Key, Data>::~LRUCache()
	{
		delete[] key;
		key = 0;

		delete[] ref;
		ref = 0;

		for(int i = 0; i < size; i++)
		{
			if(data[i])
			{
				data[i]->unbind();
				data[i] = 0;
			}
		}

		delete[] data;
		data = 0;
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

		return 0;   // Not found
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
}

#endif   // sw_LRUCache_hpp
