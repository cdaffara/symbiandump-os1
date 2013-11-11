// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hal.h>
#include "_WININC.H"
#include <bitdrawinterfaceid.h>

#if defined(SYMBIAN_GRAPHICS_GCE)
#include <surface.h>
#endif //SYMBIAN_GRAPHICS_GCE

template <class T> CScreenDevice<T>::~CScreenDevice()
	{
	iUpdateRegion.Close();
	iWindowInUse.Close();
	RWindows::ReleaseWindow(iScreenNo);
	}

/**
Constructs the CScreenDevice<T> object.
@param aScreenNo Screen number. It will be used in HAL::Get() calls.
@param aHwnd Windows OS - window handle.
@param aSize Screen size
@return System-wide error codes, KErrNone if the construction was successfull.
*/
template <class T> TInt CScreenDevice<T>::ConstructScreen(TInt aScreenNo, TAny* aHwnd,const TSize& aSize)
	{
    iScreenNo = aScreenNo;
	TInt ret = Construct(aSize);
	if (ret != KErrNone)
		return ret;

	if (aSize.iWidth < aSize.iHeight)
		{
		TInt bpp = BitsPerPixel(iDispMode);
		delete[] iScanLineBuffer;
		TInt scanLineWords = ((iSize.iHeight * bpp) + 31) / 32;
		iScanLineBuffer = (TUint32*)(User::Heap().Alloc(scanLineWords * 4));
		if (!iScanLineBuffer)
			return KErrNoMemory;
		}

	iWindow = RWindows::GetWindow(iScreenNo, aHwnd,aSize);
	if (iWindow == NULL)
		return KErrNoMemory;

	SetBits(iWindow->EpocBitmapBits());
    //Some explanations about iWindowInUse semaphore.
    //On the Emulator, a single Windows OS window can be shared between many Symbian OS
    //screen devices. iWindowInUse semaphore is used to help to determine the moment,
    //when the related Windows OS window can be destroyed safely. Because it is a named
    //semaphore, it will be created once by the first client (Symbian OS screen device)
    //and opened (reference counted) by the every next client (Symbian OS screen device).
    //In the CScreenDevice<T>'s deastructor iWindowInUse semaphore will be closed 
    //(its reference counter gets decremented). When its value reaches zero, the Symbian OS
    //will destroy the semaphore. This action will be detected by the RWindows::ReleaseWindow()'s
    //implementation and the related Windows OS window will be destroyed.
    TBuf<32> screenSemaphoreName;
    ::CreateScreenSemaphoreName(iScreenNo, screenSemaphoreName);
	ret = iWindowInUse.CreateGlobal(screenSemaphoreName,0,EOwnerThread);
	if (ret == KErrAlreadyExists)
        {
		iWindowInUse.OpenGlobal(screenSemaphoreName,EOwnerThread);
        }
	return KErrNone;
	}

template <class T> TInt CScreenDevice<T>::HorzTwipsPerThousandPixels() const
	{
	if (iSize.iWidth==0)
		return(0);
	TInt twips = 0;
	HAL::Get(iScreenNo, HALData::EDisplayXTwips,twips);
	return twips * 1000 / iSize.iWidth;
	}

template <class T> TInt CScreenDevice<T>::VertTwipsPerThousandPixels() const
	{
	if (iSize.iHeight==0)
		return(0);
	TInt twips = 0;
	HAL::Get(iScreenNo, HALData::EDisplayYTwips,twips);
	return twips * 1000 / iSize.iHeight;
	}

template <class T> void CScreenDevice<T>::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal] = ETrue;
	aOrientation[EOrientationRotated90] = ETrue;
	aOrientation[EOrientationRotated180] = ETrue;
	aOrientation[EOrientationRotated270] = ETrue;
	}

template <class T> void CScreenDevice<T>::SetDisplayMode(CFbsDrawDevice* aDrawDevice)
	{
	iUpdateRegion.Clear();
	SetOrientation(static_cast<CFbsDrawDevice::TOrientation>(iWindow->Orientation()));
	CopyOldSettings(aDrawDevice);
	InitScreen();
	}

template <class T> void CScreenDevice<T>::SetAutoUpdate(TBool aAutoUpdate)
	{
	iAutoUpdate = aAutoUpdate;
	}

