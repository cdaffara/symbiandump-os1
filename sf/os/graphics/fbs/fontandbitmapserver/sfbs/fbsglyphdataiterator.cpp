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
#include <graphics/fbsglyphdataiterator.h>
#include "FbsMessage.h"
#include "UTILS.H"

const TInt KFbsGlyphDataIterCodeInvalid = -1;

extern void Panic(TFbsPanic aPanic);

/**
The default constructor sets the iterator to a closed and empty state. It 
is the only way of constructing an iterator as instances cannot be copied by 
assignment or passed by value.
 */
EXPORT_C RFbsGlyphDataIterator::RFbsGlyphDataIterator() :
    iImpl(NULL)
    {
    }

/**
For a given font (aFont), this method retrieves the glyph data for a list of 
glyph codes (aGlyphCodes), containing a number (aCount) of codes. On success, 
the iterator is initialised with the data for the first glyph in aGlyphCodes.

The memory allocated to aGlyphCodes must not be freed or altered while the 
iterator is in use (i.e. until the iterator has been closed). 

Open() may not be called on an already open iterator. In order to re-open an 
iterator, it must first be closed by a call tor Close().

If a glyph code is passed in that is not a recognised glyph code for the 
associated font, an empty-box glyph will be returned. This behaviour is 
consistent with CFbsFont::GetCharacterData().

@pre The iterator is not already open. 

@param aFont The font to provide the glyph code data for.
@param aGlyphCodes An array of glyph codes that the iterator will 
	provide data for. This memory allocated for this array must not be 
	freed before the iterator is closed.
@param aCount The number of glyph codes in aGlyphCodes.

@return 
	KErrNone, if the iterator is successfully initialised to retrieve 
		the glyph data;
	KErrInUse, if the iterator is already open, in which case the state of
        the iterator is left unchanged;
	KErrNoMemory, if the iterator cannot be opened due to insufficient 
		system memory;
	KErrNotSupported, if aFont refers to a bitmap font or an outline & shadow
        font, if the required version of the hardware driver is not available
        (EGL 1.3 or later is required), or if RSgImages are not supported by
        the system;
	KErrArgument, if aCount is negative or zero, or if aGlyphCodes is null.
 */
EXPORT_C TInt RFbsGlyphDataIterator::Open(CFbsFont& aFont, const TUint* aGlyphCodes, TInt aCount)
	{  
	if (iImpl)
		{
		return KErrInUse;
		}
	if ((aCount <= 0) || !aGlyphCodes)
		{
		return KErrArgument;
		}
	if (!aFont.Address()->IsOpenFont())
        {
        return KErrNotSupported;
        }
    TInt glyphBitmapType = aFont.Address()->GlyphBitmapType();
    if (!( (glyphBitmapType == EMonochromeGlyphBitmap) || (glyphBitmapType == EAntiAliasedGlyphBitmap) ))
        {
        //Only supported bitmap types can be used i.e. EMonochromeGlyphBitmap or EAntiAliasedGlyphBitmap
        return KErrNotSupported;
        }
    // Check that the max width and height of the font are both no more than 2048.
    // This is the smallest maximum size an RSgImage can be created with.
    // This limit is arbitrarily set as it should cover nearly all use cases.
    const TInt KMaxFontSizeInPixels = 2048;
    TInt maxHeight = aFont.FontMaxHeight();
    TInt maxWidth = aFont.MaxCharWidthInPixels();
    if ( (KMaxFontSizeInPixels < maxHeight) || (KMaxFontSizeInPixels < maxWidth) )
        {
        return KErrTooBig;
        }
    // Construct implementor object that holds the state for the iterator.
	iImpl = new CGlyphDataIteratorImpl(aFont.iHandle, aGlyphCodes, aCount);
	if (!iImpl)
	    {
	    return KErrNoMemory;
	    }
	TInt err = iImpl->Initialise();
	if (err != KErrNone)
	    {
        Close();
	    }
	return err;   
	}

