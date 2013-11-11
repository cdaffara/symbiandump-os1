// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "scrdevextension.h"
#include "../SERVER/w32cmd.h"
#include <graphics/displaycontrolbase.h>
#include "CLIENT.H"
#include "w32comm.h"

CWsScreenDevice::CScrDevExtension::CScrDevExtension(RWsBuffer *aBuffer,TInt32 aWsHandle):	MWsClientClass(aBuffer)
	{
	iWsHandle=aWsHandle;
	__ASSERT_DEBUG(aBuffer,Panic(EW32PanicBadClientInterface));
	__ASSERT_DEBUG(aWsHandle,Panic(EW32PanicBadClientInterface));
	}

CWsScreenDevice::CScrDevExtension::~CScrDevExtension()
	{
	//typeface store is not owned by this class, and is created/destroyed in CWsScreenDevice
	}
/** Interface Extension capability
 * 	Use of this interface going forward will allow the published client interface to be dynamically extended.
 * 	Note that the pointer returned is only good for the lifetime of the called CBase derived object.
 * 	@pre	caller has already checked that the implementation is initialised using RepeatableConstruct
 *	@param  aInterfaceId	uniqueid or well known id of interface
 * 	@return	pointer to interface object matching this ID or NULL if no match.
 **/
void* CWsScreenDevice::CScrDevExtension::GetInterface(TUint aInterfaceId)
	{
	__ASSERT_DEBUG(this!=NULL,Panic(EW32PanicBadClientInterface));
	__ASSERT_DEBUG(iBuffer,Panic(EW32PanicBadClientInterface));
	__ASSERT_DEBUG(iWsHandle!=NULL,Panic(EW32PanicBadClientInterface));
	
	switch (aInterfaceId)
		{
		case MTestScreenCapture::ETypeId:
			{
				return static_cast<MTestScreenCapture*>(this);
			}
		default:
			break;
		}
	return NULL;
}
//Accessor to typeface store instance
CFbsTypefaceStore* CWsScreenDevice::CScrDevExtension::TypefaceStore()
	{
	return iTypefaceStore;
	}
//Accessor to typeface store instance
void  CWsScreenDevice::CScrDevExtension::SetTypefaceStore(CFbsTypefaceStore* aTypeFaceStore)
	{
	iTypefaceStore=aTypeFaceStore;
	}


TInt CWsScreenDevice::CScrDevExtension::TranslateExtent(const TRect& aInitial, TRect& aTarget) const
	{
  	// Current screen mode scale
	TPckgBuf<TSize> pntPkg;
	TInt err = WriteReplyP(&pntPkg,EWsSdOpGetCurrentScreenModeScale);
	TSize scale = pntPkg();	
	
	// Current screen mode offset
	TPckgBuf<TPoint> soPkg;
	err = WriteReplyP(&soPkg,EWsSdOpGetCurrentScreenModeScaledOrigin);
	TPoint offset = soPkg();
	
	// Calculate the top left point taking into account scale and offset
	TInt startingPointWidth = (scale.iWidth * aInitial.iTl.iX) + offset.iX;
	TInt startingPointHeight = (scale.iHeight * aInitial.iTl.iY) + offset.iY;	
	TPoint targetXY(startingPointWidth,startingPointHeight);
	
	// Calculate the extent size taking into account scale
	TInt targetWidth = (scale.iWidth * aInitial.Width());
	TInt targetHeight = (scale.iHeight * aInitial.Height());
	TSize targetSize(targetWidth,targetHeight);
	
	aTarget.SetRect(targetXY,targetSize);
	return err;
	}


/** Gets the size of the screen device area in pixels.

This function always causes a flush of the window server buffer.

@return The x and y dimensions of the screen device area, in pixels. 
@see CGraphicsDevice::SizeInPixels() */
TInt CWsScreenDevice::CScrDevExtension::GetCompositedSize(TSize& aSize) const
	{
	TPckgBuf<TSize> sizePkg;
  	TInt err = WriteReplyP(&sizePkg,EWsSdOpPixelSize);
  	aSize = sizePkg();
	return err;
	}



/** Saves the entire screen to a bitmap.

This function always causes a flush of the window server buffer.

@param aBitmap Bitmap to be filled with the screen image. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
TInt CWsScreenDevice::CScrDevExtension::ComposeScreen(const CFbsBitmap& aBitmap) const
	{
	TInt bitmapHandle = aBitmap.Handle();
	AddToBitmapArray(bitmapHandle);
	TWsSdCmdCopyScreenToBitmap rectCompare(bitmapHandle);
	return(WriteReply(&rectCompare,sizeof(rectCompare),EWsSdOpCopyScreenToBitmap));
	}
