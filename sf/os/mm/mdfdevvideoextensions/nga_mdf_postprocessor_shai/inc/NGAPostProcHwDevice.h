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


#ifndef __NGAPOSTPROCHWDEVICE_H__
#define __NGAPOSTPROCHWDEVICE_H__

#include <e32std.h> 
#include <w32std.h>
#include <e32base.h>
#include <e32def.h>
#include <e32cmn.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <mmf/devvideo/videoplayhwdevice.h>
#include <mmf/devvideo/devvideostandardcustominterfaces.h>
#include <mmf/devvideo/devvideovideosurfacecustominterface.h>
#include <graphics/surfacemanager.h> //RSurfaceManager
#include <graphics/surface.h> //TSurfaceId
#include <mmf/common/mmfvideo.h> //TVideoAspectRatio
//TBC++
//#include <mmf/devvideo/devvideosurfacehandlecustominterface.h>
#include "devvideosurfacehandlecustominterface.h"
#include "postinitializeci.h"
//TBC--
#include <mmf/devvideo/devvideoplayratecustominterface.h>  
#include <surface_hints.h>
#include <secureoutputci.h>
#include <advancedsecureoutputci.h>
#include <graphics/suerror.h>

#include "fetchframecustominterface.h"
#include "MdfRDebug.h"
#include "NGAPostProcHwDevice_UID.hrh"
#include "NGAPostProcSessionManagerObserver.h"
#include "common.h"

class CNGAPostProcSessionManager;
class CNGAPostProcSurfaceHandler;
class CNGAPostProcTimer;
class RWsSession;

