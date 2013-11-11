// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFVIDEOSURFACECUSTOMCOMMANDS_H
#define MMFVIDEOSURFACECUSTOMCOMMANDS_H

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfvideo.h>
#include <graphics/surface.h>

/**
@publishedPartner
@prototype
*/
const TUid KUidInterfaceMMFVideoPlaySurfaceSupport = {0x1028340D};

/**
@publishedPartner 
@prototype

The controller sends this event when (1) a surface has been created after a play command and
the first frame for the stream has been passed to the surface, and (2) when an existing surface
should be replaced with a newly created surface. 
*/
const TUid KMMFEventCategoryVideoSurfaceCreated = { 0x1028340F };

/**
@publishedPartner 
@prototype
*/
const TUid KMMFEventCategoryVideoSurfaceParametersChanged = { 0x10283410 };

/**
@publishedPartner 
@prototype

The controller sends this event when a surface must be replaced with another surface
but there are not enough resources to have both created at the same time. The client
utility must respond with MvpssSurfaceRemovedL() command.
*/
const TUid KMMFEventCategoryVideoRemoveSurface = { 0x10283411 };

/**
@publishedPartner 
@prototype

Mixin class for the custom commands implemented by the controller. The custom command parser calls 
methods in this class to deliver the requests to the controller.
*/
class MMMFVideoPlaySurfaceSupportCustomCommandImplementor
	{
public:

	/**
	Enables using graphics surfaces for video playback.

	An interface implemented by either the decoder or the controller.
	*/
	virtual void MvpssUseSurfacesL() = 0;

	/**
	Gets surface parameters.
	
	An interface implemented by either the decoder or the controller.

	@param  aSurfaceId
	        Surface id of the current surface.
	@param  aCropRect
	        Cropping rectangle within the surface. The crop rectangle identifies the area of 
	        the surface that should be shown on the screen.
	@param  aPixelAspectRatio
	        Video picture pixel aspect ratio.
	*/
	virtual void MvpssGetSurfaceParametersL(TSurfaceId& aSurfaceId, TRect& aCropRect,
											TVideoAspectRatio& aPixelAspectRatio) = 0;

	/**
	Informs the controller that the surface is no longer in use and can
	be destroyed.
	
	An interface implemented by either the decoder or the controller.

	@param  aSurfaceId
	        Surface that has been removed and can be destroyed.
	*/
	virtual void MvpssSurfaceRemovedL(const TSurfaceId& aSurfaceId) = 0;

	};

/**
@publishedPartner 
@prototype

Custom command parser class to be used by controller plugins wishing to support video surface play 
controller commands.

The controller plugin must be derived from MMMFVideoPlaySurfaceSupportCustomCommandImplementor to use 
this class. The controller plugin should create an object of this type and add it to the list of 
custom command parsers in the controller framework.
*/

class CMMFVideoPlaySurfaceSupportCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser capable of handling video surface support commands.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.

	@return A pointer to the object created.

	*/
	IMPORT_C static CMMFVideoPlaySurfaceSupportCustomCommandParser* NewL(MMMFVideoPlaySurfaceSupportCustomCommandImplementor& aImplementor);

	/**
	Destructor.

	*/
	IMPORT_C ~CMMFVideoPlaySurfaceSupportCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	*/
	void HandleRequest(TMMFMessage& aMessage);
private:
	/**
	Constructor.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.

	*/
	CMMFVideoPlaySurfaceSupportCustomCommandParser(MMMFVideoPlaySurfaceSupportCustomCommandImplementor& aImplementor);
	// Internal request handling methods.
	void DoHandleRequestL(TMMFMessage& aMessage);
	TBool DoUseSurfacesL(TMMFMessage& aMessage);
	TBool DoGetSurfaceParametersL(TMMFMessage& aMessage);
	TBool DoSurfaceRemovedL(TMMFMessage& aMessage);
	
private:
	/** 
	The object that implements the video surface support interface 
	*/
	MMMFVideoPlaySurfaceSupportCustomCommandImplementor& iImplementor;
	};

/**
@publishedPartner 
@prototype

Client class to access functionality specific to a video surface support playback controller.

The class uses the custom command function of the controller plugin, and removes the necessity
for the client to formulate the custom commands.
*/
class RMMFVideoPlaySurfaceSupportCustomCommands : public RMMFCustomCommandsBase
	{
public:

	/**
	Constructor.

	@param  aController
	        The client side controller object to be used by this custom command interface.
	*/
	IMPORT_C RMMFVideoPlaySurfaceSupportCustomCommands(RMMFController& aController);

	/**
	Enables using graphics surfaces for video playback.

	Instructs the controller to use graphics surfaces as destination. Note that direct screen 
	access and graphics surface use is mutually exclusive, enabling one will disable the other.
	
	@return KErrNone if successful. KErrNotSupported if graphic surfaces are not supported by the 
	controller or otherwise one of the system wide error codes.
	*/
	IMPORT_C TInt UseSurfaces() const;

	/**
	Gets the surface parameters for a display.
	
	The client utility typically calls this in response to KMMFEventCategoryVideoSurfaceCreated and 
	KMMFEventCategoryVideoSurfaceParametersChanged events to retrieve new or updated surface 
	information for a display.

	@param  aSurfaceId
	        Surface id of the current surface.
	@param  aCropRect
	        Cropping rectangle within the surface. The crop rectangle identifies the area of 
	        the surface that should be shown on the screen.
	@param  aPixelAspectRatio
	        Video picture pixel aspect ratio.

	@return KErrNone if successful. KErrNotSupported if graphic surfaces are not supported by the 
	controller or KErrNotReady if no surface is available for the display or otherwise one of the 
	system wide error codes.
	*/
	IMPORT_C TInt GetSurfaceParameters(TSurfaceId& aSurfaceId, TRect& aCropRect, TVideoAspectRatio& aPixelAspectRatio) const;

	/**
	Indicates that the surface is no longer in use and can be destroyed.
	
	The client utility typically calls this in response to either:
	
	KMMFEventCategoryVideoSurfaceCreated  - when a surface is already registered with the utility. This
	indicates that the client utility should stop using the current surface and use the one supplied
	in the notification. When the client utility is no longer using the current surface it calls
	SurfaceRemoved()
	
	KMMFEventCategoryVideoRemoveSurface  - when the current surface should be removed. This indicates
	that the client utility should stop using the current surface immediately. When the client utility
	is no longer using the current surface it calls	SurfaceRemoved()
	
	@param  aSurfaceId
	        Surface which is no longer being used by client utility.

	@return KErrNone if successful. KErrNotSupported if graphic surfaces are not supported by the 
	controller or KErrNotReady if no surface is available for the display or otherwise one of the 
	system wide error codes.
	*/
	IMPORT_C TInt SurfaceRemoved(TSurfaceId& aSurfaceId) const;
	
	};

#endif // MMFVIDEOSURFACECUSTOMCOMMANDS_H
