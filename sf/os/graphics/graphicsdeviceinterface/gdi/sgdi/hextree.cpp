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
// Hexadecimal trees - implementation
//

#include <hextree.h>
#include <e32atomics.h>

EXPORT_C RHexTreeBase::RHexTreeBase(RHeap* aHeap)
    : iHeap(aHeap)
    {
    Mem::FillZ(iRootOffsets, sizeof(iRootOffsets));
    }

EXPORT_C TInt RHexTreeBase::SetAt(TUint aKey, TAny* aValue)
    {
    TUint mask = 0xF0000000;
    for (TInt height = EMaxNumHexDigits; height > 1; --height)
        {
        if ((aKey & mask) != 0)
            {
            return SetAt(aKey, aValue, height);
            }
        mask >>= 4;
        }
    return SetAt(aKey, aValue, 1);
    }

EXPORT_C TAny* RHexTreeBase::At(TUint aKey) const
    {
    TUint mask = 0xF0000000;
    for (TInt height = EMaxNumHexDigits; height > 1; --height)
        {
        if ((aKey & mask) != 0)
            {
            return At(aKey, height);
            }
        mask >>= 4;
        }
    return At(aKey, 1);
    }

/**
Empties this associative array and frees all memory allocated both for the
associative array implementation and for the values that have been added to
this associative array.

The internal state of this associative array is reset so that it can be reused
or allowed to go out of scope after a call to this function.
*/
EXPORT_C void RHexTreeBase::ResetAndDestroy()
    {
    for (TInt height = 1; height <= EMaxNumHexDigits; ++height)
        {
        TInt offset = iRootOffsets[height - 1];
        if (offset != 0)
            {
            TAny* root = PtrAdd(this, offset);
            ResetAndDestroy(height, root, 1);
            iRootOffsets[height - 1] = 0;
            }
        }
    }

TInt RHexTreeBase::SetAt(TUint aKey, TAny* aLeaf, TInt aHeight)
    {
    TInt err;
    TInt offset = iRootOffsets[aHeight - 1];
    if (offset == 0)
        {
        TAny* root = iHeap->AllocZ(aHeight > 1 ? sizeof(TInt) * 15 : sizeof(TInt) * 16);
        if (!root)
            {
            return KErrNoMemory;
            }
        err = SetAt(aKey, aLeaf, aHeight, root, 1);
        if (err == KErrNone)
            {
            __e32_atomic_store_rel32(&iRootOffsets[aHeight - 1], reinterpret_cast<TInt>(root) - reinterpret_cast<TInt>(this));
            }
        else
            {
            iHeap->Free(root);
            }
        }
    else
        {
        TAny* root = PtrAdd(this, offset);
        err = SetAt(aKey, aLeaf, aHeight, root, 1);
        }
    return err;
    }

TInt RHexTreeBase::SetAt(TUint aKey, TAny* aLeaf, TInt aHeight, TAny* aNode, TInt aLevel)
    {
    TInt err = KErrNone;
    TInt branch = (aKey >> ((aHeight - aLevel) << 2)) & 0xF;
    if (aLevel == 1 && aHeight > 1)
        {
        --branch;
        }
    TInt offset = static_cast<TInt*>(aNode)[branch];
    if (aLevel == aHeight)
        {
        if (offset == 0)
            {
            __e32_atomic_store_rel32(&static_cast<TInt*>(aNode)[branch], reinterpret_cast<TInt>(aLeaf) - reinterpret_cast<TInt>(aNode));
            }
        else
            {
            err = KErrAlreadyExists;
            }
        }
    else if (offset == 0)
        {
        TAny* newNode = iHeap->AllocZ(sizeof(TInt) * 16);
        if (!newNode)
            {
            return KErrNoMemory;
            }
        err = SetAt(aKey, aLeaf, aHeight, newNode, aLevel + 1);
        if (err == KErrNone)
            {
            __e32_atomic_store_rel32(&static_cast<TInt*>(aNode)[branch], reinterpret_cast<TInt>(newNode) - reinterpret_cast<TInt>(aNode));
            }
        else
            {
            iHeap->Free(newNode);
            }
        }
    else
        {
        TAny* nextNode = PtrAdd(aNode, offset);
        err = SetAt(aKey, aLeaf, aHeight, nextNode, aLevel + 1);
        }
    return err;
    }

TAny* RHexTreeBase::At(TUint aKey, TInt aHeight) const
    {
    TInt offset = __e32_atomic_load_acq32(&iRootOffsets[aHeight - 1]);
    if (offset == 0)
        {
        return NULL;
        }
    const TAny* node = PtrAdd(this, offset);
    for (TInt level = 1; level <= aHeight; ++level)
        {
        TInt branch = (aKey >> ((aHeight - level) << 2)) & 0xF;
        if (level == 1 && aHeight > 1)
            {
            --branch;
            }
        offset = __e32_atomic_load_acq32(&static_cast<const TInt*>(node)[branch]);
        if (offset == 0)
            {
            return NULL;
            }
        node = PtrAdd(node, offset);
        }
    return const_cast<TAny*>(node);
    }

void RHexTreeBase::ResetAndDestroy(TInt aHeight, TAny* aNode, TInt aLevel)
    {
    TInt maxNumBranches = (aLevel == 1 && aHeight > 1 ? 15 : 16);
    for (TInt branch = 0; branch < maxNumBranches; ++branch)
        {
        TInt offset = static_cast<TInt*>(aNode)[branch];
        if (offset != 0)
            {
            TAny* nextNode = PtrAdd(aNode, offset);
            if (aLevel == aHeight)
                {
                iHeap->Free(nextNode);
                }
            else
                {
                ResetAndDestroy(aHeight, nextNode, aLevel + 1);
                }
            }
        }
    iHeap->Free(aNode);
    }
