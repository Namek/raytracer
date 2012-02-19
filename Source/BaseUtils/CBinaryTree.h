#pragma once


/*! \file CBinaryTree.h
	\brief This file contains the definitions of CBinaryTree and TBinaryNode classes.
*/

namespace Utils
{

//! A class representing a binary tree node.
template <typename T> struct TBinaryNode
{
	TBinaryNode(){m_pParent = m_pLeft = m_pRight = 0; m_Value.~T();};
	TBinaryNode(const T& Value, TBinaryNode<T>* Left, TBinaryNode<T>* Right, TBinaryNode<T>* Parent) : m_pParent(Parent), m_pLeft(Left), m_pRight(Right) {};

	
	//! Deletes all of the child nodes as well as itself.
	inline void Clear()
	{
		if(m_pRight) m_pRight->Clear();
		if(m_pLeft) m_pLeft->Clear();

		SafeDelete(m_pRight);
		SafeDelete(m_pLeft);
		m_pParent = 0;

		m_Value.~T();
	}
	//! Deletes the left child and all of its children.
	inline void DeleteLeft()
	{
		if(m_pLeft)
		{
			m_pLeft->Clear();
			SafeDelete(m_pLeft);
		}
	}
	//! Deletes the right child and all of its children.
	inline void DeleteRight()
	{
		if(m_pLeft)
		{
			m_pRight->Clear();
			SafeDelete(m_pRight);
		}
	}


	/*! @{
		@name Traversal functtons
	*/
	//! Traverses the tree from the root to the deepest nodes.
	inline void Preorder(void (*Proc) (T&, void*), void* Data)
	{
		Proc(m_Value, Data);

		if(m_pLeft) m_pLeft->Postorder(Proc, Data);
		if(m_pRight) m_pRight->Postorder(Proc, Data);		
	}
	//! Traverses the tree from the 'deepest' nodes to the root.
	inline void Postorder(void (*Proc) (T&, void*), void* Data)
	{
		if(m_pLeft) m_pLeft->Postorder(Proc, Data);
		if(m_pRight) m_pRight->Postorder(Proc, Data);

		Proc(m_Value, Data);
	}
	//! @}


	//! The node data.
	T m_Value;
	//! A pointer to the parent node.
	TBinaryNode<T>* m_pParent;
	//! A pointer to the left node.
	TBinaryNode<T>* m_pRight;
	//! A pointer to the right node.
	TBinaryNode<T>* m_pLeft;

	~TBinaryNode(){Clear();};
};


/*! \brief A class representing a binary tree.
	<br> Remarks:
		\li An invalid iterator (a null one) should be reseted immediately, since it disables performing most operations on the tree.
*/
template <typename T> class CBinaryTree
{
	public:
		/*! The default constructor. Clears the tree and sets the iterator to root. 
			@see ClearAll
		*/
		CBinaryTree() {ClearAll();};
		//! Initializes and sets the iterator to root.
		explicit CBinaryTree(const T& v) {m_Root(v, 0, 0, 0); SetRoot();};


		/*! @{
			@name Main functions
			These are the main functions.
		*/
		//! Clears the tree and calls CBinaryTree::SetRoot
		inline void ClearAll() {SetRoot(); m_Root.Clear();};
		//! Deletes the left child of the selected node and all its children.
		inline void DeleteLeft() {if(IsValid() m_Iterator->DeleteLeft();)};
		//! Deletes the right child of the selected node and all its children.
		inline void DeleteRight() {if(IsValid() m_Iterator->DeleteRight();)};
		//! Deletes the current node and all of its children.
		inline void DeleteCurr();
		/*! Inserts a data object into the left child node (if it does not exist, it creates one, otherwise the previous data is replaced)
			@param v A const reference to the data.
		*/
		inline void InsertLeft(const T& v);
		/*! Inserts a data object into the right child node (if it does not exist, it creates one, otherwise the previous data is replaced)
			@param v A const reference to the data.
		*/
		inline void InsertRight(const T& v);
		//! @}


		/*! @{
			@name Manual tree traversal functions
		*/
		//! Moves the iterator to the root.
		inline void SetRoot() {m_Iterator = &m_Root;};
		//! Moves the iterator to the left node.
		inline void Left();
		//! Moves the iterator to the right node.
		inline void Right();
		//! Moves the iterator to the parent node.
		inline void Parent();
		//! @}


		/*! @{
			@name Automatic tree traversal
		*/
		/*! Performs preorder tree traversal.
			@see CGeneralTree::Preorder
		*/
		void Preorder(void (*Proc) (T&, void*), void* Data);
		/*! Performs postorder tree traversal.
			@see CGeneralTree::Postorder
		*/
		void Postorder(void (*Proc) (T&, void*), void* Data);
		//! @}


		/*! @{
			@name Get accessors
			The Get accessor functions.
		*/
		inline T* GetValue() {return (IsValid()) ? m_Iterator->m_Value : 0;};
		inline TBinaryNode<T>* GetIterator() {return m_Iterator;};
		inline TBinaryNode<T>* GetRoot() {return &m_Root;};
		//! @}
		

		/*! @{
			@name Set accessors
			The Set accessor functions.
		*/
		inline void SetValue(const T& v) {if(IsValid()) m_Iterator->m_Value = v;};
		//! @}


		/*! @{
			@name Iterator validity checking
		*/
		inline bool IsValid() const {return (m_Iterator) ? true : false;};
		inline bool IsRoot() const {return (m_Iterator == &m_Root);};
		//! @}


		/*! The destructor. Clears the tree and sets the iterator to root.
			@see ClearAll
		*/
		~CBinaryTree() {ClearAll();};

	private:
		//! The root of the tree.
		TBinaryNode<T> m_Root;
		//! The iterator used to traverse through the tree.
		TBinaryNode<T>* m_Iterator;
		
};



//Implementation
template <typename T> inline void CBinaryTree<T>::DeleteCurr()
{
	if(IsValid())
	{
		TBinaryNode<T>* pIterator = m_Iterator;
		if(!IsRoot()) m_Iterator = m_Iterator->m_pParent;
		
		pIterator->Clear();
	}
}


template <typename T> inline void CBinaryTree<T>::InsertLeft(const T& v)
{
	if(IsValid())
	{
		if(m_Iterator->m_pLeft)
		{
			m_Iterator->m_pLeft->m_Value = v;
		}
		else
		{
			m_Iterator->m_pLeft = new TBinaryNode<T>(v, 0, 0, m_Iterator);
		}
	}
}

template <typename T> inline void CBinaryTree<T>::InsertRight(const T& v)
{
	if(IsValid())
	{
		if(m_Iterator->m_pRight)
		{
			m_Iterator->m_pRight->m_Value = v;
		}
		else
		{
			m_Iterator->m_pRight = new TBinaryNode<T>(v, 0, 0, m_Iterator);
		}
	}
}


template <typename T> inline void CBinaryTree<T>::Left()
{
	if(IsValid())
	{
		if(m_Iterator->m_pLeft) m_Iterator = m_Iterator->m_pLeft;
	}
}


template <typename T> inline void CBinaryTree<T>::Right()
{
	if(IsValid())
	{
		if(m_Iterator->m_pRight) m_Iterator = m_Iterator->m_pRight;
	}
}

template <typename T> inline void CBinaryTree<T>::Parent()
{
	if(IsValid())
	{
		if(m_Iterator->m_pParent) m_Iterator = m_Iterator->m_pParent;
	}
}


template <typename T> inline void CBinaryTree<T>::Preorder(void (*Proc) (T&, void*), void* Data)
{
	if(IsValid())
	{
		m_Iterator->Preorder();
	}
}

template <typename T> inline void CBinaryTree<T>::Postorder(void (*Proc) (T&, void*), void* Data)
{
	if(IsValid())
	{
		m_Iterator->Postorder(Proc, Data);
	}
}


}