class CNGAPostProcHwDevice: public CMMFVideoPostProcHwDevice,
							public MMmfVideoBufferManagement,
							public MNGAPostProcSessionManagerObserver,
							public MMMFVideoSurfaceSupport,
							public MMmfVideoPropertiesObserver,
							public MMmfVideoResourceObserver,
							public MMmfVideoSurfaceHandleControl,
							public MMmfVideoPropertiesNotifier,
							public MMmfVideoPlayRateControl,
							public MMmfVideoSecureOutput,
							public MMmfAdvancedVideoSecureOutput,
							public MMmfPostInitializeRequest
{

public: 
    //  === Constructors and destructor ===
    /**
    * Two-phased constructor.   
    * @return pointer to an instance of CMMFVideoPostProcHwDevice
    */
    static CMMFVideoPostProcHwDevice* NewL();

    /**
    * Destructor.
    */
    ~CNGAPostProcHwDevice();

public: 
    
    // === CMMFVideoPostProcHwDevice ===
    
    /**
    Sets the device input format to an uncompressed video format.

    @param  "aFormat"   "The input format to use."
    @leave  "The method will leave if an error occurs. Typical error codes used:
            * KErrNotSupported - The input format is not supported."    
    @pre    "This method can only be called before the hwdevice has been initialized with Initialize()."
    */
    void SetInputFormatL(const TUncompressedVideoFormat& aFormat);

    /**
    Sets the decoder device that will write data to this post-processor. Decoded pictures will be 
    written with WritePictureL() or through a custom interface. After pictures have been processed, 
    they must be returned to the decoder using ReturnPicture().

    @param  "aDevice"   "The decoder source plug-in to use."
    @pre    "This method can only be called before the hwdevice has been initialized with Initialize()."
    */
    void SetInputDevice(CMMFVideoDecodeHwDevice* aDevice);

    /**
    Writes an uncompressed video picture to the post-processor. The picture must be returned to the 
    client or source plug-in after it has been used.

    @param  "aPicture"  "The picture to write."
    @leave  "This method may leave with one of the system-wide error codes."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void WritePictureL(TVideoPicture* aPicture);    

    /**
    Retrieves post-processing information about this hardware device. 
    The device creates a CPostProcessorInfo structure, fills it with correct data, pushes it 
    to the cleanup stack and returns it. The client will delete the object when it is no 
    longer needed.

    @return "Post-processor information as a CPostProcessorInfo object. 
            The object is pushed to the cleanup stack, and must be deallocated by the caller."
    @leave  "This method may leave with one of the system-wide error codes.
    */
    CPostProcessorInfo* PostProcessorInfoLC();

    /**
    Retrieves the list of the output formats that the device supports. The list is ordered in 
    plug-in preference order, with the preferred formats at the beginning of the list. The list 
    can depend on the device source format, and therefore SetSourceFormatL() must be called before 
    calling this method.

    @param "aFormats" "An array for the result format list. The array must be created and destroyed by the caller."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method may only be called before the hwdevice has been initialized using Initialize()."
    */
    void GetOutputFormatListL(RArray<TUncompressedVideoFormat>& aFormats);

    /**
    Sets the device output format.

    @param  "aFormat" "The format to use."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method may only be called before the hwdevice has been initialized using Initialize()."
    */
    void SetOutputFormatL(const TUncompressedVideoFormat &aFormat);

    /**
    Sets the clock source to use for video timing. If no clock source is set. video playback 
    will not be synchronized, but will proceed as fast as possible, depending on input data 
    and output buffer availability. 

    @param  "aClock" "The clock source to be used."
    @pre    "This method can only be called before the hwdevice has been initialized with Initialize()."
    */
    void SetClockSource(MMMFClockSource* aClock);

    /**
    Sets the device video output destination. The destination can be the screen (using direct 
    screen access) or memory buffers. By default memory buffers are used. If data is written 
    to another device, this method is ignored, and suitable memory buffers are always used.
    
    @param  "aScreen" "True if video output destination is the screen, false if memory buffers."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can only be called before the hwdevice has been initialized with Initialize()."
    */
    void SetVideoDestScreenL(TBool aScreen);

    /**
    Sets the post-processing types to be used.

    @param  "aPostProcCombination" "The post-processing steps to perform, a bitwise OR of values from TPostProcessType."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can be called either before or after the hwdevice has been initialized with Initialize().
            If called after initialization, the change must only be committed when CommitL() is called."
    */
    void SetPostProcessTypesL(TUint32 aPostProcCombination);

    /**
    Sets post-processing options for input (pan-scan) cropping.

    @param  "aRect" "The cropping rectangle to use."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can be called either before or after the hwdevice has been initialized with Initialize().
            If called after initialization, the change must only be committed when CommitL() is called."
    */
    void SetInputCropOptionsL(const TRect& aRect);

    /**
    Sets post-processing options for YUV to RGB color space conversion. 
    Specifies the input YUV and output RGB formats to use explicitly. SetSourceFormatL(), 
    SetOutputFormatL(), and SetPostProcessTypesL() must be called before this method is used.

    @param  "aOptions"      "The conversion options to use."
    @param  "aYuvFormat"    "Conversion source YUV format"
    @param  "aRgbFormat"    "Conversion target RGB format"
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can be called either before or after the hwdevice has been initialized with Initialize().
            If called after initialization, the change must only be committed when CommitL() is called."
    */
    void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, 
                                TRgbFormat aRgbFormat);

    /**
    Sets post-processing options for YUV to RGB color space conversion.
    Uses the device input and output formats. For decoder devices the default YUV format used is 
    the format specified in the input bitstream. SetSourceFormatL(), SetOutputFormatL(), and 
    SetPostProcessTypesL() must be called before this method is used.

    @param  "aOptions"      "The conversion options to use."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can be called either before or after the hwdevice has been initialized with Initialize().
            If called after initialization, the change must only be committed when CommitL() is called."
    */
    void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions);

    /**
    Sets post-processing options for rotation. SetPostProcessTypesL() must be called before 
    this method is used.

    @param  "aRotationType" "The rotation to perform."  
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can be called either before or after the hwdevice has been initialized with Initialize().
            If called after initialization, the change must only be committed when CommitL() is called."
    */
    void SetRotateOptionsL(TRotationType aRotationType);

    /**
    Sets post-processing options for scaling. SetPostProcessTypesL() must be called before 
    this method is used.
    
    @param  "aTargetSize"           "Scaling target size. If a fixed scale factor size is used, 
                                    the new dimensions must be set to width=floor(factor*width), 
                                    height=floor(factor*height). For example, scaling a 
                                    QCIF (176x144) picture up by a factor of 4/3 yields a size 
                                    of 234x192."
    @param  "aAntiAliasFiltering"   "True if anti-aliasing filtering should be used. 
                                    If the post-processor does not support anti-aliased scaling, 
                                    or supports anti-aliased scaling only, this argument is ignored."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can be called either before or after the hwdevice has been initialized with Initialize().
            If called after initialization, the change must only be committed when CommitL() is called."
    */
    void SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering);

    /**
    Sets post-processing options for output cropping. SetPostProcessTypesL() must be called before 
    this method is used.
    
    @param  "aRect" "Output cropping area."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can be called either before or after the hwdevice has been initialized with Initialize().
            If called after initialization, the change must only be committed when CommitL() is called."
    */
    void SetOutputCropOptionsL(const TRect& aRect);

    /**
    Sets post-processing plug-in specific options. SetPostProcessTypesL() must be called before 
    this method is used.

    @param  "aOptions" "The options. The format is plug-in specific."
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can be called either before or after the hwdevice has been initialized with Initialize().
            If called after initialization, the change must only be committed when CommitL() is called."
    */
    void SetPostProcSpecificOptionsL(const TDesC8& aOptions);

    /**
    Initializes the device. This method is asynchronous, the device will call 
    MMFVideoPlayProxy::MdvppInitializeComplete() after initialization has completed. After this 
    method has successfully completed, further configuration changes are not possible except where 
    separately noted.
    */
    void Initialize();

    /**
    Commit all changes since the last CommitL(), Revert() or Initialize()
    to the hardware device.  This only applies to methods which can be called both
    before AND after DevVideoPlay has been initialized.

    @see    SetPostProcessTypesL
    @see    SetInputCropOptionsL
    @see    SetYuvToRgbOptionsL
    @see    SetRotateOptionsL
    @see    SetScaleOptionsL
    @see    SetOutputCropOptionsL
    @see    SetPostProcSpecificOptionsL

    @leave  "The method will leave if an error occurs."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void CommitL();

    /**
    Revert all changes since the last CommitL(), Revert() or Initialize()
    back to their previous settings.  This only applies to methods which can 
    be called both before AND after DevVideoPlay has been initialized.

    @see    SetPostProcessTypesL
    @see    SetInputCropOptionsL
    @see    SetYuvToRgbOptionsL
    @see    SetRotateOptionsL
    @see    SetScaleOptionsL
    @see    SetOutputCropOptionsL
    @see    SetPostProcSpecificOptionsL

    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void Revert();

    /**
    Starts writing output directly to the display frame buffer using Direct Screen Access.

    @param  "aVideoRect"    "The video output rectangle on screen."
    @param  "aScreenDevice" "The screen device to use. The screen device object must be valid in the current thread."
    @param  "aClipRegion"   "Initial clipping region to use."
    
    @leave  "This method may leave with one of the system-wide error codes.
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void StartDirectScreenAccessL(const TRect& aVideoRect, 
        CFbsScreenDevice& aScreenDevice, const TRegion& aClipRegion);

    /**
    Sets a new clipping region for Direct Screen Access. After the method returns, no video will 
    be drawn outside of the region. If clipping is not supported, or the clipping region is too 
    complex, either playback will pause or will resume without video display, depending on the 
    current setting of SetPauseOnClipFail(), and the result can be verified with IsPlaying(). 
    Clipping can be disabled by setting a new clipping region that includes the whole video window.

    @param  "aRegion" "The new clipping region. After the method returns, no video will be drawn outside the region."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void SetScreenClipRegion(const TRegion& aRegion);

    /**
    Sets whether the system should pause playback when it gets a clipping region it cannot handle, 
    or Direct Screen Access is aborted completely. If not, processing will proceed normally, but no 
    video will be drawn. By default, playback is paused.

    @param "aPause" "True if playback should be paused when clipping fails, false if not. 
                    If playback is not paused, it will be continued without video display."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void SetPauseOnClipFail(TBool aPause);

    /**
    Aborts Direct Screen Access completely, to be called from MAbortDirectScreenAccess::AbortNow() 
    and similar methods. DSA can be resumed by calling StartDirectScreenAccessL().
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void AbortDirectScreenAccess();

    /**
    Indicates whether playback is proceeding. This method can be used to check whether playback was 
    paused or not in response to a new clipping region or DSA abort.

    @return "ETrue if video is still being played (even if not necessarily displayed)."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    TBool IsPlaying();

    /**
    Re-draws the latest video picture. Only available when DSA is being used. If DSA is aborted or a 
    non-supported clipping region has been set, the request may be ignored.
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void Redraw();


    /**
    Starts video playback, including decoding, post-processing, and rendering. Playback will proceed 
    until it has been stopped or paused, or the end of the bitstream is reached.
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void Start();

    /**
    Stops video playback. No new pictures will be decoded, post-processed, or rendered.
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void Stop();

    /**
    Pauses video playback, including decoding, post-processing, and rendering. No pictures will be 
    decoded, post-processed, or rendered until playback has been resumed.
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void Pause();

    /**
    Resumes video playback after a pause.
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void Resume();

    /**
    Changes to a new decoding and playback position, used for randomly accessing (seeking) the 
    input stream. The position change flushes all input and output buffers. Pre-decoder and 
    post-decoder buffering are handled as if a new bitstream was being decoded. If the device still
    has buffered pictures that precede the new playback position, they will be discarded. If playback
    is synchronized to a clock source, the client is responsible for setting the clock source to the 
    new position.
    
    @param "aPlaybackPosition" "The new playback position in the video stream."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition);

    /**
    Freezes a picture on the screen. After the picture has been frozen, no new pictures are 
    displayed until the freeze is released with ReleaseFreeze(). If the device output is being 
    written to memory buffers or to another plug-in, instead of the screen, no decoded pictures 
    will be delivered while the freeze is active, and they are simply discarded.

    @param "aTimestamp" "The presentation timestamp of the picture to freeze. The frozen picture 
                        will be the first picture with a timestamp greater than or equal to this 
                        parameter."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void FreezePicture(const TTimeIntervalMicroSeconds& aTimestamp);

    /**
    Releases a picture frozen with FreezePicture().

    @param "aTimestamp" "The presentation timestamp of the picture to release. The first picture 
                        displayed after the release will be the first picture with a timestamp 
                        greater than or equal to this parameter. To release the freeze immediately, 
                        set the timestamp to zero."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void ReleaseFreeze(const TTimeIntervalMicroSeconds& aTimestamp);


    /**
    Returns the current playback position, i.e. the timestamp for the most recently displayed or 
    virtually displayed picture. If the device output is written to another device, the most recent 
    output picture is used.

    @return "Current playback position."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    TTimeIntervalMicroSeconds PlaybackPosition();

    /**
    Returns the total amount of memory allocated for uncompressed pictures. This figure only 
    includes the pictures actually allocated by the plug-in itself, so that the total number of 
    bytes allocated in the system can be calculated by taking the sum of the values from all plug-ins.

    @return "Total number of bytes of memory allocated for uncompressed pictures."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    TUint PictureBufferBytes();

    /**
    Reads various counters related to decoded pictures. The counters are reset when Initialize() 
    or this method is called, and thus they only include pictures processed since the last call.

    Post-processor devices return the number of input pictures in iPicturesDecoded and 
    iTotalPictures. If the decoded pictures are written to another plug-in, they are considered 
    to be "virtually displayed".

    @param "aCounters" "The counter structure to fill."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& aCounters);


    /**
    Sets the computational complexity level to use. If separate complexity levels are not available, 
    the method call is ignored. If the level specified is not available, the results are undefined. 
    Typically the device will either ignore the request or use the nearest suitable level.

    The complexity level can be changed at any point during playback.

    @param "aLevel" "The computational complexity level to use. Level zero (0) is the most complex 
                    one, with the highest quality. Higher level numbers require less processing 
                    and may have lower quality."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void SetComplexityLevel(TUint aLevel);

    /**
    Gets the number of complexity levels available.
    
    @return "The number of complexity control levels available, or zero if the information is not 
            available yet. The information may not be available if the number of levels depends on 
            the input data, and enough input data has not been read yet. In that case, using level 
            zero is safe."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    TUint NumComplexityLevels();

    /**
    Gets information about a computational complexity level. This method can be called after 
    NumComplexityLevels() has returned a non-zero value - at that point the information is guaranteed 
    to be available. Some hardware device implementations may not be able to provide all values, 
    in that case the values will be approximated.

    @param "aLevel" "The computational complexity level to query. The level numbers range from zero 
                    (the most complex) to NumComplexityLevels()-1."
    @param "aInfo"  "The information structure to fill."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void GetComplexityLevelInfo(TUint aLevel, CMMFDevVideoPlay::TComplexityLevelInfo& aInfo);

    /**
    Returns a picture back to the device. This method is called by CMMFDevVideoPlay to return pictures 
    from the client (after they have been written with NewPicture()), or by the output device when 
    it has finished using a picture.

    @param "aPicture" "The picture to return. The device can re-use the memory for the picture."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void ReturnPicture(TVideoPicture* aPicture);

    /**
    Gets a copy of the latest picture sent to output.

    @param "aPictureData"   "Target picture. The memory for the picture must be allocated by the 
                            caller, and initialized properly. The data formats must match the snapshot 
                            format requested."
    @param "aFormat"        "The picture format to use for the snapshot."

    @return "ETrue if the snapshot was taken, EFalse if a picture is not available. The picture may not 
            be available if decoding has not progressed far enough yet."

    @leave  "The method will leave if an error occurs. Typical error codes used:
            * KErrNotSupported - The requested data format or picture size is not supported, or the 
            plug-in does not support snapshots."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
//  TBool GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat) {return EFalse;};
    TBool GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat);

    /**
    When the snapshot is available, it will be returned to the client using the TimedSnapshotComplete()
    callback. To cancel a timed snapshot request, use CancelTimedSnapshot(). Only one timed snapshot 
    request can be active at a time.

    @param "aPictureData"           "Target picture. The memory for the picture must be allocated by 
                                    the caller, and initialized properly. The data formats must match 
                                    the snapshot format requested. The picture must remain valid until 
                                    the snapshot has been taken or until the request has been cancelled 
                                    with CancelTimedSnapshot()."
    @param "aFormat"                "The picture format to use for the snapshot."
    @param "aPresentationTimestamp" "Presentation timestamp for the picture to copy."

    @leave  "The method will leave if an error occurs. Typical error codes used:
            * KErrNotSupported - The requested data format or picture size is not supported or 
            the plug-in does not support timed snapshots."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    //void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, 
    //                        const TTimeIntervalMicroSeconds& aPresentationTimestamp){};
    void GetTimedSnapshotL(TPictureData*, const TUncompressedVideoFormat&, const TTimeIntervalMicroSeconds&){};


    /**
    When the snapshot is available, it will be returned to the client using the TimedSnapshotComplete()
    callback. To cancel a timed snapshot request, use CancelTimedSnapshot(). Only one timed snapshot 
    request can be active at a time.

    @param "aPictureData"           "Target picture. The memory for the picture must be allocated by 
                                    the caller, and initialized properly. The data formats must match 
                                    the snapshot format requested. The picture must remain valid until 
                                    the snapshot has been taken or until the request has been cancelled 
                                    with CancelTimedSnapshot()."
    @param "aFormat"                "The picture format to use for the snapshot."
    @param "aPictureId"             "Picture identifier for the picture to copy."

    @leave  "The method will leave if an error occurs. Typical error codes used:
            * KErrNotSupported - The requested data format or picture size is not supported or 
            the plug-in does not support timed snapshots."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
//    void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, 
//                            const TPictureId& aPictureId){};
    void GetTimedSnapshotL(TPictureData*, const TUncompressedVideoFormat&, const TPictureId& ){};

    /**
    Cancels a timed snapshot request.
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void CancelTimedSnapshot(){};

    /**
    Gets a list of the supported snapshot picture formats.

    @param "aFormats" "An array for the result format list. The array must be created and destroyed by 
    the caller."

    @leave "This method may leave with one of the standard error codes."
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>&){};


    /**

    Notifies the hardware device that the end of input data has been reached and no more input data 
    will be written. The hardware device can use this signal to ensure that the remaining data gets 
    processed, without waiting for new data. For example when the data type is not EDuCodedPicture,
    calling this method is necessary otherwise a hardware device implementation might be looking for 
    the start code for the next picture to ensure it has a complete picture before starting to decode
    the previous one. 
    
      
    After the remaining data has been processed (and displayed, if applicable), the hardware 
    device must notify the proxy with the MdvppStreamEnd() callback.

    DevVideo clients are encouraged to call this method, but its use is not mandatory for synchronized
    processing.  For synchronized playback, all video pictures are processed or discarded according to 
    their timestamps, and so the client can easily infer when processing is complete.  However, it 
    should be noted that the last picture might not be displayed if this method is not called and the 
    input data type is not EDuCodedPicture.

    For non-synchronized playback (e.g. file conversion), a client must call this method otherwise it
    will never find out when the hardware device has finished processing the data.
    
    @pre    "This method can only be called after the hwdevice has been initialized with Initialize()."
    */
    void InputEnd();

    /**
    Retrieves a custom interface to the device.
    @param  "aInterface"    "Interface UID, defined with the custom interface."
    @return "Pointer to the interface implementation, or NULL if the device does not 
            implement the interface requested. The return value must be cast to the 
            correct type by the user."
    */
    TAny* CustomInterface(TUid aInterface);  
    
    // === MNGAPostProcSessionManagerObserver ===
    /**
    Set the proxy implementation to be used. Called just after the object is constructed.
    @param  "aProxy"    "The proxy to use."
    */
    void BufferAvailable(TInt aBufId, TInt aStatus);
    
