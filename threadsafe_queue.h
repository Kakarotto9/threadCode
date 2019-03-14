#include<cstdio>
#include<memory>
#include<mutex>
#include<exception>

using std::mutex;
using std::shared_ptr;
using std::unique_ptr;
using std::unique_lock;
using std::lock_guard;

struct empty_queue: std::exception
{
	const char* what() const throw() {};
};

template <typename T>
class threadsafe_queue{
	private:
	struct node{
		shared_ptr<T> data;
		unique_ptr<node> next;
	};
	std::mutex head_mutex;
	std::mutex tail_mutex;
	std::condtion_variable cond;
	unique_ptr<node> head;
	node* tail;
	node* get_tail(){
		std::lock_guard<mutex> tail_lock(tail_mutex);
		return tail;
	}
	public:
	threadsafe_queue():head(),tail(head.get()){};
	threadsafe_queue(const threadsafe_queue& other)=delete;
	threadsafe_queue& operator=(const threadsafe_queue& other)=delete;

	void push(T n);
	void wait_pop(T value);
	shared_ptr<T> wait_pop();
	bool try_pop(T value);
	shared_ptr<T> try_pop();

	bool empty(){
		std::unique_lock<mutex> head_lock(head_mutex);
		return head.get()==get_tail();
	}
}
