

#ifndef Link_h
#define Link_h

template<class Elem>

class Link
{
public:



	Link(const Elem &elemValue, Link* nextValue = nullptr, Link *prevValue = nullptr)
	{
		element = elemValue;
	
		next = nextValue;
		
		prev = prevValue;
	}

	Link(Link *nextValue = nullptr, Link* prevValue = nullptr)
	{
		next = nextValue;
		prev = prevValue;
	}



	Link *next;
	Link *prev;
	
	Elem element;
};

#endif