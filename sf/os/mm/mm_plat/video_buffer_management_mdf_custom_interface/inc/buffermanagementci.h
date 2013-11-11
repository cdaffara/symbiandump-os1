/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Defines a custom interface for zero-buffer copy handling    
*
*/





#ifndef BUFFERMANAGEMENTCI_H
#define BUFFERMANAGEMENTCI_H

//  INCLUDES
#include <mmf/devvideo/devvideobase.h>


// CONSTANTS

/** Buffer Management Custom Interface UID */
const TUid KMmfVideoBuffermanagementUid = { 0x10204bea };


// CLASS DECLARATIONS

class MMmfVideoBufferManagementObserver;

/**
 * Buffer Management Custom Interface main interface class. Media
 * devices that support this custom interface implement this class.
 *
 * The Buffer Management Custom Interface enchances the MDF DevVideo
 * data flow model by enabling target media devices to allocate input
 * data buffers for uncompressed data input. This lets client
 * applications and source media devices write the input data directly
 * into a suitable memory area that the target media device can use,
 * possibly eliminating one extra memory copy from the data path. This
 * is especially important with hardware implementations, where the
 * target hardware may only have access to specially allocated memory
 * that only the media device can allocate.
 * 
 * The interface is typically implemented in pre-processing, encoding,
 * and post-processing media devices. It is mainly used for video
 * encoder memory buffer input in applications like video editors, and
 * when using hardware-accelerated post-processors with software video
 * decoders.
 */

class MMmfVideoBufferManagement
    {
public:
    /** A class used to signal the buffer options used. */
    class TBufferOptions
        {
    public:
        /**
         * The number of input buffers that the media device *
         * has. This is the number of buffers the client can request *
         * through MmvbmGetBufferL() before writing any back.
         */        
        TUint iNumInputBuffers;

        /**
         * Buffer size in pixels. This should match the input data
         * picture size, optionally with padding to meet coding
         * standard requirements.
         */
        TSize iBufferSize;
        };

    
    /** 
     * Sets the observer object to use. The observer gets notified
     * when new buffers are available and when buffers need to be
     * released back to the media device.
     *
     * This method can only be called before the media device has
     * been initialised with InitializeL().
     * 
     * @param aObserver The observer object to use.
     */
    virtual void MmvbmSetObserver(MMmfVideoBufferManagementObserver* aObserver) = 0;
    
    /** 
     * Enables input buffer management mode. In buffer management mode
     * the target media device allocates memory for input buffers and
     * the client can only use input buffers allocated with
     * MmvbmGetBufferL().
     *
     * This method can only be called before the media device has been
     * initialised with InitializeL(). This method must be called if
     * the client uses MmvbmGetBufferL().
     * 
     * @param aEnable ETrue if input buffer management mode is used.
     */
    virtual void MmvbmEnable(TBool aEnable) = 0;
    
    /** 
     * Sets the buffer options to use. The client can request the
     * number of input buffers that should be available, but typically
     * only few (one or two) buffers can be used.
     *
     * This method can only be called before the media device has been
     * initialised with InitializeL().
     * 
     * @param aOptions The buffer options to use, see TBufferOptions.
     *
     * @leave KErrNotSupported The requested buffer options are not
     *   supported. Typically the client has requested too many input
     *   buffers.
     */
    virtual void MmvbmSetBufferOptionsL(const TBufferOptions& aOptions) = 0;
    
    /** 
     * Gets the buffer options currently in use.
     *
     * This method can only be called before the media device has been
     * initialised with InitializeL().
     * 
     * @param aOptions Target buffer options data structure.
     */
    virtual void MmvbmGetBufferOptions(TBufferOptions& aOptions) = 0;
    
    /** 
     * Gets a new input picture buffer. The client can then write data
     * into the buffer and write it back to the media device with
     * WritePictureL().
     *
     * This method can only be called after the media device has been
     * initialised with InitializeL(). This method can only be called
     * in buffer management mode, i.e. if the client has called
     * MmvbmEnable(ETrue).
     *
     * Note that target-allocated and client-allocated input buffers
     * cannot be mixed. In buffer management mode only input buffers
     * allocated with this method can be sent to the media device.
     *
     * If a client has retrieved buffers with MmvbmGetBufferL(), it
     * must be prepated to release them synchronously at any point if
     * MmmfBufferManagementObserver::MmvbmoReleaseBuffers() is
     * called. This may happen if the target media device suddenly
     * loses access to the buffers due to DSA abort, resource
     * management conflict, or media device destruction.
     * 
     * @param aSize The requested buffer size, in pixels. The buffer
     *   size should match the picture size set at initialisation phase,
     *   or otherwise suitable buffer may not be available. If the size
     *   is smaller than the size set at initialisation phase, the
     *   allocated buffer may be larger than requested.
     * 
     * @return A new input picture buffer. If no free buffers are
     * available, the return value is NULL.
     *
     * @leave General The method will leave if an error occurs. Lack
     * of free buffers is not considered an error.
     */
    virtual TVideoPicture* MmvbmGetBufferL(const TSize& aSize) = 0;
    
    /** 
     * Releases an input buffer back to the media device without using
     * it. This method is mainly used as a response to a
     * MmvbmReleaseBuffers() callback.
     * 
     * @param aBuffer The buffer to release.
     */
    virtual void MmvbmReleaseBuffer(TVideoPicture* aBuffer) = 0;
    };


/**
 * An observer class for the buffer management custom interface.
 * Clients and source media devices using the interface
 * must implement this class.
*/
class MMmfVideoBufferManagementObserver
    {
public:
    /**
     * Notifies the observer that one or more new input buffers are
     * available. The client can then use MmvbmGetBufferL() to get a
     * buffer.
     */
    virtual void MmvbmoNewBuffers() = 0;

    /**
     * Notifies the observer all outstanding input buffers must be
     * released immediately. The client must synchronously call
     * MmvbmReleaseBuffer() for all buffers it has allocated but not
     * returned before returning from this method. This method is
     * mainly used when the target media device loses access to the
     * buffers, as a result of a Direct Screen Access abort, resource
     * management override, or media device desctruction.
     */
    virtual void MmvbmoReleaseBuffers() = 0;
    };


#endif      // BUFFERMANAGEMENTCI_H
            
// End of File
