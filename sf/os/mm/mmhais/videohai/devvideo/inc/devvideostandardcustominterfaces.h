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

#ifndef DEVVIDEOSTANDARDCUSTOMINTERFACES_H
#define DEVVIDEOSTANDARDCUSTOMINTERFACES_H

#include <e32base.h>
#include <mmf/devvideo/devvideoplay.h>

/**
MMmfVideoMediaProtected interface UID.
@publishedPartner
@released
*/
const TUid KUidMmfVideoMediaProtected = { 0x10273827 };

/**
Used to notify a video rendering media device that the data can only be passed to a secure output. 
Typically used for DRM-protected content and implemented on video decoding or post-processing media 
device where insecure video outputs could be used.
@publishedPartner
@released
*/
class MMmfVideoMediaProtected
    {
public:
	/**
	Notifies a media device if the content is protected, with the intent that it may only be sent to a secure output.

	The device display is typically considered a secure output, but a high-quality unprotected analogue 
	or digital video output connector is not. 
	Once protection has been indicated, playback could fail when Start() is called or at any time 
	during playback. The failure is reported using MdvpoFatalError(KErrorOutputUnsecured), if the current output 
	has been switched to an insecure output.

	@param aProtected Set to ETrue if the content can only be sent to a secure output, 
	EFalse if both secure and non-secure outputs can be used. By default all content can be sent to 
	both secure and insecure outputs. (aProtected == EFalse)
	@leave KErrNotSupported Secure outputs are not supported. The client should not play protected content.
	*/
    virtual void MmvsoMediaIsProtectedL(TBool aProtected) = 0;
    };

/*****************************************************************************/
/**
KUidMmfVideoInputSnapshot interface UID.
@publishedPartner
@released
*/
const TUid KUidMmfVideoInputSnapshot = { 0x10204beb };
/**
The callback interface used by MMmfVideoInputSnapshot to notify snapshot completion.
@publishedPartner
@released
*/
class MMmfVideoInputSnapshotObserver
    {
public:
	/**
	The callback method that the observer should implement to receive notifications
	from the MMmfVideoInputSnapshot implementation.
	@param aError One of the system wide error codes.
	*/
    virtual void SnapshotComplete(TInt aError) = 0;
    };
	
/**
Single frame capture support for video recording media devices.

The interface is typically implemented in pre-processing and encoding media devices with direct capture support.
@publishedPartner
@released
*/
class MMmfVideoInputSnapshot
    {
public:
	/**
	Sets the observer object to use for snapshot complete callbacks.

	This method can be called both before and after the media device has been initialised with InitializeL(). 
	It must be called before GetSnapshotL() is called for the first time.
	@param aObserver A pointer to the observer object to use.
	*/
	virtual void MmvisSetObserver(MMmfVideoInputSnapshotObserver* aObserver) = 0;

	/**
	Grabs a duplicate of the next input frame. This method is asynchronous; the media device will call 
	MMmfVideoInputSnapshotObserver::SnapshotComplete() when the picture is available. 

	This method may only be called after the media device has been initialised with InitialiseL() 
	and after Start() has been called. The request can be cancelled with CancelSnapshot(). Some implementations 
	may not support format conversions or scaling for snapshot images. 

	@param	aPictureData	Target picture. The memory for the picture must be allocated by the caller and 
			initialised properly. The data formats must match the snapshot format requested.
	@param	aFormat A reference to the picture format structure that indicates the snapshot format to use.

	@leave	KErrNotSupported The requested data format or picture size is not supported.

	@pre	This method can only be called after the media device has been initialized with InitializeL() 
			and the MMmfVideoInputSnapshotObserver has been set.
	*/
	virtual void MmvisGetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat) = 0;
   
	/**
	Cancels an outstanding snapshot request.

	If there is no active request pending then this call is ignored.
	*/
	virtual void MmvisCancelSnapshot() = 0;
    };

/*****************************************************************************/
/**
MMmfVideoPlayHWDeviceColourInfoCustomHeader interface UID.
@publishedPartner
@released
*/
const TUid KUidMmfVideoPlayHWDeviceColourInfoCustomHeader = { 0x10273828 };

