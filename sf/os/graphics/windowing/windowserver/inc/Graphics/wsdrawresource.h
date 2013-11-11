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
 @publishedPartner
 @prototype
*/

#ifndef __WSDRAWRESOURCE_H__
#define __WSDRAWRESOURCE_H__

#include <w32std.h>
#include <graphics/sgresource.h>

class RWsDrawableSource;

/**
The UID for MWsDrawResource.
@see MWsDrawResource
@see CWindowGc::Interface
@publishedPartner
@prototype
*/
static const TUid KMWsDrawResourceInterfaceUid = { 0x102864E9 };

/**
An extension interface for CWindowGc and CRemoteGc to draw RWsDrawableSources.

Usage:
@code
MWsDrawResource* gcDrawResource = static_cast<MWsDrawResource*>(gc->Interface(KMWsDrawResourceInterfaceUid));
if(gcDrawResource)
	{
	gcDrawResource->DrawResource(TPoint(5,5), drawableSource);
	}
@endcode
@see RWsDrawableSource
@see CWindowGc::Interface
@publishedPartner
@prototype
*/
class MWsDrawResource
	{
public:
	/**
	Draws an image based resource which may be generated using non-native rendering API such as OpenGL ES 
	or OpenVG. The resource will be drawn at the specified position in its original size with orientation 
	according to the specified rotation parameter. The current clipping region applies. 

	@see RWsDrawableSource

	@param	aPos		The position of the top-left corner on a window.
	@param	aSource		The resource to be drawn.
	@param	aRotation	The rotation to be applied to the resource before it is drawn. The default value is CWindowGc::EGraphicsRotationNone.

	@pre	Drawing context has been activated on a window. The drawable source has been created.
	@post	Request to draw resource has been accepted. There is no guarantee that the request has been completed 
			when this method returns.
	*/
	virtual void DrawResource(const TPoint& aPos, const RWsDrawableSource& aSource, CWindowGc::TGraphicsRotation aRotation=CWindowGc::EGraphicsRotationNone) = 0;

	/**
	Draws an image based resource. The resource will be rendered to the given destination rectangle on 
	the window in its original dimensions with orientation according to the specified rotation parameter. 
	Drawing will be clipped to the given destination rectangle. The current clipping region applies. The resource can be
	drawn rotated using the CWindowGc::TGraphicsRotation enum which defines possible rotation values in 
	clockwise degrees.

	@see RWsDrawableSource

	@param	aDestRect	Destination rectangle to which the resource will be rendered.
	@param	aSource		The resource to be drawn.
	@param	aRotation	The rotation to be applied to the resource before it is drawn. The default value is CWindowGc::EGraphicsRotationNone.

	@pre	Drawing context has been activated on a window. The drawable source has been created.
	@post	Request to draw resource has been accepted. There is no guarantee that the request has been completed 
			when this method returns.
	*/
	virtual void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, CWindowGc::TGraphicsRotation aRotation=CWindowGc::EGraphicsRotationNone) = 0;

	/**
	Draws an image based resource. The resource is rendered into the given destination rectangle on a window.
	Scaling (stretching or compression) applies if the destination rectangle is different from the
	source rectangle. The resource orientation is set based on the specified rotation parameter
	before scaling and drawing operations are performed.

	@see RWsDrawableSource

	@param	aDestRect	The destination rectangle to which the resource will be rendered on a window.
	@param	aSource		The resource to draw.
	@param	aSrcRect	The source rectangle specifying the area/sub-area of the resource to be rendered.
	@param	aRotation	Rotation to be applied to the resource before it is drawn. The default value is CWindowGc::EGraphicsRotationNone.

	@pre	Drawing context has been activated on a window. The drawable source has been created.
	@post	Request to draw an image based resource has been accepted. There is no guarantee that the
			request has been completed when this method returns.
	*/
	virtual void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, const TRect& aSrcRect, CWindowGc::TGraphicsRotation aRotation=CWindowGc::EGraphicsRotationNone) = 0;

	/**
	Draws a non-image based resource. The resource will be rendered into the given destination rectangle on a window.
	The current clipping region applies. The adaptation is free to interpret the parameter aParam and may define
	its own rules on how to handle the rendering of a non-image based resource.

	@see RWsDrawableSource

	@param	aDestRect	The destination rectangle to which the resource will be rendered on a window.
	@param	aSource		The resource.
	@param	aParam		Parameters specifying how to draw the resource. 

	@pre	Drawing context has been activated on a window. The drawable source has been created.
	@post	Request to draw a non-image based resource has been accepted. 
			There is no guarantee that the request has been completed when this method returns.
	*/
	virtual void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, const TDesC8& aParam) = 0;
	};


/**
The intention of this class is to enable drawing of drawables (RSgDrawable and RSgImage)
using CWindowGc and CRemoteGc.

You have to use the extension interface MWsDrawResource in order to use this class with
CWindowGc and CRemoteGc.

@publishedPartner
@prototype
@see RSgDrawable
@see RSgImage
@see MWsDrawResource
*/
class RWsDrawableSource : public MWsClientClass
	{
public:
	IMPORT_C RWsDrawableSource();
	IMPORT_C RWsDrawableSource(RWsSession& aWs);
	IMPORT_C TInt Create(const RSgDrawable& aDrawable);
	IMPORT_C TInt Create(const RSgDrawable& aDrawable, TInt aScreenNumber);
	IMPORT_C const TSgDrawableId& DrawableId() const;
	IMPORT_C TInt ScreenNumber() const;
	IMPORT_C void Close();
private:
	TSgDrawableId iDrawableId;
	TInt iScreenNumber;
	TInt iReserved; // reserved for use by Symbian
	};

#endif //__WSDRAWRESOURCE_H__

