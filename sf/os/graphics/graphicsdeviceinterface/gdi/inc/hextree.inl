// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Hexadecimal trees - inline functions
//

#ifndef HEXTREE_INL
#define HEXTREE_INL

/**
Constructor. It constructs an associative array with no key-value pairs.

@param aHeap A pointer to the heap to be used by the associative array
       implementation to allocate memory for internal data structures. This
       heap can be shared between several processes.
*/
template<class T>
inline RHexTree<T>::RHexTree(RHeap* aHeap)
    : RHexTreeBase(aHeap)
    {
    }

/**
Adds a key-value pair to this associative array.

@param aKey The 32-bit key to add to this associative array.
@param aValue A pointer to the value to associate with aKey. It must have been
       allocated on the same heap as the one used by the associative array
       implementation to allocate memory for internal data structures. Ownership
       is transferred to this associative array.
@return KErrNone if the key-value pair was added successfully. KErrNoMemory if
        there was not enough memory in the heap for internal data structures.
        KErrAlreadyExists if an attempt was made to add a duplicate key.
*/
template<class T>
inline TInt RHexTree<T>::SetAt(TUint aKey, T* aValue)
    {
    return RHexTreeBase::SetAt(aKey, aValue);
    }

/**
Looks up a given key in this associative array and returns a pointer to the
corresponding value.

@param aKey The 32-bit key to look up.
@return A pointer to the corresponding value in this associative array, if the
        given key was found. The value may not be modified via this pointer.
        NULL if the given key was not found.
*/
template<class T>
inline const T* RHexTree<T>::At(TUint aKey) const
    {
    return static_cast<T*>(RHexTreeBase::At(aKey));
    }

/**
Looks up a given key in this associative array and returns a pointer to the
corresponding value. Note that if values are modified after being added to an
associative array, then the user is responsible for synchronisation when
concurrent access is needed.

@param aKey The 32-bit key to look up.
@return A pointer to the corresponding value in this associative array, if the
        given key was found. The value may be modified via this pointer.
        NULL if the given key was not found.
*/
template<class T>
inline T* RHexTree<T>::At(TUint aKey)
    {
    return static_cast<T*>(RHexTreeBase::At(aKey));
    }

#endif // HEXTREE_INL
