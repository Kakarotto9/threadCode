
#pragma once
#include"function_wrapper.h"
#include<deque.h>
#include<mutex.h>


class work_stealing_queue
{
	private:
		typedef function_wrapper data_type;
		std::deque<data_type> the_queue; // 1
		mutable std::mutex the_mutex;
	public:
		work_stealing_queue()
		{}
		work_stealing_queue(const work_stealing_queue& other)=delete;
		work_stealing_queue& operator=(
				const work_stealing_queue& other
				)=delete;
		void push(data_type data) // 2
		{
			std::lock_guard<std::mutex> lock(the_mutex);
			the_queue.push_front(std::move(data));
			
		}
		bool empty() const
		{
			std::lock_guard<std::mutex> lock(the_mutex);
			return the_queue.empty();
			
		}
		bool try_pop(data_type& res) // 3
		{
			std::lock_guard<std::mutex> lock(the_mutex);
			if(the_queue.empty())
			{
				return false;
				
			}
			res=std::move(the_queue.front());
			the_queue.pop_front();
			return true;
			
		}
		bool try_steal(data_type& res) // 4
		{
			std::lock_guard<std::mutex> lock(the_mutex);
				if(the_queue.empty())
				{
					return false;
					
				}
			res=std::move(the_queue.back());
			the_queue.pop_back();
			return true;
			
		}
};