// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Part of the MVS Agents for TechView
//

#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#include <surfaceeventhandler.h>
#endif

#include "videoplayagent.h"

/**
Constructs and initialises a new instance of the MVS video play agent

This function leaves if the video play agent object cannot be created.

@param  aObserver
        A client class to receive notifications from the video player.

@return A pointer to the new video play agent object.
*/
EXPORT_C CMVSVideoPlayAgent* CMVSVideoPlayAgent::NewL(MMVSClientObserver& aObserver)
	{
	CMVSVideoPlayAgent* self = new(ELeave) CMVSVideoPlayAgent(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMVSVideoPlayAgent::ConstructL()
	{
	User::LeaveIfError(iFileLogger.Connect());
    iFileLogger.CreateLog(_L("LogMVSappUi"),_L("LogFile.txt"),EFileLoggingModeAppend);
    }


/**
Destructor. Closes any open video clips and frees any resources held by the Video Player.
*/
EXPORT_C CMVSVideoPlayAgent::~CMVSVideoPlayAgent()
	{
#ifdef SYMBIAN_BUILD_GCE
    delete iVideoPlayer2;
    iVideoPlayer2 = NULL;
#endif  // SYMBIAN_BUILD_GCE
    
	delete iVideoPlayer;
	iVideoPlayer = NULL;

    if(iFileLogger.Handle())
     	{
     	iFileLogger.CloseLog();
	 	iFileLogger.Close();
     	}
     	
    iFileName.Close();
    }


CMVSVideoPlayAgent::CMVSVideoPlayAgent(MMVSClientObserver& aObserver)
    : iObserver(aObserver),
      iControllerUid(KNullUid)
	{
	}

/**
Check if CVideoPlayerUtility2 is used for current video playback.

@return ETrue if CVideoPlayerUtility2 is used for video playback.  EFalse if 
        CVideoPlayerUtility is used.
*/
EXPORT_C TBool CMVSVideoPlayAgent::SupportVideoPlayerUtility2() const
    {
    if (iCurrentVideoPlayer == NULL)
        {            
        return EFalse;
        }
    
    if (iCurrentVideoPlayer == iVideoPlayer)
        {
        return EFalse;        
        }
        
    return ETrue;
    }

/**
Set the auto scale on the current video player for the specified window.

@see CVideoPlayerUtility
*/
#ifdef SYMBIAN_BUILD_GCE
EXPORT_C void CMVSVideoPlayAgent::SetAutoScaleL(RWindow& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
	iVideoPlayer2->SetAutoScaleL(aWindow, aScaleType, aHorizPos, aVertPos);
#else
EXPORT_C void CMVSVideoPlayAgent::SetAutoScaleL(RWindow& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
	User::Leave(KErrNotSupported);
#endif
	}

/**
Set the auto scale on the current video player.

@see CVideoPlayerUtility
*/
EXPORT_C void CMVSVideoPlayAgent::SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
#ifdef SYMBIAN_BUILD_GCE
	iCurrentVideoPlayer->SetAutoScaleL(aScaleType, aHorizPos, aVertPos);
#else
	// Remove warnings
	aScaleType = aScaleType;
	aHorizPos = aHorizPos;
	aVertPos = aVertPos;
	User::Leave(KErrNotSupported);
#endif
	}

/**
Video output settings, pre initialisation of all the required settings to play a 
video clip

This function leaves if the video player utility object cannot be created.

@param  aObserver
        A client class to receive notifications from the video player.
@param  aPriority
        This client's relative priority. This is a value between EMdaPriorityMin and
        EMdaPriorityMax and represents a relative priority. A higher value indicates
        a more important request.
@param  aPref
        The required behaviour if a higher priority client takes over the sound output device.
        One of the values defined by TMdaPriorityPreference.
@param  aWs
        The window server session id.
@param  aScreenDevice
        The software device screen.
@param  aWindow
        The display window.
@param  aScreenRect
        The dimensions of the display window.
@param  aClipRect
        The area of the video clip to display in the window.
*/     
EXPORT_C void CMVSVideoPlayAgent::SetVideoOutputL(RWsSession& aWs,
                                                  CWsScreenDevice& aScreenDevice,
                                                  RWindow& aWindow,
                                                  TRect& aScreenRect,
                                                  TRect& aClipRect)
	{
	iFileLogger.Write(_L("SettingVideoOutput"));

#ifdef SYMBIAN_BUILD_GCE
    // if CVideoPlayerUtility2 is available, instantiate both CVideoPlayerUtility2 &
    // CVideoPlayerUtility, as it is not clear whether the controller used for a 
    // particular media file also supports the CVideoPlayerUtility2 API until OpenFileL
    // is called.  
	if (iVideoPlayer2)
	    {
        delete iVideoPlayer2;
        iVideoPlayer2 = NULL;
	    }
	    
    iVideoPlayer2 = CVideoPlayerUtility2::NewL(*this, 
                                               EMdaPriorityNormal, 
    	                                       EMdaPriorityPreferenceNone);
#endif  // SYMBIAN_BUILD_GCE

	if (iVideoPlayer)
	    {
        delete iVideoPlayer;
        iVideoPlayer = NULL;
	    }	
    iVideoPlayer = CVideoPlayerUtility::NewL(*this, 
                                             EMdaPriorityNormal, 
    	                                     EMdaPriorityPreferenceNone, 
    	                                     aWs, 
    	                                     aScreenDevice,
    	                                     aWindow,
    	                                     aScreenRect,
    	                                     aClipRect);
	}

/**
Adds a new window for displaying the video picture.  This is only applicable if 
CVideoPlayerUtility2 is used.

This function leaves if the video player utility object failed to add display window or
if this function is called when CVideoPlayerUtility2 is not being used.

@param  aObserver
        A client class to receive notifications from the video player.
@param  aPriority
        This client's relative priority. This is a value between EMdaPriorityMin and
        EMdaPriorityMax and represents a relative priority. A higher value indicates
        a more important request.
@param  aPref
        The required behaviour if a higher priority client takes over the sound output device.
        One of the values defined by TMdaPriorityPreference.
@param  aWs
        The window server session id.
@param  aScreenDevice
        The software device screen.
@param  aWindow
        The display window.
@param  aScreenRect
        The dimensions of the display window.
@param  aClipRect
        The area of the video clip to display in the window.
@leave  KErrNotSupported if CVideoPlayerUtility2 is not the current video utility in use
@see    CVideoPlayerUtility2::AddDisplayWindowL       
*/     
#ifdef SYMBIAN_BUILD_GCE
EXPORT_C void CMVSVideoPlayAgent::AddDisplayWindowL(RWsSession& aWs,
                                                    CWsScreenDevice& aScreenDevice,
                                                    RWindow& aWindow,
                                                    const TRect& aScreenRect,
                                                    const TRect& aClipRect)
#else
EXPORT_C void CMVSVideoPlayAgent::AddDisplayWindowL(RWsSession& /* aWs */,
                                                    CWsScreenDevice& /* aScreenDevice */,
                                                    RWindow& /* aWindow */,
                                                    const TRect& /* aScreenRect */,
                                                    const TRect& /* aClipRect */)
#endif  // SYMBIAN_BUILD_GCE
    {
#ifdef SYMBIAN_BUILD_GCE
    if (SupportVideoPlayerUtility2())
        {
        iVideoPlayer2->AddDisplayWindowL(aWs, 
                                         aScreenDevice, 
                                         aWindow, 
                                         aScreenRect,
                                         aClipRect);
        iWindow = &aWindow;

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
        if (iEnableSubtitlesOnAdd)
        	{
        	iVideoPlayer2->EnableSubtitlesL();
        	iEnableSubtitlesOnAdd = EFalse;
        	}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
        
        return;
        }
#endif  // SYMBIAN_BUILD_GCE        
    User::Leave(KErrNotSupported);
    }

#ifdef SYMBIAN_BUILD_GCE
EXPORT_C void CMVSVideoPlayAgent::AddDisplayWindowL(RWsSession& aWs,
                                                    CWsScreenDevice& aScreenDevice,
                                                    RWindow& aWindow)
#else
EXPORT_C void CMVSVideoPlayAgent::AddDisplayWindowL(RWsSession& /* aWs */,
                                                    CWsScreenDevice& /* aScreenDevice */,
                                                    RWindow& /* aWindow */)
#endif  // SYMBIAN_BUILD_GCE
    {
#ifdef SYMBIAN_BUILD_GCE
    if (SupportVideoPlayerUtility2())
        {
        iVideoPlayer2->AddDisplayWindowL(aWs, 
                                         aScreenDevice, 
                                         aWindow);
        iWindow = &aWindow;
        
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
        if (iEnableSubtitlesOnAdd)
        	{
        	iVideoPlayer2->EnableSubtitlesL();
        	iEnableSubtitlesOnAdd = EFalse;
        	}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
        return;
        }
#endif  // SYMBIAN_BUILD_GCE        
    User::Leave(KErrNotSupported);
    }

EXPORT_C void CMVSVideoPlayAgent::AddDisplayL(RWsSession& aWs, TInt aDisplay, MMMFSurfaceEventHandler& aEventHandler)
	{
	if (!SupportVideoPlayerUtility2())
		{
		User::Leave(KErrNotSupported);
		}
	
	iVideoPlayer2->AddDisplayL(aWs, aDisplay, aEventHandler);
	}

/**
Removes a window that is currently being used to display the video picture.

This function should only be called if CVideoPlayerUtility2 is currently being used for display 
of video.

@param  aWindow
        The window to be removed
@leave  KErrNotSupported if CVideoPlayerUtility2 is not the current video player utility.
@see    CVideoPlayerUtility2::RemoveDisplayWindow
*/                                    
EXPORT_C void CMVSVideoPlayAgent::RemoveDisplayWindowL(RWindow& aWindow)
    {
#ifdef SYMBIAN_BUILD_GCE
    if (SupportVideoPlayerUtility2())
        {
        iVideoPlayer2->RemoveDisplayWindow(aWindow);
        iWindow = NULL;        
        return;
        }
#endif  // SYMBIAN_BUILD_GCE
	// Remove compiler warning
	aWindow = aWindow;        
    User::Leave(KErrNotSupported);
    }

EXPORT_C void CMVSVideoPlayAgent::RemoveDisplay(TInt aDisplay)
	{
	if (!SupportVideoPlayerUtility2())
		{
		User::Leave(KErrNotSupported);
		}

	iVideoPlayer2->RemoveDisplay(aDisplay);
	}

/**
Opens a video clip from a file.

This function opens a video clip from a file,this function searches through a list of all available
plugins and attempts to use each one until successful or the end of the list is reached.

Once the opening of the video clip is complete, successfully or otherwise, the callback function 
CMVSVideoPlayAgent::MvpuoOpenComplete() is called.

This function leaves if errors are encountered opening the specified video clip file, or in initialising a 
specified/unspecified controller plugin.

This function can leave with one of the specified error codes. 

@param  aFileName
        The full path name of the file containing the video data.
*/
EXPORT_C void CMVSVideoPlayAgent::OpenFileL(const TDesC& aFileName)
	{
    OpenFileL(aFileName, KNullUid);                
    }

EXPORT_C void CMVSVideoPlayAgent::OpenFileL(const TDesC& aFileName, TUid aControllerUid)
	{
	iFileLogger.Write(_L("Opening a File For CMVSVideoPlayAgent "));
	
	// remembers the file name & the controller Uid used for opening file
	iFileName.Close();
	iFileName.CreateL(aFileName);
	iControllerUid = aControllerUid;

    __ASSERT_DEBUG(iVideoPlayer, User::Leave(KErrNotReady));
    iVideoPlayer->Close();	
    
#ifdef SYMBIAN_BUILD_GCE
    __ASSERT_DEBUG(iVideoPlayer2, User::Leave(KErrNotReady));
    iVideoPlayer2->Close();

    // First attempt is always to open a file with CVideoPlayerUtility2 if GCE 
    // is enabled
    iVideoPlayer2->OpenFileL(aFileName, aControllerUid);
    iCurrentVideoPlayer = iVideoPlayer2;
#else
    iVideoPlayer->OpenFileL(aFileName, aControllerUid);
    iCurrentVideoPlayer = iVideoPlayer;
#endif  // SYMBIAN_BUILD_GCE	

	// Now that we have selected the video player, we can issue the request for notification.
	if (iRequestNotify)
		{
		RegisterForNotification();
		}
	} 

EXPORT_C void CMVSVideoPlayAgent::OpenFileL(TMMSource& /*aSource*/)
	{
	}

EXPORT_C void CMVSVideoPlayAgent::OpenDesL(const TDesC8& /*aDescriptor*/)
	{
	}
		 
void CMVSVideoPlayAgent::MvpuoOpenComplete(TInt aError)
	{
	iFileLogger.Write(_L("CMVSVideoPlayAgent:MvpuoOpenComplete"));
	if(aError == KErrNone)
    	{
    	iState = EVideoOpening;
    	iFileLogger.Write(_L("MvpuoOpenComplete:VideoOpening"));
    	iObserver.UpdateStateChange(iState, KErrNone);

        iCurrentVideoPlayer->Prepare();            
    	}
#ifdef SYMBIAN_BUILD_GCE    	
    else if (iCurrentVideoPlayer == iVideoPlayer2 && aError == KErrNotSupported)
        {	
        // if the controller cannot support GS, try use CVideoPlayerUtility instead
        TRAPD(err, iVideoPlayer->OpenFileL(iFileName, iControllerUid));
        
        if (err != KErrNone)
        	{
        	iState = ENotReady; //init failed so from opening to NotReady
	     	iFileLogger.Write(_L("MvpuoOpenComplete:From Opening to  NotReady"));
	     	iObserver.UpdateStateChange(iState, err);
        	}
        
        iCurrentVideoPlayer = iVideoPlayer;
        
        // We are switching to a different controller.  Cancel the notification request on one
        // controller and reissue it on the other.
		if (iRequestNotify)
			{
			iVideoPlayer2->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
			RegisterForNotification();
			}
        }
#endif  // SYMBIAN_BUILD_GCE    	        
    else            
		{
     	iState = ENotReady; //init failed so from opening to NotReady
     	iFileLogger.Write(_L("MvpuoOpenComplete:From Opening to  NotReady"));
     	iObserver.UpdateStateChange(iState, aError);
		}
	}

void CMVSVideoPlayAgent::MvpuoPrepareComplete(TInt aError)
	{
	iFileLogger.Write(_L("CMVSVideoPlayAgent:MvpuoPrepareComplete"));
	if(aError == KErrNone)
    	{
    	iState = EVideoOpened;
        iFileLogger.Write(_L("MvpuoPrepareComplete:Video Opened"));
    	}
    else
		{
     	iState = ENotReady; //prepare failed so from preparing to NotReady
     	iFileLogger.Write(_L("MvpuoPrepareComplete failed : from preparing to NotReady")) ;
		}
	iObserver.UpdateStateChange(iState, aError);
	}

void CMVSVideoPlayAgent::MvpuoPlayComplete(TInt aError)
	{
	if(aError == KErrNone)
    	{
    	iState = EVideoOpened;
    	iFileLogger.Write(_L("MvpuoPlayComplete:Video Opened"));
    	}
    else
		{
     	iState = ENotReady; //playcomplete failed so from playcomplete to NotReady
     	iFileLogger.Write(_L("MvpuoPlayComplete failed : from playcomplete to NotReady")) ;
		}
	iObserver.UpdateStateChange(iState, aError);
	}

void CMVSVideoPlayAgent::MvpuoFrameReady(class CFbsBitmap&, TInt)
	{
	}

void CMVSVideoPlayAgent::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{	
	}	
	
void CMVSVideoPlayAgent::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	if(aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		iObserver.MvsResourceNotification(aNotificationData);
		}
	}
	
EXPORT_C void CMVSVideoPlayAgent::Prepare()
	{	
    iCurrentVideoPlayer->Prepare();	
	}


/**
Closes the video clip.
Updates the state to ENotReady after closing
*/
EXPORT_C void CMVSVideoPlayAgent::Close()
	{
	iFileLogger.Write(_L("CMVSVideoPlayAgent Closed"));
	
#ifdef SYMBIAN_BUILD_GCE
    if (SupportVideoPlayerUtility2())
        {            	
        // Can't leave at this point, this will just remove the leave scan warning
        TRAP_IGNORE(RemoveDisplayWindowL(*iWindow));
        iVideoPlayer2->Close();
        }
#endif  // SYMBIAN_BUILD_GCE
    iVideoPlayer->Close();
    
	iObserver.UpdateStateChange(ENotReady, KErrNone);
    iCurrentVideoPlayer = NULL;	
	}


/**
Begins playback of the initialised video sample at the current volume and priority levels.

When playing of the video sample is complete, successfully or otherwise, the callback function
CMVSVideoPlayAgent::MvpuoPlayComplete() is called.Updates the current state to EVideoPlaying,
and once playing is over it updates the state to EVideoOpen.
*/
EXPORT_C void CMVSVideoPlayAgent::Play()
	{
    iCurrentVideoPlayer->Play();

	iState = EVideoPlaying;
	iObserver.UpdateStateChange(iState, KErrNone);
	}


/**
Begins playback of the initialised video sample at the specified start and end points.

When playing of the video sample is complete, successfully or otherwise, the callback function
CMVSVideoPlayAgent::MvpuoPlayComplete() is called.Updates the current state to EVideoPlaying,
and once playing is over it updates the state to EVideoOpen.

@param  aStartTime
        The point at which to start playback.
@param  aEndTime
        The point at which to terminate playback.
*/
EXPORT_C void CMVSVideoPlayAgent::Play(const TTimeIntervalMicroSeconds& aStartPoint, 
                                       const TTimeIntervalMicroSeconds& aEndPoint)
	{
    iCurrentVideoPlayer->Play(aStartPoint, aEndPoint);

	iState = EVideoPlaying;
	iObserver.UpdateStateChange(iState, KErrNone);
	}


/**
Stops playback of the video sample as soon as is possible. 

If the video sample is playing, playback is stopped as soon as possible and the callback function 
CMVSVideoPlayAgent::MvpuoPlayComplete() is called.

If playback is already complete, this function has no effect. In addition, under these circumstances 
the callback function CMVSVideoPlayAgent::MvpuoPlayComplete() is not called either.

The position is reset to the beginning of the file.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
Also Updates the state to EVideoStopped
*/
EXPORT_C TInt CMVSVideoPlayAgent::Stop()
	{
	TInt err = KErrNotReady;
    if (iCurrentVideoPlayer)
		{
		err = iCurrentVideoPlayer->Stop();
		}

	if (err == KErrNone)
		{
		iState = EVideoStopped;
		iFileLogger.Write(_L("CMVSVideoPlayAgent Stopped"));
		}
	else
		{
		iState = ENotReady;
		}
    iObserver.UpdateStateChange(iState, err);
    
    return err;
	}


/**
Video playback is paused. The current position is maintained and playback can be
resumed by calling Play.
*/
EXPORT_C void CMVSVideoPlayAgent::PauseL()
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
	
    TRAPD(err, iCurrentVideoPlayer->PauseL());
	if(err == KErrNone)
		{
		iState = EVideoPaused;
		iFileLogger.Write(_L("CMVSVideoPlayAgent Paused "));
		}
	else
		{
		iState = ENotReady;
		iFileLogger.Write(_L("CMVSVideoPlayAgent Not Ready"));	
		}	
	iObserver.UpdateStateChange(iState, err);
	}
	

