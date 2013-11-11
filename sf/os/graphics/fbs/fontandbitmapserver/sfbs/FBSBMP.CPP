// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include <bitmap.h>
#include <graphicsaccelerator.h>
#include <fbs.h>
#include <graphics/bitmapuid.h>
#include "fbsdefs.h"
#include "UTILS.H"
#include "fbshelper.h"
#include "fbsrasterizer.h"
#include "BitwiseBitmap.inl"
#include "FbsMessage.H"
#include "bitmapconst.h"
#include "OstTraceDefinitions.h"
#include "fbstrace.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FBSBMPTraces.h"
#endif

const TInt KMaxPixelSize = KMaxTInt / 4; // Maximum pixel size to avoid some overflow problems
const TInt KMaxBitmapHandleBufferSize = KNumBytesPerBitmapHandle * 2000; 	// Maximum size of buffer to store all bitmap handles.

GLREF_C void Panic(TFbsPanic aPanic);

//If we have to handle RAM located file with an embedded ROM mbm file section - 
//KRomMBMInRamRSC should be ETrue.
//If it is not allowed to do that - KRomMBMInRamRSC should be EFalse.
//The same constant is defined into TDefect test app. It should be changed too.
#pragma warning(disable : 4127)   //conditional expression is constant
LOCAL_D const TBool KRomMBMInRamRSC = EFalse;

//Cleanup function used by CFbsBitmap::LoadShiftedRomBmpL(...)
LOCAL_D void FreeMem(TAny* aMem)
	{
	delete[] static_cast<TUint8*>(aMem);
	}

// Fbs style bitmap client class for font bitmap server
/** @publishedAll */
EXPORT_C CFbsBitmap::CFbsBitmap():
	CBase(),
	iFbs(RFbsSession::GetSession()),
	iAddressPointer(NULL),
	iFlags(0),
	iUseCount(0),
	iHandle(0),
	iServerHandle(0)
	{
	}
	
/** Destructor. Calls Reset().
@see Reset()
@publishedAll 
@released
*/
EXPORT_C CFbsBitmap::~CFbsBitmap()
	{
	Reset();
	}
	
/** Gets the physical length in bytes of a scanline in memory. 
This is aligned to a 4 byte (DWORD) boundary for performance reasons.

@param aLength The length of a scanline in pixels. 
@param aDispMode The display mode of the bitmap. 
@return Number of bytes in the scanline in memory. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::ScanLineLength(TInt aLength,TDisplayMode aDispMode)
	{
	if (aDispMode == ERgb)
		return aLength * 4;
	else if (aDispMode == ENone)
		return 0;

	return CBitwiseBitmap::ByteWidth(aLength,aDispMode);
	}

/** Releases the bitmap's handle from the font and bitmap server and decrements 
its access count.
The server-side bitmap is only deleted when the access count for the bitmap 
decrements to zero. 
@publishedAll
@released
*/
EXPORT_C void CFbsBitmap::Reset()
	{
    FBS_OST(TInt ssh = (iFbs ? iFbs->ServerSessionHandle() : 0);)
    FBS_OST(OstTraceExt4( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_RESET_ENTRY, "> this=0x%08x; iH=0x%08x; iSH=0x%08x; iSSH=0x%08x", (TUint)this, iHandle, iServerHandle, ssh );)
	if (iHandle && !(iFlags & EIsRomBitmap))
		{
		iFbs->SendCommand(EFbsMessClose, iHandle, Handle());
		iFbs->iHelper->RemoveBitmap(*this);
		}
	if (KRomMBMInRamRSC && (iFlags & EIsRomBitmap))
		{
		// If it is a ROM bitmap, we have to check, is it a ROM bitmap located
		// in RAM? If yes, then we have to deallocate the bitmap memory.
		TBool isInRom = EFalse;
		TInt ret = User::IsRomAddress(isInRom, iAddressPointer);
		if (ret == KErrNone && !isInRom)
			delete[] reinterpret_cast<TUint8*>(iAddressPointer);
		}
	iAddressPointer = NULL;
	iFlags = 0;
	iUseCount = 0;
	iHandle = 0;
	iServerHandle = 0;
	FBS_OST(OstTrace1( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_RESET_EXIT, "< this=0x%08x", (TUint)this );)
	}
	
/** Tests whether or not the bitmap is read-only.
@return ETrue if the bitmap is read-only, EFalse otherwise.
@publishedAll
@released
*/
EXPORT_C TBool CFbsBitmap::IsRomBitmap() const
	{
	return (iFlags & EIsReadOnlyBitmapMask) > 0;
	}

/**  Sets the bitmap to use a bitmap image stored in ROM.
@param aRomBitmapPointer Pointer to a bitmap stored in ROM.
@param aBitmapSizeInBytes On return, indicates the size of 
the bitmap in bytes. 
@leave KErrUnknown aRomBitmapPointer is not in ROM, or has an invalid UID.
@publishedAll
@released
*/	
EXPORT_C void CFbsBitmap::SetRomBitmapL(CBitwiseBitmap* aRomBitmapPointer,TInt& aBitmapSizeInBytes)
    {
	TBool isInRom = EFalse;
	TInt ret = User::IsRomAddress(isInRom,(TAny*)aRomBitmapPointer);
	if (ret != KErrNone || !isInRom || aRomBitmapPointer->Uid() != KCBitwiseBitmapUid)
		User::Leave(KErrUnknown);

	Reset();
	iAddressPointer = aRomBitmapPointer;
	iFlags = EIsRomBitmap;
	iHandle = 1;

	User::LeaveIfError(iFbs->AllocScanLineBuffer(aRomBitmapPointer->iByteWidth + 4));
	aBitmapSizeInBytes = Align4(aRomBitmapPointer->iHeader.iBitmapSize + 28);
	}

CBitwiseBitmap* CFbsBitmap::Address() const
	{
	if (!iHandle)
		return NULL;
	return iAddressPointer;
	}

EXPORT_C CBitwiseBitmap* CFbsBitmap::CleanAddress() const
	{
	if (!iHandle)
		return NULL;
	// Don't try to clean a dirty bitmap between calls to BeginDataAccess() and EndDataAccess(), i.e. iUseCount > 0
	// ROM bitmaps can never be dirty
	if (!(iFlags & EIsReadOnlyBitmapMask) && iUseCount == 0 && iAddressPointer->iSettings.IsDirtyBitmap())
		{
		TPckgBuf<TBmpHandles> handlebuf;
		TIpcArgs args(iHandle, &handlebuf);
		if (iFbs->SendCommand(EFbsMessBitmapClean, args) == KErrNone)
			{
			const_cast<CFbsBitmap*>(this)->iHandle = handlebuf().iHandle;
			const_cast<CFbsBitmap*>(this)->iServerHandle = handlebuf().iServerHandle;
			const_cast<CFbsBitmap*>(this)->iAddressPointer = (CBitwiseBitmap*)(iFbs->HeapBase() + handlebuf().iAddressOffset);
			}
		}
	return iAddressPointer;
	}

inline CBitwiseBitmap* CFbsBitmap::BeginDataAccessAndGetCleanAddress(TUint32*& aDataAddress) const
	{
	BeginDataAccess();
	CBitwiseBitmap* bmp = Address();
	// aDataAddress should be consistent with bmp since after the call to BeginDataAccess()
	// the call to DataAddress() will not clean the bitmap again
	aDataAddress = DataAddress();
	return bmp;
	}

/** Gets the address of the first pixel in the bitmap. 
The first pixel is at the top-left. Access to the pixel data of a bitmap
should be surrounded by calls to BeginDataAccess() and EndDataAccess(),
otherwise performance may be degraded on certain platforms.

Note: Performing a Resize() or Compress() operation changes the value returned by this function.
@return The address of the first pixel of the bitmap.
@publishedAll
@released
@see CFbsBitmap::BeginDataAccess()
@see CFbsBitmap::EndDataAccess()
*/
EXPORT_C TUint32* CFbsBitmap::DataAddress() const
	{
	if(!iHandle) return(NULL);
	CBitwiseBitmap* bmp = CleanAddress();

	if (!(iFlags & EIsReadOnlyBitmapMask) && (iUseCount == 0))
		bmp->iSettings.SetVolatileBitmap();
		
	if(bmp->iUid.iUid==KCBitwiseBitmapHardwareUid.iUid)   // RHardwareBitmap
		{
		RHardwareBitmap hwb(bmp->iDataOffset);	// iDataOffset = handle for hardware bitmap
		TAcceleratedBitmapInfo info;
		const TInt ret = hwb.GetInfo(info);
		return ret!=KErrNone ? NULL : (reinterpret_cast<TUint32*>(info.iAddress));
		}

	if (bmp->iHeap == NULL)
		return(reinterpret_cast<TUint32*>((TUint8*)bmp+bmp->iDataOffset));
	return(reinterpret_cast<TUint32*>(iFbs->iLargeBitmapChunk.Base()+bmp->iDataOffset));
	}

/** Gets the length in bytes between scanlines in memory.
@return The length in bytes between scanlines in memory. 
@internalAll
@released
*/
EXPORT_C TInt CFbsBitmap::DataStride() const
	{
	if (!iHandle)
		{
		return 0;
		}
		
	CBitwiseBitmap* bmp = CleanAddress();
	if (bmp==NULL)
		{
		return 0;
		}
		
	return bmp->DataStride();	
	}

/** Creates a bitmap with the specified size and display mode. The bitmap is 
created on the font and bitmap server's shared heap.
@param aSizeInPixels The size of the bitmap to be created. 
@param aDispMode The display mode of the bitmap to be created. 
@return KErrNone if successful; KErrCouldNotConnect if no connection to the 
font and bitmap server could be made; KErrArgument if either the width or height specified 
in aSizeInPixels are negative or if the requested display mode is invalid; KErrTooBig 
if the requested size is too big. 
@publishedAll
@released
*/
EXPORT_C TInt CFbsBitmap::Create(const TSize& aSizeInPixels,TDisplayMode aDispMode)
	{
    FBS_OST(OstTraceExt4(GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_CREATE_ENTRY, "> this=0x%08x; w=%d; h=%d; dm=%d", (TUint)this, aSizeInPixels.iWidth, aSizeInPixels.iHeight, aDispMode); ) 
	TInt err = DoCreate(aSizeInPixels,aDispMode,KUidCFbsBitmapCreation);
    FBS_OST(TInt ssh = (iFbs ? iFbs->ServerSessionHandle() : 0);)
	FBS_OST(OstTraceExt5(GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_CREATE_EXIT, "< this=0x%08x; err=%d; iH=0x%08x; iSH=0x%08x; iSSH=0x%08x", (TUint)this, err, iHandle, iServerHandle, ssh); )
    return err;
	}

TInt CFbsBitmap::DoCreate(const TSize& aSizeInPixels, TDisplayMode aDispMode, TUid aUid, TInt aDataSize)
	{
	if(!iFbs) return(KErrCouldNotConnect);
	if (aDispMode <= ENone || aDispMode == ERgb || aDispMode >= EColorLast) return KErrArgument;
	if(aSizeInPixels.iWidth<0 || aSizeInPixels.iHeight<0) return(KErrArgument);
	if (aDataSize < 0) return KErrArgument;
	Reset();
	TBmpSpec bmpspec;
	bmpspec.iSizeInPixels=aSizeInPixels;
	bmpspec.iDispMode=aDispMode;
	bmpspec.iHandle = aUid.iUid;	// Use iHandle to pass UID
	bmpspec.iServerHandle = aDataSize;	// Use iServerHandle to pass data size
	TPckgBuf<TBmpSpec> b;
	b=bmpspec;
	TIpcArgs args(&b);
	TInt ret=iFbs->SendCommand(EFbsMessBitmapCreate,args);
	if(ret!=KErrNone) return(ret);
	iHandle=b().iHandle;
	iServerHandle=b().iServerHandle;
	iAddressPointer=(CBitwiseBitmap*)(iFbs->HeapBase()+b().iAddressOffset);
	if (aDataSize > 0) // explicitly specified data size means extended bitmap
		{
		iFlags = EIsExtendedBitmap;
		}
	ret = iFbs->iHelper->AddBitmap(*this);
	if (ret != KErrNone)
		return ret;
	return iFbs->AllocScanLineBuffer(CBitwiseBitmap::ByteWidth(aSizeInPixels.iWidth, aDispMode)+4);
	}