/**
Moves the iterator to the data for the next glyph code in the array passed 
into RFbsGlyphDataIterator::Open(). Data for the glyph can then be accessed 
using the Image(), Rect() and Metrics() methods.

Once Next() has been called, the references returned by Image(), Rect() and 
Metrics() methods during the previous iteration should be considered invalid 
and must be discarded. 

Calling Next() repeatedly will iterate through the glyph data for all the glyph
codes, until it has reached the last glyph code in the array (assuming no errors
are encountered), at which point KErrNotFound is returned, and the array of glyph
codes passed to RFbsGlyphDataIterator::Open() can safely be deleted.

If the call was successful, KErrNone is returned. If an error is encountered an
error code will be returned and the state of the iterator is left unchanged.

@pre The iterator has been opened by a successful call to Open().
@post The properties of the iterator are of the glyph corresponding
	to the next code passed in the array to Open().	However, if an error code 
	was returned, the state of the iterator	is unchanged.
	
@return 
	KErrNone, if the iterator was successfully advanced;
	KErrNotFound, if the iterator is already at the last element and cannot
		be advanced any further;
	KErrNoMemory, if there is insufficient system memory available;
	KErrNoGraphicsMemory, if there is insufficient graphics memory available.
	
@panic FBSCLI 31, if the iterator is not open.
@panic FBSERV -8, if as a result of this call, communication with the 
    server is invoked, and the associated CFbsFont has been destroyed.
 */
EXPORT_C TInt RFbsGlyphDataIterator::Next()
    {
    __ASSERT_ALWAYS(iImpl, Panic(EFbsPanicGlyphDataIteratorClosed));
    return iImpl->Next();
    }

/**
Closes the iterator and releases its internal resources. After calling, all data 
retrieved by the iterator is no longer safe to use. Once closed, this iterator 
can be re-opened. Calling Close() on an already closed iterator has no effect.

Once an iterator is closed, the array of glyphs (aGlyphCodes) passed to
RFbsGlyphDataIterator::Open() can safely be deleted.

@post The iterator is closed.
 */
EXPORT_C void RFbsGlyphDataIterator::Close()
    {
    delete iImpl;
    iImpl = NULL;
    }

/**
Returns a reference to the RSgImage that contains the glyph for the current 
iteration. The image representation of the glyph is the same as the image 
returned by the existing CFbsFont::GetCharacterData() method (i.e. an alpha mask). 

The RSgImage should only be considered a temporary handle for use in this 
iteration, and should not be used after a call to Next() or Close() has 
been made.

Note: For glyphs such as space which have no visible representation, Image() 
will return a null image handle (i.e. RSgImage::IsNull() returns ETrue). This 
cannot be used for drawing. In this case Rect() will be empty however 
Metrics() will still be valid. 

@pre The iterator has been initialised by successfully calling Open().
     
@return A handle to the image where the glyph for this iteration is stored.
     
@panic FBSCLI 31, if the iterator is not open.
 */
EXPORT_C const RSgImage& RFbsGlyphDataIterator::Image() const
    {
    __ASSERT_ALWAYS(iImpl, Panic(EFbsPanicGlyphDataIteratorClosed));
    __ASSERT_DEBUG(!iImpl->iGlyphBatch.IsEmpty(), Panic(EFbsPanicGlyphDataIteratorInvalidState));
    return iImpl->iGlyphBatch.First()->iImage;
    }

/**
Returns the area within the RSgImage where the glyph for the current
iteration is located. The reference returned by Rect() should be considered 
temporary for use within this iteration and should not be used after a call to 
Next() or Close() has been made.
 
@pre The iterator has been initialised by successfully calling Open().
     
@return A rectangle representing the position and size in pixels, 
	of the glyph for this iteration on the RSgImage provided by Image().
     
@panic FBSCLI 31, if the iterator is not open.
 */