template <class T> void CScreenDevice<T>::SetScreenOrientation(TInt aOrientation)
	{
	Update();

	TEmulatorFlip flip = EEmulatorFlipRestore;
	switch (aOrientation)
		{
	case 0: // Already set
		break;
	case 1:
		flip = EEmulatorFlipLeft;
		break;
	case 2:
		flip = EEmulatorFlipInvert;
		break;
	case 3:
		flip = EEmulatorFlipRight;
		break;
	default:
		ASSERT(0);
		break;
		}
	EmulatorFlip(flip, iScreenNo);
	iWindow->SetOrientation(aOrientation);
	}

//
// Update the screen with the update region.
//
template <class T> void CScreenDevice<T>::Update()
	{
	if (iUpdateRegion.IsEmpty())
		return;
	UpdateScreen(iUpdateRegion);
	iUpdateRegion.Clear();
	}

//
// Update the screen with the union of the update and specified regions.
// aRegion - logical coordinates
template <class T> void CScreenDevice<T>::Update(const TRegion& aRegion)
	{
	if(!aRegion.IsEmpty() && !aRegion.CheckError())
		{
		if(iScalingOff && iOriginIsZero)
			{
			iUpdateRegion.Union(aRegion);
			}
		else
			{
			register TInt rcCnt = aRegion.Count();
			RRegion physRegion(rcCnt);
			register TInt originX = iOrigin.iX;
			register TInt originY = iOrigin.iY;
			register TInt factorX = iScalingSettings.iFactorX;
			register TInt factorY = iScalingSettings.iFactorY;
			for(register TInt i=0;i<rcCnt;++i)
				{
				const TRect& logRect = aRegion[i];
				TRect physRect; 
				physRect.iTl.iX = ::Log2Phys(logRect.iTl.iX, originX, factorX, iSize.iWidth);
				physRect.iTl.iY = ::Log2Phys(logRect.iTl.iY, originY, factorY, iSize.iHeight);
				physRect.iBr.iX = ::RBtmLog2Phys(logRect.iBr.iX, originX, factorX, iSize.iWidth);
				physRect.iBr.iY = ::RBtmLog2Phys(logRect.iBr.iY, originY, factorY, iSize.iHeight);
				//The next statement sometimes have to allocate a block of memory and may
				//fail setting RRegion's internal error flag. But there is nothing we can do.
				physRegion.AddRect(physRect);
				}
			iUpdateRegion.Union(physRegion);
			physRegion.Close();
			}
		}
	Update();
	}

//
// Update the update region.
// aRect - logical coordinates
template <class T> void CScreenDevice<T>::UpdateRegion(const TRect& aRect)
	{
	register TInt originX = iOrigin.iX;
	register TInt originY = iOrigin.iY;
	register TInt factorX = iScalingSettings.iFactorX;
	register TInt factorY = iScalingSettings.iFactorY;
	TRect physRect; 
	physRect.iTl.iX = ::Log2Phys(aRect.iTl.iX, originX, factorX, iSize.iWidth);
	physRect.iTl.iY = ::Log2Phys(aRect.iTl.iY, originY, factorY, iSize.iHeight);
	physRect.iBr.iX = ::RBtmLog2Phys(aRect.iBr.iX, originX, factorX, iSize.iWidth);
	physRect.iBr.iY = ::RBtmLog2Phys(aRect.iBr.iY, originY, factorY, iSize.iHeight);
	physRect.Normalize();
	iUpdateRegion.AddRect(physRect);
	if(iUpdateRegion.Count() >= 10)
		{
		iUpdateRegion.AddRect(iUpdateRegion.BoundingRect());
		}
	if(iAutoUpdate)
		{
		Update();
		}
	}

template <class T> void CScreenDevice<T>::UpdateScreen(const TRegion& aRegion)
	{
	if (aRegion.CheckError())
		UpdateRect(iSize);
	else
		for(TInt count = 0; count < aRegion.Count(); count++)
			UpdateRect(aRegion[count]);

	iWindow->Update(aRegion,iSize);
	}

template <class T> TUint8* CScreenDevice<T>::WinPixelAddress(TInt aX,TInt aY) const
	{
	return iWindow->PixelAddress(aX,aY);
	}

