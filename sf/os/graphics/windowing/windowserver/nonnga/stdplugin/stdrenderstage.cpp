// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "stdrenderstage.h"
#include <graphics/wsgraphicdrawerinterface.h>
#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>

CStdRenderStage* CStdRenderStage::NewL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen)
	{
	CStdRenderStage * stage = new(ELeave) CStdRenderStage;
	CleanupStack::PushL(stage);
	stage->ConstructL(aEnv, aScreen);
	CleanupStack::Pop(stage);
	return stage;
	}
	
void CStdRenderStage::ConstructL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen)
	{
	BaseConstructL();
	iEnv = aEnv;
	iScreen = aScreen;
	iFrontBuffer = aScreen->ObjectInterface<MWsFrontBuffer>();
	User::LeaveIfNull(iFrontBuffer);
	}

CStdRenderStage::CStdRenderStage()
	{
	}
	
CStdRenderStage::~CStdRenderStage()
	{
	}

CFbsBitGc * CStdRenderStage::Begin()
	{
	return iFrontBuffer->GetBitGcCurrent();
	}
	
void CStdRenderStage::End()
	{
	}
