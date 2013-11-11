// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32def.h>
#include <gdi.h>
#include <graphics/gdi/gdistructs.h>
#include <graphics/gdi/gdiconsts.h>
#include <graphics/fbsglyphmetricsarray.h>
#include "UTILS.H"
#include "FbsMessage.h"

// 'most significant bit' flag to ensure value is interpreted as a glyph code rather than an ascii code
const TUint KGlyphCodeFlag = 0x80000000;      

extern void Panic(TFbsPanic aPanic);

/** Helper function for converting an offset (that was calculated using
PointerToOffset()) back to a pointer relative to the passed heap base.
@param aOffset The offset to be converted to a pointer.
@param aHeapBase A pointer to the heap base of the current process.
@return A pointer relative to the passed heap base.
*/
static TAny* OffsetToPointer(const TInt aOffset, TUint8* aHeapBase)
    {
    if ( (aOffset != 0) && (aHeapBase != NULL) )
        {
        return (TAny*)(aOffset + aHeapBase);
        }
    return NULL;
    }

/**
Constructs an empty RFbsGlyphMetricsArray instance. This will not allocate any memory.
 */
EXPORT_C RFbsGlyphMetricsArray::RFbsGlyphMetricsArray() :
    iGlyphCodes(NULL)
    {
    }

/**
Allocates the memory for the array if it has not already been allocated, and 
populates the RFbsGlyphMetricsArray with the metrics information for aCount glyph 
codes passed in as the array aGlyphCodes, for the font aFont. If the operation 
is successful, KErrNone is returned and the array is populated with glyph 
metrics data. Each entry in the array will be in the same order as the 
corresponding codes in aGlyphCodes.

The memory allocated to aGlyphCodes can be freed after the call to Get().

Get() can be called on an RFbsGlyphMetricsArray multiple times without calling 
Close(), since the memory for the array is not de-allocated until a call to 
Close(). Calls to Get() will cause the previous content of the array to be 
overwritten.

In the event of an error code other than KErrNone, the state of the array
will remain unchanged.
 
@param aFont A font which to retrieve the glyph metrics for.
@param aGlyphCodes An array of glyph codes to retrieve the metrics for.
@param aCount The number of glyph codes in aGlyphCodes.

@return
	KErrNone, if the array is successfully populated with glyph metrics;
	KErrNoMemory, if insufficient system memory is available;
	KErrArgument, if aCount is negative or zero, or if aGlyphCodes is null;
	KErrNotSupported, if aFont is a bitmap font.
*/
EXPORT_C TInt RFbsGlyphMetricsArray::Get(CFbsFont& aFont, const TUint* aGlyphCodes, TInt aCount)
	{
    if ((aCount <= 0) || !aGlyphCodes)
        {
        return KErrArgument;
        }
    if (iMetrics.Reserve(aCount) != KErrNone)
        {
        return KErrNoMemory;
        }
    if (!aFont.Address()->IsOpenFont())
        {
        return KErrNotSupported;
        }
    
    iGlyphCodes = aGlyphCodes;
    iCount = aCount;
    TInt err = KErrNone;
    
    
    // If iMetrics array already has a count greater than aCount, remove entries
    // until count is same as aCount so that we can reuse the existing entries.
    TInt numEntriesToRemove = iMetrics.Count() - aCount;
    while (0 < numEntriesToRemove)
        {
        --numEntriesToRemove;
        iMetrics.Remove(aCount + numEntriesToRemove);
        }
    const TInt indexToGrowArrayAt = iMetrics.Count();
    
    CBitmapFont* font = aFont.Address();
    TUint ipcGlyphArrayIndex[KMaxMetricsBatchSize];
    TInt ipcGlyphArrayIndexCount = 0;   
    const TUint8* dummyBitmap;
    TOpenFontCharMetrics charDataMetrics;
            
    for (TInt i = 0; i < aCount && (err == KErrNone); ++i)
        {
        // First check the cache in shared memory - if present it will avoid using IPC.
        if (font->GetCharacterData(aFont.iFbs->ServerSessionHandle(), aGlyphCodes[i] | KGlyphCodeFlag, charDataMetrics, dummyBitmap))
            {
            if (i < indexToGrowArrayAt)
                {
                iMetrics[i] = charDataMetrics;
                }
            else
                {
                // Extending the size of the array, but memory is already reserved.
                (void) iMetrics.Append(charDataMetrics);    
                }
            }            
        else
            {
            // Not found in shared memory - instead add the index to index array, which will
            // be processed when the array is full or at the end of the loop.
            ipcGlyphArrayIndex[ipcGlyphArrayIndexCount++] = i;
            if (ipcGlyphArrayIndexCount == KMaxMetricsBatchSize)
                {
                err = SendRecvGlyphMetrics(aFont, ipcGlyphArrayIndex, ipcGlyphArrayIndexCount, &iMetrics);
                ipcGlyphArrayIndexCount = 0;
                }
            else if (i >= indexToGrowArrayAt)
                {
                // Add a metrics placeholder to keep the size of the array and the currently
                // processed glyph in sync. It will later get overwritten when it is received
                // from the server.
                (void) iMetrics.Append(charDataMetrics);
                }
            }
        }
    if ((err == KErrNone) && (ipcGlyphArrayIndexCount != 0))
        {
        err = SendRecvGlyphMetrics(aFont, ipcGlyphArrayIndex, ipcGlyphArrayIndexCount, &iMetrics);
        }
    
    __ASSERT_DEBUG((err != KErrNone) || (aCount == iMetrics.Count()), Panic(EFbsPanicGlyphMetricsArrayInvalidState));  
    
    return err;    
	}
