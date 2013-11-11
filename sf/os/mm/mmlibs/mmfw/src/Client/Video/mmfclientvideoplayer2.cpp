// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <videoplayer.h>
#include <videoplayer2.h>
#include "mmfvideocallback.h"
#include "VideoPlayerBody.h"

/**
Creates a new instance of the video player utility. Unlike CVideoPlayerUtility::NewL(), 
the CVideoPlayerUtility2 factory does not require window handles and other video display 
information as its arguments. The client can set up rendering later with 
AddDisplayWindowL(), or optionally use the utility without a window, for example, for metadata 
query purposes.
	
@param  aObserver
        A client class to receive notifications from the video player.
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.
	
@return A pointer to the new video player utility object.
	
@leave The method will leave if an error occurs. Typical error codes used:
		* KErrNoMemory if out of memory. 

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 

*/
EXPORT_C CVideoPlayerUtility2* CVideoPlayerUtility2::NewL(MVideoPlayerUtilityObserver& aObserver,
											  					TInt aPriority,
											  					TInt aPref)
	{
	CVideoPlayerUtility2* s = new(ELeave) CVideoPlayerUtility2();
	CleanupStack::PushL(s);
	s->iBody = CVideoPlayerUtility::CBody::NewL(s, aObserver, aPriority, aPref);
	CleanupStack::Pop();
	return s;
	}

/**
Destructor. Closes any open video clips and frees any resources held by the Video Player.
*/
CVideoPlayerUtility2::~CVideoPlayerUtility2()
	{
	}
	
/**
Adds a new window for displaying the video picture. Client applications must use this method 
instead of SetDisplayWindowL() when using CVideoPlayerUtility2.

This method can only be called after opening the source is complete and the client has 
received an MvpuoOpenComplete() callback.
	
@param  aWs
       	The window server session for this window.
@param  aScreenDevice
       	The screen device for the screen that the window is displayed on.
@param  aWindow
       	The display window.
@param  aVideoExtent
       	Video extent on the screen, relative to the window. Video picture position within 
       	the extent depends on the scaled picture and content alignment or offset. The video 
       	extent can be partially or completely outside the window.
@param  aWindowClipRect
       	Window clipping rectangle, relative to the window. The clipping rectangle specifies 
       	the part of the window used for video display. The rectangle must be contained 
       	completely within the window.
@leave	The method will leave if an error occurs. Typical error codes used:
		* KErrNotReady if the source file, URL, or descriptor has not been opened.
*/	
EXPORT_C void CVideoPlayerUtility2::AddDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, 
														RWindow& aWindow, const TRect& aVideoExtent, 
														const TRect& aWindowClipRect)
	{
	iBody->AddDisplayWindowL(aWs, aScreenDevice, aWindow, aVideoExtent, aWindowClipRect);
	}

/**
A simplified variant of AddDisplayWindowL(). When this variant is used, the video extent and 
window clipping rectangle default to the whole window.

This method can only be called after opening the source is complete and the client has 
received an MvpuoOpenComplete() callback.

@param  aWs
       	The window server session for this window.
@param  aScreenDevice
       	The screen device for the screen that the window is displayed on.
@param  aWindow
       	The display window.
@leave	The method will leave if an error occurs. Typical error codes used:
		* KErrNotReady if the source file, URL, or descriptor has not been opened.
*/	
EXPORT_C void CVideoPlayerUtility2::AddDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
	{
	iBody->AddDisplayWindowL(aWs, aScreenDevice, aWindow);
	}

/**
Removes a window that is currently being used to display the video picture. The window must
have previously been added with AddDisplayWindowL(). 

Note Depending on underlying implementation it may also remove any graphics resources associated
with video playback on this window. 

This method cannot fail. If the window has not been added with AddDisplayWindowL(), the 
method call will be ignored. 
	
@param  aWindow
       	The display window.
*/	
EXPORT_C void CVideoPlayerUtility2::RemoveDisplayWindow(RWindow& aWindow)
	{
	iBody->RemoveDisplayWindow(aWindow);
	}

