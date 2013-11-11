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

This contains CT_SoundDriverServer
*/

#ifndef __T_SOUND_DRIVER_SERVER_H__
#define __T_SOUND_DRIVER_SERVER_H__

// EPOC Includes
#include <testblockcontroller.h>
#include <testserver2.h>

// User includes
#include "t_mdadevsounddata.h"
#include "t_currentsoundformatdata.h"
#include "t_soundformatssupporteddata.h"

// classes being tested
_LIT(KRMdaDevSound,					"RMdaDevSound");
_LIT(KTCurrentSoundFormat,			"TCurrentSoundFormat");
_LIT(KTSoundFormatsSupported,		"TSoundFormatsSupported");

// This wrapper class extends the test server and creates test server for sound driver
class CT_SoundDriverServer : public CTestServer2
	{
private:
	class CT_SoundDriverBlock : public CTestBlockController
		{
	public:
		inline CT_SoundDriverBlock();
		inline ~CT_SoundDriverBlock();
		inline void ConstructL();

		inline CDataWrapper*	CreateDataL(const TDesC& aData);
		
	public:
		RMdaDevSound* iServerStore;
		};

public:
	static CT_SoundDriverServer* NewL();
	inline ~CT_SoundDriverServer();

	inline CTestBlockController*	CreateTestBlock();

protected:
	inline CT_SoundDriverServer();
	
private:
	RMdaDevSound iServerStore;
	};

#include "t_sounddriverserver.inl"

#endif // __T_SOUND_DRIVER_SERVER_H__
