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

/**
@test
@internalComponent

This contains CT_SoundDriverServer inline functions
*/

CT_SoundDriverServer::CT_SoundDriverBlock::CT_SoundDriverBlock()
:	CTestBlockController()
	{
	}

void CT_SoundDriverServer::CT_SoundDriverBlock::ConstructL()
	{
	iServerStore->Close();
	iServerStore->SetHandle(KNullHandle);
	}

CT_SoundDriverServer::CT_SoundDriverBlock::~CT_SoundDriverBlock()
	{
	}

/**
 * Method to create wrapper according to param aData
 * @param aData contains wrapper class name
 */
CDataWrapper* CT_SoundDriverServer::CT_SoundDriverBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper = NULL;

	if( KRMdaDevSound() == aData )
		{
		wrapper = CT_MdaDevSoundData::NewL(iServerStore);
		}
	else if( KTCurrentSoundFormat() == aData )
		{
		wrapper = CT_CurrentSoundFormatData::NewL();
		}
	else if( KTSoundFormatsSupported() == aData )
		{
		wrapper = CT_SoundFormatsSupportedData::NewL();
		}
	return wrapper;
	}

CT_SoundDriverServer::CT_SoundDriverServer()
	{
	}

CT_SoundDriverServer::~CT_SoundDriverServer()
	{
	iServerStore.Close();
	iServerStore.SetHandle(KNullHandle);
	}

CTestBlockController* CT_SoundDriverServer::CreateTestBlock()
	{
	CT_SoundDriverServer::CT_SoundDriverBlock* result = new 
	CT_SoundDriverServer::CT_SoundDriverBlock();
	result->iServerStore = &iServerStore;
	result->ConstructL();
	return result;
	}
