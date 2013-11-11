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
// FBRENDERSTAGE.CPP
// 
//

#ifndef __FBRENDERSTAGE_H__
#define __FBRENDERSTAGE_H__

#include "Graphics/WsRenderStage.h"

//#define USE_DEBUG_FRAME_CAPTURE

#ifdef USE_DEBUG_FRAME_CAPTURE
#include <bitdev.h>
#endif

class MWsObjectProvider;
class MWsGraphicDrawerEnvironment;

/**
This is the implementation of CWsRenderStage which is created by a CFbRenderStageFactory
*/
class CFbRenderStage : public CWsRenderStage
	{
public:
	static CFbRenderStage * NewL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen, MWsScreenRedraw * aScreenRedraw);
	virtual ~CFbRenderStage();

public: // implementation of CWsRenderStage
	virtual CFbsBitGc * Begin();
	virtual void End();	
	
private:
	CFbRenderStage();
	void ConstructL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen, MWsScreenRedraw * aScreenRedraw);
	void BlitRegion(const TRegion* aRegion, CFbsBitGc* aGc);
#ifdef USE_DEBUG_FRAME_CAPTURE
	void CaptureFrame(const TRegion* aRegion);
	void LogRegion(const TDesC& aPrefix, const TDesC& aFunctionName, const TRegion* aRegion);
#endif
	
private:
	MWsGraphicDrawerEnvironment * iEnv;
	MWsScreen * iScreen;
	MWsBackBuffer * iBackBuffer;
	MWsScreenRedraw * iScreenRedraw;
	MWsScreenConfig * iScreenConfig;
#ifdef USE_DEBUG_FRAME_CAPTURE
	TBool iFrameCapture;
	TPtrC iFrameCaptureLocation;
	CFbsBitmap* iDebugBitmap;
	CFbsBitmapDevice* iDebugBitmapDevice;
	CFbsBitGc* iDebugBitmapGc;
	// Defining log levels here (CDebugLogBase is in w32cmd.h, not exposed to render stage plugin)
	enum TLogLevel
		{
		ELogEverything=1,	// Gives the complete WSERV client server interaction along with client logging.
		ELogIntermediate=5,	// Gives the important WSERV client server interaction along with client logging.
		ELogImportant=9,	// Gives logging of panics and client logging.
		};		
#endif
	};

#ifdef __WINS__
	_LIT(KWSERVIniFileVarFrameCaptureLocation, "c:\\");
#else
	_LIT(KWSERVIniFileVarFrameCaptureLocation, "e:\\storyboard\\");
#endif

	
	
#endif //__FBRENDERSTAGE_H__
