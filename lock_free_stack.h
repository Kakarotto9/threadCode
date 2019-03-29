#pragma once

#include<atomic>
#include<memory>

tempalte<typename T>
class lock_free_stack{
	struct node{
		shared_ptr<T> data;
		/*
		   unique_ptr<node> next;
		   */
		node* next;
		node(T& value):data(make_shared<T>(value)){

		}
	}

	void push(T& n){
		node* new=new node(n);
		new->next=head.load();
		/*
		   mutex;
		   while(true){
		   if(head.load()==new->next){
		   head=new;
		   break;
		   }
		   else{
		   new->next=head.load();
		   }
		   }*/
		while(head.exchange_and_weak(new->next,new));
	}
	shared_ptr<T> pop(){
		node* old=head.load();
		/*
		   mutex;
		   while(true){
		   if(head==old){
		   head=old->next;
		   break;
		   }
		   else 
		   old=head.load();
		   }*/
		while(head.exchange_and_weak(old,old->next));
		shared_ptr<T> res(old->data);
		delete old;
		return res;
	}
}