/**
Interface support for the HW Device Plug-in to supply colour space and pixel aspect ratio information to the controller
@publishedPartner
@released
*/
class MMmfVideoPlayHWDeviceColourInfoCustomHeader
	{
public:
	/**
	 Reads colour space data format information from a coded data unit. Returns the colour space, data 
	 range and pixel aspect ratio used in the bitstream.
	 
	 @param aDataUnitType The type of coded data unit that is contained in aDataUnit. If the data is a 
	 		simple piece of bitstream, use EDuArbitraryStreamSection.
	 @param aDataUnitEncapsulation The encapsulation type used for the coded data. If the data is a simple 
	 		piece of bitstream, use EDuElementaryStream.
	 @param aDataUnit The coded data unit, contained in a TVideoInputBuffer.
	 @param aYuvFormat Target data structure for data format information. The media device will fill the 
	 		iCoefficients, iPattern, iAspectRatioNum, and iAspectRatioDenum fields.
	 @leave KErrNotSupported The data is not in a supported format.
	 @leave KErrCorrupt The data appears to be in a supported format, but is corrupted.
	 */
	virtual void MmvpciGetColourSpaceHeaderInfoL(TVideoDataUnitType& aDataUnitType,
												TVideoDataUnitEncapsulation& aDataUnitEncapsulation, 
												TVideoInputBuffer* aDataUnit, TYuvFormat& aYuvFormat) = 0;
	};

/*****************************************************************************/
/**
MMmfVideoHwDevClientNotifier interface UID.
@publishedPartner
@released
*/
const TUid KUidMmfVideoHwDevVideoPolicyExtension = {0x101FBFC3};

/**
This class needs to be inherited by the video client. This is so that the video client will
receive a notification if its access has been revoked. 
@publishedPartner
@released
*/
class MMmfVideoHwDevClientNotifier
	{
public:
	/**
	Method to allow notification to the video client that their access to the video driver has been 
	revoked and their access to the video resources has already been removed. VideoAccessRevoked can 
	be called anytime after Initialize()  has been called.

	Once called it means that the video client should no longer be inputting any data.
	The only thing the video client can do at this stage is destroy the MDF plug-in. 
	*/
	virtual void MmvhcnVideoAccessRevoked() = 0 ;
	
	/**
	Method to allow notification to the video client that they need to release the video resources. 
	This allows the client slightly more control over their release process behaviour. 

	ReleaseVideoResource can be called anytime after Initialize has been called. Once called it 
	means that the video client needs to either call InputEnd() or Stop() to release the video resource
	*/
	virtual void MmvhcnReleaseVideoResource() = 0;
	};

	
/**
Custom extension to allow MMF controller to set video policy priority preference.

Acceptable Priorities range from -100 to 100.

@publishedPartner
@released
*/
class MMmfVideoHwDeviceVideoPolicyExtension
	{
public:
	/**
	Video client should call this function so that the MDF plugins will have a handle to the video client. 
	This is required so that the MDF plugin can notify the video client if their access has been revoked.

	@param aClient Pointer to the video client 
	*/
	virtual void MmvhvpeSetVideoClient(MMmfVideoHwDevClientNotifier* aClient) = 0;

	/**
	Allows the video client to specify the video priority they wish their process to have for their 
	video request. This may or may not be honoured depending upon the client process's platform security capabilities.

	@param aPriority Preferred priority for video processing into the video manager.
	*/
	virtual void MmvhvpeSetVideoPriority(TInt aPriority) = 0;  
	};	

/*****************************************************************************/
/**
MMmfVideoBufferManagement interface UID.

@publishedPartner
@released
*/
const TUid KUidMmfVideoBufferManagement = {0x10204bea};

/**
An observer class for the buffer management custom interface, MMmfVideoBufferManagement. 
Clients and source media devices using the interface must implement this class. 

@publishedPartner
@released
*/
class MMmfVideoBufferManagementObserver
	{
public:
	/**
	Notifies the observer that one or more new input buffers are available. The client can then 
	use MMmfVideoBufferManagement:: MmvbmGetBufferL() to get a buffer.
	*/
	virtual void MmvbmoNewBuffers() = 0;

	/**
	Notifies the observer all outstanding input buffers must be released immediately. The client 
	must synchronously call MMmfVideoBufferManagement:: MmvbmReleaseBuffer() for each of the buffers 
	it has allocated but not returned before returning from this method. It is intended that this method 
	is used when the target media device loses access to the buffers, as a result of a Direct Screen Access 
	abort, resource management override, or media device destruction.
	*/
	virtual void MmvbmoReleaseBuffers() = 0;
	};