/**
Sends custom command to the plugin controller.
Passes handle and size of the drawable window.

Sets the display window. This is used to provide the video controller with an area of
the display to render the current video frame.

@param  aWs
        The window server session ID.
@param  aScreenDevice
        The software device screen.
@param  aWindowRect
        The window to display.
@param  aScreenRect
        The dimensions of the display window.
@param  aClipRect
        The area of the video clip to display in the window.
*/	
EXPORT_C void CMVSVideoPlayAgent::SetDisplayWindowL(RWsSession &aWs, CWsScreenDevice &aScreenDevice, RWindow &aWindow, const TRect &aWindowRect, const TRect &aClipRect)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
	
	iFileLogger.Write(_L("SettingDisplayWindow"));	
    iCurrentVideoPlayer->SetDisplayWindowL(aWs, aScreenDevice, aWindow, aWindowRect, aClipRect);
	}
	

/**
Sets the video frame rate.

@param  aFramesPerSecond
        The number of frames per second to request.
*/
EXPORT_C void CMVSVideoPlayAgent::SetVideoFrameRateL(TReal32 aFramesPerSecond)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
	
	iFileLogger.Write(_L("SettingVedioFrameRate "));
    iCurrentVideoPlayer->SetVideoFrameRateL(aFramesPerSecond);
	}


