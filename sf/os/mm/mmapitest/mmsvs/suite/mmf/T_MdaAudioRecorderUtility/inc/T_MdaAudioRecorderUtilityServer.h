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



#if (!defined __T_MDA_AUDIO_RECORDER_UTILITY_SERVER_H__)
#define __T_MDA_AUDIO_RECORDER_UTILITY_SERVER_H__

//	EPOC Includes
#include <testblockcontroller.h>
#include <testserver2.h>

class CT_MdaAudioRecorderUtilityServer : public CTestServer2
	{
private:
	class CT_MdaAudioRecorderUtilityBlock : public CTestBlockController
		{
	public:
		inline CT_MdaAudioRecorderUtilityBlock();
		inline ~CT_MdaAudioRecorderUtilityBlock();

		inline CDataWrapper*	CreateDataL(const TDesC& aData);
		};

public:
	inline ~CT_MdaAudioRecorderUtilityServer();

	inline void	DeleteActiveSchedulerL();

	static CT_MdaAudioRecorderUtilityServer* NewL();

	inline CTestBlockController*	CreateTestBlock();

protected:
	inline CT_MdaAudioRecorderUtilityServer();
	};

#include "T_MdaAudioRecorderUtilityServer.inl"

#endif /* __T_MDA_AUDIO_RECORDER_UTILITY_SERVER_H__ */