/**
Sets the video extent on the screen, relative to the window. The extent specifies the area 
of screen in which the video picture is placed, and may be partially or completely outside of
the video window. Video picture position within the extent depends on the picture size and 
content alignment or offset.
	
This method can only be called after opening the source is complete and the client has 
received an MvpuoOpenComplete() callback.

@param  aWindow
		Window to set video extent for.
@param  aVideoExtent
       	The new video extent, relative to the video window.
@leave	The method will leave if an error occurs. Typical error codes used:
		* KErrNotReady if the source file, URL, or descriptor has not been opened.
*/	
EXPORT_C void CVideoPlayerUtility2::SetVideoExtentL(const RWindow& aWindow, const TRect& aVideoExtent)
	{
	iBody->SetVideoExtentL(aWindow, aVideoExtent);
	}
	
/**
Sets the window clipping rectangle, relative to the window. The clipping rectangle specifies 
the part of the window used to display the video picture and must be fully contained within 
the window.
	
This method can only be called after opening the source is complete and the client has 
received an MvpuoOpenComplete() callback.

@param	aWindow
		Window to set clipping rectangle for.
@param  aWindowClipRect
       	The clipping rectangle to use for this window.
@leave	The method will leave if an error occurs. Typical error codes used:
		* KErrArgument if the rectangle is not contained within the window. 
		* KErrNotReady if the source file, URL, or descriptor has not been opened.
*/	
EXPORT_C void CVideoPlayerUtility2::SetWindowClipRectL(const RWindow& aWindow, const TRect& aWindowClipRect)
	{
	iBody->SetWindowClipRectL(aWindow, aWindowClipRect);
	}

/**
Rotates the video image within the window. This is the preferred method to use with CVideoPlayerUtility2.

The rotation will replace any rotation set with CVideoPlayerUtility::SetRotationL. 
Likewise with setting the rotation with CVideoPlayerUtility::SetRotationL after a call to CVideoPlayerUtility2::SetRotationL has been
made, then the rotation specified will replace any rotation set with CVideoPlayerUtility2::SetRotationL.

@param aWindow Window to set rotation for.
@param aRotation The video rotation to use for aWindow.

@leave KErrNotFound if aWindow isn't currently added to CVideoPlayerUtility2
@leave KErrNotReady if controller hasn't been opened.

@see CVideoPlayerUtility::SetRotationL
@see TVideoRotation
*/
		
EXPORT_C void CVideoPlayerUtility2::SetRotationL(const RWindow& aWindow, TVideoRotation aRotation)
	{
	iBody->SetRotationL(aWindow, aRotation);
	}

/**
Retrieves the video rotation set for a window. This is the preferred method to use with CVideoPlayerUtility2.

@param aWindow Window to retrieve rotation for.
@return The video rotation.

@leave KErrNotFound if aWindow isn't currently added to CVideoPlayerUtility2
@leave KErrNotReady if controller hasn't been opened.

@see TVideoRotation
@see CVideoPlayerUtility2::AddDisplayWindowL
*/
	
EXPORT_C TVideoRotation CVideoPlayerUtility2::RotationL(const RWindow& aWindow)
	{
	return iBody->RotationL(aWindow);
	}

/**
Scales the video image to a specified percentage of its original size within the window. 
This is the preferred method to use with CVideoPlayerUtility2. 
Setting scale factor will set auto scale to EAutoScaleNone for the window.

The scale factor will replace any scale factor set with CVideoPlayerUtility::SetScaleFactorL. 
Likewise with setting the scale factor with CVideoPlayerUtility::SetScaleFactorL after a call to CVideoPlayerUtility2::SetScaleFactorL has been
made, then the scale factor specified will replace any scale factor set with CVideoPlayerUtility2::SetScaleFactorL.

@param aWindow Window to set scale factor for.
@param aWidthPercentage
        The percentage (100 = original size) to be used to scale the width of the video image
@param aHeightPercentage
        The percentage (100 = original size) to be used to scale the height of the video image. 
        If this is not equal to aWidthPercentage then the image may be distorted.

@leave KErrNotFound if aWindow isn't currently added to CVideoPlayerUtility2
@leave KErrNotReady if controller hasn't been opened.

@see CVideoPlayerUtility::SetScaleFactorL
*/
	
EXPORT_C void CVideoPlayerUtility2::SetScaleFactorL(const RWindow& aWindow, TReal32 aWidthPercentage, TReal32 aHeightPercentage)
	{
	iBody->SetScaleFactorL(aWindow, aWidthPercentage, aHeightPercentage);
	}