EXPORT_C const TRect& RFbsGlyphDataIterator::Rect() const
    {
    __ASSERT_ALWAYS(iImpl, Panic(EFbsPanicGlyphDataIteratorClosed));
    __ASSERT_DEBUG(!iImpl->iGlyphBatch.IsEmpty(), Panic(EFbsPanicGlyphDataIteratorInvalidState));
    return iImpl->iGlyphDataIterRect;
    }

/**
Returns the glyph metrics for the current iteration. The reference returned by 
Metrics() should be considered temporary for use within this iteration and 
should not be used after a call to Next() or Close() has been made.

@pre The iterator has been initialised by successfully calling Open().
 
@return The metrics for the glyph at the current iteration.

@panic FBSCLI 31, if the iterator is not open.
 */
EXPORT_C const TOpenFontCharMetrics& RFbsGlyphDataIterator::Metrics() const
    {
    __ASSERT_ALWAYS(iImpl, Panic(EFbsPanicGlyphDataIteratorClosed));
    __ASSERT_DEBUG(!iImpl->iGlyphBatch.IsEmpty(), Panic(EFbsPanicGlyphDataIteratorInvalidState));
    return iImpl->iGlyphBatch.First()->iInfo.iMetrics;
    }

/**
Returns the glyph code associated with the data for the current iteration.

@pre The iterator has been initialised by successfully calling Open().
 
@return The glyph code of the glyph at the current iteration.

@panic FBSCLI 31, if the iterator is not open.
 */
EXPORT_C TUint RFbsGlyphDataIterator::GlyphCode() const
    {
    __ASSERT_ALWAYS(iImpl, Panic(EFbsPanicGlyphDataIteratorClosed));
    __ASSERT_DEBUG(!iImpl->iGlyphBatch.IsEmpty(), Panic(EFbsPanicGlyphDataIteratorInvalidState));
    return iImpl->iGlyphDataIterCodes[iImpl->iGlyphDataIterCodeIndex];
    }


/**
Constructs a CGlyphDataIteratorImpl. 
@param aFbsFontHandle The handle of the FbsFont that the iterator is working with.
@param aGlyphCodes The array of glyph codes sent to RFbsGlyphDataIterator::Open()
@param aCount The number of glyph codes in aGlyphCodes.
 */
CGlyphDataIteratorImpl::CGlyphDataIteratorImpl(TInt aFbsFontHandle, const TUint* aGlyphCodes, TInt aCount) :
    iGlyphBatch(_FOFF(TGlyphBatchItem, iLink)),
    iGlyphDataIterCodes(aGlyphCodes),
    iGlyphDataIterCodeCount(aCount),
    iGlyphDataIterCodeIndex(KFbsGlyphDataIterCodeInvalid),
    iFbsFontHandle(aFbsFontHandle)
    {
    }

/** 
Destructor. Releases all resources, disconnects from server and frees any
items in the list of batched items.
 */
CGlyphDataIteratorImpl::~CGlyphDataIteratorImpl()
    {
    if (iFbs)
        {
        if (iGlyphDataIterCodeIndex != KFbsGlyphDataIterCodeInvalid)
            {
            //Send the No-Op command to ensure that the "In Transit" RSgImage(s) are closed.
            iFbs->SendCommand(EFbsMessNoOp);
            }
        RFbsSession::Disconnect();
        iFbs = NULL;
        }
    while (!iGlyphBatch.IsEmpty())
        {
        TGlyphBatchItem* item = iGlyphBatch.First();
        item->iImage.Close();
        iGlyphBatch.Remove(*item);
        delete item;
        }
    iGlyphBatch.Reset();
    }

/**
Sets up the CGlyphDataIteratorImpl, populating the first batch of glyphs.
Should only be called once, immediately after construction.
 */