/** Creates a hardware bitmap with a size and display mode.
@param aSizeInPixels The bitmap's width and height in pixels.
@param aDispMode The bitmap's display mode.
@param aCreatorUid The UID of the application calling this function. This is 
used to allow segregation of the memory used for hardware bitmaps. For instance, 
if a device has video memory attached to display and graphics accelerator 
hardware, this UID is used to determine whether any video memory is pre-allocated 
for that application's use.
@return KErrNone if successful, otherwise one of the system wide error codes. 
These include KErrCouldNotConnect if no connection has been made to the font 
and bitmap server, KErrArgument if either the width or height specified in 
aSizeInPixels are negative or if the requested display mode is invalid, or KErrNotSupported 
if hardware bitmaps are not supported on the device. 
@publishedAll
@released
*/
EXPORT_C TInt CFbsBitmap::CreateHardwareBitmap(const TSize& aSizeInPixels,TDisplayMode aDispMode,TUid aCreatorUid)
	{
    FBS_OST(OstTraceExt5( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_CREATEHARDWAREBITMAP_ENTRY, "> this=0x%08x; w=%d; h=%d; dm=%d; uid=0x%08x", (TUint)this, aSizeInPixels.iWidth, aSizeInPixels.iHeight, aDispMode, aCreatorUid.iUid);)
	TInt err = DoCreate(aSizeInPixels,aDispMode,aCreatorUid);
    FBS_OST(TInt ssh = (iFbs ? iFbs->ServerSessionHandle() : 0);)
	FBS_OST(OstTraceExt5( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_CREATEHARDWAREBITMAP_EXIT, "< this=0x%08x; err=%d; iH=0x%08x; iSH=0x%08x; iSSH=0x%08x", (TUint)this, err, iHandle, iServerHandle, ssh);)
	return err;
	}

/** Resets the pixel-size of the bitmap.
If the new size is bigger than the old, the original bitmap is still situated 
at (0,0), but pixels outside the range of the old pixel-size are set to zero.
@param aSizeInPixels The new size of the bitmap. 
@return KErrNone if successful; KErrArgument if the new size is illegal; 
KErrGeneral if the bitmap has not yet been created; KErrAccessDenied if the 
bitmap is in ROM or is an extended bitmap; otherwise another of the system-wide error codes. 
@publishedAll
@released
*/
EXPORT_C TInt CFbsBitmap::Resize(const TSize& aSizeInPixels)
	{
	if(aSizeInPixels.iWidth<0 || aSizeInPixels.iHeight<0) 
		return(KErrArgument);
	if(aSizeInPixels.iWidth>KMaxPixelSize || aSizeInPixels.iHeight>KMaxPixelSize)
		return(KErrTooBig);	
	if(!iHandle) 
		return(KErrGeneral);
	if (iFlags & EIsReadOnlyBitmapMask)
		return(KErrAccessDenied);	
	TPckgBuf<TBmpHandles> handlebuf;
	TIpcArgs args(iHandle, aSizeInPixels.iWidth, aSizeInPixels.iHeight, &handlebuf);
	TInt err = iFbs->SendCommand(EFbsMessBitmapResize, args);
	if (err != KErrNone)
		return (err);
	iHandle = handlebuf().iHandle;
	iServerHandle = handlebuf().iServerHandle;
	iAddressPointer = (CBitwiseBitmap*)(iFbs->HeapBase() + handlebuf().iAddressOffset);
	return iFbs->AllocScanLineBuffer(CBitwiseBitmap::ByteWidth(aSizeInPixels.iWidth, DisplayMode())+4);
	}

/** Gets the display mode of the bitmap.
@return The display mode of the bitmap. 
@publishedAll
@released
*/
EXPORT_C TDisplayMode CFbsBitmap::DisplayMode() const
	{
	if(iHandle==NULL) return(ENone);
	return CleanAddress()->DisplayMode();
	}

/** Returns the display mode that was used to create the bitmap.
@return The display mode used to create the bitmap.
@publishedAll
@released
*/
EXPORT_C TDisplayMode CFbsBitmap::InitialDisplayMode() const
	{
	if(iHandle == NULL) 
		{
		return ENone;
		}
	return CleanAddress()->InitialDisplayMode();
	}

/**
Changes the display mode of the bitmap.
The requested display mode cannot be greater (in bpp value) than the initial display mode.
This method cannot leave, for instance because of an out of memory condition. No 
additional memory is allocated or leaving methods called.
The bitmap's content is preserved when converting it to the requested display mode,
but there may be some loss of quality.
@publishedAll
@released
@param aDisplayMode The requested display mode.
@return KErrArgument if the requested mode is invalid, or has a greater bpp value 
than the initial mode. KErrNotSupported if the bitmap is compressed, or is a
ROM bitmap, an extended bitmap or a hardware bitmap. KErrGeneral if the bitmap
handle is NULL. KErrNone if the method call is successful.
@see CFbsBitmap::InitialDisplayMode()
*/
EXPORT_C TInt CFbsBitmap::SetDisplayMode(TDisplayMode aDisplayMode)
	{
	if(!iHandle) 
		{
		return KErrGeneral;
		}	
	TUint32* data;
	CBitwiseBitmap* bmp = BeginDataAccessAndGetCleanAddress(data);
	TInt err = bmp->SetDisplayMode(aDisplayMode, data);
	EndDataAccess(EFalse);
	return err;
	}

/** Duplicates a bitmap.
This function does not create a copy of the bitmap. It just assigns another 
handle to the bitmap in the font and bitmap server, and sets this object's 
handle to that. If the specified bitmap is in the ROM, it just assigns a pointer 
to it.
@param aHandle The handle to an existing bitmap.
@return KErrNone if successful; KErrCouldNotConnect if no connection to the 
font and bitmap server could be made; KErrUnknown if no bitmap could be found 
with the specified handle number.
@publishedAll
@released
@see CFbsBitmap::Handle()
*/
EXPORT_C TInt CFbsBitmap::Duplicate(TInt aBitmapHandle)	
    {
    TInt ret = KErrNone;
    FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_DUPLICATE_ENTRY, "> this=0x%08x; iH=0x%08x;", (TUint)this, aBitmapHandle);)
	if(!iFbs) 
		{
		ret = KErrCouldNotConnect;
		FBS_OST(OstTrace1( TRACE_ERROR, CFBSBITMAP_DUPLICATE_ERROR, "! this=0x%08x; !iFbs", (TUint)this);)
		}
	else if(!aBitmapHandle) 
		{
		ret = KErrUnknown;
		FBS_OST(OstTrace1( TRACE_ERROR, CFBSBITMAP_DUPLICATE_ERROR2, "! this=0x%08x; !aBitmapHandle", (TUint)this);)
		}
    else
        {
        TBool isinrom = EFalse;
        ret = User::IsRomAddress(isinrom, (TAny*)aBitmapHandle);
        if (ret == KErrNone)
            {
            if (isinrom)
                {
                ret = DuplicateInRom(aBitmapHandle);
                FBS_OST_IF(ret != KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_DUPLICATE_ERROR4, "! this=0x%08x; DuplicateInRom() returned %d;", (TUint)this, ret);)
                }
            else
                {
                ret = DuplicateInRam(aBitmapHandle);
                FBS_OST_IF(ret != KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_DUPLICATE_ERROR5, "! this=0x%08x; DuplicateInRam() returned %d;", (TUint)this, ret);)
                }
            }
        else 
            {
            FBS_OST(OstTraceExt2( TRACE_ERROR, CFBSBITMAP_DUPLICATE_ERROR3, "! this=0x%08x; IsRomAddress() returned %d", (TUint)this, ret);)
            ret = KErrUnknown;
            }
        }
    FBS_OST(TInt ssh = (iFbs ? iFbs->ServerSessionHandle() : 0);)
    FBS_OST(OstTraceExt5( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_DUPLICATE_EXIT, "< this=0x%08x; iH=0x%08x; iSH=0x%08x; ret=%d; iSSH=0x%08x", (TUint)this, iHandle, iServerHandle, ret, ssh);)
	return ret;
    }

/** Duplicates a bitmap where the bitmap handle refers to a rom bitmap.
@param aBitmapHandle A valid Rom bitmap handle.
@return KErrNone on success.
 */
TInt CFbsBitmap::DuplicateInRom(TInt aBitmapHandle)
    {
    TInt ret = KErrNone;
    Reset();
    TUid uid = ((CBitwiseBitmap*)aBitmapHandle)->Uid();
    if (uid != KCBitwiseBitmapUid)
        {
        FBS_OST(OstTraceExt2( TRACE_ERROR, CFBSBITMAP_DUPLICATEINROM_ERROR, "! this=0x%08x; 0x%08x != KCBitwiseBitmapUid", (TUint)this, (TUint)uid.iUid);)
        ret = KErrUnknown;
        }
    else
        {
        iAddressPointer = (CBitwiseBitmap*)aBitmapHandle;
        iFlags = EIsRomBitmap;
        iHandle=1;
        ret = iFbs->AllocScanLineBuffer(iAddressPointer->iByteWidth + 4);
        FBS_OST_IF(ret!=KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_DUPLICATEINROM_ERROR2, "! this=0x%08x; AllocScanLineBuffer() returned %d", (TUint)this, ret);)
        }
    return ret;
    }

/** Duplicates a bitmap where the bitmap handle refers to a ram bitmap
@param aBitmapHandle A valid Ram bitmap handle.
@return KErrNone on success.
 */
TInt CFbsBitmap::DuplicateInRam(TInt aBitmapHandle)
    {
    TInt ret = KErrNone;
    Reset();
     
    TPckgBuf<TBmpHandles> b;
    TIpcArgs args(aBitmapHandle,&b);
    ret=iFbs->SendCommand(EFbsMessBitmapDuplicate,args);
    FBS_OST_IF(ret!=KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_DUPLICATEINRAM_ERROR, "! this=0x%08x; SendCommand(EFbsMessBitmapDuplicate) returned %d", (TUint)this, ret);)
    if(ret==KErrNone) 
        {
        iHandle=b().iHandle;
        iServerHandle=b().iServerHandle;
        iAddressPointer=(CBitwiseBitmap*)(iFbs->HeapBase()+b().iAddressOffset);
        if (iAddressPointer->iUid.iUid != KCBitwiseBitmapUid.iUid && iAddressPointer->iUid.iUid != KCBitwiseBitmapHardwareUid.iUid)
            {
            iFlags = EIsExtendedBitmap;
            }
        ret = iFbs->iHelper->AddBitmap(*this);
        FBS_OST_IF(ret!=KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_DUPLICATEINRAM_ERROR2, "! this=0x%08x; AddBitmap() returned %d", (TUint)this, ret);)
        if (ret == KErrNone)
            {
            ret = iFbs->AllocScanLineBuffer(iAddressPointer->iByteWidth+4);
            FBS_OST_IF(ret!=KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_DUPLICATEINRAM_ERROR3, "! this=0x%08x; AllocScanLineBuffer() returned %d", (TUint)this, ret);)
            }
        }
    return ret;
    }

/** Loads a specific bitmap from a multi-bitmap file.
The bitmap may be shared by other font and bitmap server clients.
@param aFileName The filename of the multi-bitmap (.mbm) file. 
@param aId The bitmap identifier.
@param aShareIfLoaded Specifies whether or not the loaded bitmap will be made 
available for sharing between font and bitmap server clients. 
@return KErrNone if successful, otherwise another of the system error 
codes. 
@publishedAll
@released
*/	
EXPORT_C TInt CFbsBitmap::Load(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded)
	{
    FBS_OST(OstTraceExt4( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOAD_ENTRY, "> this=0x%08x; file=%S, id=0x%08x; share=%d", (TUint)this, aFileName, aId, aShareIfLoaded);)
	TInt err = Load(aFileName,aId,aShareIfLoaded,0);
    FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOAD_EXIT, "< this=0x%08x; err=%d", (TUint)this, err);)
	return err; 
	}