/**
Retrieves the scale factor currently set for a window. This is the preferred method to use with CVideoPlayerUtility2.

@param aWindow Window to retrieve scale factor for.
@param aWidthPercentage
        On function return, contains the current scaling percentage applied to the width of the
        video image (100 = original size).
@param aHeightPercentage
        On function return, contains the current scaling percentage applied to the height
        of the video image (100 = original size).
        
@leave KErrNotFound if aWindow isn't currently added to CVideoPlayerUtility2
@leave KErrNotReady if controller hasn't been opened.
*/

EXPORT_C void CVideoPlayerUtility2::GetScaleFactorL(const RWindow& aWindow, TReal32& aWidthPercentage, TReal32& aHeightPercentage)
	{
	iBody->GetScaleFactorL(aWindow, aWidthPercentage, aHeightPercentage);
	}

/** 
Set video automatic scaling. When automatic scaling is active, the
video picture is scaled automatically to match the video extent,
based on the scaling type. This variant of SetAutoScaleL() will
always center the picture in the extent.

This is the preferred method to use with CVideoPlayerUtility2.

Calling SetAutoScaleL() will override any scaling factors set with
SetScaleFactorL(). Calling SetScaleFactorL() will disable automatic
scaling.

@see TAutoScaleType, THorizontalAlign, TVerticalAlign

@param aWindow Window to set auto scaling options for.
@param aScaleType Automatic scaling type

@pre The video clip has been opened by the client

@leave KErrNotFound if aWindow isn't currently added to CVideoPlayerUtility2
@leave KErrNotReady if controller hasn't been opened.
*/
	
EXPORT_C void CVideoPlayerUtility2::SetAutoScaleL(const RWindow& aWindow, TAutoScaleType aScaleType)
	{
	iBody->SetAutoScaleL(aWindow, aScaleType);
	}

/** 
Set video automatic scaling. When automatic scaling is active, the
video picture is scaled automatically to match the video extent,
based on the scaling type, and positioned according to the
parameters.

This is the preferred method to use with CVideoPlayerUtility2.

Calling SetAutoScaleL() will override any scaling factors set with
SetScaleFactorL(). Calling SetScaleFactorL() will disable automatic
scaling.

@see TAutoScaleType, THorizontalAlign, TVerticalAlign

@param aWindow Window to set auto scaling options for.
@param aScaleType Automatic scaling type
@param aHorizPos Video picture horizontal position, relative to the
                 video window. The value can be either a pixel offset
                 (positive or negative) from the top left corner of the
                 window to the top left corner of the picture, or an
                 alignment constant from enum THorizontalAlign.
@param aVertPos Video picture vertical position, relative to the
                 video window. The value can be either a pixel offset
                 (positive or negative) from the top left corner of the
                 window to the top left corner of the picture, or an
                 alignment constant from enum TVerticalAlign.

@pre The video clip has been opened by the client.

@leave KErrNotFound if aWindow isn't currently added to CVideoPlayerUtility2
@leave KErrNotReady if controller hasn't been opened.
*/

EXPORT_C void CVideoPlayerUtility2::SetAutoScaleL(const RWindow& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
	iBody->SetAutoScaleL(aWindow, aScaleType, aHorizPos, aVertPos);
	}

/**
Adds the specified display to the list of surface rendering targets. 
This API can be used in conjunction with AddDisplayWindowL calls. 
The caller is responsible for handling surface events generated for the specific display. 
A single graphics surface is created and shared between all windows and displays.
Surface registration and de-registration is managed by the MMF framework.

@see AddDisplayWindowL

@param aWs Window server session. 
@param aDisplay Display to create graphics surface on.
@param aEventHandler Call-back interface for receiving surface specific events.

@leave Any of the system wide error codes.
@leave KErrNotReady if the source file, URL or descriptor has not been opened.
@leave KErrInUse if the display has already been added by a previous AddDisplayL call.
*/
	
EXPORT_C void CVideoPlayerUtility2::AddDisplayL(RWsSession& /* aWs */, TInt aDisplay, MMMFSurfaceEventHandler& aEventHandler)
	{
	iBody->AddDisplayL(aDisplay, aEventHandler);
	}

/**
Removes the specified display from the list of surface rendering targets. 

@param aDisplay Display id of display to remove
*/
    
EXPORT_C void CVideoPlayerUtility2::RemoveDisplay(TInt aDisplay)
	{
	iBody->RemoveDisplay(aDisplay);
	}