public: 

    // === MMmfVideoBufferManagement ===
    
    /** 
     * Sets the observer object to use. The observer gets notified
     * when new buffers are available and when buffers need to be
     * released back to the media device.
     *
     * This method can only be called before the media device has
     * been Initialized with InitializeL().
     * 
     * @param aObserver The observer object to use.
     */
    void MmvbmSetObserver(MMmfVideoBufferManagementObserver* aObserver);
    
    /** 
     * Enables input buffer management mode. In buffer management mode
     * the target media device allocates memory for input buffers and
     * the client can only use input buffers allocated with
     * MmvbmGetBufferL().
     *
     * This method can only be called before the media device has been
     * Initialized with InitializeL(). This method must be called if
     * the client uses MmvbmGetBufferL().
     * 
     * @param aEnable ETrue if input buffer management mode is used.
     */
    void MmvbmEnable(TBool aEnable);
    
    /** 
     * Sets the buffer options to use. The client can request the
     * number of input buffers that should be available, but typically
     * only few (one or two) buffers can be used.
     *
     * This method can only be called before the media device has been
     * Initialized with InitializeL().
     * 
     * @param aOptions The buffer options to use, see TBufferOptions.
     *
     * @leave KErrNotSupported The requested buffer options are not
     *   supported. Typically the client has requested too many input
     *   buffers.
     */
    void MmvbmSetBufferOptionsL(const TBufferOptions& aOptions);
    
    /** 
     * Gets the buffer options currently in use.
     *
     * This method can only be called before the media device has been
     * Initialized with InitializeL().
     * 
     * @param aOptions Target buffer options data structure.
     */
    void MmvbmGetBufferOptions(TBufferOptions& aOptions);
    
    /** 
     * Gets a new input picture buffer. The client can then write data
     * into the buffer and write it back to the media device with
     * WritePictureL().
     *
     * This method can only be called after the media device has been
     * Initialized with InitializeL(). This method can only be called
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
    TVideoPicture* MmvbmGetBufferL(const TSize& aSize);
    
    /** 
     * Releases an input buffer back to the media device without using
     * it. This method is mainly used as a response to a
     * MmvbmReleaseBuffers() callback.
     * 
     * @param aBuffer The buffer to release.
     */
    void MmvbmReleaseBuffer(TVideoPicture* aBuffer);    
    
