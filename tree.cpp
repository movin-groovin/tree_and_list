
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <utility>

#include <cassert>
#include <cstring>


//
// Node
//
template <typename TKey, typename TValue>
class CNode {
public:
/*
	CNode (
		CNode *left,
		CNode *right,
		std::shared_ptr<CNode> parent,
		TKey key,
		TValue val
	):
		m_left(left),
		m_right(right),
		m_parent(parent),
		key,
		val
	{}
*/
	
	CNode(
		std::shared_ptr<CNode> right,
		std::shared_ptr<CNode> left,
		std::shared_ptr<CNode> parent,
		TKey key,
		TValue val
	):
		m_left(left),
		m_right(right),
		m_parent(parent),
		m_key(key),
		m_val(val)
	{}
	
	virtual ~ CNode () {}
	
	bool operator < (const CNode & ref) {
		return m_key < ref.m_key;
	}
	
	std::shared_ptr<CNode> GetLeft() const {
		return m_left;
	}
	
	std::shared_ptr<CNode> GetRight() const {
		return m_right;
	}
	
	std::shared_ptr<CNode> GetParent() const {
		return m_parent.lock();
	}
	
	void SetLeft(std::shared_ptr<CNode> ptr) {
		m_left = ptr;
	}
	
	void SetRight(std::shared_ptr<CNode> ptr) {
		m_right = ptr;
	}
	
	void SetParent(std::shared_ptr<CNode> ptr) {
		m_parent = ptr;
	}
	
	TKey GetKey() const {
		return m_key;
	}
	
	void SetKey(TKey val) {
		m_key = val;
	}
	
	TValue GetValue() const {
		return m_val;
	}
	
	void SetValue(TValue val) {
		m_val = val;
	}
	
private:
	std::shared_ptr<CNode> m_left;
	std::shared_ptr<CNode> m_right;
	std::weak_ptr<CNode> m_parent;
	
	TKey m_key;
	TValue m_val;
};


std::string operator * (const std::string & str, size_t i) {
	std::string tmp;
	for (size_t j = 0; j < i; ++j)
		tmp += str;
	return tmp;
}


//
// Tree
//
template <typename TKey, typename TValue>
class CBSTree {
public:
	typedef std::shared_ptr <CNode<TKey, TValue> > pointer;
	typedef CNode <TKey, TValue> basetype;
	
	CBSTree (): m_root(nullptr), m_node_count(0) {}
	~ CBSTree () {}
	
	TValue GetMax() const;
	TValue GetMin() const;
	void Insert(TKey key, TValue val);
	pointer Delete(TKey key);
	pointer FindNewParent(pointer ptr) const;
	std::shared_ptr<basetype> Find(TKey key) const;

#ifndef NDEBUG
	void PrintTree() const {
		std::string pref = "    ";
		unsigned int pref_num = 0;
		std::pair <pointer, unsigned int> cur_entry = std::make_pair(m_root, 0);
		std::queue<std::pair <pointer, unsigned int>> nodes;
		
		nodes.push(cur_entry);
		while (!nodes.empty()) {
			cur_entry = nodes.front();
			nodes.pop();
			
			if (cur_entry.first->GetLeft())
				nodes.push(std::make_pair(cur_entry.first->GetLeft(), cur_entry.second + 1));
			if (cur_entry.first->GetRight())
				nodes.push(std::make_pair(cur_entry.first->GetRight(), cur_entry.second + 1));
			
			std::string tmp_pref = pref * cur_entry.second;
			std::cout << tmp_pref << cur_entry.first->GetKey() << " - "
					  << cur_entry.first->GetValue() << "\n";
			//
		}
		
		return;
	}
#endif
	
private:
	
	std::shared_ptr <CNode <TKey, TValue> > m_root;
	size_t m_node_count;
};


template <typename TKey, typename TValue>
TValue CBSTree<TKey, TValue>::GetMax() const {
	pointer cur = m_root;
	while (cur->GetRight())
		cur = cur->GetRight();
	return cur->GetValue();
}

template <typename TKey, typename TValue>
TValue CBSTree<TKey, TValue>::GetMin() const {
	pointer cur = m_root;
	while (cur->GetLeft())
		cur = cur->GetLeft();
	return cur;
}

template <typename TKey, typename TValue>
void CBSTree<TKey, TValue>::Insert(TKey key, TValue val) {
	if (!m_root) {
		m_root = pointer(new CNode<TKey, TValue> (nullptr, nullptr, nullptr, key, val));
		return;
	}
	
	pointer cur_node = m_root;
	pointer prev_node;
	while (cur_node) {
		prev_node = cur_node;
		if (cur_node->GetKey() < key)
			cur_node = cur_node->GetRight();
		else
			cur_node = cur_node->GetLeft();
	}
	
	if (prev_node->GetKey() < key)
		prev_node->SetRight(pointer(new basetype (nullptr, nullptr, prev_node, key, val)));
	else
		prev_node->SetLeft(pointer(new basetype (nullptr, nullptr, prev_node, key, val)));
	
	return;
}


