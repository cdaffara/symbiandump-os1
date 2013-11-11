/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __NGAPOSTPROCSURFACEHANDLER_H__
#define __NGAPOSTPROCSURFACEHANDLER_H__

#include <e32std.h>
#include <graphics/surface.h>

class RSurfaceManager;

class CNGAPostProcSurfaceHandler
{

public: 
    //  === Constructors and destructor ===
    /**
    * Two-phased constructor.   
    * @return pointer to an instance of CMMFVideoPostProcHwDevice
    */
    static CNGAPostProcSurfaceHandler* NewL();

    /**
    * Destructor.
    */
    ~CNGAPostProcSurfaceHandler();

public: 
    
    /**
    Sets the device input format to an uncompressed video format.

    @param  "aFormat"   "The input format to use."
    @leave  "The method will leave if an error occurs. Typical error codes used:
            * KErrNotSupported - The input format is not supported."    
    @pre    "This method can only be called before the hwdevice has been initialized with Initialize()."
    */
    TInt CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf & aAttributes, TSurfaceId& aSurfaceId);

    /**
    Sets the decoder device that will write data to this post-processor. Decoded pictures will be 
    written with WritePictureL() or through a custom interface. After pictures have been processed, 
    they must be returned to the decoder using ReturnPicture().

    @param  "aDevice"   "The decoder source plug-in to use."
    @pre    "This method can only be called before the hwdevice has been initialized with Initialize()."
    */
    TInt OpenSurface(const TSurfaceId& aSurfaceId);

    /**
    Writes an uncompressed video picture to the post-processor. The picture must be returned to the 
    client or source plug-in after it has been used.

    @param  "aPicture"  "The picture to write."
    @leave  "This method may leave with one of the system-wide error codes."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    TInt MapSurface(const TSurfaceId& aSurfaceId, RChunk& aHandle);    

    /**
    Retrieves post-processing information about this hardware device. 
    The device creates a CPostProcessorInfo structure, fills it with correct data, pushes it 
    to the cleanup stack and returns it. The client will delete the object when it is no 
    longer needed.

    @return "Post-processor information as a CPostProcessorInfo object. 
            The object is pushed to the cleanup stack, and must be deallocated by the caller."
    @leave  "This method may leave with one of the system-wide error codes.
    */
    TInt SurfaceInfo(const TSurfaceId& aSurfaceId, RSurfaceManager::TInfoBuf& aInfo);

    /**
    Retrieves the list of the output formats that the device supports. The list is ordered in 
    plug-in preference order, with the preferred formats at the beginning of the list. The list 
    can depend on the device source format, and therefore SetSourceFormatL() must be called before 
    calling this method.

    @param "aFormats" "An array for the result format list. The array must be created and destroyed by the caller."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method may only be called before the hwdevice has been initialized using Initialize()."
    */
    TInt DestroySurface(const TSurfaceId& aSurfaceId);
    
    /**
	Get the offset of the specified buffer from the base address of the underlying
	chunk.
	
	To obtain the address of the buffer, the offset returned must be added onto the
	base address of the RChunk returned in a call to MapSurface(). Note that
	buffer offsets are immutable during the lifetime of the surface.
	@param aParam The input parameters including the surface ID and buffer index.
	@pre The surface is open in the calling process.
	@return KErrNone if successful, KErrArgument if aSurfaceId or aBuffer are invalid,
	KErrAccessDenied if the surface is not open in the current process, KErrNotSupported if
	the surface is not mappable, otherwise a system wide error code.
	*/
    TInt GetBufferOffset(const TSurfaceId& aSurfaceId, TInt aBuffer, TInt& aOffset);
    
     /**
    Adds a Surface hint to the video surface. 
    
    */
    TInt AddSurfaceHint(const TSurfaceId& aSurfaceId,const RSurfaceManager::THintPair& aHint);
    
    /**
    Updates an existing Surface hint. 
    
    */
    TInt SetSurfaceHint(const TSurfaceId& aSurfaceId,const RSurfaceManager::THintPair& aHint);
    
protected:
	CNGAPostProcSurfaceHandler();
    /**
     *  Symbian 2nd phase constructor .
     */
    void ConstructL();

private: 

    RSurfaceManager*      iSurfaceManager;
};    

#endif //__NGAPOSTPROCSURFACEHANDLER_H__