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

#ifndef __WSUIBUFFER_H__
#define __WSUIBUFFER_H__

#include <pixelformats.h>
#include <graphics/wsgraphicdrawerinterface.h>

/**
@file
@publishedPartner
@prototype 
*/

/**
This class provides direct access to the memory of the UI buffer.

This interface is to be used by CRPs that don't want to go through MWsGraphicsContext
to handle their rendering, but want direct memory access.
@publishedPartner
@prototype
*/
class MWsUiBuffer: public MWsObjectProvider
    {
public:
    DECLARE_WS_TYPE_ID(KMWsUiBufferInterfaceId)

    /**
    Finishes pending rendering to the buffer and temporarily makes the pixel 
    data of the buffer image accessible for reading and writing by the CPU.
    
    When finished with the pixel data, the caller must end the mapping by
    calling @c Unmap().

    This method should only be called from the context of 
    @c CWsGraphicDrawer::Draw().
    @param aDataAddress On return, the base address of the pixel data in the
    address space of the calling process
    @param aDataStride On return, the number of bytes between rows of the
    pixel data
    @return KErrNone if successful, otherwise one of the standard Symbian
    error codes
    @see Unmap()
    @publishedPartner
    @prototype
    */
    virtual TInt MapReadWrite(TAny*& aDataAddress, TInt& aDataStride) = 0;
    
    /**
    Finishes pending rendering to the buffer and temporarily makes the pixel 
    data of the buffer image accessible for writing by the CPU.
    
    When finished with the pixel data, the caller must end the mapping by
    calling @c Unmap().

    This method should only be called from the context of 
    @c CWsGraphicDrawer::Draw().
    @param aDataAddress On return, the base address of the pixel data in the
    address space of the calling process
    @param aDataStride On return, the number of bytes between rows of the
    pixel data
    @return KErrNone if successful, otherwise one of the standard Symbian
    error codes
    @see Unmap()
    @publishedPartner
    @prototype
    */
    virtual TInt MapWriteOnly(TAny*& aDataAddress, TInt& aDataStride) = 0;
        
    /**
    Makes the pixel data of an image no longer accessible to the CPU.

    Before calling this method the buffer must be mapped for CPU access by
    a previous call to @c MapReadWrite() or @c MapWriteOnly().

    This method should only be called from the context of 
    @c CWsGraphicDrawer::Draw().
    @return KErrNone if successful, otherwise one of the standard Symbian
    error codes
    @see MapReadWrite()
    @see MapWriteOnly()
    @publishedPartner
    @prototype
    */
    virtual TInt Unmap() = 0;

    /**
    Gets the pixel format of the buffer.
    @return The pixel format of the buffer
    @publishedPartner
    @prototype
    */
    virtual TUidPixelFormat PixelFormat() const = 0;

    /** 
    Gets the current size of the buffer.

    The actual size of the whole buffer might be larger than the returned
    value depending on the current screen mode. However, it is the size
    returned by this method that should be used when accessing the pixel data.
    @return The current size of the buffer
    @publishedPartner
    @prototype
    */
    virtual TSize SizeInPixels() const = 0;
    };

#endif //__WSUiBUFFER_H__