/**
Sets the playback volume for the audio track of the video clip.

The volume can be changed before or during playback and is effective immediately. The volume can
be set to any value between zero (mute) and the maximum permissible volume
(determined using MaxVolume()).

To determine if the current video clip contains an audio track, use AudioEnabledL().


@param  aVolume
        The volume, between 0 and MaxVolume().
*/
EXPORT_C void CMVSVideoPlayAgent::SetVolumeL(TInt aVolume)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));			
	
	iFileLogger.Write(_L("Setting CMVSVideoPlayAgent's Volume "));
	iCurrentVideoPlayer->SetVolumeL(aVolume);
	}
	

/**
Sets the current playback balance for the audio track of the video clip.

The balance can be any value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight,
the default value being KMMFBalanceCenter.

@param  aBalance
        The balance value to set.
*/
EXPORT_C void CMVSVideoPlayAgent::SetBalanceL(TInt aBalance)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));				
	
	iFileLogger.Write(_L("Setting CMVSVideoPlayAgent's Balance"));
	iCurrentVideoPlayer->SetBalanceL(aBalance);
	}
	

/**
Sets the playback priority. 

This is used to arbitrate between multiple objects simultaneously trying to accesses
the sound hardware.

@param  aPriority
        The priority level to apply, EMdaPriorityMin client can be interrupted by any other 
        client, EMdaPriorityNormal client can only be interrupted by a client with a higher 
        priority or EMdaPriorityMax client cannot be interrupted by other clients.
@param  aPref
        The time and quality preferences to apply, enumerated in TMdaPriorityPreference.
*/
EXPORT_C void CMVSVideoPlayAgent::SetPriorityL(TInt aPriority, TMdaPriorityPreference aPref)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));				
	
	iFileLogger.Write(_L("Setting Priority of CMVSVideoPlayAgent"));
	iCurrentVideoPlayer->SetPriorityL(aPriority, aPref);
	}
	

