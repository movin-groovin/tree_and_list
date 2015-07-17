
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

#include <cassert>



template <typename T>
class CBList {
private:
	
	template <typename U>
	class CNode {
	private:
		typedef std::shared_ptr <CNode> link;
		T m_value;
		
		std::shared_ptr <CNode> m_next;
		std::weak_ptr <CNode> m_prev;
		
	public:
		CNode (const U & val): m_value(val) {}
		CNode (const U & val, link next, link prev): m_value(val), m_next(next), m_prev(prev) {}
		
		virtual ~CNode(){}
		
		CNode (const CNode & ref): m_value(ref.m_value), m_next(ref.m_next), m_prev(ref.m_prev) {}
		
		CNode& operator = (const CNode & ref) {
			if (this == &ref)
				return *this;
				
			CNode tmp(ref);
			swap(tmp);
			
			return *this;
		}
		
		void swap (CNode & ref) {
			T tmp_value = ref.m_value;
			std::shared_ptr <CNode> tmp_next = ref.m_next;
			std::weak_ptr <CNode> tmp_prev = ref.m_prev;
			
			ref.m_value = m_value;
			ref.m_next = m_next;
			ref.m_prev = m_prev;
			
			m_value = tmp_value;
			m_next = tmp_next;
			m_prev = tmp_prev;
			
			return;
		}
		
		link GetNext() {
			return m_next;
		}
		link GetPrev() {
			return m_prev.lock();
		}
		U GetValue() const {
			return m_value;
		}
		U& GetValueRef() {
			return m_value;
		}
		
		void SetNext(link ptr) {
			m_next = ptr;
		}
		void SetPrev(link ptr) {
			m_prev = ptr;
		}
		void SetNext() {
			m_next = std::make_shared<CNode>(nullptr);
		}
		void SetPrev() {
			m_prev = std::weak_ptr<CNode>();
		}
		void SetValue(U val) {
			m_value = val;
		}
	};
	
	friend class Iterator;
public:
	class Iterator {
	private:
		typedef std::shared_ptr <CNode<T> > link;
		
		link m_pos;
		
	public:
		Iterator(CBList & lst): m_pos(lst.m_base) {}
		Iterator(link pos = link()): m_pos(pos) {}
		Iterator(const Iterator & ref): m_pos(ref.m_pos) {
			return;
		}
		
		bool finished() const {
			return !m_pos;
		}
		
		Iterator & operator = (const Iterator & ref) {
			if (&ref == this)
				return *this;
			
			m_pos = ref.m_pos;
			return *this;
		}
		
		Iterator & operator ++ () {
			if (!m_pos)
				return *this;
			m_pos = m_pos->GetNext();
			return *this;
		}
		Iterator & operator -- () {
			if (!m_pos)
				return *this;
			m_pos = m_pos->GetPrev();
			return *this;
		}
		Iterator operator ++ (int) {
			if (!m_pos)
				return *this;
			Iterator for_ret(*this);
			m_pos = m_pos->GetNext();
			return for_ret;
		}
		Iterator operator -- (int) {
			if (!m_pos)
				return *this;
			Iterator for_ret(*this);
			m_pos = m_pos->GetPrev();
			return for_ret;
		}
		
		T & operator * () {
			if (!m_pos)
				throw std::out_of_range("Incorrect iterator");
			return m_pos->GetValueRef();
		}
		T * operator-> () {
			if (!m_pos)
				throw std::out_of_range("Incorrect iterator");
			return & m_pos->GetValueRef();
		}
		//
	};
	
private:
	typedef CNode<T> entry;
	typedef std::shared_ptr<entry> entry_ptr;
	typedef std::shared_ptr<T> value_type;
	
	entry_ptr m_base;
	std::weak_ptr<entry> m_last;
	size_t m_number;
	
public:
	CBList (): m_base(new entry( T() )), m_number(0) {
		m_last = m_base;
	}
	
	~CBList () {
		m_base.reset();
	}
	
	bool empty() const {
		return m_number;
	}
	
