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


#ifndef __DRAWRESOURCE_H__
#define __DRAWRESOURCE_H__

#include "OBJECT.H"
#include <graphics/wsgraphicscontext.h>
#include <graphics/wsdrawablesourceprovider.h>



/**
The class represents RWsDrawableSource on the server side. 
It maintains a reference count. Drawing commands will be redirected to the resource drawing 
interface retrieved from the graphics context.

@internalComponent
@see RWsDrawableSource
*/
class CWsDrawableSource : public CWsObject
	{
public:
	CWsDrawableSource(CWsClient *aOwner);
	void ConstructL(const TWsClCmdCreateDrawableSource &aParams);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void DrawResource(MWsGraphicsContext *aGc, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation) const;
	void DrawResource(MWsGraphicsContext *aGc, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation) const;
	void DrawResource(MWsGraphicsContext *aGc, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation) const;
	void DrawResource(MWsGraphicsContext *aGc, const TRect& aRect, const TDesC8& aDes) const;
	void CloseObject();
	void IncRefCount();
	void DecRefCount();
private:
	~CWsDrawableSource();
private:
	enum // Flags
		{
		EWsClientHandleClosed = 0x00000001 // Set if the associated RWsDrawableSource handle has been closed
		};
private:
	TInt iRefCount;
	TUint32 iFlags;
	TAny* iDrawableSource; // to be used by plug-ins
	MWsDrawableSourceProvider* iDrawResource; // doesn't own
	};


#endif
