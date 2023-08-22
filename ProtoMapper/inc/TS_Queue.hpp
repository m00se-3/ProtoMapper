#ifndef PROTOMAPPER_TS_QUEUE_HPP
#define PROTOMAPPER_TS_QUEUE_HPP

#include <queue>
#include <shared_mutex>

namespace proto
{
	/*
		A thread-safe std::queue wrapper used for passing events around
		various program components, like the GUI.
	*/
	template<typename T>
	class ts_queue
	{
		std::queue<T> _queue;
		std::shared_mutex _mutex;

	public:
		ts_queue() = default;

		size_t size()
		{
			_mutex.try_lock_shared();
			auto& s = _queue.size();
			_mutex.unlock_shared();
			return s;
		}

		size_t size() const
		{
			_mutex.try_lock_shared();
			auto& s = _queue.size();
			_mutex.unlock_shared();
			return s;
		}

		T pop()
		{
			_mutex.try_lock();
			auto ref = _queue.front();
			_queue.pop();
			_mutex.unlock();
			return ref
		}

		void push(const T& value)
		{
			_mutex.try_lock();
			_queue.push(value);
			_mutex.unlock();
		}

		void push(T&& value)
		{
			_mutex.try_lock();
			_queue.push(std::move(value));
			_mutex.unlock();
		}

		template<typename... Args>
		void emplace(Args&&... args)
		{
			_mutex.try_lock();
			_queue.emplace(std::forward<Args>(args)...);
			_mutex.unlock();
		}

	};
}
#endif // !PROTOMAPPER_TS_QUEUE_HPP