public:
    
     // === MMMFVideoSurfaceSupport ===
    
	/** 
    Requests the media device to use graphics surfaces for video rendering. 
    The client must call this method before Initialize() to ensure the media 
    device allocates the right types of resources for rendering.
    */
    void MmvssUseSurfaces();
         
    /** 
    Sets a new video surface support observer to receive surface management events 
    from the media device.
     
    @param aObserver New observer object to use.
    */
    void MmvssSetObserver(MMMFVideoSurfaceObserver& aObserver);
    
    /** 
    Retrieves surface parameters for a display. The client typically calls this in response 
    to a MmvsoSurfaceCreated() or MmvsoSurfaceParametersUpdated() observer callback.
     
    @param  aSurfaceId
	        Surface ID for the display.
	@param  aCropRect
	        Cropping rectangle within the surface. The crop rectangle identifies the area of 
	        the surface that should be shown on the screen.
	@param  aPixelAspectRatio
	        Video picture pixel aspect ratio.

	@leave KErrNotReady if no surface is available for the display.
    */
    
    void MmvssGetSurfaceParametersL(TSurfaceId& aSurfaceId, TRect& aCropRect, 
    					TVideoAspectRatio& aPixelAspectRatio);
    /** 
    Indicates that the surface is no longer in use and can be destroyed. The client typically calls
    this in response to MmvsoSurfaceCreated() (while old surface is already in use and the current
    should be removed) or MmvsoRemoveSurface() observer callback.  
         
    @param aSurfaceId Surface ID that is no longer in use
    
    @leave KErrNotFound if the surface does not exist
    */
    void MmvssSurfaceRemovedL(const TSurfaceId& aSurfaceId);    

