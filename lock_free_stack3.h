#pragma once

#include<memory>
#include<atomic>

using std::shared_ptr;

template<typename T>
class lock_free_stack
{
	private:
		struct node;  //前置声明
		struct counted_node_ptr 
		{
			int external_count;
			node* ptr;
		};
		struct node
		{
			std::shared_ptr<T> data;
			std::atomic<int> internal_count; // 2
			counted_node_ptr next; // 3
			node(T const& data_):
				data(std::make_shared<T>(data_)),
				internal_count(0)
			{}

		};
		void increase_head_count(counted_node_ptr & old){
			// counted_node_ptr new;我本来想用 new来简洁表达，但忘了new是 关键字了
			counted_node_ptr new_node;
			do{
				new_node=old;
				++new_node.counted_node_ptr;
			}while(head.compare_exchange_weak(old,new_node));  //4  和注释6 相互作用
			old.external_count=new_node.external_count;
		}
		std::atomic<counted_node_ptr> head; 
	public:
		~lock_free_stack()//需要主动释放全部
		{
			while(pop());  
		}
		void push(T const& data) // 5
		{
			counted_node_ptr new_node;
			new_node.ptr=new node(data);
			new_node.external_count=1;
			new_node.ptr->next=head.load();
			while(!head.compare_exchange_weak(new_node.ptr->next,new_node));
		}
		shared_ptr<T> pop(){
			counted_node_ptr old_node=head.load();
			for(;;){  //因为判定情况，可能导致没有数据抛出，所以需要循环到数据抛出为止
				increase_head_count(old_node);
				node* ptr=old_node.ptr;
				if(old_node){
					if(head.compare_exchange_weak(old_node,ptr->next)){ // 6 
						shared_ptr<T> res;
						res.swap(ptr->data);
						int count=old_node.external_count;
						if(ptr.internal_count.fetch_add(count)==-count){
							delete ptr;
						}
						return res;
					}
					else if(ptr->internal_count.fetch_sub()){
						delete ptr;
					}
				}
				else {
					return shared_ptr<T> ();
				}
			}
		}

};