template <typename TKey, typename TValue>
std::shared_ptr<CNode<TKey, TValue>> CBSTree<TKey, TValue>::Find(TKey key) const {
	pointer current = m_root;
	
	while (current) {
		if (current->GetKey() < key)
			current = current->GetRight();
		else if (current->GetKey() > key)
			current = current->GetLeft();
		else
			return current;
	}
	
	return pointer();
}

// return a node without left child from right subtree of ptr node-parameter
template <typename TKey, typename TValue>
std::shared_ptr<CNode<TKey, TValue>> CBSTree<TKey, TValue>::FindNewParent(pointer ptr) const {
	ptr = ptr->GetRight();
	
	while (ptr) {
		if (!ptr->GetLeft())
			return ptr;
		ptr = ptr->GetLeft();
	}
	
	assert(1 != 1);
	return pointer();
}

template <typename TKey, typename TValue>
std::shared_ptr<CNode<TKey, TValue>> CBSTree<TKey, TValue>::Delete(TKey key) {
	pointer ptr = Find(key);
	
	// node not found
	if (!ptr)
		return pointer (nullptr);
	
	// if the node is the root
	if (ptr == m_root) {
		pointer for_ret(m_root);
		pointer new_root(m_root->GetRight());
		pointer tmp = new_root;
		
		while(tmp->GetLeft()) {
			tmp = tmp->GetLeft();
		}
		tmp->SetLeft(m_root->GetLeft());
		m_root = new_root;
		
		return for_ret;
	}
	
	// leaf deleting
	if (!ptr->GetLeft() && !ptr->GetRight()) {
		pointer parent = ptr->GetParent();
		if (parent->GetLeft()->GetKey() == key)
			parent->SetLeft(nullptr);
		else
			parent->SetRight(nullptr);
		return ptr;
	}
	
	// deleting a node with one child (left or right)
	// with left child
	if (!ptr->GetRight()) {
		pointer parent = ptr->GetParent();
		if (parent->GetLeft()->GetKey() == key) {
			parent->SetLeft(ptr->GetLeft());
			ptr->GetLeft()->SetParent(parent);
		}
		else {
			parent->SetRight(ptr->GetLeft());
			ptr->GetLeft()->SetParent(parent);
		}
		return ptr;
	}
	// with right child
	else if (!ptr->GetLeft()) {
		pointer parent = ptr->GetParent();
		if (parent->GetLeft()->GetKey() == key) {
			parent->SetLeft(ptr->GetRight());
			ptr->GetRight()->SetParent(parent);
		}
		else {
			parent->SetRight(ptr->GetRight());
			ptr->GetRight()->SetParent(parent);
		}
		return ptr;
	}
	
	// deleting a node with both childs (left, right)
	pointer new_parent = FindNewParent(ptr);
	// if new_parent has right child change it's parent and change parent of new parent
	if (new_parent->GetRight()) {
		new_parent->GetRight()->SetParent(new_parent->GetParent());
		if (new_parent->GetParent()->GetLeft()->GetKey() == new_parent->GetKey()) {
			new_parent->GetParent()->SetLeft(new_parent->GetRight());
		} 
		else {
			new_parent->GetParent()->SetRight(new_parent->GetRight());
		}
	} else {
		if (new_parent->GetParent()->GetLeft()->GetKey() == new_parent->GetKey()) {
			new_parent->GetParent()->SetLeft(nullptr);
		} 
		else {
			new_parent->GetParent()->SetRight(nullptr);
		}
	}
	
	// change deleted node
	new_parent->SetRight(ptr->GetRight());
	new_parent->SetLeft(ptr->GetLeft());
	new_parent->SetParent(ptr->GetParent());
	if (ptr->GetParent()->GetLeft()->GetKey() == ptr->GetKey())
		ptr->GetParent()->SetLeft(new_parent);
	else
		ptr->GetParent()->SetRight(new_parent);
	
	return ptr;
}


int main (int argc, char **argv) {
	CBSTree <std::string, double> bst;
	
	bst.Insert ("123", 1231);
	bst.Insert ("234", 2341);
	bst.Insert ("345", 3451);
	bst.Insert ("567", 5671);
	bst.Insert ("119", 1191);
	bst.Insert ("120", 1201);
	bst.Insert ("124", 1241);
	bst.Insert ("677", 6771);
	
#ifndef NDEBUG
	bst.PrintTree();
#endif
	bst.Delete("123");
std::cout << "\n================================\n";
#ifndef NDEBUG
	bst.PrintTree();
#endif
	
	return 0;
}












