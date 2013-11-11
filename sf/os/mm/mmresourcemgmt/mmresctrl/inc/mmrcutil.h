// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef _MMRCUTIL_H_
#define _MMRCUTIL_H_

#include <e32base.h>

template <class T>
class RMMRCFifoQueue
	{
public:
	RMMRCFifoQueue( );
	~RMMRCFifoQueue( );

	void Reset( );
	void ResetAndDestroy();
	TBool IsEmpty( ) const;
	TInt Count() const;
	T* PopAndRemove( );
	T* PopAndRemove( TInt aIndex );
	void PushL( T const* const  aElement );
	T* Pop( ) const;
	T* Pop( TInt aIndex ) const;
	T* operator[] ( TInt aIndex ) const;

private:
	RMMRCFifoQueue( const RMMRCFifoQueue & aMMRCFifoQueue ); //not implemented
	const RMMRCFifoQueue & operator= ( const RMMRCFifoQueue & aMMRCFifoQueue );  //not implemented

private:
	struct TListNode
		{
		T const*  iElement;
		TListNode *iNext;

		TListNode( T const* const aElement )
		: iElement( aElement ), iNext(NULL) { }
		};

	TListNode *iFirst;
	TListNode *iLast;
	TInt iCount;
	};
	
/**
 Construct the queue.
*/
template <class T>
RMMRCFifoQueue<T>::RMMRCFifoQueue( )
	{
	iFirst = iLast = NULL;
	iCount = 0;
	}

/**
 Destructor.
*/
template <class T>
RMMRCFifoQueue<T>::~RMMRCFifoQueue( )
	{
	Reset( );
	}


/**
 Delete each node without deleting the elements.
*/
template <class T>
void RMMRCFifoQueue<T>::Reset( )
	{
	while( !IsEmpty( ) )
		{
		PopAndRemove( );
		}
	}

/**
 Delete each node and each element.
*/
template <class T>
void RMMRCFifoQueue<T>::ResetAndDestroy( )
	{
	while( !IsEmpty( ) )
		{
		T* element = PopAndRemove( );
		delete element;
		}
	}

/**
 Return the number of elements.
*/
template <class T>
TInt RMMRCFifoQueue<T>::Count( ) const
	{
	return iCount;
	}


/**
 Test if the queue is logically empty.
 Return TTrue if empty, TFalse, otherwise.
*/
template <class T>
TBool RMMRCFifoQueue<T>::IsEmpty( ) const
	{
	return iFirst == NULL;
	}


/**
 Return and remove the least recently inserted item from the queue.
*/
template <class T>
T* RMMRCFifoQueue<T>::PopAndRemove( )
	{
	T* element = NULL;
	if( !IsEmpty( ) )
		{
		TListNode* front = iFirst;
		element = const_cast<T*>(iFirst->iElement);
		iFirst = iFirst->iNext;
		if( NULL == iFirst )
			{
			iLast = NULL;
			}
		iCount--;
		delete front;
		front = NULL;
		}
	return element;
	}

/**
 Return and remove the least recently inserted item from the queue.
*/
template <class T>
T* RMMRCFifoQueue<T>::PopAndRemove( TInt aIndex )
	{
	T* element = NULL;
	TListNode* PreviousNode = NULL;
	TListNode* node = iFirst;
	TInt i = 0;
	for( ; i<aIndex && i<iCount && node->iNext ; i++)
		{
		PreviousNode = node;
		node = node->iNext;
		}
	
	if(node && i < iCount)
		{
		if(PreviousNode)
			{
			PreviousNode->iNext = node->iNext;
			if( NULL == node->iNext )
				{
				iLast = PreviousNode;
				}
			}
		else 
			{
			iFirst = node->iNext;
			if( NULL == iFirst )
				{
				iLast = NULL;
				} 
			}
		element = const_cast<T*>(node->iElement);
		iCount--;
		delete node;
		}
	return element;
	}

/**
 Insert aElement into the queue.
*/
template <class T>
void RMMRCFifoQueue<T>::PushL( T const* const aElement )
	{
	if( IsEmpty( ) )
		{
		iLast = iFirst = new(ELeave) TListNode( aElement );
		}
	else
		{
		iLast = iLast->iNext = new(ELeave) TListNode( aElement );
		}
	iCount++;
	}