/**  Loads a specific bitmap from a multi-bitmap file.
The bitmap may be shared by other font and bitmap server clients.
@param aFileName The filename of the multi-bitmap (.mbm) file.
@param aId The bitmap identifier.
@param aShareIfLoaded  Specifies whether or not the loaded bitmap will be made 
available for sharing between FBSERV clients.
@param aFileOffset Bitmap file section offset within the file.
@return KErrNone if successful, otherwise another of the system error codes.
@publishedAll
@released
*/	
EXPORT_C TInt CFbsBitmap::Load(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset)
	{
    TInt err = KErrNone;
    FBS_OST(OstTraceExt5( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOAD2_ENTRY, "> this=0x%08x; file=%S, id=0x%08x; share=%d; off=%d", (TUint)this, aFileName, aId, aShareIfLoaded, aFileOffset);)
	if(!iFbs)
		{
        FBS_OST(OstTrace1( TRACE_ERROR, CFBSBITMAP_LOAD2_ERROR, "! this=0x%08x; !iFbs", (TUint)this);)
		err = KErrCouldNotConnect;
		}
	else
	    {
        Reset();
        TUint32* rompointer = NULL;
        //access using filename has the advantage of using rom address lookup cache
        IsFileInRom(aFileName, rompointer);
        TBool romPointerValid;
        err = DoLoadFromRom(rompointer, aId, aFileOffset, romPointerValid);
        if(!romPointerValid)
            {
            _LIT(KResourcePath, "?:\\Resource\\*");
            TInt match = aFileName.MatchF(KResourcePath);
            //if the file is in the resource directory we don't need to check capabilities and the file can just be opened on the server side.
            if (match == 0)
                {
                err = DoLoad(aFileName,aId,aShareIfLoaded,aFileOffset);
                FBS_OST_IF(err!=KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_LOAD2_ERROR3, "! this=0x%08x; DoLoad returned %d", (TUint)this, err);)
                }
            else
                {
                RFile file;
                err = file.Open(iFbs->FileServer(),aFileName,EFileShareReadersOnly);
                if (err==KErrNone)
                    {
                    err = DoLoad(file,aId,aShareIfLoaded,aFileOffset);
                    FBS_OST_IF(err!=KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_LOAD2_ERROR4, "! this=0x%08x; DoLoad returned %d", (TUint)this, err);)
                    }
                file.Close();
                }
            }
	    }
    FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOAD2_EXIT, "< this=0x%08x; err=%d", (TUint)this, err);)
	return err;
	}

/** Loads and compresses a specific bitmap from a multi-bitmap file.
The bitmap may be shared by other font and bitmap server clients.
If the bitmap is loaded from ROM then compression is not allowed.
@param aFileName The filename of the multi-bitmap (.mbm) file.
@param aId The bitmap identifier.
@param aShareIfLoaded Specifies whether or not the loaded bitmap will be
made available for sharing between FBSERV clients.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@publishedAll 
@released
*/	
EXPORT_C TInt CFbsBitmap::LoadAndCompress(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded)
    {	
    FBS_OST(OstTraceExt4( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOADANDCOMPRESS_ENTRY, "> this=0x%08x; file=%S; id=0x%08x; share=%d", (TUint)this, aFileName, aId, aShareIfLoaded);)
    TInt ret = LoadAndCompress(aFileName, aId, aShareIfLoaded, 0);
    FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOADANDCOMPRESS_EXIT, "< this=0x%08x; err=%d", (TUint)this, ret);)
    return ret;
	}

/** Loads and compresses a specific bitmap from a multi-bitmap file.
The bitmap may be shared by other font and bitmap server clients. If the 
bitmap is loaded from ROM then compression is not allowed.
@param aFileName The filename of the multi-bitmap (.mbm) file.
@param aId The bitmap identifier.
@param aShareIfLoaded Specifies whether or not the loaded bitmap will be made 
available for sharing between FBSERV clients.
@param aFileOffset Bitmap file section offset within the file.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@publishedAll 
@released
*/	
EXPORT_C TInt CFbsBitmap::LoadAndCompress(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset)
    {
    FBS_OST(OstTraceExt5( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOADANDCOMPRESS2_ENTRY, "> this=0x%08x; file=%S, id=0x%08x; share=%d; off=%d", (TUint)this, aFileName, aId, aShareIfLoaded, aFileOffset);)
	TInt err = Load(aFileName,aId,aShareIfLoaded,aFileOffset);
	if (err == KErrNone)
		{
		err = !(iFlags & EIsRomBitmap) ? Compress() : KErrAccessDenied;
		FBS_OST_IF(err!=KErrNone, OstTraceExt3( TRACE_ERROR, CFBSBITMAP_LOADANDCOMPRESS2_ERROR, "! this=0x%08x; iFlags=0x%08x; err=%d", (TUint)this, (TUint)iFlags, err);)
		}
	FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOADANDCOMPRESS2_EXIT, "< this=0x%08x; err=%d", (TUint)this, err);)
	return err;
	}

/** Saves the bitmap as a direct file store.
The file store overwrites any existing file with the same name.
@param aFilename The name of the file. 
@return KErrNone if successful, KErrNotSupported if this CFbsBitmap is an 
extended bitmap, otherwise another of the system-wide error codes. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::Save(const TDesC& aFilename)
	{
	if (!iHandle)
		{
		return(KErrGeneral);
		}
	RFile file;
	TInt ret=file.Replace(iFbs->FileServer(),aFilename,EFileWrite);
	if(ret!=KErrNone) return(ret);
	TRAP(ret,DoSaveL(file));
	file.Close();
	return(ret);
	}

/** Saves the bitmap as a direct file store using an opened file handle.
The file store overwrites any existing file with the same name.
@param aFile The opened file handle
@return KErrNone if successful, KErrNotSupported if this CFbsBitmap is an 
extended bitmap, otherwise another of the system-wide error codes. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::Save(RFile& aFile)
	{
	if (!iHandle)
		{
		return KErrGeneral;
		}
	TRAPD(ret,DoSaveL(aFile));
	return ret;
	}

void CFbsBitmap::DoSaveL(RFile& aFile)
	{
	CDirectFileStore* filestore=CDirectFileStore::NewLC(aFile); // create from open file
	TUidType uidtype(KDirectFileStoreLayoutUid,KMultiBitmapFileImageUid);
	filestore->SetTypeL(uidtype);
	RStoreWriteStream bmpstream;
	TStreamId bmpstreamid=bmpstream.CreateLC(*filestore); // create bitmap stream
	ExternalizeL(bmpstream);
	bmpstream.CommitL();
	CleanupStack::PopAndDestroy(); // bitmap stream
	RStoreWriteStream rootstream;
	TStreamId rootstreamid=rootstream.CreateLC(*filestore); // create root stream
	rootstream.WriteInt32L(1); // number of bitmaps
	rootstream<<bmpstreamid; // stream id of bitmap
	rootstream.CommitL();
	CleanupStack::PopAndDestroy(); // root stream
	filestore->SetRootL(rootstreamid);
	filestore->CommitL();
	CleanupStack::PopAndDestroy(); // file store
	}

/** Constructs a multi-bitmap file.
@param aFilename The name of the multi-bitmap file to be created.
@param aNumSources The number of bitmaps to store in the file.
@param aSources  An array of pointers to bitmaps to be stored.
@param aSourceIds An array of identifiers for the bitmaps to be stored. 
@publishedAll
@released
*/	
EXPORT_C void CFbsBitmap::StoreL(const TDesC& aFilename,TInt aNumSources,const TDesC* aSources[],TInt32 aSourceIds[])
	{
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	CDirectFileStore* filestore = CDirectFileStore::ReplaceLC(bitmap->iFbs->FileServer(),aFilename,EFileWrite);
	DoStoreL(filestore,bitmap,aNumSources,aSources,aSourceIds);
	CleanupStack::PopAndDestroy(2,bitmap);	
	}

/** Constructs a multi-bitmap file using an opened file handle.
@param aFile The opened file handle of multi-bitmap file
@param aNumSources The number of bitmaps to store in the file.
@param aSources  An array of pointers to bitmaps to be stored.
@param aSourceIds An array of identifiers for the bitmaps to be stored. 
@publishedAll
@released
*/	
EXPORT_C void CFbsBitmap::StoreL(RFile& aFile,TInt aNumSources,const TDesC* aSources[],TInt32 aSourceIds[])
	{
	CDirectFileStore* filestore = CDirectFileStore::NewLC(aFile);
	DoStoreL(filestore,NULL,aNumSources,aSources,aSourceIds);
	CleanupStack::PopAndDestroy(filestore);	
	}

/**
@internalComponent
*/
void CFbsBitmap::DoStoreL(CDirectFileStore* aFileStore,CFbsBitmap* aBitmap,TInt aNumSources,const TDesC* aSources[],TInt32 aSourceIds[])
	{
	if(aNumSources<1 || aSources==NULL) User::Leave(KErrArgument);
	TInt count=0;
	for(;count<aNumSources;count++)
		if(aSources[count]==NULL) User::Leave(KErrArgument);
	TStreamId* ids=new(ELeave) TStreamId[aNumSources];
	CleanupArrayDeletePushL(ids);
	TInt nPushed=1;
	if (!aBitmap)
		{
		aBitmap=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(aBitmap);
		++nPushed;
		}
	TUidType uidtype(KDirectFileStoreLayoutUid,KMultiBitmapFileImageUid);
	aFileStore->SetTypeL(uidtype);
	for(count=0;count<aNumSources;count++)
		{
		User::LeaveIfError(aBitmap->Load(*aSources[count],aSourceIds[count]));
		RStoreWriteStream bmpstream;
		ids[count]=bmpstream.CreateLC(*aFileStore); // create bitmap stream
		aBitmap->ExternalizeL(bmpstream);
		bmpstream.Close();
		CleanupStack::Pop(); // bitmap stream
		}
	RStoreWriteStream rootstream;
	TStreamId rootstreamid=rootstream.CreateLC(*aFileStore); // create root stream
	rootstream.WriteInt32L(aNumSources); // number of bitmaps
	for(count=0;count<aNumSources;count++)
		rootstream<<ids[count]; // stream ids of bitmaps
	rootstream.Close();
	CleanupStack::Pop(); // root stream
	aFileStore->SetRootL(rootstreamid);
	CleanupStack::PopAndDestroy(nPushed); // ids [and bitmap]
	}

/** Gets the bitmap's scanline for the specified line starting from the 
specified point.
The dither offset of the bitmap is taken to be TPoint(0,0).
@param aBuf The buffer in which the scanline is returned. 
@param aPoint The start pixel. 
@param aLength The number of pixels to get. 
@param aDispMode Format to be used to write the data to the buffer. 
@publishedAll
@released
*/
EXPORT_C void CFbsBitmap::GetScanLine(TDes8& aBuf,const TPoint& aPoint,TInt aLength,TDisplayMode aDispMode) const
	{
	GetScanLine(aBuf,aPoint,aLength,TPoint(0,0),aDispMode);
	}

