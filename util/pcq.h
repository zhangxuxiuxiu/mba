/*
 * Copyright 2016 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// @author Bo Hu (bhu@fb.com)
// @author Jordan DeLong (delong.j@fb.com)

#pragma once

#include <atomic>
#include <cstdlib>
#include <cassert>
#include <type_traits>

namespace utl{

/*
 * ProducerConsumerQueue is a one producer and one consumer queue
 * without locks.
 */
template<class T>
struct ProducerConsumerQueue {
  typedef T value_type;

  ProducerConsumerQueue(const ProducerConsumerQueue&) = delete;
  ProducerConsumerQueue& operator = (const ProducerConsumerQueue&) = delete;

  // size must be >= 2.
  //
  // Also, note that the number of usable slots in the queue at any
  // given time is actually (size-1), so if you start with an empty queue,
  // isFull() will return true after size-1 insertions.
  explicit ProducerConsumerQueue(uint32_t size= 10)
    : m_size(size)
    , m_records(static_cast<T*>(std::malloc(sizeof(T) * size)))
    , m_read_index(0)
    , m_write_index(0)
  {
    assert(size >= 2);
    if (!m_records) {
      throw std::bad_alloc();
    }
  }

  ~ProducerConsumerQueue() {
    // We need to destruct anything that may still exist in our queue.
    // (No real synchronization needed at destructor time: only one
    // thread can be doing this.)
    if (!std::is_trivially_destructible<T>::value) {
      size_t read = m_read_index;
      size_t end = m_write_index;
      while (read != end) {
        m_records[read].~T();
        if (++read == m_size) {
          read = 0;
        }
      }
    }

    std::free(m_records);
  }

 // bool write(Args&&... recordArgs) {
  template<class ...Args>
  bool Push(Args&&... args){
	// only writer thread could change $m_write_index, so $m_write_index is always synchronizedly right
    auto const currentWrite = m_write_index.load(std::memory_order_relaxed);
    auto nextRecord = currentWrite + 1;
    if (nextRecord == m_size) {
      nextRecord = 0;
    }
    if (nextRecord != m_read_index.load(std::memory_order_acquire)) {
      new (&m_records[currentWrite]) T(std::forward<Args>(args)...);
	  // memory_order_release here will sync m_records with reader, 
	  // while memory_order_consume will not
      m_write_index.store(nextRecord, std::memory_order_release);
      return true;
    }

    // queue is full
    return false;
  }

  // move (or copy) the value at the front of the queue to given variable
  bool Pop(T& record) {
	// only reader thread could change $m_read_index, so $m_read_index is always synchronizedly right
    auto const currentRead = m_read_index.load(std::memory_order_relaxed);
    if (currentRead == m_write_index.load(std::memory_order_acquire)) {
      // queue is empty
      return false;
    }

    auto nextRecord = currentRead + 1;
    if (nextRecord == m_size) {
      nextRecord = 0;
    }
    record = std::move(m_records[currentRead]);
    m_records[currentRead].~T();
	// wondering if memory_order_consume could be used here ?
	// b' writer needs not to sync m_records[currentRead] with reader
    m_read_index.store(nextRecord, std::memory_order_release);
    return true;
  }

	private:
		const uint32_t				m_size;
		T* const					m_records;

 		std::atomic<unsigned int>	m_read_index;
 		std::atomic<unsigned int>	m_write_index;
};

template<class T>
using pcq = ProducerConsumerQueue<T>;

}