TInt CGlyphDataIteratorImpl::Initialise()
    {
    __ASSERT_DEBUG(iFbsFontHandle, Panic(EFbsPanicGlyphDataIteratorInvalidState));    
    __ASSERT_DEBUG(iGlyphDataIterCodes, Panic(EFbsPanicGlyphDataIteratorInvalidState));
    __ASSERT_DEBUG(iGlyphDataIterCodeCount, Panic(EFbsPanicGlyphDataIteratorInvalidState));
    __ASSERT_DEBUG(iGlyphDataIterCodeIndex == KFbsGlyphDataIterCodeInvalid, Panic(EFbsPanicGlyphDataIteratorInvalidState));
    
    // If the client already has a session open, this is just a reference counting exercise and should incur no performance impact.
    TInt err = RFbsSession::Connect();
    if (err == KErrNone)
        {
        iFbs = RFbsSession::GetSession();
        err = UpdateGlyphBatch(0);
        }
    if (err == KErrNone)
        {
        iGlyphDataIterCodeIndex = 0;
        UpdateGlyphRect();
        }
    return err;
    }

/**
Increments the current iteration if possible, re-sending the request
for more glyphs if the current batch of glyphs is down to the last
item.
@see RFbsGlyphDataIterator::Next()
 */
TInt CGlyphDataIteratorImpl::Next()
    {
    __ASSERT_DEBUG(!iGlyphBatch.IsEmpty(), Panic(EFbsPanicGlyphDataIteratorInvalidState));
    if ( (iGlyphDataIterCodeIndex + 1) >= iGlyphDataIterCodeCount) 
        {
        return KErrNotFound; 
        }
    TInt err = UpdateGlyphBatch(iGlyphDataIterCodeIndex + 1);
    if (err == KErrNone)
        {
        ++iGlyphDataIterCodeIndex;
        // Close the current image and pop the head of the batch.
        TGlyphBatchItem* item = iGlyphBatch.First();
        item->iImage.Close();
        iGlyphBatch.Remove(*item);
        delete item;
        __ASSERT_DEBUG(!iGlyphBatch.IsEmpty(), Panic(EFbsPanicGlyphDataIteratorInvalidState));
        UpdateGlyphRect();
        }
    return err;
    }

/**
Checks whether a call to the server is required to get a new batch of glyph 
info, and processes the response from the server as necessary.

@param aIndex Specifies the index into the glyph array which needs to be in
the active glyph batch. If it is not there, a request is made to the server
to get it.
@return KErrNone if getting at least one glyph succeeded or a call to the
    server was not necessary, otherwise one of the system wide error codes.
@panic FBSCLI 31 (debug only), if the iterator is not open
@panic FBSCLI 33 (debug only), if an unexpected number of glyphs was received
    as a result of requesting glyphs from the server, or if the current batch
    of glyphs is empty when there should be at least one item.
 */