/** Gets the bitmap's scanline for the specified line starting from the specified 
point and using the specified dither offset.
@param aBuf The buffer in which the scanline is returned. 
@param aPixel The start pixel. 
@param aLength The number of pixels to get. 
@param aDitherOffset The dither offset of the bitmap. 
@param aDispMode Format to be used to write the data to the buffer. 
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::GetScanLine(TDes8& aBuf,const TPoint& aPoint,TInt aLength,const TPoint& aDitherOffset,TDisplayMode aDispMode) const
	{
	if(!iHandle) return;
	TUint32* data;
	CBitwiseBitmap* bmp = BeginDataAccessAndGetCleanAddress(data);
	CFbsRasterizer* rasterizer = NULL;
	if ((iFlags & EIsExtendedBitmap) && iFbs)
		{
		rasterizer = iFbs->iHelper->Rasterizer();
		if (rasterizer)
			{
			CFbsRasterizer::TBitmapDesc desc;
			desc.iSizeInPixels = bmp->SizeInPixels();
			desc.iDispMode = bmp->DisplayMode();
			desc.iDataType = bmp->iUid;
			desc.iData = data;
			desc.iDataSize = bmp->iHeader.iBitmapSize - bmp->iHeader.iStructSize;
			rasterizer->BeginBitmap(bmp->Extra()->iSerialNumber, desc, NULL);
			}
		}
	bmp->GetScanLine(aBuf, aPoint, aLength, ETrue, aDitherOffset, aDispMode, data);
	if (rasterizer)
		{
		rasterizer->EndBitmap(bmp->Extra()->iSerialNumber);
		}
	EndDataAccess(ETrue);
	}

/** Sets the bitmap's horizontal scanline at the specified y co-ordinate to the 
scanline contained in the buffer.
@param aBuf The new scanline to be written to the bitmap. 
@param aY The y co-ordinate of the scanline.
@panic FBSCLI 11 in debug builds if this is a compressed bitmap.
@panic FBSCLI 28 in debug builds if this is a read-only bitmap.
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::SetScanLine(TDes8& aBuf,TInt aY) const
	{
	if (!iHandle)
		return;	
	if (iFlags & EIsReadOnlyBitmapMask)
		{
		__ASSERT_DEBUG(EFalse, ::Panic(EFbsPanicBitmapReadOnly));
		return;
		}
	TUint32* data;
	CBitwiseBitmap* bmp = BeginDataAccessAndGetCleanAddress(data);
	if (bmp->IsCompressed())
		{
		EndDataAccess(ETrue);
		__ASSERT_DEBUG(EFalse, ::Panic(EFbsBitmapInvalidCompression));
		return;
		}
	data = bmp->ScanLineAddress(data, aY);
	TInt bytewidth = bmp->iByteWidth;
	TInt bytelen=aBuf.Length();
	if(bytelen<bytewidth) bytewidth=bytelen;
	TInt wordlen=bytewidth>>2;
	TUint32* ptr=(TUint32*)aBuf.Ptr();
	TUint32* ptrlim=ptr+wordlen;
	while(ptr<ptrlim)
		*data++=*ptr++;
	TInt limit=wordlen<<2;
	if(limit<bytewidth)
		{
		TUint8* byteptr=(TUint8*)ptrlim;
		TUint8* databyte=(TUint8*)data;
		while(limit<bytewidth)
			{
			*databyte++=*byteptr++;
			limit++;
			}
		}
	EndDataAccess(EFalse);
	}

/** Gets the bitmap's vertical scanline starting at the specified x co-ordinate.
Note: The method only works for uncompressed bitmaps.
Note: The dither offset of the bitmap is taken to be TPoint(0,0).
@param aBuf The buffer in which the vertical scanline is returned. 
@param aX The x co-ordinate of the vertical scanline. 
@param aDispMode Format to be used to write the data to the buffer. 
@panic FBSCLI 11 in debug builds if this is not an ucompressed bitmap or an extended bitmap.
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::GetVerticalScanLine(TDes8& aBuf,TInt aX,TDisplayMode aDispMode) const
	{
	GetVerticalScanLine(aBuf,aX,TPoint(0,0),aDispMode);
	}

/** Gets the bitmap's vertical scanline starting at the specified x co-ordinate 
and using the specified dither offset.
Note: The method only works for uncompressed bitmaps.
@param aBuf The buffer in which the vertical scanline will be returned. 
@param aX The x co-ordinate of the vertical scanline to get. 
@param aDitherOffset The dither offset of the bitmap. 
@param aDispMode Format to be used to write the data to the buffer. 
@panic FBSCLI 11 in debug builds if this is not an ucompressed bitmap or an extended bitmap.
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::GetVerticalScanLine(TDes8& aBuf,TInt aX,const TPoint& aDitherOffset,TDisplayMode aDispMode) const
	{
	if(!iHandle) return;
	TUint32* data;
	CBitwiseBitmap* bmp = BeginDataAccessAndGetCleanAddress(data);
	CFbsRasterizer* rasterizer = NULL;
	if ((iFlags & EIsExtendedBitmap) && iFbs)
		{
		rasterizer = iFbs->iHelper->Rasterizer();
		if (rasterizer)
			{
			CFbsRasterizer::TBitmapDesc desc;
			desc.iSizeInPixels = bmp->SizeInPixels();
			desc.iDispMode = bmp->DisplayMode();
			desc.iDataType = bmp->iUid;
			desc.iData = data;
			desc.iDataSize = bmp->iHeader.iBitmapSize - bmp->iHeader.iStructSize;
			rasterizer->BeginBitmap(bmp->Extra()->iSerialNumber, desc, NULL);
			}
		}
	bmp->GetVerticalScanLine(aBuf, aX, ETrue, aDitherOffset, aDispMode, data, rasterizer);
	if (rasterizer)
		{
		rasterizer->EndBitmap(bmp->Extra()->iSerialNumber);
		}
	EndDataAccess(ETrue);
	}

/** Gets the handle number of the bitmap.
The returned value can be used to give another thread access to the bitmap.
@return The handle number of the bitmap. 
@publishedAll 
@released
@see CFbsBitmap::Duplicate()
*/
EXPORT_C TInt CFbsBitmap::Handle() const
	{
	if(!iHandle) 
		return(0);
	if (iFlags & EIsRomBitmap)
		return TInt(iAddressPointer);
	else
		return(iServerHandle);
	}

/** Creates a bitmap header.
This is used when streaming bitmaps to stores.
@return The bitmap header for the bitmap. 
@publishedAll 
@released
*/
EXPORT_C SEpocBitmapHeader CFbsBitmap::Header() const
	{
	if (iHandle)
		return CleanAddress()->iHeader;
	SEpocBitmapHeader header;
	return(header);
	}

/** Converts a horizontal dimension on the graphics device from pixels to twips.
@param aPixels A horizontal dimension on the graphics device in pixels. 
@return A horizontal dimension on the graphics device in twips. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::HorizontalPixelsToTwips(TInt aPixels) const
	{
	if(iHandle==NULL) return(0);
	return CleanAddress()->HorizontalPixelsToTwips(aPixels);
	}

/** Converts a horizontal dimension on the graphics device from twips to pixels.
@param aTwips A horizontal dimension on the graphics device in twips. 
@return A horizontal dimension on the graphics device in pixels. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::HorizontalTwipsToPixels(TInt aTwips) const
	{
	if(iHandle==NULL) return(0);
	return CleanAddress()->HorizontalTwipsToPixels(aTwips);
	}

/** Gets the RGB value of the specified pixel.
Note: The method only works for uncompressed bitmaps and extended bitmaps.
@param aColor On return, the RGB value of the specified pixel. 
@param aPixel The pixel whose colour is to be determined.
@panic FBSCLI 11 in debug builds if this is a compressed bitmap.
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::GetPixel(TRgb& aColor,const TPoint& aPoint) const
	{
	if(!iHandle) 
		{
		return;
		}	
	
	TUint32* data;
	CBitwiseBitmap* bmp = BeginDataAccessAndGetCleanAddress(data);
	CFbsRasterizer* rasterizer = NULL;
	if ((iFlags & EIsExtendedBitmap) && iFbs)
		{
		rasterizer = iFbs->iHelper->Rasterizer();
		if (rasterizer)
			{
			CFbsRasterizer::TBitmapDesc desc;
			desc.iSizeInPixels = bmp->SizeInPixels();
			desc.iDispMode = bmp->DisplayMode();
			desc.iDataType = bmp->iUid;
			desc.iData = data;
			desc.iDataSize = bmp->iHeader.iBitmapSize - bmp->iHeader.iStructSize;
			rasterizer->BeginBitmap(bmp->Extra()->iSerialNumber, desc, NULL);
			}
		}
	bmp->GetPixel(aColor, aPoint, data, rasterizer);
	if (rasterizer)
		{
		rasterizer->EndBitmap(bmp->Extra()->iSerialNumber);
		}
	EndDataAccess(ETrue);
	}

/** Gets the pixel-size of the bitmap.
@return The size of the bitmap, in pixels. 
@publishedAll 
@released
*/
EXPORT_C TSize CFbsBitmap::SizeInPixels() const
	{
    TSize zero;
	if(!iHandle) return(zero);
	return CleanAddress()->SizeInPixels();
	}

