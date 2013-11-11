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
// Hexadecimal trees - declaration
//

#ifndef HEXTREE_H
#define HEXTREE_H

#include <e32std.h>

/**
Base class that provides the implementation for RHexTree, which is just a thin
template.

An instance of this class can have up to eight 16-way prefix trees, with heights
from 1 to 8. All the values are stored in the leaves. To find a value from a
32-bit key, first the key is decomposed into 8 hexadecimal digits and then the
prefix tree with height matching the number of digits in the key (ignoring zeros
to the left) is traversed using the sequence of digits in the key as the
indexing string. Offsets are internally used instead of pointers to allow
instances to be placed in a heap shared between several processes.
*/
class RHexTreeBase
    {
public:
    IMPORT_C void ResetAndDestroy();
protected:
    IMPORT_C RHexTreeBase(RHeap* aHeap);
    IMPORT_C TInt SetAt(TUint aKey, TAny* aValue);
    IMPORT_C TAny* At(TUint aKey) const;
private:
    TInt SetAt(TUint aKey, TAny* aLeaf, TInt aHeight);
    TInt SetAt(TUint aKey, TAny* aLeaf, TInt aHeight, TAny* aNode, TInt aLevel);
    TAny* At(TUint aKey, TInt aHeight) const;
    void ResetAndDestroy(TInt aHeight, TAny* aNode, TInt aLevel);
private:
    enum { EMaxNumHexDigits = 8 };
private:
    RHeap* iHeap;
    TInt iRootOffsets[EMaxNumHexDigits];
    };

/**
An associative array implementation optimised for the case where the keys are
32-bit codes with spatial locality of reference. The values can be of any
self-contained data type (that is, without destructor or clean-up functions).
It allows multiple-readers, single-writer concurrent access from different
processes in an SMP-safe manner without locking, excluding deletion of
individual key-value pairs.
*/
template<class T>
class RHexTree : public RHexTreeBase
    {
public:
    inline RHexTree(RHeap* aHeap);
    inline TInt SetAt(TUint aKey, T* aValue);
    inline const T* At(TUint aKey) const;
    inline T* At(TUint aKey);
    };

#include <hextree.inl>

#endif // HEXTREE_H
