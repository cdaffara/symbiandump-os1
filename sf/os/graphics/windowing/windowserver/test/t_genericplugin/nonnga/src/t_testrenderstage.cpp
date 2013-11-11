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
 @test
 @internalComponent
*/

#include "t_testrenderstage.h"
#include <graphics/wsgraphicdrawerinterface.h>
#include "t_logfile.h"
#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>

const TUint KBlackMap = 50;
const TUint KWhiteMap = 200;

/***************************************************
CTestRenderStage
****************************************************/
/**
Creates a new CTestRenderStage object. 
Standardized safe construction which leaves nothing on the cleanup stack.
@param	aEnv The environment for a graphic drawer 
@param	aScreen A pointer points to the interface for drawing occasion of a CWsGraphicDrawer 
		used by animation schedulers to update screens 
@param	aScreenRedraw A pointer points to the interface for handling the animation aspects of 
		the redraw sequence.
@return A pointer to the newly created object.
@post	This object is fully constructed and initialized.
*/ 
CTestRenderStage* CTestRenderStage::NewL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw)
	{
	CTestRenderStage* stage = new(ELeave) CTestRenderStage;
	CleanupStack::PushL(stage);
	stage->ConstructL(aEnv, aScreen, aScreenRedraw);
	CleanupStack::Pop(stage);
	return stage;
	}
	
/**
Initialisation phase of two phase construction.
@param	aEnv The environment for a graphic drawer 
@param	aScreen A pointer points to the interface for drawing occasion of a CWsGraphicDrawer 
		used by animation schedulers to update screens 
@param	aScreenRedraw A pointer points to the interface for handling the animation aspects of 
		the redraw sequence.
*/
void CTestRenderStage::ConstructL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw)
	{
	BaseConstructL();
	iEnv = aEnv;
	iScreen = aScreen;
	iScreenRedraw = aScreenRedraw;
	iBackBuffer = aScreen->ObjectInterface<MWsBackBuffer>();
	User::LeaveIfNull(iBackBuffer);
	iScreenConfig = iScreen->ObjectInterface<MWsScreenConfig>();
	User::LeaveIfNull(iScreenConfig);
	User::LeaveIfError(iEnv->RegisterWsEventHandler(this, TWservCrEvent::EWindowClosing));
	MWsPluginManager* WsPluginManager = aEnv->ObjectInterface<MWsPluginManager>();
	CTestServicePlugin* servicePlugin = static_cast<CTestServicePlugin*>(WsPluginManager->ResolvePluginInterface(KServicePluginUid));
	servicePlugin->SetFadingParamsL(KBlackMap, KWhiteMap);
	}

/**
The constructor of CTestRenderStage  
*/
CTestRenderStage::CTestRenderStage()
	{
	}
	
/**
The destructor of CTestRenderStage
*/
CTestRenderStage::~CTestRenderStage()
	{
	}

/**
Returns the GC to use for drawing in this render stage:
*/
CFbsBitGc* CTestRenderStage::Begin()
	{
	return iBackBuffer->GetBitGcCurrent();
	}

/**
Completes a pass of this render stage.
*/
void CTestRenderStage::End()
	{
	CFbsBitGc* gc =  iBackBuffer->GetBitGcCurrent();
	gc->SetPenSize(TSize(2,2));
	gc->SetPenColor(KRgbRed);
	gc->DrawLine(TPoint(50,0),TPoint(0,50));
	gc->SetPenColor(KRgbGreen);
	gc->DrawLine(TPoint(60,0),TPoint(0,60));
	
	if (Next())
		{
		gc = Next()->Begin();
		const TRegion* region = iScreenRedraw->AnimationRegion();
		if(region && !region->IsEmpty() && !region->CheckError())
			{
			if (iBackBuffer->Observer())
				iBackBuffer->Observer()->BeforeUpdate(*iBackBuffer,*region);
			
			gc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
			gc->SetClippingRegion(region);
			gc->BitBlt(-iScreenConfig->ScaledOrigin(),iBackBuffer->GetBitmap());
			gc->SetDrawMode(CGraphicsContext::EDrawModePEN);
			gc->CancelClipping();
			
			if (iBackBuffer->Observer())
				iBackBuffer->Observer()->AfterUpdate(*iBackBuffer,*region);
			}
		Next()->End();
		}
	}

/**
Handles the event of CTestRenderStage
@param	aEvent Wserv event representation to plugin side
*/
void CTestRenderStage::DoHandleEvent(const TWservCrEvent& aEvent)
	{
	TInt err;
	if(aEvent.Type() == TWservCrEvent::EWindowClosing)
		{
		TRAP(err,OnWindowCloseL(aEvent.Window()));
		if(err != KErrNone)
			{
			User::Panic(KLogFileName, err);
			}		
		}
	}

/**
Handles the window close event
@param	aWindow The closing window.
*/
void CTestRenderStage::OnWindowCloseL(MWsWindow* aWindow)
	{
	TPoint origin = aWindow->Origin();
	TRect absRect = aWindow->AbsRect();
	TSize size = aWindow->Size();
	TUint32 handle = aWindow->Handle();

	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TBuf<255> print(_L(""));
	log->WriteToLogL(print);
	print.Format(_L("Closing Window Handle %d - Origin: %d, %d"), handle, origin.iX, origin.iY);
	log->WriteToLogL(print);
	print.Format(_L("Closing Window Handle %d - AbsRec: Height %d, Width %d"), handle, absRect.Height(), absRect.Width());
	log->WriteToLogL(print);
	print.Format(_L("Closing Window Handle %d - Size: Height %d, Width %d"), handle, size.iHeight, size.iWidth);
	log->WriteToLogL(print);
	CleanupStack::PopAndDestroy(log);
	}
