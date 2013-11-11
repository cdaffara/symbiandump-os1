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



#if (!defined __T_VIDEO_PLAYER_UTILITY_SERVER_H__)
#define __T_VIDEO_PLAYER_UTILITY_SERVER_H__

//	EPOC Includes
#include <testblockcontroller.h>
#include <testserver2.h>

class CT_VideoPlayerUtilityServer : public CTestServer2
	{
private:
	class CT_VideoPlayerUtility : public CTestBlockController
		{
	public:
		inline CT_VideoPlayerUtility();
		inline ~CT_VideoPlayerUtility();

		inline CDataWrapper*	CreateDataL(const TDesC& aData);
		};

public:
	inline ~CT_VideoPlayerUtilityServer();

	inline void	DeleteActiveSchedulerL();

	static CT_VideoPlayerUtilityServer* NewL();

	inline CTestBlockController*	CreateTestBlock();

protected:
	inline CT_VideoPlayerUtilityServer();
	};

#include "T_VideoPlayerUtilityServer.inl"

#endif /* __T_VIDEO_PLAYER_UTILITY_SERVER_H__ */