/**
Enables target media devices to allocate input data buffers for uncompressed data. 

Client applications and source media devices can therefore write the input data directly 
into a suitable memory area that the target media device can use, possibly eliminating one extra 
memory copy from the data path. This is especially important with hardware implementations, where the 
target hardware may only have access to specially allocated memory that only the media device can allocate.
 
The interface is typically implemented in pre-processing, encoding, and post-processing media devices. 
It is mainly used for video encoder memory buffer input in applications like video editors and when 
using hardware-accelerated post-processors with software video decoders.

@publishedPartner
@released
*/
class MMmfVideoBufferManagement
	{
public:
	/** 
	Used to signal the buffer options used.
	*/
	class TBufferOptions
		{
	public:
		/**
		The number of input buffers that the media device has. This is the number of buffers the 
		client can request through GetBufferL() before writing any back.
		*/        
		TUint iNumInputBuffers;

		/**
		Buffer size in pixels. This should match the input data picture size, optionally with padding to 
		meet coding standard requirements.
		*/
		TSize iBufferSize;
		};
    
	/** 
	Sets the observer object to use. The observer gets notified when new buffers are available 
	and when buffers need to be released back to the media device.

	This method can only be called before the media device has been initialised with InitializeL().

	@param aObserver The observer object to use.
	*/
	virtual void MmvbmSetObserver(MMmfVideoBufferManagementObserver* aObserver) = 0;

	/** 
	Enables input buffer management mode. In buffer management mode the target media device allocates 
	memory for input buffers and the client can only use input buffers allocated with GetBufferL().

	This method can only be called before the media device has been initialised with InitializeL(). 
	This method must be called if the client uses GetBufferL().

	@param aEnable ETrue if input buffer management mode is used.
	*/
	virtual void MmvbmEnable(TBool aEnable) = 0;

	/** 
	Sets the buffer options to use. The client can request the number of input buffers that should 
	be available, but typically only few (one or two) buffers can be used.

	This method can only be called before the media device has been initialised with InitializeL().

	@param aOptions The buffer options to use.
	@see TBufferOptions.

	@leave KErrNotSupported The requested buffer options are not supported. Typically the client 
	has requested too many input buffers.
	*/
	virtual void MmvbmSetBufferOptionsL(const TBufferOptions& aOptions) = 0;

	/** 
	Gets the buffer options currently in use.

	This method can only be called before the media device has been initialised with InitializeL().

	@param aOptions Target buffer options data structure.
	*/
	virtual void MmvbmGetBufferOptions(TBufferOptions& aOptions) = 0;

	/** 
	Gets a new input picture buffer. The client can then write data into the buffer and write it back 
	to the media device with WritePictureL(). After the media device has processed the TVideoPicture and 
	no longer needs any data from the buffer, it will notify the client by calling ReturnPicture().

	This method can only be called after the media device has been initialised with InitializeL(). This 
	method can only be called in buffer management mode, i.e. if the client has called Enable(ETrue).

	Note that target-allocated and client-allocated input buffers cannot be mixed. In buffer management mode 
	only input buffers allocated with this method can be sent to the media device.

	If a client has retrieved buffers with GetBufferL(), it must be prepared to release them synchronously at 
	any point if MmmfBufferManagementObserver::ReleaseBuffers() is called. This may happen if the target media 
	device suddenly loses access to the buffers due to DSA abort, resource management conflict, or media device 
	destruction.

	The returned TVideoPicture object will have iHeader and iLayerBitRates set to NULL. The client can 
	allocate memory for those fields, and any data they may contain (such as TVideoPictureHeader::iOptional) 
	and set the pointers accordingly before writing the buffer back to the target media device. The memory must 
	remain valid and accessible until the media device returns the buffer with ReturnPicture(), at which time the 
	client can free or reuse the memory.

	@param aSize The requested buffer size, in pixels. The buffer size should match the picture size set at 
	initialisation phase, or otherwise a suitable buffer may not be available. If the size is smaller than 
	the size set at initialisation phase, the allocated buffer may be larger than requested.

	@return A new input picture buffer. If no free buffers are available, the return value is NULL.

	@leave KErrGeneral The method will leave only if an error occurs. Note: Lack of free buffers is not 
	considered an error.
	*/
	virtual TVideoPicture* MmvbmGetBufferL(const TSize& aSize) = 0;

	/** 
	Releases an input buffer back to the media device without using it. This method is mainly used as a 
	response to a ReleaseBuffers() callback.

	@param aBuffer The buffer to release.
	*/
	virtual void MmvbmReleaseBuffer(TVideoPicture* aBuffer) = 0;
    };

