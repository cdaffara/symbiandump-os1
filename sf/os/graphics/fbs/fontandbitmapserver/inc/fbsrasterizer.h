// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef FBSRASTERIZER_H
#define FBSRASTERIZER_H

#include <e32base.h>
#include <gdi.h>

/** CFbsRasterizer represents the system's rasterizer that has the ability to generate 
standard pixel data from data stored in proprietary formats. 

Multiple formats of proprietary data may be supported by the rasterizer.  Formats are 
identified by UID.  Support for formats is platform-specific and controlled 
by the platform vendor.  The OS does not mandate that the system's rasterizer must 
support any specific data formats.

CFbsRasterizer is used solely by the Font & Bitmap Server component to simulate pixel 
access to extended bitmaps, i.e. bitmaps created using CFbsBitmap::CreateExtendedBitmap().  
CFbsRasterizer is not intended for use by applications.

CFbsRasterizer is intended to be implemented by licensees through derivation from the 
abstract base class.  If a licensee wishes to provide a rasterizer implementation, they 
should create a rasterizer dll that is exported as "fbsrasterizer.dll" and include it in 
their ROM. Note that a stub rasterizer is exported to "fbsrasterizer.dll" and is loaded
by the Font & Bitmap server by default to prevent a rogue dll being loaded in the rasterizer's
place from the C:\ drive or removable media. Licensees can override this by using the 
following build option:
-DFBSRASTERIZER_DLL <rasterizer_dll_name>
where <rasterizer_dll_name> is the name of the licensee rasterizer dll. 
 
CFbsRasterizer will be instantiated in client threads that use the Font & Bitmap Server, 
including the Window Server's main thread.  Therefore the implementation of the rasterizer 
must be robust.  Rasterizer functions should never lead to process termination, even 
when invalid proprietary data is passed in.  Instances of CFbsRasterizer are not intended 
to be shared between threads.

The CFbsRasterizer interface has been designed to allow the implementation to optimise 
certain access patterns for performance.  For example, the implementation might prepare 
for sequential access to a region by rasterizing the entire region inside a private scratch 
buffer.  A further opportunity would be to take advantage of the immutability of extended 
bitmaps by maintaining rasterized scratch buffers for the most recently used bitmaps.  
 */
