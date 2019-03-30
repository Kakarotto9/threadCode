
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
		std::atomic<counted_node_ptr> head; // 4
	public:
		~lock_free_stack()
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
		void pop(){
			counted_node_ptr old_node=head.load();
			head.compare_exchange_weak(old_node,old_node.ptr->next);
			if(old)

		}
		
};

