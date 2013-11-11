// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CDSB_H__
#define __CDSB_H__

#include <e32base.h>
#include <bitdev.h>

/**
Direct Screen Bitmap.
API to allow provide synchronisation of the display of images with the
displays refresh to prevent tearing.
 
This is an abstract base class, so must be derived from on a per-variant basis.

@publishedPartner
@released

Note: this class is likely to be deprecated in the future
*/ 
class CDirectScreenBitmap : public CBase
	{
public:
	enum TSettingsFlags
		{
		ENone = 0,
		EDoubleBuffer = 1,
		EIncrementalUpdate = 2
		};

public:
	/**
	Constructs a CDirectScreenBitmap derived object.
    The default screen (with number 0) will be used.
    @return A pointer to the created CDirectScreenBitmap object
    @leave KErrNoMemory There was insufficient memory to allocate the CDirectScreenBitmap derived object
	*/
	IMPORT_C static CDirectScreenBitmap* NewL();	

	/**
	Constructs a CDirectScreenBitmap derived object.
	@param aScreenNo Screen number, used by the CDirectScreenBitmap object.
    @return A pointer to the created CDirectScreenBitmap object
    @leave KErrNoMemory There was insufficient memory to allocate the CDirectScreenBitmap derived object
	*/
	IMPORT_C static CDirectScreenBitmap* NewL(TInt aScreenNo);	

    /**
	Creates a CDirectScreenBitmap object which can be used for drawing to
	a region of the screen indicated by aScreenRect.  This region must have
	previously been 'claimed' via the Window Servers Direct Screen Access API.

	@param aScreenRect The region to be displayed
	@param aSettingsFlags The mode of operation. The upper 3 bits are used 
                          for the screen number value: 0..7.  By default the screen 
                          with  number 0 will be used.
	@return KErrNone if successful
	        KErrNoMemory if there was insufficient memory
	        KErrNotSupported if the creation failed for other reasons
	*/
	virtual TInt Create(const TRect& aScreenRect, TSettingsFlags aSettingsFlags) =0;

	/**
	Returns a TAcceleratedBitmapInfo referring to a bitmap which the
	applicationcan render to.

	@param aBitmapInfo The Bitmap
	@return KErrNone if successful, another error code otherwise
	*/
	virtual TInt BeginUpdate(TAcceleratedBitmapInfo& aBitmapInfo) =0;

	/**
	Indicates to the Video Driver that the bitmap corresponding to the
	update has been fully rendered.  The video driver will perform
	the actions required to copy this to the frame buffer.

	The request status aComplete will be signalled when the copying has completed.
	
	@param aComplete Asynchronous completion status
	*/
	virtual void EndUpdate(TRequestStatus& aComplete) =0;

	/**
	Indicates to the Video Driver that the area indicated in aScreenRect has
	been fully rendered.  The video driver will perform the actions required 
	to copy this to the frame buffer.
	
	The request status aComplete will be signalled when the copying has completed.
	
	Note: aScreenRects coordinates are relative to the screen, not the update region specified
	in Create().  aScreenRect must fit entirely within the bounds of the original region passed
	to Create().

	@param aScreenRect The region to update
	@param aComplete Asynchronous completion status
	*/
	virtual void EndUpdate(const TRect& aScreenRect, TRequestStatus& aComplete) =0;

	/**
	Deletes all resources associated with the CDirectScreenBitmap object.
	*/
	virtual void Close() =0;
	};

#endif