/** Sets the twip-size of the bitmap by converting the bitmaps pixel-size from 
pixels to twips, using the conversion functions in the specified graphics 
device map.
@param aMap The graphics device map to be used for providing pixel to twip 
conversion. 
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::SetSizeInTwips(const MGraphicsDeviceMap* aMap)
	{
	if (!iHandle || (iFlags & EIsRomBitmap) || aMap == NULL)
		return;
	TSize size=SizeInPixels();
	size.iWidth=aMap->HorizontalPixelsToTwips(size.iWidth);
	size.iHeight=aMap->VerticalPixelsToTwips(size.iHeight);
	iFbs->SetCallBackPtr(&iServerHandle);
	iFbs->CallBack();
	// SizeInPixels() called CleanAddress() so call Address() now to make sure we don't clean the bitmap twice
	Address()->iHeader.iSizeInTwips=size;
	}

/** Sets the twip-size of the bitmap directly to the specified size.
@param aSizeInTwips The new size of the bitmap, in twips. 
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::SetSizeInTwips(const TSize& aSizeInTwips)
	{
	if (!iHandle || (iFlags & EIsRomBitmap))
		return;
	iFbs->SetCallBackPtr(&iServerHandle);
	iFbs->CallBack();
	CleanAddress()->iHeader.iSizeInTwips = aSizeInTwips;
	}

/** Externalises the bitmap to the specified stream. Not supported for extended bitmaps.
@param aStream The write stream. 
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::ExternalizeL(RWriteStream& aStream) const
	{
	if (!iHandle)
		User::Leave(KErrGeneral);
	BeginDataAccess();
	Address()->ExternalizeL(aStream, *this);	
	EndDataAccess(ETrue);
	}

/** Externalises that area of the bitmap contained within a specified 
rectangular area. Not supported for extended bitmaps.
@param aStream The write stream
@param aRect The rectangular area of the bitmap to externalise. The bitmap 
that is externalized will be of this size. 
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::ExternalizeRectangleL(RWriteStream& aStream,const TRect& aRect) const
	{
	if (!iHandle)
		User::Leave(KErrGeneral);
	BeginDataAccess();
	Address()->ExternalizeRectangleL(aStream, aRect, *this);
	EndDataAccess(ETrue);
	}

/** Internalises a CFbsBitmap from a stream.
@param aStream The read stream.
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::InternalizeL(RReadStream& aStream)
	{
	if(!iFbs) User::Leave(KErrCouldNotConnect);
	Reset();
	SEpocBitmapHeader header;
	CBitwiseBitmap::InternalizeHeaderL(aStream,header);

	TDisplayMode dispmode = CBitwiseBitmap::DisplayMode(header.iBitsPerPixel,header.iColor);
	User::LeaveIfError(Create(header.iSizeInPixels,dispmode));

	TUint32* data;
	CBitwiseBitmap* bmp=BeginDataAccessAndGetCleanAddress(data);
	bmp->iHeader=header;
	TInt bytesize = header.iBitmapSize - header.iStructSize;
	if (bytesize > 0)
		{
		bmp->DoInternalizeL(aStream, bytesize, data);
		EndDataAccess(EFalse);
		}
	else
		{
		EndDataAccess(ETrue);
		}
	}

EXPORT_C TInt CFbsBitmap::Compress()
 	{
	return Compress(ERLECompression);
	}

/** Compresses bitmap in RAM.
@param aScheme specifies preferred compression type ERLECompression or EPaletteCompression
@return KErrNone on success, KErrGeneral if the bitmap handle is NULL, KErrAccessDenied if 
the bitmap is in ROM or it is an extended bitmap, otherwise one of the system wide error codes. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::Compress(TBitmapfileCompressionScheme aScheme)
	{
 	if (!iHandle)
 		return KErrGeneral;
 	if (iFlags & EIsReadOnlyBitmapMask)
 		return KErrAccessDenied; 	
	TPckgBuf<TBmpHandles> handlebuf;
	TIpcArgs args(iHandle, aScheme, &handlebuf);
	TInt err = iFbs->SendCommand(EFbsMessBitmapCompress, args);
	if (err != KErrNone)
		return err;
	iHandle = handlebuf().iHandle;
	iServerHandle = handlebuf().iServerHandle;
	iAddressPointer = (CBitwiseBitmap*)(iFbs->HeapBase() + handlebuf().iAddressOffset);
	return KErrNone;
	}

/** Submits the bitmap for asynchronous background compression.
@param aRequestStatus The request status which will be completed with the appropriate error code after the compression has finished
The error code will be KErrNone on success, KErrGeneral if the bitmap handle is NULL, KErrAccessDenied if the
bitmap is in ROM or it is an extended bitmap, otherwise one of the system wide error codes.
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::CompressInBackground(TRequestStatus& aRequestStatus)
	{
	CompressInBackground(aRequestStatus, ERLECompression);
	}

/** Submits the bitmap for asynchronous background compression. No notification will be provided when the compression has completed.
@return KErrNone if the bitmap was successfully submitted to the background compression queue, KErrGeneral if the bitmap handle is NULL, 
KErrAccessDenied if the bitmap is in ROM or it is an extended bitmap,  otherwise another of the system-wide error codes.
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::CompressInBackground()
	{
	return CompressInBackground(ERLECompression);
	}
	
/** Submits the bitmap for asynchronous background compression.
@param aRequestStatus The request status which will be completed with the appropriate error code after the compression has finished.
The error code will be KErrNone on success, KErrGeneral if the bitmap handle is NULL, KErrAccessDenied if the
bitmap is in ROM or it is an extended bitmap, otherwise one of the system wide error codes.
@param aScheme Specifies preferred compression type: ERLECompression or EPaletteCompression
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::CompressInBackground(TRequestStatus& aRequestStatus, TBitmapfileCompressionScheme aScheme)
	{
	TRequestStatus* reqStat = &aRequestStatus;
	aRequestStatus = KRequestPending;
	if (!iHandle)
		User::RequestComplete(reqStat, KErrGeneral);
	else if (iFlags & EIsReadOnlyBitmapMask)
		User::RequestComplete(reqStat, KErrAccessDenied);	
	else
		{
		TIpcArgs args(iHandle, aScheme, ETrue);
		iFbs->SendCommand(EFbsMessBitmapBgCompress, args, aRequestStatus);
		}
	}

/** Submits the bitmap for asynchronous background compression. No notification will be provided when the compression has completed.
@return KErrNone if the bitmap was successfully submitted to the background compression queue, KErrGeneral if the bitmap handle is NULL, 
KErrAccessDenied if the bitmap is in ROM or it is an extended bitmap, otherwise another of the system-wide error codes.
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::CompressInBackground(TBitmapfileCompressionScheme aScheme)
	{
	if (!iHandle)
		return KErrGeneral;
	if (iFlags & EIsReadOnlyBitmapMask)
		return KErrAccessDenied;	
	TIpcArgs args(iHandle, aScheme, EFalse);
	return iFbs->SendCommand(EFbsMessBitmapBgCompress, args);
	}

/**Tests whether the bitmap located in RAM has been compressed.
@return ETrue if the bitmap is compressed, EFalse otherwise.
@publishedAll
@released
*/	
EXPORT_C TBool CFbsBitmap::IsCompressedInRAM() const
	{
	CBitwiseBitmap* bitmap = CleanAddress();
	if (bitmap==NULL)
		{
		return EFalse;
		}	
	return bitmap->IsCompressedInRAM();
	}

/** Gets the twip-size of the bitmap.
@return The size of the bitmap, in twips. 
@publishedAll 
@released
*/
EXPORT_C TSize CFbsBitmap::SizeInTwips() const
	{
    TSize zero;
	if(iHandle==NULL) return(zero);
	return CleanAddress()->SizeInTwips();
	}

/** Converts a vertical dimension on the graphics device from pixels to twips.
@param aPixels A vertical dimension on the graphics device in pixels. 
@return A vertical dimension on the graphics device in twips. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::VerticalPixelsToTwips(TInt aPixels) const
	{
	if(iHandle==NULL) return(0);
	return CleanAddress()->VerticalPixelsToTwips(aPixels);
	}

/** Converts a vertical dimension on the graphics device from twips to pixels.
@param aTwips A vertical dimension on the graphics device in twips. 
@return A vertical dimension on the graphics device in pixels. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::VerticalTwipsToPixels(TInt aTwips) const
	{
	if(iHandle==NULL) return(0);
	return CleanAddress()->VerticalTwipsToPixels(aTwips);
	}

/** Tests whether or not the specified file is in ROM.
@param aFilename The name of the file. 
@param aWord On return, contains the address of the file in ROM. 
@return ETrue if the file is in the ROM; EFalse otherwise. 
@publishedAll 
@released
*/
EXPORT_C TBool CFbsBitmap::IsFileInRom(const TDesC& aFilename,TUint32*& aWord)
	{
	RFbsSession* fbs=RFbsSession::GetSession();
	__ASSERT_ALWAYS(fbs,Panic(EFbsPanicNoConnection));
	return fbs->LookupBitmapInROM (aFilename, (TAny*&)aWord);
	}

/** Tests whether or not the specified file is in ROM.
@param aFile The file handle
@param aWord On return, contains the address of the file in ROM. 
@return ETrue if the file is in the ROM; EFalse otherwise. 
@publishedAll 
@released
*/
EXPORT_C TBool CFbsBitmap::IsFileInRom(RFile& aFile,TUint32*& aWord)
	{
	// cannot use rom lookup cache as filename is not available
	// offset must be initialised to zero to indicate beginning of the file
	aWord = 0;
	return aFile.Seek(ESeekAddress,(TInt&)aWord)==KErrNone;
	}
	
/** Tests whether or not the bitmap is monochrome.
Monochrome bitmaps have a display-mode of 1 bit-per-pixel.
@return ETrue if the bitmap is monochrome; EFalse otherwise. 
@publishedAll 
@released
*/
EXPORT_C TBool CFbsBitmap::IsMonochrome() const
	{
	if(!iHandle) return(EFalse);
	TUint32* data;
	CBitwiseBitmap* bmp = BeginDataAccessAndGetCleanAddress(data);
	TBool isMonochrome = bmp->IsMonochrome(data);
	EndDataAccess(ETrue);
	return isMonochrome;
	}

/** Marks the beginning of direct access to the bitmap data.
This function prepares the bitmap for direct access to its pixel data
and should be used before calling DataAddress(), otherwise performance
may be degraded on certain platforms.
Calls to BeginDataAccess() must be coupled with subsequent calls to EndDataAccess().

@publishedAll
@released
@see CFbsBitmap::DataAddress()
@see CFbsBitmap::EndDataAccess()
*/
EXPORT_C void CFbsBitmap::BeginDataAccess() const
	{
    FBS_OST_VERBOSE(OstTrace1(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_BEGINDATAACCESS_ENTRY, "> this=0x%08x;", (TUint)this););
	FBS_OST_IF(!iHandle, OstTrace1(TRACE_ERROR, CFBSBITMAP_BEGINDATAACCESS_ERROR, "! this=0x%08x; !iHandle", (TUint)this););
	
	if (iHandle)
	    {
        (void)CleanAddress();	//called for side-effect to make sure bitmap reference is current. Should be low overhead.
        const_cast<CFbsBitmap*>(this)->iUseCount++;
	    }
    
	FBS_OST_VERBOSE(OstTraceExt2(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_BEGINDATAACCESS_EXIT, "< this=0x%08x; iUseCount=%d;", (TUint)this, const_cast<CFbsBitmap*>(this)->iUseCount);)
	}

/** Marks the end of direct access to the bitmap data.
Use this function after ending direct access to the bitmap data.
Calls to EndDataAccess() must correspond to prior calls to BeginDataAccess().
See BeginDataAccess() for more details.

@param aReadOnly Whether or not the bitmap data has only been read since
                 the corresponding call to BeginDataAccess().

@publishedAll
@released
@param aReadOnly True if the bitmap data had only been read.  False if the data has been modified.
@see CFbsBitmap::BeginDataAccess()
*/
EXPORT_C void CFbsBitmap::EndDataAccess(TBool aReadOnly) const
	{
    FBS_OST_VERBOSE(OstTraceExt2(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_ENDDATAACCESS_ENTRY, "> this=0x%08x; aReadOnly=%d;", (TUint)this, (TUint)aReadOnly);)
    FBS_OST_IF(!iHandle, OstTrace1(TRACE_ERROR, CFBSBITMAP_ENDDATAACCESS_ERROR, "! this=0x%08x; !iHandle", (TUint)this););
    if (iHandle)
        {
        const_cast<CFbsBitmap*>(this)->iUseCount--;
        if (!aReadOnly && !(iFlags & EIsReadOnlyBitmapMask))
            {
            User::LockedInc(iAddressPointer->Extra()->iTouchCount);
            }
        }
    FBS_OST_VERBOSE(OstTraceExt2(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_ENDDATAACCESS_EXIT, "< this=0x%08x; iUseCount=%d;", (TUint)this, const_cast<CFbsBitmap*>(this)->iUseCount);)
	}

/** Locks the global bitmap heap.
This function is deprecated, since it is no longer necessary to lock the global
bitmap heap to prevent the pixel data from being moved in memory asynchronously,
as the value returned by DataAddress() can now only change as a result of bitmap
operations explicitly requested by clients of the Font and Bitmap Server.
Calls to LockHeap() should be replaced by calls to BeginDataAccess().

Calls to LockHeap() must be coupled with subsequent calls to CFbsBitmap::UnlockHeap().
Code called between a LockHeap() - UnlockHeap() pair must not include any other calls to
CFbsBitmap methods, which internally may call CFbsBitmap::LockHeap(). Also, code must
not leave between a LockHeap() - UnlockHeap() pair.
@note	IMPORTANT: CFbsBitmap::LockHeap() cannot be used as a means of synchronization between
threads concurrently accessing bitmap data.

@publishedAll 
@deprecated
@see CFbsBitmap::UnlockHeap()
@see CFbsBitmap::BeginDataAccess()
*/
EXPORT_C void CFbsBitmap::LockHeap(TBool /*aAlways*/) const
	{
    FBS_OST(OstTrace1(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_LOCKHEAP_ENTRY, "> this=0x%08x;", (TUint)this);)
	BeginDataAccess();
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	//These debug checks now refer to the cleaned data address
    FBS_OST_IF(!iHandle, OstTrace1(TRACE_ERROR, CFBSBITMAP_LOCKHEAP_ERROR, "! this=0x%08x; !iHandle", (TUint)this););
	if (iHandle && !(iFlags & EIsRomBitmap)) // can't do anything with ROM bitmaps
		{
		TThreadId threadId = RThread().Id();
		iFbs->iHelper->iDebugMutex.Wait();
		if (iAddressPointer->Extra()->iLockCount++ == 0)
			{
			__ASSERT_ALWAYS(iAddressPointer->Extra()->iThreadId == TThreadId(KNullThreadId), Panic(EFbsPanicBadHeapLock));
			iAddressPointer->Extra()->iThreadId = threadId;
			}
		else
			__ASSERT_ALWAYS(iAddressPointer->Extra()->iThreadId == threadId, Panic(EFbsPanicBadHeapLock));
		iFbs->iHelper->iDebugMutex.Signal();
		}
#endif
	FBS_OST(OstTrace1(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_LOCKHEAP_EXIT, "< this=0x%08x;", (TUint)this);)
	}

