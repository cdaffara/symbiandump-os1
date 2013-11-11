/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "T_MMDataCamera.h"

_LIT(KT_CameraWrapper,	"CCamera");

CT_CameraServer::CT_CameraBlock::CT_CameraBlock()
:	CTestBlockController()
	{
	}

CT_CameraServer::CT_CameraBlock::~CT_CameraBlock()
	{
	}

CDataWrapper* CT_CameraServer::CT_CameraBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper = NULL;

	if( KT_CameraWrapper() == aData )
		{
		wrapper = CT_MMDataCamera::NewL();
		}

	return wrapper;
	}

CT_CameraServer::CT_CameraServer()
	{
	}

CT_CameraServer::~CT_CameraServer()
	{
	}

void CT_CameraServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_CameraServer::CreateTestBlock()
	{
	return new CT_CameraBlock();
	}
