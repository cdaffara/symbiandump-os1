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

This contains CT_SoundActiveCallback
*/

//	User includes
#include "t_soundactivecallback.h"

/**
 * Two phase constructor
 */
CT_SoundActiveCallback* CT_SoundActiveCallback::NewL(CDataWrapperBase& aCallback, TInt aPriority)
	{
	CT_SoundActiveCallback*	ret = NewLC(aCallback, aPriority);
	CleanupStack::Pop(ret);
	return ret;	
	}

/**
 * Two phase constructor
 */
CT_SoundActiveCallback* CT_SoundActiveCallback::NewLC(CDataWrapperBase& aCallback, TInt aPriority)
	{
	CT_SoundActiveCallback*	ret = new (ELeave) CT_SoundActiveCallback(aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	return ret;	
	}

/**
 * Protected constructor. First phase construction
 */
CT_SoundActiveCallback::CT_SoundActiveCallback(CDataWrapperBase& aCallback, TInt aPriority)
:	CActiveCallbackBase(aCallback, aPriority),
	iDataPosition(0),
    iSoundData(NULL),
    iSection(NULL),
    iBufferBlock(NULL, NULL, NULL)
	{
	}

/**
 * Public destructor
 */
CT_SoundActiveCallback::~CT_SoundActiveCallback()
	{
	if (iSoundData)
		{
		iSoundData->Close();
		delete iSoundData;
		iSoundData = NULL;
		}
	}

/**
 * Set section
 * 
 * @param aSection 			section name to set
 *
 * @return					void
 *
 * @leave					no
 */
void CT_SoundActiveCallback::SetSection(const TDesC& aSection)
	{
	this->iSection=const_cast<TDesC*>(&aSection);
	}

/**
 * Create sound data buffer
 * 
 * @param aLength 			Length of the buffer to be created
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_SoundActiveCallback::CreateSoundDataBufferL(TInt aLength)
	{
	iSoundData = new(ELeave) TSoundUtil::RAudioBuffer();
	User::LeaveIfError(iSoundData->Create(aLength));
	}

