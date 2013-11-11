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

#ifndef DISPMAP_CLIENT_H_
#define DISPMAP_CLIENT_H_
#include <w32std.h>

/** Defines the coordinate space UID values for use with
	MDisplayMappingBase::MapCoordinates().
*/
enum TCoordinateSpace
		{
	/**
		Application coordinate space, possibly offset but not scaled
		relative to full-screen space.
	 */
	EApplicationSpace = 0x1028656F,
	/**
		User interface coordinate space, possibly scaled but not offset
		relative to composition space.
	*/
	EFullScreenSpace = 0x10286570,
	/**
		Composition coordinate space, used by composition back-end in
		compositing the display contents.
	*/
	ECompositionSpace = 0x10286571,
	/**
		Coordinate space for the DSA buffer. May be offset but not scaled
		relative to both application and full-screen spaces. Identical to
		full-screen space if the DSA buffer is used for UI rendering.
	*/
	EDirectScreenAccessSpace = 0x1028656E
	};

/** Provides facilities to map coordinates between various coordinate
	spaces.

*/
class MDisplayMappingBase
	{
public:
	enum	
		{
		KUidDisplayMappingBase =  0x1028656D, 	  
		ETypeId=	 KUidDisplayMappingBase 
		};
public:
	/** Maps the source rectangle from a source coordinate space to a
		target coordinate space. Since there may be scaling involved, the
		target rectangle may be larger or smaller than the source one, or
		even become empty.

		Two rectangles that abut each other before mapping, when mapped
		using the same source and target space UIDs, shall also abut each
		other after mapping.

		@param aSourceSpace source space
		@param aSource source rectangle
		@param aTargetSpace target space
		@param aTarget target rectangle to be filled in
		@return KErrNone if successful or KErrNotSupported if invalid TCoordinateSpace
	*/
	virtual TInt MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const =0;
	};

/** Provides the Window Server client coordinate space mapping
	functions.

*/
class MDisplayMapping : public MDisplayMappingBase
	{
public:
	enum	
		{
		KUidDisplayMapping =  0x1028656B, 	  
		ETypeId=	 KUidDisplayMapping 
		};
public:
	/** Retrieves the largest window extent that is visible at the current
		display resolution. This may be larger than the size reported by
		SizeInPixels(), and have a negative top-left position, for example.

		@see MDisplayControl
		@see CWsScreenDevice::SizeInPixels

	 	@param aExtent extent to be filled in
	*/
	virtual void GetMaximumWindowExtent(TRect& aExtent) const =0;
	/** Returns the maximum useful size for a surface to be shown on the
		display at the current resolution, in both pixels and twips.

		This may be larger than both the screen device SizeInPixels() and
		the size of the maximum window extent, if the user interface is
		being scaled by the display. In general, the value will be the
		native display resolution.

		@see MDisplayControl
		@see CWsScreenDevice::SizeInPixels
		
		@param aPixels pixels to be filled in
		@param aTwips twips to be filled in
	*/
	virtual void GetMaximumSurfaceSize(TSize& aPixels, TSize& aTwips) const =0;
	/** Retrieves the extent of a window relative to the top-left corner of
		the display.

		This will differ from the window's AbsPosition() if the current
		screen size mode is offset from the corner of the display, or if
		the user interface is being scaled on the display.

		@param aWindow window in question
		@param aExtent extent to be filled in
	*/
	virtual void GetDisplayExtentOfWindow(const RWindowBase& aWindow, TRect& aExtent) const =0;
	};



#endif 	/*DISPMAP_CLIENT_H_*/