/** Unlocks the global heap. 
This function is deprecated. See LockHeap() for more details.
Calls to UnlockHeap() should be replaced by calls to EndDataAccess().
Calls to UnlockHeap() must correspond to prior calls to LockHeap() or LockHeapLC().

@publishedAll 
@deprecated
@see CFbsBitmap::LockHeap()
@see CFbsBitmap::EndDataAccess()
*/
EXPORT_C void CFbsBitmap::UnlockHeap(TBool /*aAlways*/) const
	{
    FBS_OST(OstTrace1(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_UNLOCKHEAP_ENTRY, "> this=0x%08x;", (TUint)this);)
    FBS_OST_IF(!iHandle, OstTrace1(TRACE_ERROR, CFBSBITMAP_UNLOCKHEAP_ERROR, "! this=0x%08x; !iHandle", (TUint)this););
	if (iHandle)
	    {
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	    if (!(iFlags & EIsRomBitmap)) // can't do anything with ROM bitmaps
            {
            TThreadId threadId = RThread().Id();
            iFbs->iHelper->iDebugMutex.Wait();
            __ASSERT_ALWAYS(iAddressPointer->Extra()->iLockCount > 0, Panic(EFbsPanicBadHeapLock));
            __ASSERT_ALWAYS(iAddressPointer->Extra()->iThreadId == threadId, Panic(EFbsPanicBadHeapLock));
            if (--iAddressPointer->Extra()->iLockCount == 0)
                iAddressPointer->Extra()->iThreadId = TThreadId(KNullThreadId);
            iFbs->iHelper->iDebugMutex.Signal();
            }
#endif
	    EndDataAccess();
	    }
	FBS_OST(OstTrace1(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_UNLOCKHEAP_EXIT, "< this=0x%08x;", (TUint)this);)
	}

/** Locks the global bitmap heap, leaving on the clean-up stack a pointer
to a TCleanupItem that unlocks the heap on deletion.
Use this function instead of LockHeap() if code may leave between the
LockHeap() - UnlockHeap() pair. Calls to LockHeapLC() must be coupled with
subsequent calls to CFbsBitmap::UnlockHeap() or CleanupStack::PopAndDestroy().
This function is deprecated. See CFbsBitmap::LockHeap() for more details.

@publishedAll 
@deprecated
@see CFbsBitmap::LockHeap()
*/
EXPORT_C void CFbsBitmap::LockHeapLC(TBool /*aAlways*/) const
	{
    FBS_OST(OstTrace1(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_LOCKHEAPLC_ENTRY, "> this=0x%08x;", (TUint)this);)
    LockHeap();
	TCleanupItem cleanitem(CFbsBitmap::UnlockHeap, (TAny*)this);
	CleanupStack::PushL(cleanitem);
	FBS_OST(OstTrace1(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_LOCKHEAPLC_EXIT, "< this=0x%08x;", (TUint)this);)
	}

EXPORT_C void CFbsBitmap::UnlockHeap(TAny* aFbsBitmap)
	{
    FBS_OST(OstTrace1(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_UNLOCKHEAP2_ENTRY, "> bitmap=0x%08x;", (TUint)aFbsBitmap);)
    ((CFbsBitmap*)aFbsBitmap)->UnlockHeap();
    FBS_OST(OstTrace0(GRAPHICS_CONTROL_FUNCTIONS, CFBSBITMAP_UNLOCKHEAP2_EXIT, "<");)
	}

/** Tests whether the bitmap is volatile.
A bitmap becomes volatile if CFbsBitmap::DataAdress() is called without
CFbsBitmap::BeginDataAccess() having been called before and it can become non-volatile
again if a resizing or compression is performed.

@internalTechnology
@prototype
*/
EXPORT_C TBool CFbsBitmap::IsVolatile() const
	{
	if (!iHandle)
		return EFalse;
	return CleanAddress()->iSettings.IsVolatileBitmap();
	}

/** Tests how many times the bitmap has been touched.
A bitmap is touched whenever CFbsBitmap::EndDataAccess() is called with the parameter
aReadOnly set to EFalse and also whenever a resizing or compression is performed.

@internalTechnology
@prototype
@return The number of times the bitmap has been touched.
*/
EXPORT_C TInt CFbsBitmap::TouchCount() const
	{
	if (!iHandle || (iFlags & EIsReadOnlyBitmapMask))
		return 0; // A read-only bitmap can never be touched.
	return CleanAddress()->Extra()->iTouchCount;
	}

/** Returns the serial number of the bitmap
The serial number is unique to this bitmap.
The serial number is a signed 64-bit integer, with only the positive values being assigned.
As ROM bitmaps do not have serial numbers, the serial number will use the negative range
of values so that ROM bitmap's serial number cannot be the same as a RAM bitmap's.
ROM bitmap's address pointers are unique to the ROM bitmap, so the serial number will just
be negative value of the address pointer.

@internalTechnology
@prototype
@return The unique serial number for the bitmap
*/
EXPORT_C TInt64 CFbsBitmap::SerialNumber() const
	{
	if (!iHandle)
		return 0;
	if (iFlags & EIsRomBitmap)
		return -TInt64(reinterpret_cast<TUint32>(iAddressPointer));
	return CleanAddress()->Extra()->iSerialNumber;
	}

/** Tests whether the bitmap is large.
@return ETrue if the bitmap is large, EFalse if not. 
@publishedAll 
@released
*/
EXPORT_C TBool CFbsBitmap::IsLargeBitmap() const
	{
	CBitwiseBitmap* bitmap = CleanAddress();
	if (!bitmap)
		{
		return EFalse;
		}
	return bitmap->IsLargeBitmap();
	}

/** Returns the handle for the hardware bitmap which this CFbsBitmap is using.
@return The handle to the hardware bitmap. The handle is NULL if it is not 
a hardware bitmap. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::HardwareBitmapHandle() const
	{
	CBitwiseBitmap* bitmap = CleanAddress();
	if (!bitmap)
		{
		return 0;
		}
	return bitmap->HardwareBitmapHandle();
	}

/** Gets the attributes of the bitmap's palette.
This is not currently supported.
@param aModifiable On return, whether or not the palette is modifiable. 
@param aNumEntries On return, the number of entries in the palette. 
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const
	{
	aModifiable=EFalse;
	aNumEntries=0;
	}

/** Sets the bitmap's palette.
This is not currently supported.
@param aPalette Not used. 
@publishedAll 
@released
*/
EXPORT_C void CFbsBitmap::SetPalette(CPalette* /*aPalette*/)
	{
	}

/** Gets the bitmap's palette.
This is not currently supported.
@param aPalette Not used. 
@return KErrNotSupported. 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsBitmap::GetPalette(CPalette*& /*aPalette*/) const
	{
	return(KErrNotSupported);
	}

/**
@internalComponent
This method loads a bitmap from an opened file handle.

@param  aFile mbm or rsc file handle (rsc file format: header + rsc
	data section + mbm file section).
@param  aId Bitmap ID - should be less than mbm file bitmaps count.
@param  aShareIfLoaded Specifies whether or not the loaded bitmap will be
    made available for sharing between FBSERV clients.
@param  aFileOffset mbm file section offset into rsc file.
@return KErrNone if successful, otherwise another
            of the system-wide error codes.
*/	
TInt CFbsBitmap::DoLoad(RFile& aFile,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset)
	{
	TInt ret=KErrNone;
	TPckgBuf<TBmpHandles> handlebuf;
	TPckgBuf<TLoadBitmapArg> loadBitmapArg;
	loadBitmapArg().iBitmapId = aId;
	loadBitmapArg().iShareIfLoaded = TInt(aShareIfLoaded);
	loadBitmapArg().iFileOffset = aFileOffset;
	//Getting the RFs Handle(2) and RFile handle(3) into a TIpcArgs into 2nd and 3rd argument
	TIpcArgs fileargs;
	ret=aFile.TransferToServer(fileargs,2,3);
	if (ret!=KErrNone)
		return ret;	
	fileargs.Set(0,&handlebuf);
	fileargs.Set(1,&loadBitmapArg);
	ret=iFbs->SendCommand(EFbsMessBitmapLoad,fileargs);
	if(ret!=KErrNone) return(ret);
	iHandle=handlebuf().iHandle;
	iServerHandle=handlebuf().iServerHandle;
	iAddressPointer=(CBitwiseBitmap*)(iFbs->HeapBase()+handlebuf().iAddressOffset);
	ret = iFbs->iHelper->AddBitmap(*this);
	if (ret != KErrNone)
		return ret;
	return iFbs->AllocScanLineBuffer(iAddressPointer->iByteWidth+4);
	}

/**
@internalComponent
This method loads a bitmap from the mbm or rsc file specified by the filename.

@param  aFileName mbm or rsc file name (rsc file format: header + rsc
	data section + mbm file section).
@param  aId Bitmap ID - should be less than mbm file bitmaps count.
@param  aShareIfLoaded Specifies whether or not the loaded bitmap will be
    made available for sharing between FBSERV clients.
@param  aFileOffset mbm file section offset into rsc file.
@return KErrNone if successful, otherwise another
            of the system-wide error codes.
*/
TInt CFbsBitmap::DoLoad(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset)
	{
	TInt ret=KErrNone;
	TPckgBuf<TBmpHandles> handlebuf;
	TPckgBuf<TLoadBitmapArg> loadBitmapArg;
	loadBitmapArg().iBitmapId = aId;
	loadBitmapArg().iShareIfLoaded = TInt(aShareIfLoaded);
	loadBitmapArg().iFileOffset = aFileOffset;
	TIpcArgs fileargs;
	fileargs.Set(0,&handlebuf);
	fileargs.Set(1,&loadBitmapArg);
	fileargs.Set(2,&aFileName);
	ret=iFbs->SendCommand(EFbsMessBitmapLoadFast,fileargs);
	if(ret!=KErrNone) return(ret);
	iHandle=handlebuf().iHandle;
	iServerHandle=handlebuf().iServerHandle;
	iAddressPointer=(CBitwiseBitmap*)(iFbs->HeapBase()+handlebuf().iAddressOffset);
	ret = iFbs->iHelper->AddBitmap(*this);
	if (ret != KErrNone)
		return ret;
	return iFbs->AllocScanLineBuffer(iAddressPointer->iByteWidth+4);
	}

