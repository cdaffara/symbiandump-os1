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

#ifndef __WSDRAWABLESOURCEPROVIDER_H__
#define __WSDRAWABLESOURCEPROVIDER_H__

#include <graphics/sgresource.h>
#include <graphics/wsgraphicdrawerinterface.h>

/**
Window server evokes this interface from rendering stage plugin every time client 
requests resource drawing operations.

@publishedPartner
@prototype
 */
class MWsDrawableSourceProvider : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsDrawableSourceProvider)
public:
	/**
	 Create a source object for future drawing of the drawable resource with the specified ID.
	 @param aDrawableId The ID of the drawable resource.
	 @param aSource On output, a pointer to the new source object associated with the drawable resource,
	                which must be passed in calls to DrawResource() and CloseDrawableSource().
	      	        After the call to CloseDrawableSource() the source object is destroyed and this pointer becomes invalid.
	      	        The type of the source object is defined by the implementation of MWsDrawableSourceProvider.
	 @post If successful, a new source object is created and the associated drawable resource is ready to use.
	 @return KErrNone if successful, otherwise one of the system-wide error codes.
	 */
	virtual TInt CreateDrawableSource(const TSgDrawableId& aDrawableId, TAny*& aSource) = 0;

	/**
	 Destroy a source object associated with a drawable resource.
	 @param aSource A pointer to a source object created by a previous call to CreateDrawableSource().
	 @post Drawing of the associated drawable resource is no longer possible.
	 @return KErrNone if successful, otherwise one of the system-wide error codes.
	 */
	virtual void CloseDrawableSource(TAny* aSource) = 0;

	/**
	 Draw a graphics resource.
	 @param aSource A pointer to the source object associated with the drawable resource.
	 @param aPos Destination position for the top-left corner of the drawable resource.
	 @param	aRotation Rotation to be applied to the drawable resource before it is drawn.
	 @pre A source object must have been created for the drawable resource by a previous call to CreateDrawableSource().
	 */
	virtual void DrawResource(const TAny* aSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone) = 0;

	/**
	 Draw a graphics resource.
	 @param aSource A pointer to the source object associated with the drawable resource.
	 @param aRect Destination rectangle. The drawable resource could be scaled if its size doesn't match the destination rectangle.
	 @param	aRotation Rotation to be applied to the drawable resource before it is drawn.
	 @pre A source object must have been created for the drawable resource by a previous call to CreateDrawableSource().
	 */
	virtual void DrawResource(const TAny* aSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone) = 0;

	/**
	 Draw a graphics resource.
	 @param aSource A pointer to the source object associated with the drawable resource.
	 @param aRectDest Destination rectangle. The drawable resource could be scaled if its size doesn't match the destination rectangle.
	 @param aRectSrc A rectangle defining the piece of the drawable resource to be drawn, in the co-ordinate system of the drawable resource.
	 @param	aRotation Rotation to be applied to the drawable resource before it is drawn.
	 @pre A source object must have been created for the drawable resource by a previous call to CreateDrawableSource().
	 */
	virtual void DrawResource(const TAny* aSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone) = 0;

	/**
	 Draw a graphics resource.
	 @param aSource A pointer to the source object associated with the drawable resource.
	 @param aRect Destination rectangle. The drawable resource could be scaled if its size doesn't match the destination rectangle.
	 @param	aParam Parameters specifying how to draw the drawable resource.
	 @pre A source object must have been created for the drawable resource by a previous call to CreateDrawableSource().
	 */
	virtual void DrawResource(const TAny* aSource, const TRect& aRect, const TDesC8& aParam) = 0;
	};

#endif //__WSDRAWABLESOURCEPROVIDER_H__
