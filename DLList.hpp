
#ifndef DLList_h
#define DLList_h

#include "Link.h"
//double linked List that I used to store all object sprites
template <class Elem>

class DLList : public Link <Elem>
{
public:

	explicit DLList();
	//insert a new item into the list the item will be inserte at the fence next pointer
	void insert(const Elem&);
	//remove the fence next item
	bool remove(Elem &);
	//set the postion on the list
	bool setPos(int);
	//get the objecet value
	bool getValue(Elem &) const;
	//append an item to the end of the list
	void append(const Elem &);
	//set the postion to the start of the list
	void setStart();
	//set the position to the end of the list
	void setEnd();
	//move to the previous item
	bool prev();
	//move to the next item
	bool next();
	// void print();
	//insert item to the fence->next and move to the next
	bool InsertMovNex(Elem &);

	bool checkIfNextAvaiable();


	//get the right lenght
	int rightLenght();
	//get the left lenght
	int leftLenght();
	//get the total lengh of the list
	int totalLenght();




private:
	//pointers to the place on the list
	Link <Elem>* head;
	Link <Elem>* tail;
	Link <Elem>* fence;

	//this will store the size of the list
	int leftCnt;
	int rightCnt;

	//List initiliaziation
	void initialize()
	{
		fence = tail = head = new Link<Elem>;
		leftCnt = rightCnt = 0;
	}

	//remove all the items from the list
	void removeAll()
	{
		while (fence != tail)
		{
			fence = head;

			head = head->next;

			delete fence;
		}
	}



};



//intialize the list
template <class Elem>
DLList<Elem>::DLList() { initialize(); };


//insert an item at teh fence next
template<class Elem>
void DLList<Elem>::insert(const Elem &item)
{
	fence->next = new Link<Elem>(item, fence->next, fence);
	if (fence->next->next != nullptr) fence->next->next->prev = fence->next;

	if (tail == fence) tail = fence->next;

	rightCnt++;
}
//remove the fence next item
template<class Elem>
bool DLList<Elem>::remove(Elem & item)
{
	if (fence->next == nullptr) return false;

	Link<Elem> *temp = fence->next;

	item = temp->element;
	fence->next = temp->next;

	if (fence->next != nullptr) fence->next->prev = fence;

	else tail = fence;

	delete temp;

	rightCnt--;

	return true;
}

//set the position on the list
template <class Elem>
bool DLList<Elem>::setPos(int pos)
{


	if (pos < 0 || pos > rightCnt + leftCnt) return false;

	if (pos > leftCnt)
	{
		for (int i = 0; i < (pos - leftCnt); i++) fence = fence->next;
	}

	else
	{
		for (int i = 0; i < (leftCnt - pos); i++) fence = fence->prev;
	}

	rightCnt = rightCnt + leftCnt - pos;

	leftCnt = pos;

	return true;
}
//get the item value on the fence next
template<class Elem>
bool DLList<Elem>::getValue(Elem & item)const
{
	if (fence->next == nullptr) return false;

	item = fence->next->element;

	return true;
}
//append an item to the end of the list
template<class Elem>
void DLList<Elem>::append(const Elem & item)
{
	tail = tail->next = new Link<Elem>(item, nullptr, tail);

	rightCnt++;
}
//set the postion to the start of the list
template<class Elem>
void DLList<Elem>::setStart()
{
	fence = head;
	rightCnt += leftCnt;

	leftCnt = 0;
}
//set the postion to the end of the list
template<class Elem >
void DLList<Elem>::setEnd()
{
	fence = tail;

	leftCnt += rightCnt;

	rightCnt = 0;
}

//move to the next item
template<class Elem>
bool DLList<Elem>::next()
{
	if (fence == tail) return false;

	fence = fence->next;

	rightCnt--;
	leftCnt++;

	return true;

}
//move to the previous item
template<class Elem>
bool DLList<Elem>::prev()
{
	if (fence == head) return false;

	fence = fence->prev;

	rightCnt++;
	leftCnt--;

	return true;
}
//get the right lengh from the fence
template<class Elem>
int DLList<Elem>::rightLenght()
{
	return rightCnt;
}
//get the left lenght from the fence
template<class Elem>
int DLList<Elem>::leftLenght()
{
	return leftCnt;
}
//get the total lenght of the list
template <class Elem>
int DLList<Elem>::totalLenght()
{
	return rightCnt + leftCnt;
}

template <class Elem>
bool DLList<Elem>::checkIfNextAvaiable()
{
	if (fence->next == tail) return false;

	return true;
}
#endif /* DLList_h */