/*****************************************************************************/
/** 
MMmfVideoWindowControl Custom Interface UID.

@publishedPartner
@released
*/
const TUid KUidMmfVideoWindowControl = { 0x10282646 };

/** 
Enables the client to control video position using a logical video window, 
and specify the exact location of the picture within that window. 
The logical video window may be partially or completely off-screen.

@publishedPartner
@released
*/
class MMmfVideoWindowControl
    {
public:
    /**
    Horizontal alignment constants.
    */
    enum THorizAlign
        {
        /**
        The picture is horizontally centered.
        */
        EHorizAlignCenter,
        /**
        The picture is horizontally left-aligned.
        */
        EHorizAlignLeft,
        /**
        The picture is horizontally right-aligned.
        */
        EHorizAlignRight
        };

    /**
    Vertical alignment constants.
    */
    enum TVertAlign
        {
        /**
        The picture is vertically centered.
        */
        EVertAlignCenter,
        /**
        The picture is vertically top-aligned.
        */
        EVertAlignTop,
        /**
        The picture is vertically bottom-aligned.
        */
        EVertAlignBottom
        };

    /** 
    Sets the logical video window to use for positioning the video
    picture on the screen. By default the logical video window is the same
    as the display video window set with StartDirectScreenAccessL().
    
    @param aWindow Logical window rectangle, relative to the device display.
    */
    virtual void MmvwcSetLogicalVideoWindow(const TRect& aWindow) = 0;

    /**
    Sets the offset where the video picture will be located in the
    logical video window. By default the picture is centered in the window
    if its content and window sizes do not match.
    
    This method and MmvwcSetContentAlignment() are mutually exclusive, and
    only one of them should be used. Calling one will override the settings
    made by the other.

 	If an error occurs, this function leaves with any of the system
    wide error codes. Common error codes are listed below.   
    
    @param aOffset Offset, the position of the upper left hand corner of the
                   picture relative to the upper left hand corner of the
                   logical window. The picture must be fully contained within the 
       			   window.
       
    @leave KErrArgument The picture is not fully contained within the window.
    */
    virtual void MmvwcSetContentOffsetL(const TPoint& aOffset) = 0;

    /**
    Sets video picture alignment within the logical video window.
    By default the picture is centered in the window
    if its content and window sizes do not match.
    
    This method and MmvwcSetContentOffsetL() are mutually exclusive, and
    only one of them should be used. Calling one will override the settings
    made by the other.
    
    @param aHorizAlign Horizontal alignment within the logical window
    @param aVertAlign Vertical alignment within the logical window
    */
    virtual void MmvwcSetContentAlignment(const THorizAlign aHorizAlign, const TVertAlign aVertAlign) = 0;
    };

/*****************************************************************************/
/** 
KUidMmfVideoResourceManagement Custom Interface UID.

@publishedPartner
@released
*/
const TUid KUidMmfVideoResourceManagement = { 0x1028264B  };