/**
Sets the position within the video clip from where to start playback.

@param  aPosition
        Position from start of clip in microseconds.
*/
EXPORT_C void CMVSVideoPlayAgent::SetPositionL(const TTimeIntervalMicroSeconds &aPosition)
    {
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));					

    iFileLogger.Write(_L("Setting Position of CMVSVideoPlayAgent"));
    iCurrentVideoPlayer->SetPositionL(aPosition);
    }
	

/**
Rotates the video image on the screen.

@param  aRotation
        The desired rotation to apply in 90 degree increments.
*/
EXPORT_C void CMVSVideoPlayAgent::SetRotationL(TVideoRotation aRotation)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));					
		
	iFileLogger.Write(_L("Setting Rotations of CMVSVideoPlayAgent"));
    iCurrentVideoPlayer->SetRotationL(aRotation);
	}
	


/**
Scales the video image to a specified percentage of its original size.

@param  aWidthPercentage
        The percentage (100 = original size) to be used to scale the width of the video image
@param  aHeightPercentage
        The percentage (100 = original size) to be used to scale the height of the video image. 
        If this is not equal to aWidthPercentage then the image may be distorted.
@param  aAntiAliasFiltering
        A boolean specifying if anti-aliasing should be used. True if anti-aliasing filtering 
        should be used.  If the plugin does not	support this kind of processing, 
        this value will be ignored.
*/
EXPORT_C void CMVSVideoPlayAgent::SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));					
		
	iFileLogger.Write(_L("Setting ScaleFactor of CMVSVideoPlayAgent"));
	iCurrentVideoPlayer->SetScaleFactorL(aWidthPercentage, aHeightPercentage, aAntiAliasFiltering);
	}