public:    

    // === MMmfVideoPropertiesObserver ===
    
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
    virtual void MmvpoUpdateVideoProperties(const TYuvFormat& aYuvFormat, const TSize& aPictureSize);
    
public:

    // === MMmfVideoResourceObserver ===
    
    /** 
    Indicates that a media device has lost its resources. The client must
    synchronously pause or stop processing before returning from this method
    so that the media device can release its resources to the system. If
    the client does not pause or stop, resource loss will be handled as a
    fatal error.
     
    The client may start again or resume after receiving a
    MvroResourcesRestored() callback.
     
    @param aMediaDevice UID for the media device that lost resources. The
                        client can use this e.g. to determine whether the
                        decoder or the post-processor lost resources. This
                        is typically not required though since the client
                        must pause DevVideo entirely.
    */
    virtual void MmvroResourcesLost(TUid aMediaDevice);

    /** 
    Indicates that a media device has regained its resources after a
    previous resource loss. The client can restart or resume processing.
    This can be done either synchronously or asynchronously.
     
    @param aMediaDevice UID for the media device that regained resources.
    */
    virtual void MmvroResourcesRestored(TUid aMediaDevice);
            
    
    // === MMmfVideoSurfaceHandleControl ===
    
    /** 
    Sets an external surface ID. This should be called as soon as external surface is created.    
     
    @param aSurfaceID external surface ID.
        
    */
    
    virtual void MmvshcSetSurfaceHandle(const TSurfaceId& aSurfaceHandle);
    
    /** 
    Sets a redraw buffer to be used during resource loss.    
     
    @param aRedrawBuffer redraw buffer.
        
    */
     virtual void MmvshcRedrawBufferToSurface(TPtrC8& aRedrawBuffer);
     
     /**
      * Creates a surface and maps the surface with the chunk handle passed in.
      * @param aAttributes
      * @param aHandle is the Chunk handle for the data.
      * @param aSurfaceId returns the surfaceId that was just created
      * 
      * @return error code
      */
         
     virtual TInt MmvshcCreateSurface(const RSurfaceManager::TSurfaceCreationAttributes& aAttributes, TInt aHandle, TSurfaceId& aSurfaceId);
   