/**
When enabled sets automatic switching of surface to/from external display when it is connected/disconnected from the device.

Automatic switching is enabled by default, but only if the client thread that created this utility has an Active Scheduler
installed and the device supports external display switching.

To use this function the client thread must have an Active Scheduler installed otherwise it will leave with KErrNotReady.

@param  aControl
        ETrue to enable. EFalse to disable.
@param  aDisplay
        Display id of display to enable external switching for.
@leave  KErrNotSupported Device does not support external displays
@leave  KErrNotReady CActiveScheduler is not installed
*/
EXPORT_C void CVideoPlayerUtility2::SetExternalDisplaySwitchingL(TInt aDisplay, TBool aControl)
    {
    iBody->SetExternalDisplaySwitchingL(aDisplay, aControl);
    }

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
/**
Check to see if subtitles are available with the current video stream and controller.

@return ETrue if subtitles can be enabled, 
	EFalse if subtitles are not available with the current video stream or video clip has not been opened
*/
EXPORT_C TBool CVideoPlayerUtility2::SubtitlesAvailable()
	{
	return iBody->SubtitlesAvailable();
	}

/**
Enable subtitles with the current video stream.

@pre The video clip has been opened by the client and SubtitlesAvailable() return ETrue.

@leave KErrNotReady if the video source file, URL, or descriptor has not been opened, 
	or no window has been added to CVideoPlayerUtility2.
@leave KErrNotSupported if underlying video player controller does not support subtitles.
@leave KErrNotFound if the opened video source has no associated subtitle data.
@leave KErrInUse if subtitle is already enabled.
@leave Otherwise leaves with any of the system wide error codes.

@panic MMFVideoPlayUtil 1 In debug mode, if the video source file, URL, or descriptor has not been opened.
@panic MMFVideoPlayUtil 2 In debug mode, if subtitle is not supported or not available.
@panic MMFVideoPlayUtil 3 In debug mode, if no display window has been added.
*/
EXPORT_C void CVideoPlayerUtility2::EnableSubtitlesL()
	{
	iBody->EnableSubtitlesL();
	}

/**
Disable subtitles.
*/
EXPORT_C void CVideoPlayerUtility2::DisableSubtitles()
	{
	iBody->DisableSubtitles();
	}

/**
Get the current subtitle language.

@pre Subtitle has been enabled.
@return The current subtitle language, or ELangNone if no language information is available

@leave KErrNotReady if subtitle has not been enabled.
@leave Otherwise leaves with any of the system wide error codes.

@panic MMFVideoPlayUtil 4 In debug mode, if subtitle has not been enabled.
*/
EXPORT_C TLanguage CVideoPlayerUtility2::SubtitleLanguageL()
	{
	return iBody->SubtitleLanguageL();
	}

/**
Return the subtitle languages available.

@pre Subtitles have been enabled.
@return A array of the currently available languages, or an empty array if 
	subtitle source does not contain any language information.  Array is valid 
	until subtitles are disabled.

@leave KErrNotReady if subtitles have not been enabled.
@leave Otherwise leaves with any of the system wide error codes.

@panic MMFVideoPlayUtil 4 In debug mode, if subtitles have not been enabled.
*/
EXPORT_C TArray<TLanguage> CVideoPlayerUtility2::SupportedSubtitleLanguagesL()
	{
	return iBody->SupportedSubtitleLanguagesL();
	}

/**
Set the current subtitle language.

@see CVideoPlayerUtility2::GetSupportedSubtitleLanguagesL()

@pre Subtitles have been enabled.
@pre GetSupportedSubtitleLanguagesL() return a non empty array

@param aLanguage Language to be used for subtitle stream. 

@leave KErrNotReady if subtitles have not been enabled.
@leave KErrNotSupported if subtitle language is not supported
@leave Otherwise leaves with any of the system wide error codes.

@panic MMFVideoPlayUtil 4 In debug mode, if subtitles have not been enabled.
@panic MMFVideoPlayUtil 5 In debug mode, if subtitle language is not supported.
*/
EXPORT_C void CVideoPlayerUtility2::SetSubtitleLanguageL(TLanguage aLanguage)
	{
	iBody->SetSubtitleLanguageL(aLanguage);
	}

/**
To be called when the video window is asked to redraw. E.g. For cone control, when CCoeControl::Draw() is called.

@param aWindow Handle to the video window to be redrawn.
@param aRect The region of the control to be redrawn from aRect in CCodControl::Draw().
*/
EXPORT_C void CVideoPlayerUtility2::RedrawSubtitle(RWindow& aWindow, const TRect &aRect)
	{
	iBody->RedrawSubtitle(aWindow, aRect);
	}

#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
