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

#include <graphicsaccelerator.h>
#include <fbs.h>


/** 
Constructor with a software or hardware bitmap.
Its type is initialised to EFbsBitmap or EHardwareBitmap accordingly.

@param aBitmap The bitmap which the object will access. 
*/
EXPORT_C TAcceleratedBitmapSpec::TAcceleratedBitmapSpec(CFbsBitmap* aBitmap)
	{
	RHardwareBitmap hwb(aBitmap->HardwareBitmapHandle());
	if(hwb.iHandle)
		{
		::new (this) TAcceleratedBitmapSpec(hwb);
		return;
		}
	iType = EFbsBitmap;
	iHandle = REINTERPRET_CAST(TInt,aBitmap);

	iLockStatus = EBitmapNeedsLocking;

	iSpare1 = 0;
	iSpare2 = 0;
	}

/** 
Constructor with a hardware bitmap. Its type is initialised to EHardwareBitmap. 
	
@param aBitmap The bitmap which the object will access. 
*/
EXPORT_C TAcceleratedBitmapSpec::TAcceleratedBitmapSpec(RHardwareBitmap aBitmap)
	{
	iType = EHardwareBitmap;
	iHandle = aBitmap.iHandle;
	iLockStatus = EBitmapIsStatic;
	iSpare1 = 0;
	iSpare2 = 0;
	}

/** 
Fills a TAcceleratedBitmapInfo structure with data for the bitmap.

This data is only valid for the duration of any processing between a Lock()/Unlock() 
pair.

For compressed bitmaps the line pitch has no meaning so it is set to the negation
of the compression type as defined by TBitmapfileCompression.

@param aInfo On return, holds the information needed to directly access the 
bitmap.
@return KErrNone if sucessful, otherwise one of the system error codes, including 
KErrUnknown if the object's type is ENoBitmap. 
*/
EXPORT_C TInt TAcceleratedBitmapSpec::GetInfo(TAcceleratedBitmapInfo& aInfo) const
	{
	switch(iType)
		{
		case EFbsBitmap:
			{
			CFbsBitmap* bmp = REINTERPRET_CAST(CFbsBitmap*,iHandle);
			aInfo.iDisplayMode = bmp->DisplayMode();
			aInfo.iAddress = REINTERPRET_CAST(TUint8*,bmp->DataAddress());
			aInfo.iSize = bmp->SizeInPixels();
			SEpocBitmapHeader header = bmp->Header();
			if (header.iCompression != ENoBitmapCompression)
				{
				aInfo.iLinePitch = -header.iCompression;
				}
			else
				{
				aInfo.iLinePitch = bmp->ScanLineLength(aInfo.iSize.iWidth,aInfo.iDisplayMode);
				}
			TUid extendedType = bmp->ExtendedBitmapType();
			if (extendedType != KNullUid)
				{
				aInfo.iPixelShift = extendedType.iUid;
				aInfo.iDataSize = bmp->DataSize();
				}
			else
				{
				aInfo.iPhysicalAddress = NULL;
				switch(aInfo.iDisplayMode)
					{
					case ENone:
						aInfo.iPixelShift = -1;
						break;
					case EGray2:
						aInfo.iPixelShift = 0;
						break;
					case EGray4:
						aInfo.iPixelShift = 1;
						break;
					case EGray16:
					case EColor16:
						aInfo.iPixelShift = 2;
						break;
					case EGray256:
					case EColor256:
						aInfo.iPixelShift = 3;
						break;
					case EColor4K:
					case EColor64K:
						aInfo.iPixelShift = 4;
						break;
					case EColor16M:
					case ERgb:
					case EColor16MU:
					case EColor16MA: 
					case EColor16MAP: 
						aInfo.iPixelShift = 5;
						break;
					default:
						aInfo.iPixelShift = -1;
						break;
					}
				}
			}
			return KErrNone;

		case EHardwareBitmap:
			return RHardwareBitmap(iHandle).GetInfo(aInfo);

		case ENoBitmap:
		default:
			Mem::FillZ(&aInfo,sizeof(aInfo));
			return KErrUnknown;
		}
	}

/** Locks the bitmap, if required.
@param  aCount Reference to a bitmap lock count object for nesting 
(only the first instance does the locking). */
EXPORT_C void TAcceleratedBitmapSpec::DoLock(TBitmapLockCount& aCount)
	{
	switch(iType)
		{
		case EFbsBitmap:
			if(aCount.Inc()==0)
				REINTERPRET_CAST(CFbsBitmap*,iHandle)->BeginDataAccess();
			break;

		case ENoBitmap:
		case EHardwareBitmap:
		default:
			// Never needs locking
			return;
		}
	}

/** Locks the bitmap, if required, setting the accelerated
bitmap information address.
@param  aCount Reference to a bitmap lock count object for nesting 
(only the first instance does the locking).
@param  aInfo Information structure to set the address in. */
EXPORT_C void TAcceleratedBitmapSpec::DoLock(TBitmapLockCount& aCount,TAcceleratedBitmapInfo& aInfo)
	{
	switch(iType)
		{
		case EFbsBitmap:
			{
			if(aCount.Inc()==0)
				REINTERPRET_CAST(CFbsBitmap*,iHandle)->BeginDataAccess();

			CFbsBitmap* bmp = REINTERPRET_CAST(CFbsBitmap*,iHandle);
			aInfo.iAddress = REINTERPRET_CAST(TUint8*,bmp->DataAddress());
			}
			break;

		case ENoBitmap:
		case EHardwareBitmap:
		default:
			// Never needs locking
			return;
		}
	}

/** Unlocks the bitmap, if required.
@param  aCount Reference to a bitmap lock count object for nesting 
(only the last instance does the unlocking). */
EXPORT_C void TAcceleratedBitmapSpec::DoUnlock(TBitmapLockCount& aCount)
	{
	switch(iType)
		{
		case EFbsBitmap:
			if(aCount.Dec()==0)
				REINTERPRET_CAST(CFbsBitmap*,iHandle)->EndDataAccess();
			break;

		case ENoBitmap:
		case EHardwareBitmap:
		default:
			// Never needs unlocking
			return;
		}
	}

