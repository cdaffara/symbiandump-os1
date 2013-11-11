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
// Interface for creating render stages for the window server
// 
//

#ifndef __WSRENDERSTAGEFACTORY_H__
#define __WSRENDERSTAGEFACTORY_H__

#include <graphics/wsgraphicdrawerinterface.h>

class CWsRenderStage;

/**
Window server interface to allow creation of a render stage pipeline.

@publishedPartner
@prototype
*/
class MWsRenderStageFactory : public MWsObjectProvider
	{
public:
 	DECLARE_WS_TYPE_ID(KMWsRenderStageFactory)
 	
public:
#ifdef SYMBIAN_GRAPHICS_GCE
	/**
	Create a render stage which has no further render stages below it.
	*/
	virtual CWsRenderStage* CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, TInt aScreenNumber) = 0;
	
	/**
	Create a render stage which has a render stage below it.
	*/
	virtual CWsRenderStage* CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage) = 0;
#else //SYMBIAN_GRAPHICS_GCE
	/**
	*/
	virtual CWsRenderStage * CreateStageL(MWsScreen * aScreen, MWsScreenRedraw * aScreenRedraw) = 0;
#endif //SYMBIAN_GRAPHICS_GCE
	};
	
#endif //__WSRENDERSTAGEFACTORY_H__
