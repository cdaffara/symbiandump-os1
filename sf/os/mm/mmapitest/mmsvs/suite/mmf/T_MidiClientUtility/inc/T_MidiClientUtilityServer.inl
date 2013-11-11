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


#include "T_MMDataMidiClientUtility.h"

_LIT(KDataMidiClientUtility,	"CMidiClientUtility");

CT_MidiClientUtilityServer::CT_MidiClientUtilityBlock::CT_MidiClientUtilityBlock()
:	CTestBlockController()
	{
	}

CT_MidiClientUtilityServer::CT_MidiClientUtilityBlock::~CT_MidiClientUtilityBlock()
	{
	}

CDataWrapper* CT_MidiClientUtilityServer::CT_MidiClientUtilityBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper = NULL;
	if( KDataMidiClientUtility() == aData )
		{
		wrapper = CT_MMDataMidiClientUtility::NewL();
		}
	return wrapper;
	}

CT_MidiClientUtilityServer::CT_MidiClientUtilityServer()
	{
	}

CT_MidiClientUtilityServer::~CT_MidiClientUtilityServer()
	{
	}

void CT_MidiClientUtilityServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_MidiClientUtilityServer::CreateTestBlock()
	{
	return new CT_MidiClientUtilityBlock();
	}