	size_t size() const {
		return m_number;
	}
	
	Iterator begin () {
		return Iterator(*this);
	}
	
	Iterator end() {
		return Iterator(m_last.lock());
	}
	
	void push_back(const T & val) {
		if (!m_number) {
			m_base->SetValue(val);
		} else {
			entry_ptr new_node (new entry(val));
			
			new_node->SetNext();
			new_node->SetPrev(m_last.lock());
			
			m_last.lock()->SetNext(new_node);
			m_last = new_node;
		}
		++m_number;
		
		return;
	}
	
	void push_front(const T & val) {
		if (!m_number) {
			m_base->SetValue(val);
		} else {
			entry_ptr new_node = std::make_shared<CNode<T> > (CNode<T>(val));
			
			new_node->SetNext(m_base);
			new_node->SetPrev();
			
			m_base->SetPrev(new_node);
			m_base = new_node;
		}
		++m_number;
		
		return;
	}
	
	void insert (const T & val, entry_ptr pos_before) {
		if (!m_number)
			throw std::logic_error("The list is empty");
		
		entry_ptr new_node = std::make_shared<CNode> (val);
		entry_ptr before_before = pos_before->GetPrev();
		
		new_node->SetNext(pos_before);
		new_node->SetPrev(before_before);
		
		before_before->SetNext(new_node);
		pos_before->SetPrev(new_node);
		
		++m_number;
		
		return;
	}
	
	value_type pop_front() {
		if (!m_number)
			return value_type();
		
		if (m_number == 1) {
			value_type for_ret = std::make_shared<T>(m_base->GetValue());
			--m_number;
			return for_ret;
		}
		
		value_type for_ret = std::make_shared<T>(m_base->GetValue());
		entry_ptr new_base = m_base->GetNext();
		
		new_base->SetPrev();
		m_base = new_base;
		--m_number;
		
		return for_ret;
	}
	
	value_type pop_back() {
		if (!m_number)
			return value_type();
			
		if (m_number == 1) {
			value_type for_ret = std::make_shared<T>(m_base->GetValue());
			--m_number;
			return for_ret;
		}
		
		entry_ptr new_last = m_last.lock()->GetPrev();
		value_type for_ret = std::make_shared<T>(m_last.lock()->GetValue());
		
		new_last->SetNext();
		m_last = new_last;
		--m_number;
		
		return for_ret;
	}
	
	value_type remove(entry_ptr pos_before) {
		if (!m_number)
			throw std::logic_error("The list is empty");
			
		if (m_number == 1)
			throw std::logic_error("Incorrect position, at the list is only 1 element");
		
		entry_ptr pos_del = pos_before->GetPrev().lock();
		entry_ptr before_before = pos_del->GetPrev().lock();
		value_type for_ret = std::make_shared<T>(pos_del->GetValue());
		
		before_before->SetNext(pos_before);
		pos_before->SetPrev(before_before);
		
		return for_ret;
	}
	//
};



int main (int argc, char *argv[]) {
	CBList<std::string> list;
	
	std::cout << "size: " << list.size() << "\n";
	list.push_back("111111");
	list.push_front("22222");
	std::cout << "size: " << list.size() << "\n";
	for (CBList<std::string>::Iterator it = list.begin(); !it.finished(); ++it) {
		std::cout << *it << " - size: " << it->size() << "\n";
	}
	std::cout << "\n";
	CBList<std::string>::Iterator it = list.begin();
	*it = "qwer";
	for (CBList<std::string>::Iterator it = list.begin(); !it.finished(); ++it) {
		std::cout << *it << " - size: " << it->size() << "\n";
	}
	std::cout << "\n";
	list.pop_front();
	list.pop_back();
	std::cout << "size: " << list.size() << "\n";
	std::cout << "\n";
	for (CBList<std::string>::Iterator it = list.begin(); !it.finished(); ++it) {
		std::cout << *it << " - size: " << it->size() << "\n";
	}
	std::cout << "--\n";
	
	return 0;
}