/**
@internalComponent
This method handles very special case when the rsc file is in RAM, but it 
contains ROM mbm file.  ROM mbm file format is different than RAM mbm file 
format and ROM mbm file cannot be loaded into RAM using standard techniques 
(used by CFbsBitmap::DoLoad()). We have to check it is really a ROM mbm file. 
If it is - we have to allocate the right amount of RAM, read and copy 
requested ROM bitmap to the allocated RAM.

@leave KErrNotSupported if this is a RAM rsc file with ROM mbm file section, 
	or any of the RFile related error codes.
@param aFileName rsc file name (rsc file format: header + rsc data section + 
	mbm file section).
@param  aId Bitmap ID - should be less than mbm file bitmaps count.
@param aFileOffset mbm file section offset into rsc file.
@return EFalse - this is not a ROM mbm file. ETrue - this is a ROM mbm file 
			and	requested by aId bitmmap is loaded.
*/	
TBool CFbsBitmap::LoadShiftedRomBmpL(const TDesC& aFileName, TInt32 aId, TUint aFileOffset)
	{
	RFile mbm_file;
	::CleanupClosePushL(mbm_file);
	User::LeaveIfError(mbm_file.Open(iFbs->FileServer(), aFileName, EFileRead | EFileShareReadersOnly));
	TInt pos = static_cast <TInt> (aFileOffset);
	User::LeaveIfError(mbm_file.Seek(ESeekStart, pos));//move to the beginning of the mbm file section
	TBuf8<sizeof(CBitwiseBitmap)> buf;
	//Check if it is a ROM mbm file
	User::LeaveIfError(mbm_file.Read(buf, sizeof(KMultiBitmapRomImageUid.iUid)));//Bitmap file UID - ROM or RAM
	TInt32 mbm_uid = *(reinterpret_cast <const TInt32*> (buf.Ptr())); 
	TBool loaded = EFalse;
	if(mbm_uid == KMultiBitmapRomImageUid.iUid)
		{
		if(!KRomMBMInRamRSC)
			{
			User::Leave(KErrNotSupported);
			}
		else
			{
			User::LeaveIfError(mbm_file.Read(buf, sizeof(TInt32)));//Number of bitmaps
			TInt32 bmp_cnt = *(reinterpret_cast <const TInt32*> (buf.Ptr()));  
			if(aId >= bmp_cnt) 
				{
				User::Leave(KErrNotFound);
				}
			for(TInt i=0;i<aId;i++) //Read bitmap UIDs located before aId.
				{
				User::LeaveIfError(mbm_file.Read(buf, sizeof(aId)));
				}
			User::LeaveIfError(mbm_file.Read(buf, sizeof(TInt32)));//Read the offset of aId bitmap.
			TInt bmp_offset = *(reinterpret_cast <const TInt32*> (buf.Ptr())) + TInt(aFileOffset); 
			pos = static_cast <TInt> (bmp_offset);
			User::LeaveIfError(mbm_file.Seek(ESeekStart, pos));//move the file pointer to the bitmap
			User::LeaveIfError(mbm_file.Read(buf, sizeof(CBitwiseBitmap)));//Read CBitwiseBitmap data (without the bitmap data)
			const CBitwiseBitmap* bmp = reinterpret_cast <const CBitwiseBitmap*> (buf.Ptr());
			//Calculate bitmap data size, alocate enough memory for the bitmap, copy CBitwiseBitmap
			//members first, read the bitmap data from the file, copy the data to the allocated memory,
			//initialize iRomPointer.
			//If sizeof(CBitwiseBitmap) != real size of CBitwiseBitmap then we could have problems,
			//because bitmap data won't be copied at the right position.
			TInt size = bmp->iHeader.iBitmapSize - sizeof(SEpocBitmapHeader) + sizeof(CBitwiseBitmap);
			TUint8* bmp_mem = new (ELeave) TUint8[size];
			//There is no need bmp_mem to be aligned, because it is a pointer to a RAM block of memory.
			TCleanupItem cleanitem(FreeMem, bmp_mem);
			CleanupStack::PushL(cleanitem);
			Mem::Copy(bmp_mem, bmp, sizeof(CBitwiseBitmap));
			TPtr8 pbmp(bmp_mem + sizeof(CBitwiseBitmap), size - sizeof(CBitwiseBitmap));
			User::LeaveIfError(mbm_file.Read(pbmp, size - sizeof(CBitwiseBitmap)));//read the bitmap data. We've already read the CBitwiseBitmap data.
			CleanupStack::Pop(bmp_mem);
			iAddressPointer = reinterpret_cast<CBitwiseBitmap*>(bmp_mem);
			iFlags = EIsRomBitmap;
			iHandle = 1;
			loaded = ETrue;
			}//end of - if(!KRomMBMInRamRSC) - "else" part
		}//end of - if(mbm_uid == KMultiBitmapRomImageUid.iUid)
	CleanupStack::PopAndDestroy();//mbm_file
	return loaded;
	}

/**
Swaps the bitmap's width and height.
For example, if the bitmap's size is (40, 20), the new size will be (20, 40).
Bitmap content is not preserved.
@publishedAll
@released
@return KErrNone if the call was successful, KErrGeneral if the bitmap handle is 
invalid, KErrAccessDenied if the bitmap is in ROM, KErrNotSupported if the bitmap
is a hardware bitmap or an extended bitmap.
*/
EXPORT_C TInt CFbsBitmap::SwapWidthAndHeight()
	{
	if(!iHandle) 
		{
		return KErrGeneral;
		}	
	TUint32* data;
	CBitwiseBitmap* bmp = BeginDataAccessAndGetCleanAddress(data);

	// Check the new bitmap size here then decide whether to swap the bitmap on the 
	// client side or send it to be done on the server and reallocate memory for it.
	TInt newWidthInBytes = CBitwiseBitmap::ByteWidth(bmp->iHeader.iSizeInPixels.iHeight, bmp->iSettings.CurrentDisplayMode());
	TInt64 hugeDataSize = TInt64(bmp->iHeader.iSizeInPixels.iWidth) * TInt64(newWidthInBytes);

	TInt err = KErrNone;
	// If the size of the new swapped bitmap is less than or equal its original size before the swap,
	// then we do not need to reallocate memory. The swapping is straight forward.
	if( I64HIGH(hugeDataSize) == 0 && I64LOW(hugeDataSize) <= TUint(bmp->iHeader.iBitmapSize - bmp->iHeader.iStructSize) )	
		{
		err = bmp->SwapWidthAndHeight(data);
		// Even though used DataAddress() as read-only, need to increment touch count, so indicate that data has been written
		EndDataAccess(EFalse);
		}
	// Otherwise we need to reallocate memory. We do this by using the already exisitng 
	// Resize() function as a work around- Code Reusability!!
	else
		{
		EndDataAccess(ETrue); // Used DataAddress() to read only.
		// Resize will increase touch counter
		err = Resize(TSize(bmp->iHeader.iSizeInPixels.iHeight, bmp->iHeader.iSizeInPixels.iWidth));
		}
	return err;
	}
	
/** Gets a pointer to the decompression buffer owned by this thread's FBServ session.
@param aSize The size in bytes of the scan lines to decompress.
@return A pointer to the decompression buffer or NULL if there is no FBServ session.
@internalTechnology
@released
*/
EXPORT_C HBufC8* CFbsBitmap::GetDecompressionBuffer(TInt aSize)
	{
		RFbsSession* ses=RFbsSession::GetSession();
		return ses? ses->GetDecompressionBuffer(aSize) : NULL;
	}

/** Gets a pointer to the rasterizer for extended bitmaps if present.
@return A pointer to the rasterizer owned by this thread's FBServ session.
@return NULL if the rasterizer is not present.
@internalTechnology
@prototype
*/
EXPORT_C CFbsRasterizer* CFbsBitmap::Rasterizer()
	{
	RFbsSession* session = RFbsSession::GetSession();
	return session ? session->iHelper->Rasterizer() : NULL;
	}

/** Loads a specific bitmap from an opened multi-bitmap file handle.
The bitmap may be shared by other font and bitmap server clients.
@param aFile The handle of the multi-bitmap (.mbm) file. 
@param aId The bitmap identifier.
@param aShareIfLoaded Specifies whether or not the loaded bitmap will be made 
available for sharing between font and bitmap server clients. 
@return KErrNone if successful, otherwise another of the system error 
codes. 
@publishedAll
@released
*/	
EXPORT_C TInt CFbsBitmap::Load(RFile& aFile,TInt32 aId/*=0*/,TBool aShareIfLoaded/*=ETrue*/)
	{
    FBS_OST(TFullName fileName;)
    FBS_OST(aFile.FullName(fileName);)
    FBS_OST(OstTraceExt4( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOAD3_ENTRY, "> this=0x%08x; file=%S; id=0x%08x; share=%d", (TUint)this, fileName, aId, aShareIfLoaded);)
	TInt ret = Load(aFile,aId,aShareIfLoaded,0);
    FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOAD3_EXIT, "< this=0x%08x, ret=%d", (TUint)this, ret);) 
	return ret;
	}

/** Loads a specific bitmap from an opened multi-bitmap file handle.
The bitmap may be shared by other font and bitmap server clients.
@param aFile The handle of the multi-bitmap (.mbm) file. 
@param aId The bitmap identifier.
@param aShareIfLoaded  Specifies whether or not the loaded bitmap will be made 
available for sharing between FBSERV clients.
@param aFileOffset Bitmap file section offset within the file.
@return KErrNone if successful, otherwise another of the system error codes.
@publishedAll
@released
*/	
EXPORT_C TInt CFbsBitmap::Load(RFile& aFile,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset)
	{
    TInt err = KErrNone;
    FBS_OST(TFullName fileName;)
    FBS_OST(aFile.FullName(fileName);)
    FBS_OST(OstTraceExt5( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOAD4_ENTRY, "> this=0x%08x; file=%S; id=0x%08x; share=%d; off=%d", (TUint)this, fileName, aId, aShareIfLoaded, aFileOffset);)
	if (!iFbs)
		{
        FBS_OST(OstTrace1( TRACE_ERROR, CFBSBITMAP_LOAD4_ERROR, "! this=0x%08x; !iFbs", (TUint)this);)
		err = KErrCouldNotConnect;
		}
	else
	    {
        Reset();
        TUint32* rompointer;
        IsFileInRom(aFile,rompointer);
        TBool romPointerValid;
        err = DoLoadFromRom(rompointer, aId, aFileOffset, romPointerValid);
        if (!romPointerValid)
            {
            err = DoLoad(aFile,aId,aShareIfLoaded,aFileOffset);
            FBS_OST_IF(err!=KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_LOAD4_ERROR2, "! this=0x%08x; DoLoad() returned %d", (TUint)this, err);)
            }
	    }
	FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOAD4_EXIT, "< this=0x%08x; err=%d", (TUint)this, err);)
	return err;
	}

/** Loads and compresses a specific bitmap from an opened multi-bitmap file handle.
The bitmap may be shared by other font and bitmap server clients.
If the bitmap is loaded from ROM then compression is not allowed.
@param aFile The handle of the multi-bitmap (.mbm) file. 
@param aId The bitmap identifier.
@param aShareIfLoaded Specifies whether or not the loaded bitmap will be
made available for sharing between FBSERV clients.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@publishedAll 
@released
*/	
EXPORT_C TInt CFbsBitmap::LoadAndCompress(RFile& aFile,TInt32 aId/*=0*/,TBool aShareIfLoaded/*=ETrue*/)
	{
    FBS_OST(TFullName fileName;)
    FBS_OST(aFile.FullName(fileName);)
    FBS_OST(OstTraceExt4( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOADANDCOMPRESS3_ENTRY, "> this=0x%08x; file=%S; id=0x%08x; share=%d", (TUint)this, fileName, aId, aShareIfLoaded);)      
	TInt ret = LoadAndCompress(aFile,aId,aShareIfLoaded,0);
    FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOADANDCOMPRESS3_EXIT, "< this=0x%08x; ret=%d", (TUint)this, ret);)
    return ret;
	}

/** Loads and compresses a specific bitmap from an opened multi-bitmap file handle.
The bitmap may be shared by other font and bitmap server clients. If the 
bitmap is loaded from ROM then compression is not allowed.
@param aFile The handle of the multi-bitmap (.mbm) file. 
@param aId The bitmap identifier.
@param aShareIfLoaded Specifies whether or not the loaded bitmap will be made 
available for sharing between FBSERV clients.
@param aFileOffset Bitmap file section offset within the file.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@publishedAll 
@released
*/	
EXPORT_C TInt CFbsBitmap::LoadAndCompress(RFile& aFile,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset)
	{
    FBS_OST(TFullName fileName;)
    FBS_OST(aFile.FullName(fileName);)
    FBS_OST(OstTraceExt4( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOADANDCOMPRESS4_ENTRY, "> this=0x%08x; file=%S; id=0x%08x; share=%d", (TUint)this, fileName, aId, aShareIfLoaded);)
	TInt err = Load(aFile,aId,aShareIfLoaded,aFileOffset);
	if (err == KErrNone)
		{
        if (!(iFlags & EIsRomBitmap))
            {
            err = Compress();
            FBS_OST_IF(err!=KErrNone, OstTraceExt2( TRACE_ERROR, CFBSBITMAP_LOADANDCOMPRESS4_ERROR, "! this=0x%08x; Compress() returned %d", (TUint)this, err);)
            }
        else
            {
            err = KErrAccessDenied;
            FBS_OST(OstTraceExt2( TRACE_ERROR, CFBSBITMAP_LOADANDCOMPRESS4_ERROR2, "! this=0x%08x; Cannot compress bitmap in ROM; iFlags=0x%08x", (TUint)this, (TUint)iFlags);)
            }
		}
	FBS_OST(OstTraceExt2( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_LOADANDCOMPRESS4_EXIT, "< this=0x%08x; err=%d", (TUint)this, err);)
	return err;
	}

