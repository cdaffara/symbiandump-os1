// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This module implements the templated classes used to define screen display instances.
// It only needs to be #included into scnew.cpp whisch is the only place the full-qualified templates are instanced.
// Most of these methods are simply shims to the SurfaceHelper class.
// 
//

/**
 @file
*/
#ifndef __SCDRAW_INL__
#define __SCDRAW_INL__

#include "scdraw.h"	//should have already happened.

//
// Implementation of  template CGenericScreenDevice
// Arguments  <class T>
// Defines the common portions of a screendriver for GCE 
//	the actual mode and the bits per pixel (or pixels per word) is not specified.
//


/**	Nothing to do to init the screen for the display modes currently supported.
 * This is the place to set up the palette for low-colour screen devices, possibly including 12-bit.
 **/
template <class T> TInt  CGenericScreenDevice<T>::InitScreen()
	{
	return KErrNone ;
	}

//Construct splits into 2 methods. This portion is not dependent on pixel format
template <class T> TInt  CGenericScreenDevice<T>::ConstructScreen(TInt , TAny *, TSize aSize, TInt )
	{
	CDrawXxxBppBitmap::iScanLineWords = iHelper.BytesPerScanline() / 4;
	if (CDrawXxxBppBitmap::iScanLineWords==0)
		{	//Note: This will cause WServ startup to fail. WServ only accepts KErrNotSupported
		return KErrHardwareNotAvailable;
		}
	TInt ret = CDrawXxxBppBitmap::Construct(aSize, iHelper.BytesPerScanline());
	if (ret == KErrNone)
		{
		CDrawXxxBppBitmap::iBits = (TUint32*)iHelper.AddressFirstPixel();
		if (CDrawXxxBppBitmap::iBits==NULL)
			{	//Note: This will cause WServ startup to fail. WServ only accepts KErrNotSupported
			ret=KErrHardwareNotAvailable;
			}
		}
	return ret;
	}

//Switch from the previous aDrawDevice to this one
template <class T> void CGenericScreenDevice<T>::SetDisplayMode(CFbsDrawDevice* aDrawDevice)
	{
	iHelper.ResetUpdateRegion();

	// Inherit the original draw device's orientation, if available.
	TDeviceOrientation devOrientation = EDeviceOrientationNormal;

	TAny* interface = NULL;
	TInt ret = aDrawDevice->GetInterface(KSurfaceInterfaceID, interface);
	//can't survive setting a different orientation to the source device so must read it successfully
	__ASSERT_ALWAYS(ret == KErrNone,Panic(EScreenDriverPanicIncompatiblePreviousDevice));
	devOrientation = reinterpret_cast<MSurfaceId*>(interface)->DeviceOrientation();
	//SetDeviceOrientation will panic if it is incompatible
	SetDeviceOrientation(devOrientation);

	CDrawXxxBppBitmap::CopyOldSettings(aDrawDevice);
	InitScreen();
	}

template <class T> TInt CGenericScreenDevice<T>::HorzTwipsPerThousandPixels() const
	{
 	return iHelper.HorzTwipsPerThousandPixels(CDrawXxxBppBitmap::iSize);
	}

template <class T> TInt CGenericScreenDevice<T>::VertTwipsPerThousandPixels() const
	{
	return iHelper.VertTwipsPerThousandPixels(CDrawXxxBppBitmap::iSize);
	}

template <class T> void CGenericScreenDevice<T>::Update()
	{
	iHelper.Update();
	}

template <class T> void CGenericScreenDevice<T>::Update(const TRegion& aRegion)
	{
    if(!aRegion.IsEmpty() && !aRegion.CheckError())
        {
        if (aRegion.Count()>KMaxUpdateRegionRectangles)
            {
            UpdateRegion(aRegion.BoundingRect());
            }
        else
            {
            TInt rcCnt = aRegion.Count();
            for (TInt ii=0; ii < rcCnt; ++ii)
                {  
                UpdateRegion(aRegion[ii]);  //Applies deorientate (offset, scale, rotate)
                }
            }
        }
    Update();
	}

template <class T> void CGenericScreenDevice<T>::UpdateRegion(const TRect& aRect)
    {
    const TRect rect = CDrawXxxBppBitmap::DeOrientate(aRect);//rect - physical coordinates
    
    iHelper.UpdateRegion(rect);
    }

template <class T> TInt CGenericScreenDevice<T>::GetInterface(TInt aInterfaceId, TAny*& aInterface)
	{
	if(aInterfaceId == KSurfaceInterfaceID)
	    {
		aInterface = static_cast <MSurfaceId*> (this);
		return KErrNone;
	    }
	else
		{
	    return CDrawXxxBppBitmap::GetInterface(aInterfaceId, aInterface);
		}
	}

template <class T> void CGenericScreenDevice<T>::GetSurface(TSurfaceId& aSid) const 
	{
	iHelper.GetSurface(aSid);
	}


template <class T> TUint CGenericScreenDevice<T>::DeviceOrientationsAvailable() const 
	{
	return iHelper.DeviceOrientationsAvailable(CGenericScreenDevice::iSize);
	}