class CFbsRasterizer : public CBase
    {
public:
	/** Helper class for storing information about a bitmap that is being registered
	with the rasterizer.
	 */
    class TBitmapDesc
        {
    public:
        /** The width and height in pixels to rasterize into. */
        TSize        iSizeInPixels;
        /** The display mode to rasterize into. */
        TDisplayMode iDispMode;
        /** The UID identifying the proprietary data format. */
        TUid         iDataType;
        /** A pointer to the proprietary data. */
        const TAny*  iData;
        /** The size in bytes of the proprietary data. */
        TInt         iDataSize;
        };
    
public:
	/** Create a new rasterizer.
	 */
    IMPORT_C static CFbsRasterizer* New();
    
    /** Registers an extended bitmap so that scanline access requests can be made on 
    the bitmap. The same bitmap may be registered multiple times.  The rasterizer instance 
    must be able to maintain at least 3 registrations simultaneously.  This is to support 
    BitGDI drawing operations that make use of 3 source bitmaps.
    
    @see EndBitmap()
    @see ScanLine()
    
    @param aBitmapId A unique identifier for the bitmap that is being registered.  This 
    	identifier is never reused for other bitmaps.
    @param aBitmapDesc A full description of the bitmap including the proprietary data.  
    	The description and proprietary data associated with a particular value of aBitmapId 
    	never changes.  This allows the implementation to cache this information even after a 
    	bitmap has been unregistered.
    @param aRegionOfInterest The region within which scanline access requests will be made, 
    	if known.  A value of NULL implies that the user does not have any useful information 
    	to contribute and the implementation must be prepared to accept scanline requests 
    	anywhere inside the bitmap.  If the rasterizer uses the region of interest after the 
    	call to BeginBitmap() then it must make a copy of it, since the region pointed to by 
    	aRegionOfInterest is valid only during the call to BeginBitmap().
    
    @post The rasterizer is able to satisfy scanline requests for the bitmap identified by aBitmapId.
     */
    virtual void BeginBitmap(TInt64 aBitmapId,
                             const TBitmapDesc& aBitmapDesc,
                             const TRegion* aRegionOfInterest) = 0;
    
    /** Unregisters a bitmap previously registered using BeginBitmap().
    The rasterizer should access the proprietary data of a bitmap only between
    calls to BeginBitmap() and EndBitmap(), since a bitmap can be destroyed
    without notifying the rasterizer after the call to EndBitmap().
    
    @see BeginBitmap()
    @see ScanLine()
    
    @param aBitmapId A unique identifier for the bitmap that is being unregistered.
    
    @pre aBitmapId is registered with this rasterizer.
	@post aBitmapId is unregistered once from this rasterizer. If all registrations for 
		aBitmapId have been removed, scanline access for aBitmapId is no longer supported 
		and any scanline pointers previously returned by ScanLine() become invalid.    
     */
    virtual void EndBitmap(TInt64 aBitmapId) = 0;
    
    /** Requests a pointer to a scanline containing pixel data for a specified bitmap.
    
    @see BeginBitmap()
    @see EndBitmap()
    
    @param aBitmapId The unique identifier of the bitmap to extract the scanline from.
    @param aPixel An co-ordinate within the bitmap of the first pixel to retrieve.
    @param aLength The number of pixels to retrieve.
    
    @return Returns a pointer to a valid scanline, or NULL if the rasterizer can not allocate 
    	enough memory to return the scanline. Note that the scanline returned is always the 
    	length of a full scanline, where the first pixel returned represents x-coordinate zero 
    	(not aPixel.iX), however the only pixels that are guaranteed to be valid in the returned
    	scanline are from x-coordinate aPixel.iX to x-coordinate aPixel.iX+aLength-1.
    
    @pre aBitmapId must be registered with this rasterizer. The sequence of pixels identified 
    	by aPixel and aLength must lie entirely inside the conceptual width and height of 
    	the bitmap, i.e. the bounds specified by the bitmap's TBitmapDesc::iSizeInPixels.
    @post Any scanline address previously returned by this rasterizer is not guaranteed to be 
    	valid. If successful, the returned pointer references the start of a scanline buffer, 
    	the full length of which can hold an entire row of pixels.  The pixel range within 
    	the scanline buffer specified by the user (aPixel.iX...aPixel.iX+aLength) will contain 
    	accurate pixel values.    
     */
    virtual const TUint32* ScanLine(TInt64 aBitmapId,
                                    const TPoint& aPixel,
                                    TInt aLength) = 0;    

    /** Returns a pointer to an extension interface.  Allows extension of the CFbsRasterizer 
    interface without breaking compatibility with existing implementations.
    
    @param aInterfaceId A UID defined by the OS identifying a specific extension interface.
    @param aInterface A pointer that will be set to the extension interface.
    
    @return KErrNone is returned if aInterface has been set correctly to the extension interface.   
    	KErrExtensionNotSupported is returned if the rasterizer does not support the extension 
    	interface specified by aExtensionId. 
    	KErrNoMemory is returned if the rasterizer supports the extension interface but 
    	there is not enough memory to use it.
    
    @post If successful, KErrNone is returned and aInterface can be cast into a pointer to 
    	the M-class associated with the specified extension UID.    
     */
    virtual TInt GetInterface(TUid aInterfaceId, TAny*& aInterface) = 0;    
    };

/** The UID3 value in the compound identifier of the extended bitmap rasterizer DLL.
 */
const TUid KFbsRasterizerLibraryUid = {0x10285EAE};

#endif // FBSRASTERIZER_H
