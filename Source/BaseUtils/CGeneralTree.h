#pragma once



/*! \file CGeneralTree.h
	\brief This file contains the definitions of CGeneralTree and TNode classes.
*/

namespace Utils
{

//! A class representing a general tree node.
template <typename T> struct TNode
{	
	//! Clears the node and all its children.
	TNode()
	{
		m_Value = T(); 
		m_aChildren.clear(); 
		m_pParent = 0;
	};

	
	
	/*! @{
		@name Main functions
		These are the main functions.
	*/
	//! Clears the node and all its children.
	inline void Clear()
	{
		for(std::vector<TNode<T> >::iterator i = m_aChildren.begin(); i != m_aChildren.end(); ++i) i->Clear();
		m_aChildren.clear();
		
		m_Value.~T();
		m_pParent = 0;
	}
	//! Clears the children but leaves the node intact.
	inline void DeleteAll()
	{
		for(std::vector<TNode<T> >::iterator i = m_aChildren.begin(); i != m_aChildren.end(); ++i) i->Clear();
		m_aChildren.clear();
	}
	//! @}

	
	/*! @{
		@name Traversal functions
	*/
	//! @see CGeneralTree::Preorder
	inline void Preorder(void (*TreeProc) (T&, void*), void* Ptr)
	{
		TreeProc(m_Value, Ptr);

		for(std::vector<TNode<T> >::iterator i = m_aChildren.begin(); i != m_aChildren.end(); ++i) i->Preorder(TreeProc, Ptr);
	}
	//! @see CGeneralTree::Postorder
	inline void Postorder(void (*TreeProc) (T&, void*), void* Ptr)
	{		
		for(std::vector<TNode<T> >::iterator i = m_aChildren.begin(); i != m_aChildren.end(); ++i) i->Preorder(TreeProc, Ptr);

		TreeProc(m_Value, Ptr);
	}
	//! @}
	

	//! The value of the node.
	T m_Value;
	//! A pointer to the parent node.
	TNode<T>* m_pParent;
	//! The child nodes.
	std::vector<TNode<T> > m_aChildren;


	//! The destructor. Clears the node and all its children.
	~TNode() {m_Value.~T(); Clear(); m_aChildren.clear(); m_pParent = 0;};
};


/*! \brief A class representing a general tree.
	
	Remarks:
		\li An invalid iterator (a null one) should be reseted immediately, since it disables performing most operations on the tree.
*/
template <typename T> class CGeneralTree
{
	public:
		//! The default constructor. Sets the iterator to the root.
		CGeneralTree()
		{
			m_Root = TNode<T>(); 
			SetRoot();
		};


		/*! @{
			@name Main functions
			These are the main functions.
		*/
		//! Clears the whole tree.
		inline void Clear();
		/*! Deletes the child in the current node at Index.
			@param Index The index of the child in the current node.
		*/
		inline void Delete(unsigned Index);
		//! Deletes all the children in the current node.
		inline void DeleteAll();
		/*! Inserts an object to the current node (at the end).
			@param v A reference to the object to insert.
		*/
		inline void Insert(const T& v);
		/*! Inserts an object to the current node.
			@param v A reference to the object to insert.
			@param Pos The position in the node to insert 
		*/
		inline void Insert(const T& v, unsigned Pos);
		//! @}


		
		/*! @{
			@name Manual tree traversal functions
		*/
		//! Moves the iterator to the parent node.
		inline void Parent();
		/*! Moves the iterator to the child node.
			@param Index The index of the child node.
		*/
		inline void Forth(unsigned Index);		
		//! Moves the iterator to the root.
		inline void SetRoot() {m_Iterator = &m_Root;};
		//! @}


		/*! @{
			@name Automatic tree traversal
		*/
		/*! Preorder traversal (from the root to the bottom of the tree).
			@param TreeProc A pointer to a function with a reference to a data object and a user pointer.
			@param Ptr The user pointer.
		*/
		void Preorder(void (*TreeProc) (T&, void*), void* Ptr);
		/*! Postorder traversal (from the bottom of the tree to the root).
			@param TreeProc A pointer to a function with a reference to a data object and a user pointer.
			@param Ptr The user pointer.
		*/
		void Postorder(void (*TreeProc) (T&, void*), void* Ptr);
		//! @}
		



		/*! @{
			@name Get accessors
			The Get accessor functions.
		*/
		inline TNode<T>* GetRoot() {return &m_Root;};
		inline TNode<T>* GetIterator() {return m_Iterator;};
		inline T* GetValue() {return (IsValid()) ? &m_Iterator->m_Value : 0;};
		inline unsigned GetNumChildren() const {return (IsValid()) ? (unsigned)m_Iterator->m_aChildren.size() : 0;};
		inline TNode<T>* GetChild(unsigned Index);
		//! @}

		
		/*! @{
			@name Set accessors
			The Set accessor functions.
		*/
		inline void SetValue(T& v) {if(IsValid()) m_Iterator->m_Value = v;}
		//! @}


		/*! @{
			@name Iterator validity checking
		*/
		inline bool IsValid() const {return (m_Iterator) ? true : false;};
		inline bool IsRoot() const {return (m_Iterator == &m_Root);};
		//! @}


		//! The destructor. Clears the tree and sets the iterator to root.
		~CGeneralTree(){Clear(); SetRoot();};

	private:
		
		//! The root of the tree.
		TNode<T> m_Root;
		//! The iterator used to traverse through the tree.
		TNode<T>* m_Iterator;


		/*! @{
			@name Internal inserting functions
		*/
		inline void Insert(const TNode<T>& v);
		inline void Insert(const TNode<T>& v, unsigned Pos);
		//! @}

};


//Implementation
template <typename T> inline void CGeneralTree<T>::Forth(unsigned Index)
{
	if(IsValid())
	{
		if(Index < m_Iterator->m_aChildren.size()) m_Iterator = &m_Iterator->m_aChildren[Index];
	}
}


template <typename T> inline void CGeneralTree<T>::Parent()
{
	if(IsValid())
	{
		if(m_Iterator->m_pParent) m_Iterator = m_Iterator->m_pParent;
	}
}

template <typename T> inline void CGeneralTree<T>::Delete(unsigned Index)
{
	if(IsValid())
	{
		if(Index < m_Iterator->m_aChlildren.size())
		{
			m_Iterator->m_aChildren[Index].Clear();
			m_Iterator->m_aChildren[Index].erase(m_Iterator->m_aChildren.begin() + Index);
		}
	}
}

template <typename T> inline void CGeneralTree<T>::Insert(const TNode<T>& v)
{
	if(IsValid())
	{
		m_Iterator->m_aChildren.push_back(v);
	}
}

template <typename T> inline void CGeneralTree<T>::Insert(const TNode<T>& v, unsigned Pos)
{
	if(IsValid())
	{
		if(Pos < m_Iterator->m_aChlildren.size()) m_Iterator->m_aChildren.insert(m_Iterator->m_aChildren.begin() + Pos);
	}
}


template <typename T> inline void CGeneralTree<T>::Clear()
{
	SetRoot();

	m_Iterator->Clear();
}


template <typename T> inline TNode<T>* CGeneralTree<T>::GetChild(unsigned Index)
{
	if(IsValid())
	{
		if(Index < m_Iterator->m_aChildren.size()) return &m_Iterator->m_aChildren[Index];
		else return 0;
	}
	else return 0;
}


template <typename T> inline void CGeneralTree<T>::Preorder(void (*TreeProc) (T&, void*), void* Ptr)
{
	if(!IsValid()) SetRoot();
	m_Iterator->Preorder(TreeProc, Ptr);
}

template <typename T> inline void CGeneralTree<T>::Postorder(void (*TreeProc) (T&, void*), void* Ptr)
{
	if(!IsValid()) SetRoot();
	m_Iterator->Postorder(TreeProc, Ptr);
}


template <typename T> inline void CGeneralTree<T>::Insert(const T& v)
{
	if(IsValid())
	{
		TNode<T> Node;
		Node.m_pParent = m_Iterator;
		Node.m_Value = v;
		Insert(Node);	
	}	
}

template <typename T> inline void CGeneralTree<T>::Insert(const T& v, unsigned Pos)
{
	if(IsValid())
	{
		if(Pos < m_Iterator->m_aChlildren.size()) 
		{
			TNode<T> Node;
			Node.m_pParent = m_Iterator;
			Node.m_Value = v;
			Insert(Node);
		}
	}	
}


template <typename T> inline void CGeneralTree<T>::DeleteAll()
{
	if(IsValid())
	{
		m_Iterator->Clear();
	}
}


}