/**
 Pop a pointer of the aIndex elements.
*/
template <class T>
T* RMMRCFifoQueue<T>::Pop ( ) const
	{
	T* element = NULL;
	TListNode* node = iFirst;
	
	if( node )
		{
		element = const_cast<T*>(node->iElement);
		}
	
	return element;
	}

/**
 Pop a pointer of the aIndex elements.
*/
template <class T>
T* RMMRCFifoQueue<T>::Pop ( TInt aIndex ) const
	{
	T* element = NULL;
	TListNode* node = iFirst;
	
	TInt i = 0;
	for( ; (i < aIndex) && (i < iCount) && (node->iNext != NULL) ; i++)
		{
		node = node->iNext;
		}
	
	if(  ( NULL != node ) 
			&& ( i < iCount )  )
		{
		element = const_cast<T*>(node->iElement);
		}
	
	return element;
	}

/**
 Pop a pointer of the aIndex elements.
*/
template <class T>
T* RMMRCFifoQueue<T>::operator[] ( TInt aIndex ) const
	{
	return Pop(aIndex);
	}


/**
 Class for manage the list of the sessions requesting pause.
*/
template <class T>
class RMMRCFifoOrderQueue : private RMMRCFifoQueue <T>
	{

public:
	RMMRCFifoOrderQueue( );
	~RMMRCFifoOrderQueue( );
	void ResetAndDestroy( );
	TInt Count() const;
	void PushL( T const* const  aElement );
	T* PopAndRemoveForPause( TInt aIndex );
	T* operator[] ( TInt aIndex ) const;


private:
	RPointerArray< T >  iQueue;
	};


/**
 Construct the queue.
*/
template <class T>
RMMRCFifoOrderQueue<T>::RMMRCFifoOrderQueue( )
	{
	}

/**
 Destructor.
*/
template <class T>
RMMRCFifoOrderQueue<T>::~RMMRCFifoOrderQueue( )
	{
	iQueue.Reset();
	iQueue.Close();
	}
	
/**
 Return the number of elements.
*/
template <class T>
TInt RMMRCFifoOrderQueue<T>::Count( ) const
	{
	
	TInt iNum = iQueue.Count();
	return iNum;
	}	

/**
 Delete each node and each element.
*/
template <class T>
void RMMRCFifoOrderQueue<T>::ResetAndDestroy( )
	{
	iQueue.ResetAndDestroy();
	}

/**
 Insert aElement into the queue and ordered.
*/
template <class T>
void RMMRCFifoOrderQueue<T>::PushL( T const* const aElement )
	{
	TInt numElements = iQueue.Count();
	
	if( numElements == 0 )
		{
		iQueue.Append(aElement); //iQueue: We need to check the error here
		return;
		}
	
	for(TInt i(0); i<numElements ; ++i)
		{
		if(aElement->HasMultimediaCapability() && !(iQueue[i]->HasMultimediaCapability()))
			{
			iQueue.Insert(aElement, i);
			return;
			}
		else if(aElement->HasMultimediaCapability() == iQueue[i]->HasMultimediaCapability())	
			{
			if (aElement->GetPriority() > iQueue[i]->GetPriority())
			 	{
				iQueue.Insert(aElement,i);
				return;
			 	}			
			}
		}
	iQueue.Insert(aElement,numElements);
	}
	
/**
 Return and remove the inserted item from the queue.
*/
template <class T>
T* RMMRCFifoOrderQueue<T>::PopAndRemoveForPause( TInt aIndex )
	{
	T* aux = NULL;
	aux = iQueue[aIndex];
	iQueue.Remove(aIndex);	
	return aux;
	}


/**
 Pop a pointer of the aIndex elements.
*/
template <class T>
T* RMMRCFifoOrderQueue<T>::operator[] ( TInt aIndex ) const
	{
	if(iQueue.Count() != 0)
		{
		return iQueue[aIndex];
		}
	else
		{
		return NULL;
		}
	}

#endif /*_MMRCUTIL_H_*/