/**
Selects a region of the video image to be displayed.

@param  aCropRegion
        The dimensions of the crop region, relative to the video image.
@see   GetCropRegionL
*/
EXPORT_C void CMVSVideoPlayAgent::SetCropRegionL(const TRect& aCropRegion)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));					
	
	iFileLogger.Write(_L("Setting CorpRegion of CMVSVideoPlayAgent"));
    iCurrentVideoPlayer->SetCropRegionL(aCropRegion);
	}

/**
Selects the video extent on the screen, relative to the window.

@param  aVideoExtent
        The new video extent, relative to the window
@leave  KErrNotSupport if CVideoPlayerUtility is used for video playback
*/
#ifdef SYMBIAN_BUILD_GCE
EXPORT_C void CMVSVideoPlayAgent::SetVideoExtentL(const TRect& aVideoExtent)
#else
EXPORT_C void CMVSVideoPlayAgent::SetVideoExtentL(const TRect& /* aVideoExtent */)
#endif  // SYMBIAN_BUILD_GCE
	{
	iFileLogger.Write(_L("Setting VideoExtent of CMVSVideoPlayAgent"));
	
#ifdef SYMBIAN_BUILD_GCE	
	if(SupportVideoPlayerUtility2())
		{
		if (iWindow)
		    {		        
		    iVideoPlayer2->SetVideoExtentL(*iWindow, aVideoExtent);
		    }            		    
        else 
            {
		    iFileLogger.Write(_L("WARNING - Display Window has not been set.  Caller should save these values to be set again later."));
            }
		return;
		}
#endif  // SYMBIAN_BUILD_GCE		

	User::Leave(KErrNotSupported);
	}