TInt CGlyphDataIteratorImpl::UpdateGlyphBatch(TInt aIndex)
    {
    __ASSERT_DEBUG(Rng(0, aIndex, iGlyphDataIterCodeCount - 1), Panic(EFbsPanicGlyphDataIteratorIndexOutOfRange));

    TInt err = KErrNone;
    
    TBool needMoreGlyphs = EFalse;
    if (iGlyphBatch.IsEmpty())
        {
        // Current batch is empty, must request more. Should only get here when the iterator 
        // is first opened, since one item should always be in the list from then on.
        __ASSERT_DEBUG(aIndex == 0, Panic(EFbsPanicGlyphDataIteratorInvalidState));
        needMoreGlyphs = ETrue;
        }
    else if (iGlyphBatch.IsLast(iGlyphBatch.First()))
        {
        // Only one item in the list. 
        needMoreGlyphs = ETrue;
        }
    
    if (needMoreGlyphs)
        {
        // If the array of batched images is empty OR only one left, means we need to request a new batch.
        // We make sure there is at least one glyph in the batch so the iterator is always usable even
        // when a failure to move to the next iteration occurs.
    
        TBool glyphAddedToBatch = EFalse;
        TUint glyphCodes[KMaxGlyphBatchSize];
        
        TInt numGlyphsToRequest = Min(iGlyphDataIterCodeCount - aIndex, KMaxGlyphBatchSize);        
        (void)Mem::Copy(glyphCodes, &(iGlyphDataIterCodes[aIndex]), sizeof(TUint) * numGlyphsToRequest);
        TPckg<TUint[KMaxGlyphBatchSize]> argGlyphCodes(glyphCodes);
        
        TGlyphImageInfo rcvdGlyphInfo[KMaxGlyphBatchSize];
        TPckg<TGlyphImageInfo[KMaxGlyphBatchSize]> argGlyphInfo(rcvdGlyphInfo);
        
        if (numGlyphsToRequest < KMaxGlyphBatchSize)
            {
            argGlyphCodes.SetLength(numGlyphsToRequest * sizeof(TUint));
            argGlyphInfo.SetLength(numGlyphsToRequest * sizeof(TGlyphImageInfo));
            }
        
        err = iFbs->SendCommand(EFbsMessGetGlyphs, TIpcArgs(iFbsFontHandle, &argGlyphCodes, &argGlyphInfo));
        if (err == KErrNone)
            {
            __ASSERT_DEBUG(argGlyphInfo.Length() % sizeof(TGlyphImageInfo) == 0, Panic(EFbsPanicGlyphDataIteratorInvalidState));
            TInt numRcvdGlyphs = argGlyphInfo.Length() / sizeof(TGlyphImageInfo);
            __ASSERT_DEBUG(numRcvdGlyphs > 0, Panic(EFbsPanicGlyphDataIteratorInvalidState));
            __ASSERT_DEBUG(numRcvdGlyphs <= KMaxGlyphBatchSize, Panic(EFbsPanicGlyphDataIteratorInvalidState));
            
            // Store the received glyph data, and open the image handles so that the IDs
            // will not be released by FbServ between now and the client using them.
            // If a failure occurs while processing one of the recevied glyphs,
            // abort the rest but keep the ones that succeeded.
            for (TInt i = 0; (i < numRcvdGlyphs) && (err == KErrNone); ++i)
                {
                TGlyphBatchItem* glyphEntry = new TGlyphBatchItem;
                if (!glyphEntry)
                    {
                    err = KErrNoMemory;
                    }
                else
                    {
                    glyphEntry->iInfo = rcvdGlyphInfo[i];
                    
                    RSgImage glyphImage;
                    if (rcvdGlyphInfo[i].iImageId != KSgNullDrawableId)
                        {
                        err = glyphEntry->iImage.Open(rcvdGlyphInfo[i].iImageId);
                        }
                    if (err == KErrNone)
                        {
                        iGlyphBatch.AddLast(*glyphEntry);
                        glyphAddedToBatch = ETrue;
                        }
                    else
                        {
                        delete glyphEntry;
                        }
                    }
                }
            }
        if (err != KErrNone && glyphAddedToBatch)
            {
            // There was an error adding an item to the batch. Rather than return the
            // error to the client, ignore it and use what glyphs we successfully batched.
            err = KErrNone; 
            }
        }    
    return err;
    }

/**
Updates the glyph rectangle member based on the current glyph metrics.
@post The iGlyphDataIterRect member is updated to reflect the position
    and size of the currently active glyph.
 */
void CGlyphDataIteratorImpl::UpdateGlyphRect()
    {
    iGlyphDataIterRect.iTl = TPoint(iGlyphBatch.First()->iInfo.iPosX, iGlyphBatch.First()->iInfo.iPosY);
    iGlyphDataIterRect.SetSize(TSize(iGlyphBatch.First()->iInfo.iMetrics.Width(), iGlyphBatch.First()->iInfo.iMetrics.Height()));
    }