/**
Video resource observer interface, implemented by clients using the
MMmfVideoResourceHandler custom interface.

@publishedPartner
@released
*/
class MMmfVideoResourceObserver
    {
public:
    /** 
    Indicates that a media device has lost its resources. The client must
    synchronously pause or stop processing before returning from this method
    so that the media device can release its resources to the system. If
    the client does not pause or stop, resource loss will be handled as a
    fatal error.

    If the client is holding any buffers allocated by the media
    device, such as video decoder input or output buffers, it must
    synchronously return them before returning from this
    method. Additionally, in video playback use, the client must empty
    any queued output pictures from DevVideoPlay by repeatedly calling
    CMMFDevVideoPlay::NextPictureL() and
    CMMFDevVideoPlay::ReturnPicture() until no more pictures are
    available.
     
    The client may start again or resume after receiving a
    MmvroResourcesRestored() callback.
     
    @param aMediaDevice UID for the media device that lost resources. The
                        client can use this for example to determine whether 
                        the decoder or the post-processor lost resources. 
                        This is typically not required though since the client
                        must pause DevVideo entirely.
    */
    virtual void MmvroResourcesLost(TUid aMediaDevice) = 0;

    /** 
    Indicates that a media device has regained its resources after a
    previous resource loss. The client can restart or resume processing.
    This can be done either synchronously or asynchronously.
     
    @param aMediaDevice UID for the media device that regained resources.
    */
    virtual void MmvroResourcesRestored(TUid aMediaDevice) = 0;
    };



/**
Video hardware resource handler custom interface.
 
Video decoder, encoder, pre or post-processor media devices can implement
this interface. Clients can use this interface to register for resource
management notifications and to set resource management priorities.

This custom interface is only intended for media devices that can save 
their internal state as resources are lost, making resource loss recoverable. 
As such it is not appropriate for all media devices. 

By default resource loss is considered a fatal error. Clients can
use the notification callbacks to pause or stop processing when resources
are lost and restart or resume when resources are available again, thus 
avoiding errors and providing a better user experience.
 
@publishedPartner
@released
*/
class MMmfVideoResourceHandler
    {
public:
    /**
    Sets the video resource observer to use. The observer will receive
    resource loss and restore callbacks. If no observer has been set,
    resources losses will be handled as fatal errors.
     
    @param aObserver The observer to use, NULL to remove observer.
    */
    virtual void MmvrhSetObserver(MMmfVideoResourceObserver* aObserver) = 0;
    };

/*****************************************************************************/
/** 
KUidMmfVideoResourcePriority Custom Interface UID.

@publishedPartner
@released
*/
const TUid KUidMmfVideoResourcePriority = { 0x1028264C };

/** 
Video resource priority control custom interface.
 
Video decoder, encoder, pre or post-processor media devices can implement
this interface. Clients can use this interface to set resource reservation
priority for the media device. Typically this interface is used to lower
the priority for background tasks to ensure they do not remove resources
from foreground applications.
 
@publishedPartner
@released
*/
class MMmfVideoResourcePriority
    {
public:
    /**
    Sets the priority the media device uses to reserve resources.
    Media devices with higher priority may override requests from ones
    with a lower priority. This method can be called at any point, but
    implementations may ignore it after initialization. Therefore clients
    should call this before calling Initialize().
     
    The behaviour of the underlying resource manager and policy is
    implementation-specific. Most implementations will limit high
    priorities to clients that have certain capabilities
    (for example, MultimediaDD) or specific Vendor IDs only. 
    All implementations should do some policing to ensure untrusted 
    applications cannot block the system by allocating all available 
    resources at a high priority.
     
    @param aPriority Resource management priority. The values use the same 
                     range as TMdaPriority.
    */
    virtual void MmvpSetResourcePriority(TInt aPriority) = 0;
    };

/*****************************************************************************/
/** 
KUidMmfVideoPropertiesManagement Custom Interface UID.

@publishedPartner
@released
*/
const TUid KUidMmfVideoPropertiesManagement = { 0x10283231 };