public:
	// === MMmfVideoPropertiesNotifier ===
    /** 
    Sets a new video properties observer. This method can be called at any
    time after the media device has been instantiated.
     
    @param aObserver New observer object.
    */
    virtual void MmvpnSetObserver(MMmfVideoPropertiesObserver* aObserver);

public:
	
	void ReturnPicToDecoder(TVideoPicture* aPic);
	TInt AttemptToPost();
	
public:
    //=== MMdfTrickPlayControl === =     
    /**        
     * Query the Direction capabilities from the MDF decoders and       
     * post processor.       
     *        
    */       
    virtual void MmvprcGetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCapabilities);

    /**       
     * Sets the playback speed. A negative rate means play backward.       
     * +/- percentage.       
     *        
     */       
    virtual void MmvprcSetPlayRateL(const TInt aRate);

    /**       
     * Gets the playback speed. A negative rate means play backward.       
     * +/- percentage.       
     *        
     */       
    virtual TInt MmvprcPlayRateL();

    /**       
     * Sets the step frame number in frame step mode       
     * +/- frames       
     */       
    virtual void MmvprcStepFrameL(const TInt aStep);

    /**       
     * Registers the observer       
     */       
    virtual void MmvprcSetObserver(MMmfVideoPlayRateObserver& aObserver); 