/**
Helper function for Get(). 
Given a list of indices into a glyph code array, the corresponding glyph
codes are made into a single list sent to the server, and the received glyph
metrics are set in the array of metrics at the corresponding indices.

@param aFont The font to receive the glyph metrics of.
@param aArrayIndices An array of indices into the glyphcode array which
    will be sent for requesting of metrics to the server.
@param aArrayIndicesCount The number of glyphs in aGlyphArrayIndices.
@param aMetrics The array which will store the resulting metrics objects upon
    completion.
@return KErrNone if successful, otherwise one of the system-wide error codes. 

@panic FBSCLI 39 in debug builds only, if the parameters to this method are
    invalid, or if the output array is of the wrong size when appending to it.
 */
TInt RFbsGlyphMetricsArray::SendRecvGlyphMetrics(CFbsFont& aFont, TUint* aArrayIndices, TInt aArrayIndicesCount, RArray<TOpenFontCharMetrics>* aMetrics) const
    {
    __ASSERT_DEBUG(aArrayIndicesCount > 0, Panic(EFbsPanicGlyphDataIteratorInvalidState)); 
    __ASSERT_DEBUG(aArrayIndicesCount <= KMaxMetricsBatchSize, Panic(EFbsPanicGlyphDataIteratorInvalidState)); 
    __ASSERT_DEBUG(aArrayIndices, Panic(EFbsPanicGlyphDataIteratorInvalidState));
    TInt err = KErrNone;
    
    TUint glyphCodes[KMaxMetricsBatchSize];
    for (TInt i = 0; i < aArrayIndicesCount; ++i)
        {
        glyphCodes[i] = iGlyphCodes[aArrayIndices[i]];
        }
    
    TInt rcvdGlyphMetricsOffsets[KMaxMetricsBatchSize];
    TPckg<TUint[KMaxMetricsBatchSize]> argGlyphCodes(glyphCodes);
    TPckg<TInt[KMaxMetricsBatchSize]> argGlyphMetricsOffsets(rcvdGlyphMetricsOffsets);
    if (aArrayIndicesCount < KMaxMetricsBatchSize)
        {
        argGlyphCodes.SetLength(aArrayIndicesCount * sizeof(TUint));
        argGlyphMetricsOffsets.SetLength(aArrayIndicesCount * sizeof(TInt));
        }   
    err = aFont.iFbs->SendCommand(EFbsMessGetGlyphMetrics, TIpcArgs(aFont.iHandle, &argGlyphCodes, &argGlyphMetricsOffsets));
    
    if (err == KErrNone)
        {
        TInt numRcvdMetrics = argGlyphMetricsOffsets.Length() / sizeof(TInt);
        __ASSERT_DEBUG(argGlyphMetricsOffsets.Length() % sizeof(TInt) == 0, Panic(EFbsPanicGlyphMetricsArrayInvalidState));
        __ASSERT_DEBUG(numRcvdMetrics == aArrayIndicesCount, Panic(EFbsPanicGlyphMetricsArrayInvalidState));
        
        if (numRcvdMetrics == aArrayIndicesCount)
            {
            TInt arrayCount = aMetrics->Count();
            TUint8* heapBase = aFont.iFbs->HeapBase();
            for (TInt rcvdMetricsItem = 0; rcvdMetricsItem < numRcvdMetrics; ++rcvdMetricsItem)
                {
                TInt arrayIndex = aArrayIndices[rcvdMetricsItem];
                // The array should never need to grow more than one item. If the difference is larger, 
                // it means the glyph and the metrics are not in sync.
                __ASSERT_DEBUG(arrayIndex <= arrayCount, Panic(EFbsPanicGlyphMetricsArrayInvalidState));
                
                TInt metricsOffset = rcvdGlyphMetricsOffsets[rcvdMetricsItem];              
                const TOpenFontCharMetrics* metrics = (const TOpenFontCharMetrics*)(OffsetToPointer(metricsOffset, heapBase));
                
                if (arrayIndex < arrayCount)
                    {
                    // Copy metrics into existing element
                    (*aMetrics)[arrayIndex] = *metrics;
                    }
                else if (arrayIndex == arrayCount)
                    {
                    // Memory should already be reserved by GetGlyphMetricsArray()
                    (void) aMetrics->Append(*metrics);    
                    ++arrayCount;
                    }
                }
            }
        else
            {
            // did not receive the same number of glyphs as was asked.
            err = KErrGeneral;
            }
        }
    return err;
    }

/**
Closes the array, and releases the memory for the array. Calling Close() on an 
already closed RFbsGlyphMetricsArray has no effect. 

In the typical case where the array is a member of a class, Close() should only 
be called in the destructor of that class.
 */
EXPORT_C void RFbsGlyphMetricsArray::Close()
	{
	iMetrics.Close();
	}

/**
Retrieves the glyph metrics for the glyph which was at position aIndex in the 
array passed to Get().

@param aIndex The index of the entry in the array to access.
@return The metrics for the glyph at the requested index.
@panic FBSCLI 32, if aIndex is out of bounds.
 */
EXPORT_C const TOpenFontCharMetrics& RFbsGlyphMetricsArray::operator[](TInt aIndex) const
	{
	__ASSERT_ALWAYS(aIndex >= 0 && aIndex < iMetrics.Count(), Panic(EFbsPanicGlyphMetricsArrayOutOfBounds));
	return (iMetrics)[aIndex];
	}

/**
@return The number of glyph metrics held in the array.
 */
EXPORT_C TInt RFbsGlyphMetricsArray::Count() const
	{
	return iMetrics.Count();
	}