/**
Video property change observer. DevVideoPlay clients or media devices can
implement this observer and use the MMmfVideoPropertiesNotifier extension
to receive notifications when decoded video properties (such as picture
size and pixel aspect ratio) change.

@publishedPartner
@released
*/
class MMmfVideoPropertiesObserver
    {
public:
    /** 
    Decoded video properties updated. The media device implementing the
    MMmfVideoPropertiesNotifier extension will call this method immediately
    before outputting a picture with the updated properties.
    
    When the extension is used between a decoder and a post-processor media
    device, the post-processor can associate the changed properties with the
    correct picture, since the next incoming picture after this call will
    be the first updated one. 
    
    When the extension is used between a DevVideoPlay client and a
    post-processor media device, the client can synchronously reconfigure
    video display options using SetScaleOptionsL() and related methods. This
    lets the client reconfigure the display for the correct picture.
    
    @param aYuvFormat Updated YUV format parameters. The updated fields are
                      iAspectRatioDenom, iAspectRatioNum, and iCoefficients     
    @param aPictureSize	Updated picture size. This size will be the true
                       	picture display size, excluding any padding that
                       	the codec might use.
    */
    virtual void MmvpoUpdateVideoProperties(const TYuvFormat& aYuvFormat, const TSize& aPictureSize) = 0;
    };


/**
Video property notifier extension. DevVideoPlay decoders and post-processor
can implement this extension to provide notifications when decoded video
picture properties such as size or pixel aspect ratio change. The extension
is typically used to get size and pixel aspect ratio updates from a decoder
to a post-processor media device, and from a post-processor to the
DevVideoPlay client.

@publishedPartner
@released
*/
class MMmfVideoPropertiesNotifier
    {
public:
    /** 
    Sets a new video properties observer. This method can be called at any
    time after the media device has been instantiated.
     
    @param aObserver New observer object.
    */
    virtual void MmvpnSetObserver(MMmfVideoPropertiesObserver* aObserver) = 0;
    };


/** 
KUidMmfVideoH324AnnexKMode Custom Interface UID.

@publishedPartner
@released
*/
const TUid KUidMmfVideoH324AnnexKMode = { 0x102836C2 };

/**
Video encoder H.324M Annex K mode extension. DevVideoRecord MPEG-4 and H.264
encoders can implement this extension to support using ITu-T H.324 Annex K
encoding parameters.
 
@publishedPartner
@released
*/
class MMmfVideoH324AnnexKMode
    {
public:
    /** 
     * Enables or disables encoder ITU-T H.324M Annex K mode. This method
     * is only applicable for MPEG-4 and H.264 video.
     *
     * When the encoder is in Annex K mode, it will use the encoding
     * parameters and bitstream headers defined in ITU-T H.324M Annex
     * K. Enabling Annex K mode will reset all the specified settings
     * apart from picture size and output format to the values
     * specified in Annex K. This includes codec profile and level,
     * maximum bit rate, and other encoding constraints.
     *
     * When Annex K mode is active, the client cannot change encoder settings
     * that would conflict with the ones specified in Annex K. Attempts to do
     * so will cause the corresponding setting methods to fail with
     * KErrNotSupported.
     *
     * Annex K mode can be deactivated by calling this method with a false
     * parameter. This will remove restrictions on encoder settings.
     *
     * This method can only be called before the encoder is
     * initialized. The client should set the input and output formats
     * and picture size before calling this method.
     *
     * If an error occurs this method will fail with a system-wide error code.
     * Typical error codes are listed below.
     * Typically errors are only expected if the current output format or
     * picture size do not meet Annex K constraints.
     * 
     * @param aAnnexKMode ETrue to enable H.324M Annex K mode, EFalse to
     *                    disable it
     *
     * @pre The client has called SelectEncoderL(), SetOutputFormatL(), and
     *      SetInputFormatL(). This method can only be called before
     *      Initialize().
     *
     * @leave KErrNotSupported Annex K mode cannot be supported with the
     *                         current settings. Possible causes are incorrect
     *                         output format (not MPEG-4 or H.264) or picture
     *                         size (not QCIF).
     */
    virtual void MmvhakmSetH324MAnnexKModeL(TBool aAnnexKMode) = 0;
    };


/**
MMmfVideoClientThreadInfo interface UID.
@publishedPartner
@released
*/
const TUid KMmfUidDevVideoClientThreadInfoCustomInterface = {0x102834A9};

/**
@publishedPartner
@released
 
Custom interface providing support for setting the client thread info for the DevVideo.
*/
class MMmfVideoClientThreadInfo
	{	
public:
	/**
	Set client thread info for the DevVideo.
 	
	@param  aTid the client thread Id.
	@return An error code indicating if the function call was successful.
	*/
	virtual TInt MmvctSetClientThreadInfo(TThreadId aTid) = 0;
	};


#endif