public: 
     
    // === MMmfVideoSecureOutput ===
    /**
     * Notifies the media device if the content can only be sent to a
     * secure output. The device display is typically considered a
     * secure output, but a high-quality unprotected analog or digital
     * video output connector is not. By default all content can be
     * sent to both secure and insecure outputs.
     *
     * This method can only be called before the media device has been
     * initialised with InitializeL().
     *
     * @param aSecure Set to ETrue if the content can only be sent to
     *     a secure output, EFalse if both secure and unsecure outputs can
     *     be used.
     *
     * @leave KErrNotSupported Insecure outputs cannot be
     * disabled. The client should not play protected content.
     */    
     virtual void MmvsoSetSecureOutputL(TBool aSecure);

public:
     //=== AdvancedVideoSecureOutput === =     
     virtual void MmavsoSetAllowedOutputL(TUint aAllowedOutputMask);
    
public:
     // === MMmfPostInitializeRequest ===
     virtual void MmpirPostInitializeRequest(MMmfPostInitializeResponse& aResponse);
     
protected: 

    // === CMMFVideoPostProcHwDevice ===
    /**
    Set the proxy implementation to be used. Called just after the object is constructed.
    @param  "aProxy"    "The proxy to use."
    */
    void SetProxy(MMMFDevVideoPlayProxy& aProxy);
    
protected:

    /**
     *  Symbian 2nd phase constructor .
     */
    void ConstructL();
    
private:    
	TInt IsTimeToPost(TVideoPicture* frame, TInt64& delta);
	TVideoPicture* CreateBuffersL(TInt aBufId);
    void CreateVBMBuffersL();
    TInt SetupExternalSurface(const TSurfaceId &aSurfaceID);
    void ReleaseInputQ();
    void ReleaseProcessQ();
    void ReleasePicture(TVideoPicture *pic);
    void PublishSurfaceCreated();
    void PublishSurfaceUpdated();
    TInt SetupSurface();
    void SetSurfaceAttributes(const TSize& aSize, TInt aNumBuf);
    TInt GetID(TVideoPicture *aPicture);
    TInt GetExternalBufferID(TVideoPicture *aPicture);
    TInt RegisterSurface(const TSurfaceId& aSurfaceId);
    TInt IsGceReady();
    void SetTimer(TInt64 aDelta);
    TInt ConvertPostProcBuffer(TVideoPicture* pSrc, TVideoPicture* pDest);
    void AddPictureToVBMQ(TVideoPicture *pic);
    void AddPictureToColorConversionQ(TVideoPicture *pic);
    void ResetCountingBuffer();
    void PicturesSkipped();
    TVideoPicture* DoColorConvert(TVideoPicture* aPicture);

    /**       
     Adds a picture to the Input queue. Based on the timestamp of the picture,
     it is either appeneded at the end of the queue or inserted at the 
     appropriate position. The queue is arranged in the ascending order. 
     The ret value indicates if the head of the queue was changed or not.         
    */  
    TInt AddToQ(TVideoPicture* aPicture);
    
    /**       
     Removes a picture from Input queue based on the playrate.
     If the playrate is +ve ie forward playback head will be removed
     and if the playrate is -ve ie backward playback tail
     will be removed.         
    */  
    void RemoveFromQ();
    
    /**       
     Returns a picture from Input queue based on the playrate.
     If the playrate is +ve ie forward playback head will be returned
     and if the playrate is -ve ie backward playback tail  
     will be returned.       
    */  
    TVideoPicture* PeekQ();

    /**
    Adds a surface hint to a video surface. If there is any hint already added, 
    the surface is updated with the new hint. 
    */		
    TInt AddHints();
    
	#ifdef _DUMP_YUV_FRAMES
    void captureYuv(TVideoPicture* aPicture);
    #endif
    
    TInt SetSourceFormat();
    TInt SetSourceRange();
    TInt ColorConvert(tBaseVideoFrame* aInputFrame, TUint8* aDestPtr, tWndParam* aInputCropWindow, tWndParam* aOutputCropWindow);