/**
Selects the window clipping rectangle, relative to the window

@param  aWindowClipRect
        The new window clipping rectangle, relative to the window
@leave  KErrNotSupport if CVideoPlayerUtility is used for video playback
*/
#ifdef SYMBIAN_BUILD_GCE
EXPORT_C void CMVSVideoPlayAgent::SetWindowClipRectL(const TRect& aWindowClipRect)
#else
EXPORT_C void CMVSVideoPlayAgent::SetWindowClipRectL(const TRect& /* aWindowClipRect */)
#endif  // SYMBIAN_BUILD_GCE
	{
    iFileLogger.Write(_L("Setting WindowClippingRect of CMVSVideoPlayAgent"));
	
#ifdef SYMBIAN_BUILD_GCE
    if (SupportVideoPlayerUtility2())
        {            	
        if (iWindow)
            {                
            iVideoPlayer2->SetWindowClipRectL(*iWindow, aWindowClipRect);	
            }
        else 
            {
		    iFileLogger.Write(_L("WARNING - Display Window has not been set.  Caller should save these values to be set again later."));
            }
        return;
        }
#endif  // SYMBIAN_BUILD_GCE        														

    User::Leave(KErrNotSupported);
	}


/**
Gets the current frame. The current frame is requested, and will be sent to
the client asynchrynously.

@param  aDisplayMode
        The display mode for the retrieved frame.
*/
EXPORT_C void CMVSVideoPlayAgent::GetFrameL(TDisplayMode aDisplayMode)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));						
	iCurrentVideoPlayer->GetFrameL(aDisplayMode);
	}


/**
Gets the current frame. The current frame is requested, and will be sent to
the client asynchrynously.

@param  aDisplayMode
        The display mode for the retrieved frame.
@param  aIntent
        The DRM Intent to pass to the controller.
*/
EXPORT_C void CMVSVideoPlayAgent::GetFrameL(TDisplayMode aDisplayMode, ContentAccess::TIntent aIntent)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));							
	iCurrentVideoPlayer->GetFrameL(aDisplayMode, aIntent);
	}


/**
Returns the video frame rate in frames/second.

@return The video frame rate (frames/second).
*/
EXPORT_C TReal32 CMVSVideoPlayAgent::VideoFrameRateL()
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));							
	
	iFileLogger.Write(_L("Getting VideoFrameRate for CMVSVideoPlayAgent"));
	return iCurrentVideoPlayer->VideoFrameRateL();
	}


/**
Gets the video frame size.

@param  aSize
        The video frame size
*/
EXPORT_C void CMVSVideoPlayAgent::VideoFrameSizeL(TSize &aSize) 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));								
	
	iFileLogger.Write(_L("Getting VideoFrameFrameSize for CMVSVideoPlayAgent"));	
	iCurrentVideoPlayer->VideoFrameSizeL(aSize);
	}


/**
Returns the video format's MIME type.

@return The video clip's MIME type.
*/
EXPORT_C const TDesC8& CMVSVideoPlayAgent::VideoFormatMimeType() 
	{
    if (iCurrentVideoPlayer)
        {
	    return iCurrentVideoPlayer->VideoFormatMimeType();            
        }
    else 
        {            
        return KNullDesC8;								
        }
	}


