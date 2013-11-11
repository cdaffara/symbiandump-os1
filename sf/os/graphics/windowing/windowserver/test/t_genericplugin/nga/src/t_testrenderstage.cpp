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
#include <graphics/wsgraphicscontext.h>
#include "t_logfile.h"
#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>


const TUint KBlackMap = 50;
const TUint KWhiteMap = 200;

/***************************************************
CTestRenderStage
****************************************************/

CTestRenderStage* CTestRenderStage::NewL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage)
	{
	CTestRenderStage* stage = new(ELeave) CTestRenderStage;
	CleanupStack::PushL(stage);
	stage->ConstructL(aEnv, aScreen, aScreenRedraw, aNextStage);
	CleanupStack::Pop(stage);
	return stage;
	}

void CTestRenderStage::ConstructL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, CWsRenderStage* aNextStage)
	{
	BaseConstructL();
	iEnv = aEnv;
	User::LeaveIfError(aEnv->RegisterWsEventHandler(this, TWservCrEvent::EWindowClosing));
	MWsPluginManager* WsPluginManager = aEnv->ObjectInterface<MWsPluginManager>();
	CTestServicePlugin* servicePlugin = static_cast<CTestServicePlugin*>(WsPluginManager->ResolvePluginInterface(KServicePluginUid));
	servicePlugin->SetFadingParamsL(KBlackMap, KWhiteMap);
	//Default in BitGdi was 128 for the blackMap and 255 for the whiteMap
	//SetFadingParameters shows how the fade color is computed
	iFadeColor.SetInternal(0x80FFFFFF);
	SetNext(aNextStage);
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
	iRegion = NULL;
	}


void CTestRenderStage::Begin(const TRegion* aRegion)
	{
	iRegion = aRegion;
	}

void CTestRenderStage::End(TRequestStatus* aFinished)
	{
	if (Next())
		{
		Next()->Begin(iRegion);
		iContext = static_cast<MWsGraphicsContext*>(Next()->ResolveObjectInterface(KMWsGraphicsContext));

		iContext->SetPenStyle(MWsGraphicsContext::ESolidPen);
		iContext->SetPenSize(TSize(2,2));
		iContext->SetPenColor(KRgbRed);
		iContext->DrawLine(TPoint(50,0),TPoint(0,50));
		iContext->SetPenColor(KRgbGreen);
		iContext->DrawLine(TPoint(60,0),TPoint(0,60));

		Next()->End(aFinished);
		}
		iRegion = NULL;  // Region is not valid after an End.
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

/**
Sets the fade area. Test fading plugin gets the black and white map from the 
service plugin and simply uses bitgdi to perform fading
@param	aBitGc The bitmapped graphics context
@param	aRegion The region for fading
*/
void CTestRenderStage::FadeArea(const TRegion& aRegion)
	{
	//get fading parameters from service plugin
	MWsPluginManager* wsPluginManager = iEnv->ObjectInterface<MWsPluginManager>();
	CTestServicePlugin* servicePlugin = static_cast<CTestServicePlugin*>(wsPluginManager->ResolvePluginInterface(KServicePluginUid));
	TInt err;
	TRAP(err,iFadeColor = servicePlugin->GetFadeColorL());
	if(err != KErrNone)
		{
		User::Panic(KLogFileName, err);
		}
	iContext->Reset();
	iContext->SetClippingRegion(aRegion);
	iContext->SetPenStyle(MWsGraphicsContext::ENullPen);
	iContext->SetBrushStyle(MWsGraphicsContext::ESolidBrush);
	iContext->SetBrushColor(iFadeColor);
	iContext->DrawRect(aRegion.BoundingRect());
	}

/**
Sets the fading parameters. Test fading plugin uses black/white map get from serviceplugin 
as fading parameters. It doesn't care what black/white map values are passed in.  
@param	aData The fading parameters
*/
void CTestRenderStage::SetFadingParameters(const TDesC8& /*aData*/)
  	{
   	}



/**
Overides MWsObjectProvider. Resolve an instance of an interface
@param aTypeId The interface UID.
@return The pointer to the instance of an interface.
*/
TAny* CTestRenderStage::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case MWsFader::EWsObjectInterfaceId:
			return static_cast<MWsFader*>(this);
		}

	return CWsRenderStage::ResolveObjectInterface(aTypeId);
	}