/** Gets all the bitmap handles for all the bitmaps stored in the Font Bitmap Server. There is a limit of
the number of bitmaps that can be retrieved defined by KMaxBitmapHandleBufferSize. If this limit has been
reached then KErrOverflow will be returned.
@param aBitmapIdArray returns an array of all the bitmap handles
@return KErrNone if successful, KErrOverflow if the bitmapBuffer is not large enough to store 
all the bitmap handles, otherwise another of the system-wide error codes. 
@capability ReadDeviceData
@internalComponent
@released
*/	
EXPORT_C TInt CFbsBitmap::GetAllBitmapHandles(RArray<TInt>& aBitmapIdArray) const
	{
	RBuf8 bitmapBuffer;
	TInt ret = bitmapBuffer.Create(KMaxBitmapHandleBufferSize);
	if (ret==KErrNone)
		{
		TIpcArgs args(&bitmapBuffer);
		ret=iFbs->SendCommand(EFbsGetAllBitmapHandles, args);
		if (ret==KErrNone)
			{
			// Convert data returned from server and place into the RArray (aBitmapIdArray)
			aBitmapIdArray.Reset();
			TInt* bitmapIdPtr = (TInt*)bitmapBuffer.Ptr();
			const TInt numBitmapIds = bitmapBuffer.Size() / KNumBytesPerBitmapHandle; // Divide by number of bytes per bitmap handle to get total number of bitmap IDs
			for (TInt count=0; count<numBitmapIds; ++count)
				{
				TInt bitmapId = *bitmapIdPtr++;
				ret = aBitmapIdArray.Append(bitmapId);
				if (ret!=KErrNone)
					break;
				}
			}
		}
	bitmapBuffer.Close();
	return ret;
	}

/**
@internalComponent
This method tries to load a bitmap if mbm or rsc file is in ROM.

@param  aRomPointer the address of the file in ROM
@param  aId a Bitmap ID which should be less than mbm file bitmaps count.
@param  aFileOffset mbm file section offset into rsc file.
@param  aRomPointerValid on output it is set to ETrue if aRomPointer points to a valid ROM file or EFalse otherwise.
@return KErrNone if successful, otherwise another of the system-wide error codes.
*/
TInt CFbsBitmap::DoLoadFromRom(TUint32* aRomPointer, TInt32 aId, TUint aFileOffset, TBool& aRomPointerValid)
	{
	aRomPointerValid = ETrue;
	if(aRomPointer)
		{
		TUint8* temp = reinterpret_cast <TUint8*> (aRomPointer);
		__ASSERT_DEBUG(!(TUint(temp) & 0x00000003),Panic(EFbsBitmapAlignment));
		temp += aFileOffset;
		aRomPointer = reinterpret_cast <TUint32*> (temp);
		if(TInt32(*aRomPointer)==KMultiBitmapRomImageUid.iUid)
			{
			TInt numbitmaps = (TInt)*(aRomPointer+1);
			if(aId>=numbitmaps)
				{
				return(KErrEof);
				}
			TInt offset = *(aRomPointer+aId+2);
			iAddressPointer = (CBitwiseBitmap*)(((TUint8*)aRomPointer) + offset);
			iFlags = EIsRomBitmap;
			iHandle = 1;
			return iFbs->AllocScanLineBuffer(iAddressPointer->iByteWidth + 4);
			}
		}
	aRomPointerValid = EFalse;
	return KErrNone;
	}


/**
Creates an extended bitmap. Extended bitmaps are used to store immutable
data in a platform-specific format. They cannot be used as targets of
graphics contexts, and modification of their data via DataAddress() or
TBitmapUtil is not supported and results in undefined behaviour up to
and including process termination.

Initialisation of the raw data of the new bitmap is carried out by copying 
the data pointed to by the parameter aData.

Read-only access to the raw data of an extended bitmap is provided by
DataAddress() and DataSize() in conjunction with BeginDataAccess() and
EndDataAccess().

Extended bitmaps have a conceptual size in pixels and a conceptual
display mode for compatibility purposes. The raw data can be independent
of these properties.

@param aSizeInPixels The conceptual width and height of the new bitmap in pixels.
@param aDispMode The conceptual display mode of the new bitmap.
@param aType The UID identifying the data format of the new bitmap. Used by the
             extended bitmap rasterizer to distinguish between different data types.
@param aData A pointer to the raw data to be stored in the new bitmap.
@param aDataSize The size in bytes of the raw data to be stored in the new bitmap.
@return KErrNone if successful; KErrArgument if the width or height specified in
aSizeInPixels is negative, aDispMode is an invalid display mode, aData is NULL,
aDataSize is negative or zero, or aDataType is a UID reserved for OS use; KErrTooBig
if the width or height specified in aSizeInPixels exceeds KMaxTInt/4, or aDataSize
exceeds KMaxTInt/2; otherwise another of the system-wide error codes.
@publishedPartner
@prototype
@see CFbsBitmap::DataAddress()
@see CFbsBitmap::DataSize()
@see CFbsBitmap::BeginDataAccess()
@see CFbsBitmap::EndDataAccess()
*/
EXPORT_C TInt CFbsBitmap::CreateExtendedBitmap(const TSize& aSizeInPixels, TDisplayMode aDispMode, TUid aType, const TAny* aData, TInt aDataSize)
	{
    TInt err;
    FBS_OST(OstTraceExt5( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_CREATEEXTENDEDBITMAP_ENTRY, "> this=0x%08x; w=%d; h=%d; dm=%d; type=0x%08x", (TUint)this, aSizeInPixels.iWidth, aSizeInPixels.iHeight, aDispMode, aType.iUid);)
	if (!aData || aDataSize == 0)
		{
        FBS_OST(OstTrace1( TRACE_ERROR, CFBSBITMAP_CREATEEXTENDEDBITMAP_ERROR, "! this=0x%08x; (!aData || aDataSize == 0)", (TUint)this);)
		err = KErrArgument;
		}
	else
	    {
        err = DoCreate(aSizeInPixels, aDispMode, aType, aDataSize);
        if (err == KErrNone)
            {
            Mem::Copy(iFbs->iLargeBitmapChunk.Base() + iAddressPointer->iDataOffset, aData, aDataSize);
            }
	    }
	FBS_OST(OstTraceExt4( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_CREATEEXTENDEDBITMAP_EXIT, "< this=0x%08x; err=%d; iH=0x%08x; iSH=0x%08x", (TUint)this, err, iHandle, iServerHandle);)
	return err;
	}

/**
Creates an extended bitmap. Extended bitmaps are used to store immutable
data in a platform-specific format. They cannot be used as targets of
graphics contexts, and modification of their data via DataAddress() or
TBitmapUtil is not supported and results in undefined behaviour up to
and including process termination.

Initialisation of the raw data of the new bitmap is carried out by a 
callback to the MFbsExtendedBitmapInitializer::InitExtendedBitmap() 
function passed through the parameter aInitializer.

Read-only access to the raw data of an extended bitmap is provided by
DataAddress() and DataSize() in conjunction with BeginDataAccess() and
EndDataAccess().

Extended bitmaps have a conceptual size in pixels and a conceptual
display mode for compatibility purposes. The raw data can be independent
of these properties.

@param aSizeInPixels The conceptual width and height of the new bitmap in pixels.
@param aDispMode The conceptual display mode of the new bitmap.
@param aType The UID identifying the data format of the new bitmap. Used by the
             extended bitmap rasterizer to distinguish between different data types.
@param aDataSize The size in bytes of the raw data to be stored in the new bitmap.
@param aInitializer A reference to the initializer of the raw data to be stored in the new bitmap.
@return KErrNone if successful; KErrArgument if the width or height specified in
aSizeInPixels is negative, aDispMode is an invalid display mode, aData is NULL,
aDataSize is negative or zero, or aDataType is a UID reserved for OS use; KErrTooBig
if the width or height specified in aSizeInPixels exceeds KMaxTInt/4, or aDataSize
exceeds KMaxTInt/2; otherwise another of the system-wide error codes.
@publishedPartner
@prototype
@see CFbsBitmap::DataAddress()
@see CFbsBitmap::DataSize()
@see CFbsBitmap::BeginDataAccess()
@see CFbsBitmap::EndDataAccess()
@see MFbsExtendedBitmapInitializer
*/
EXPORT_C TInt CFbsBitmap::CreateExtendedBitmap(const TSize& aSizeInPixels, TDisplayMode aDispMode, TUid aType, TInt aDataSize, MFbsExtendedBitmapInitializer& aInitializer)
	{
    TInt err; 
    FBS_OST(OstTraceExt5( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_CREATEEXTENDEDBITMAP2_ENTRY, "> this=0x%08x; w=%d; h=%d; dm=%d; type=0x%08x;", (TUint)this, aSizeInPixels.iWidth, aSizeInPixels.iHeight, aDispMode, aType.iUid);)
	if (aDataSize == 0)
		{
        FBS_OST(OstTrace1( TRACE_ERROR, CFBSBITMAP_CREATEEXTENDEDBITMAP2_ERROR, "! this=0x%08x; aDataSize == 0", (TUint)this);)
		err = KErrArgument;
		}
	else
	    {
        err = DoCreate(aSizeInPixels, aDispMode, aType, aDataSize);
        if (err == KErrNone)
            {
            err = aInitializer.InitExtendedBitmap(iFbs->iLargeBitmapChunk.Base() + iAddressPointer->iDataOffset, aDataSize);
            if (err != KErrNone)
                {
                Reset();
                }
            }
	    }
	FBS_OST(OstTraceExt4( GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS, CFBSBITMAP_CREATEEXTENDEDBITMAP2_EXIT, "< this=0x%08x; err=%d; iH=0x%08x; iSH=0x%08x", (TUint)this, err, iHandle, iServerHandle);)
	return err;
	}

/**
Gets the UID identifying the data format of an extended bitmap.
@return The UID identifying the data format of the bitmap or
        KNullUid if the bitmap is not an extended bitmap.
@publishedPartner
@prototype
*/
EXPORT_C TUid CFbsBitmap::ExtendedBitmapType() const
	{
	if (iHandle == 0)
		{
		return KNullUid;
		}
	TUid type = CleanAddress()->iUid;
	if (type.iUid == KCBitwiseBitmapUid.iUid || type.iUid == KCBitwiseBitmapHardwareUid.iUid)
		{
		return KNullUid;
		}
	return type;
	}

/**
Gets the size in bytes of the bitmap data.
@return The size in bytes of the bitmap data.
@publishedPartner
@prototype
*/
EXPORT_C TInt CFbsBitmap::DataSize() const
	{
	if (iHandle == 0)
		{
		return 0;
		}
	CBitwiseBitmap* bmp = CleanAddress();
	return bmp->iHeader.iBitmapSize - bmp->iHeader.iStructSize;
	}

/**
Gets a pointer to an extra buffer for general use owned by this thread's FBServ session.
@param aSize The size of the buffer in bytes
@return A pointer to the extra buffer if successful or NULL if there is no FBServ session
@internalTechnology
@released
*/
EXPORT_C HBufC8* CFbsBitmap::GetExtraBuffer(TInt aSize)
	{
		RFbsSession* ses=RFbsSession::GetSession();
		return ses? ses->GetExtraBuffer(aSize) : NULL;
	}