/**
Returns the video bit rate.

@return The video bit rate in bits/second.
*/
EXPORT_C TInt CMVSVideoPlayAgent::VideoBitRateL() 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));									
	
	iFileLogger.Write(_L("Getting VideoBitRate for CMVSVideoPlayAgent"));
	return iCurrentVideoPlayer->VideoBitRateL();
	}


/**
Returns the audio bit rate in bits/second.

@return The audio bit rate (bits/second).
*/
EXPORT_C TInt CMVSVideoPlayAgent::AudioBitRateL()
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));									
		
	iFileLogger.Write(_L("Getting AudioBitRate for CMVSVideoPlayAgent"));	
	return iCurrentVideoPlayer->AudioBitRateL();
	}


/**
Returns the codec used for the audio component of the video clip.

@return The four character code representing the audio codec.
*/
EXPORT_C TFourCC CMVSVideoPlayAgent::AudioTypeL()
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));										
	
	iFileLogger.Write(_L("Getting AudioType for CMVSVideoPlayAgent"));
	return iCurrentVideoPlayer->AudioTypeL();
	}


/**
Returns whether the current clip has an audio stream.

@return Boolean indication the presence of an audio stream. ETrue if an audio track is present,
        otherwise EFalse.
*/
EXPORT_C TBool CMVSVideoPlayAgent::AudioEnabledL() 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));
	
	iFileLogger.Write(_L("CMVSVideoPlayAgent's Audio Enabled"));
	return iCurrentVideoPlayer->AudioEnabledL();
	}


/**
Returns the current playback position.

@return The current position from the start of the clip in microseconds.
*/
EXPORT_C TTimeIntervalMicroSeconds CMVSVideoPlayAgent::PositionL() 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));
	return iCurrentVideoPlayer->PositionL();
	}


/**
Returns the duration of video clip in mircoseconds.

@return The duration of clip in microseconds.
*/
EXPORT_C TTimeIntervalMicroSeconds CMVSVideoPlayAgent::DurationL() 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));	
	return iCurrentVideoPlayer->DurationL();
	}


/**
Returns the current playback volume for the audio track of the video clip.

@return A value between 0 (mute) and the maximum volume returned by MaxVolume().
*/
EXPORT_C TInt CMVSVideoPlayAgent::Volume() 
	{
	return iCurrentVideoPlayer->Volume();
	}


/**
Get the current playback priority. This is used to arbitrate between simultaneous accesses of
the sound hardware.

@param  aPriority
        On return, contains the priority level, EMdaPriorityMin client can be interrupted by any
        other client, EMdaPriorityNormal client can only be interrupted by a client with a higher 
        priority or EMdaPriorityMax client cannot be interrupted by other clients.
@param  aPref
        On return, contains the time and quality preferences, enumerated in TMdaPriorityPreference.
*/
EXPORT_C void CMVSVideoPlayAgent::PriorityL(TInt &aPriority, TMdaPriorityPreference &aPref) 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
	iCurrentVideoPlayer->PriorityL(aPriority, aPref);
	}


/**
Returns the current balance setting for the audio track of the video clip.

@return A balance value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight.
*/
EXPORT_C TInt CMVSVideoPlayAgent::Balance() 
	{
	return iCurrentVideoPlayer->Balance();	
	}

	
/**
Query the rotation that is currently applied to the video image.

@return The applied rotation

@see   SetRotationL
*/
EXPORT_C TVideoRotation CMVSVideoPlayAgent::RotationL() 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
	return iCurrentVideoPlayer->RotationL();
	}


/**
Gets the scale factor currently applied to the video image.

@param  aWidthPercentage
        On function return, contains the current scaling percentage applied to the width of the
        video image (100 = original size).
@param  aHeightPercentage
        On function return, contains the current scaling percentage applied to the height of the
        of the video image (100 = original size).
@param  aAntiAliasFiltering
        The boolean specifying if anit-aliasing is being used.
@see    SetScaleFactorL
*/
EXPORT_C void CMVSVideoPlayAgent::GetScaleFactorL(TReal32 &aWidthPercentage, TReal32 &aHeightPercentage, TBool &aAntiAliasFiltering) 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
	iCurrentVideoPlayer->GetScaleFactorL(aWidthPercentage, 
	                                     aHeightPercentage, 
	                                     aAntiAliasFiltering);
	}


/**
Gets the crop region currently applied to the image.

@param  aCropRegion
        The dimensions of the crop region, relative to the video image. If no
        crop region has been applied, the full dimensions of the video image will
        be returned.

@see   SetCropRegionL
*/
EXPORT_C void CMVSVideoPlayAgent::GetCropRegionL(TRect &aCropRegion) 
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
	iCurrentVideoPlayer->GetCropRegionL(aCropRegion);
	}


/**
Returns the number of meta data entries associated with this clip.

@return The number of meta data entries.
*/
EXPORT_C TInt CMVSVideoPlayAgent::NumberOfMetaDataEntriesL()
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
    return iCurrentVideoPlayer->NumberOfMetaDataEntriesL();
	}


