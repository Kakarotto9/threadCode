#include<threadsafe_queue.h>

struct empty_queue: std::exception
{
	const char* what() const throw() {};
};

template <typename T>
void threadsafe_queue<T>::push(T n){
	shared_ptr<T> new_data(make_shared<T>(std::move(n)));
	unique_ptr<node> p(new node); 
	{
		std::lock_guard<mutex> tail_lock(tail_mutex);
		tail->data=new_data;
		tail->next=std::move(p);  // tail->next接管p后会释放原来指向的元素，但原本指向一个空Node，所以无碍
		tail=p.get();
	}
	cond.notify_one();
}

template <typename T>
shared_ptr<T> threadsafe_queue<T>::wait_pop(){
	std::unique_lock<mutex> head_lock(head_mutex);
	cond.wait(head_lock,[this]{return head.get()!=get_tail()});
	unique_ptr<node> old_node=std::move(head);
	head=std::move(old_head.next);
	/* 上面两句如果改成这样error，原因head=std::move()，会先释放head所指向的元素，释放后head.next就是未知数据了 
	   head=std::move(head.next);
	   */
	head_lock.unlock();
	return old_node.data;
}
template <typename T>
void threadsafe_queue<T>::wait_pop(int value){
	std::lock_guard<mutex> head_lock(head_mutex);
	cond.wait(head_lock,[this]{return head.get()!=get_tail()});
	unique_ptr<node> old_node=std::move(head);
	value=*old_head.data; // 
	head=std::move(old_head.next); //
}

template <typename T>
bool threadsafe_queue<T>::try_pop(int value){
	std::lock_guard<mutex> head_lock(head_mutex);
	if(head.get()==get_tail())
		return false;
	unique_ptr<node> old_node=std::move(head);
	value=*old_node.data;  
	head=std::move(old_head.next);
	return true;
}

template <typename T>
shared_ptr<T> threadsafe_queue<T>::try_pop(){
	shared_ptr<T> nptr;
	std::unique_ptr<mutex> head_lock(head_mutex);
	if(head.get()==get_tail())
		return nptr;
	unique_ptr<node> old_node=std::move(head);
	head=std::move(old_head.next);
	head_lock.unlock();
	nptr=old_head.data;
	return nptr;
}