template <class T> TDeviceOrientation CGenericScreenDevice<T>::DeviceOrientation() const 
	{
	return iHelper.DeviceOrientation();
	}

//
// Implementation of  template CPalettizedScreenDevice
// Arguments  <class T,TDisplayMode displayMode,TInt pixelsPerWord>
// Defines a screendriver for GCE with a mode specified without using a GUID, probably palettized
//
//


//Initialise palletised modes that are not assigned GUIDs
template <class T,TDisplayMode displayMode,TInt pixelsPerWord> 
TInt  CPalettizedScreenDevice<T,displayMode,pixelsPerWord>::ConstructScreen(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize, TInt aHalMode)
	{
	TInt ret = CGenericScreenDevice::iHelper.Construct(aScreenNo, (TUidPixelFormat)0,aHalMode); //displayMode is NOT recorded in surfaceID
	if (ret != KErrNone)
		return ret;
	return  CGenericScreenDevice::ConstructScreen(aScreenNo,aBitmapAddress,aSize,aHalMode);
	}

//Set size members based on pixel width/height, but also set stride member using pixels per word
template <class T,TDisplayMode displayMode,TInt pixelsPerWord> 
void CPalettizedScreenDevice<T,displayMode,pixelsPerWord>::SetSize(const TSize& aSize)
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(CGenericScreenDevice::iSize == aSize, User::Invariant());
	CGenericScreenDevice::iLongWidth = CGenericScreenDevice::iScanLineWords * pixelsPerWord;
	}
	

//sets the orientation flags, and rotates  the bitmap.	Calls SetSize, which uses pixelsPerWord 
template <class T,TDisplayMode displayMode,TInt pixelsPerWord> 
TBool  CPalettizedScreenDevice<T,displayMode,pixelsPerWord>::SetDeviceOrientation(TDeviceOrientation aOrientation)
	{
	TSize newSize;

	if (!CGenericScreenDevice::iHelper.SetDeviceOrientation(aOrientation, newSize))
		{
		return EFalse;
		}

	// Need to update size, scan line size, etc.
	CGenericScreenDevice::iScanLineWords = CGenericScreenDevice::iHelper.BytesPerScanline() / 4;	 //presumption here that BPS is always mod4.
	CGenericScreenDevice::iBits = (TUint32*)CGenericScreenDevice::iHelper.AddressFirstPixel();
	__ASSERT_ALWAYS(CGenericScreenDevice::iScanLineWords && CGenericScreenDevice::iBits,Panic(EScreenDriverPanicInvalidHalValue));
	CGenericScreenDevice::SetSize(newSize);

	return ETrue;
	}

	
//
// Implementation of  template CGuidScreenDevice
// Arguments  <class T,TInt guidMode,TInt pixelsPerWord>
// Defines a screendriver for GCE with a mode specified using a GUID, probably flat colour
//
//

//Initialise modes that have been assigned GUIDs
template <class T,TUidPixelFormat guidMode,TInt pixelsPerWord> 
TInt  CGuidScreenDevice<T,guidMode,pixelsPerWord>::ConstructScreen(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize, TInt aHalMode)
	{
	TInt ret = CGenericScreenDevice::iHelper.Construct(aScreenNo, guidMode,aHalMode);
	if (ret != KErrNone)
		{
		return ret;
		}
	return  CGenericScreenDevice::ConstructScreen(aScreenNo,aBitmapAddress,aSize,aHalMode);
	}


//sets the orientation flags, and rotates  the bitmap.	Calls SetSize, which uses pixelsPerWord 
template <class T,TUidPixelFormat guidMode,TInt pixelsPerWord> 
TBool  CGuidScreenDevice<T,guidMode,pixelsPerWord>::SetDeviceOrientation(TDeviceOrientation aOrientation)
	{
	TSize newSize;

	if (!CGenericScreenDevice::iHelper.SetDeviceOrientation(aOrientation, newSize))
		{
		return EFalse;
		}

	// Need to update size, scan line size, etc.
	CGenericScreenDevice::iScanLineWords = CGenericScreenDevice::iHelper.BytesPerScanline() / 4;	 //presumption here that BPS is always mod4.
	CGenericScreenDevice::iBits = (TUint32*)CGenericScreenDevice::iHelper.AddressFirstPixel();
	__ASSERT_ALWAYS(CGenericScreenDevice::iScanLineWords && CGenericScreenDevice::iBits,Panic(EScreenDriverPanicInvalidHalValue));
	CGenericScreenDevice::SetSize(newSize);

	return ETrue;
	}



//Set size members based on pixel width/height, but also set stride member using pixels per word
template <class T,TUidPixelFormat guidMode,TInt pixelsPerWord> 
void CGuidScreenDevice<T,guidMode,pixelsPerWord>::SetSize(const TSize& aSize)
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(CGenericScreenDevice::iSize == aSize, User::Invariant());
	CGenericScreenDevice::iLongWidth = CGenericScreenDevice::iScanLineWords * pixelsPerWord;
	}
	




#endif	//__SCDRAW_INL__