/**
Returns an array containing the MetaDataEntry for the given audio clip

@param  aMetaData
        The meta data Array

@leave	Leaves with KErrNotFound if the meta data entry does not exist or
		KErrNotSupported if the controller does not support meta data 
		information for this format. Other errors indicate more general system
		failure.
*/
EXPORT_C void  CMVSVideoPlayAgent::GetMetaDataArrayL(RPointerArray<CMMFMetaDataEntry>& aMetaData)
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
	
	//Reset the meta array
    aMetaData.Reset();        
    TInt entries = NumberOfMetaDataEntriesL();
       
    CMMFMetaDataEntry* entry = NULL;        
  	for(TInt index= 0; index < entries; ++index)
		{
        entry = iCurrentVideoPlayer->MetaDataEntryL(index);
		aMetaData.Append(entry);        
        }
	}

/**
Returns the controller implementation information associated with the current controller.

@return The controller implementation structure
*/	
EXPORT_C const CMMFControllerImplementationInformation& CMVSVideoPlayAgent::ControllerImplementationInformationL()
	{
    __ASSERT_DEBUG(iCurrentVideoPlayer, User::Leave(KErrNotReady));		
    	
	iFileLogger.Write(_L("Getting the ControllerImplementationInformation of CMVSVideoPlayAgent")) ;
    return iCurrentVideoPlayer->ControllerImplementationInformationL();
	}


/**
Returns the current state of the CMVSAudioPlayAgent.

@return The current state, iState.
*/
EXPORT_C TMVSState CMVSVideoPlayAgent::GetState()
	{
	return iState;
	}

/**
Sets the specified output screen to render the video.

@return KErrNone,on success else any of the system wide error code.
*/
EXPORT_C TInt CMVSVideoPlayAgent::SetScreenNumber(TInt aScreenNumber)
	{
	TInt err = KErrNone;
	
    if (!iCurrentVideoPlayer)
        {
        err = iVideoPlayer->SetInitScreenNumber(aScreenNumber);
        }
    else
    	{
    	err = iCurrentVideoPlayer->SetInitScreenNumber(aScreenNumber);
    	}
    
    if (err == KErrNone)
		{
	    iScreenNumber = aScreenNumber;
		}
		
    return err;
	}

/**
Registers for audio resource notification.

@return KErrNone,on success else any of the system wide error code.
*/
EXPORT_C TInt CMVSVideoPlayAgent::RegisterForNotification()
	{
	if (!iCurrentVideoPlayer)
	    {
        // Set the request notify flag.  When we select which video player we are using we
	    // will issue the request for notification.
	    iRequestNotify = ETrue;
        return KErrNone;			        
	    }
	
    return iCurrentVideoPlayer->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
	}

/**
Cancels any existing registeration to audio resource notification.

@return KErrNone,on success else any of the system wide error code.
*/	
EXPORT_C TInt CMVSVideoPlayAgent::CancelNotification()
	{
	if (!iCurrentVideoPlayer)
	    {
	    iRequestNotify = EFalse;
        return KErrNone;
	    }
	
    return iCurrentVideoPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
	}
	
EXPORT_C TInt CMVSVideoPlayAgent::WillResumePlay()
	{
	if (!iCurrentVideoPlayer)
	    {
        return KErrNotReady;			        
	    }
    return iCurrentVideoPlayer->WillResumePlay();
	}

/**
Returns an integer representing the maximum volume that the audio track can support.

This is the maximum value that can be passed to SetVolumeL(). This value is platform 
independent, but is always greater than or equal to one.

@return The maximum playback volume.
*/
EXPORT_C TInt CMVSVideoPlayAgent::MaxVolume() 
    {
	if (!iCurrentVideoPlayer)
	    {
        return iVideoPlayer->MaxVolume();
	    }
	
    return iCurrentVideoPlayer->MaxVolume();
    }
    
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
EXPORT_C void CMVSVideoPlayAgent::EnableSubtitlesL()
	{
	__ASSERT_DEBUG(iVideoPlayer2, User::Leave(KErrNotReady));
	
	if (iWindow)
		{
		iVideoPlayer2->EnableSubtitlesL();
		}
	else
		{
		iEnableSubtitlesOnAdd = ETrue;
		}
	}

EXPORT_C void CMVSVideoPlayAgent::DisableSubtitlesL()
	{
	__ASSERT_DEBUG(iVideoPlayer2, User::Leave(KErrNotReady));
	iVideoPlayer2->DisableSubtitles();
	iEnableSubtitlesOnAdd = EFalse;
	}

EXPORT_C TBool CMVSVideoPlayAgent::SubtitlesAvailable()
	{
	if (iVideoPlayer2)
		{
		return iVideoPlayer2->SubtitlesAvailable();
		}
	
	return EFalse;
	}

EXPORT_C void CMVSVideoPlayAgent::RenderSubtitle(const TRect& aRect)
	{
	if (iVideoPlayer2)
		{
		iVideoPlayer2->RedrawSubtitle(*iWindow, aRect);
		}
	}

#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