private:

    //
    // C++ default constructor.  
    //
    CNGAPostProcHwDevice();

    //
    // State of post processor
    //
    enum TPPState
    {
        EInitializing,
        EInitialized,
        EPlaying,
        EPaused,
        EStopped
    };
    
    enum TTimeToPost
    {
        ESkipIt = -1,
        EPostIt = 0,
        EDelayIt = 1,
    };
    
private: 

    MMMFDevVideoPlayProxy*      				iProxy;															
    CMMFVideoDecodeHwDevice*    				iInputDecoderDevice;															
    RArray<TVideoPicture*>              		iInputQ;															
    RArray<TVideoPicture*>              		iProcessQ;															
    MMMFClockSource*            				iClockSource;															
    TTimeIntervalMicroSeconds					iCurrentPlaybackPosition;															
    TPPState        							iPPState;															
    CNGAPostProcSurfaceHandler*					iSurfaceHandler;
    CNGAPostProcSessionManager*					iSessionManager;
	
	RSurfaceManager::TSurfaceCreationAttributesBuf	iAttributes;
	RChunk										iChunk;
	RSurfaceManager::TInfoBuf 					iInfo;
	TSurfaceId									iSurfaceId;
	TBool           							iIsInputEnded;
	CNGAPostProcTimer*							iPostingTimer;
	CMMFDevVideoPlay::TPictureCounters  		iPictureCounters;
	TBool 										iFirstPictureUpdated;
	TBool 										iUsingExternalSurface;
    TBool           							iIsColorConversionNeeded;
    RArray<TVideoPicture*>              		iColorConversionQ;
    TBool 										iSurfaceCreatedEventPublished;
    TInt                                		iOverflowPictureCounter;
    TInt 								    	iVideoFrameBufSize;	
    TBool 										iResourceLost;
    TBool 										iRedrawDone;
	// Flag to indicate that the redraw surface has been created, and
	// can be used in a subsequent call ro Redraw()
	TBool										iRedrawSurfaceInUse;

    //-- members for buffer management --
    MMmfVideoBufferManagementObserver*  		iVBMObserver;
    TBufferOptions                      		iVBMBufferOptions;
    RArray<TVideoPicture*>              		iVBMBufferReferenceQ;
    RArray<TVideoPicture*>              		iVBMBufferQ;
    TBool                               		iVBMEnabled;
    RArray<TUncompressedVideoFormat>    		iSupportedInputFormats;
    TInt										count;
    
    //-- members for Surface Hints --
    RSurfaceManager::THintPair					iHint;
    TUint 										iSurfaceMask;
    TUid										iSurfaceKey;
            
    //-- members for Surface support --
    MMMFVideoSurfaceObserver*					iVideoSurfaceObserver;
	MMmfVideoPropertiesObserver*				iVPObserver;
	RWsSession									iWsSession;
	TSize										iPicSize;
	TUint										iAspectRatioNum;
	TUint										iAspectRatioDenom;
	//-- members for Trickplay support --
	TInt                                iStepFrameCount;
    TInt                                iPlayRate;       
    TBool                               iKeyFrameMode;       
    MMmfVideoPlayRateObserver*          iFPObserver;       
    TUint8                              iSkippedFramesCountingBuffer[64];       
    TUint8                              iSkippedFramesInLast64Frames;       
    TUint8                              iCurrentPosInFramesCountingBuffer; 
	TUncompressedVideoFormat 			iVideoFormat;
        //---------- utility variables -------
	// Image source format
	TUint8 								iSourceFormat;

	//Image range
	TUint8		 						iSourceRange;

    //-- members for PostInitializeRequest support --
    MMmfPostInitializeResponse* iPostInitializeResponse;
    TBool iIsExternalChunk;
};    

/**
 * Timer of Posting
 */

class CNGAPostProcTimer: public CTimer
    {
public:
    static CNGAPostProcTimer* NewL( CNGAPostProcHwDevice& aParent );
    ~CNGAPostProcTimer();
    
protected:    
    void RunL();
    
private:
    CNGAPostProcTimer( CNGAPostProcHwDevice& aParent );
    void ConstructL();
    
private:
    CNGAPostProcHwDevice& iParent;
};

#endif //__NGAPOSTPROCHWDEVICE_H__

