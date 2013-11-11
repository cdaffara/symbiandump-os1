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



#if (!defined __T_CAMERA_SERVER_H__)
#define __T_CAMERA_SERVER_H__

//	EPOC Includes
#include <testblockcontroller.h>
#include <testserver2.h>

class CT_CameraServer : public CTestServer2
	{
private:
	class CT_CameraBlock : public CTestBlockController
		{
	public:
		inline CT_CameraBlock();
		inline ~CT_CameraBlock();

		inline CDataWrapper*	CreateDataL(const TDesC& aData);
		};

public:
	inline ~CT_CameraServer();

	inline void	DeleteActiveSchedulerL();

	static CT_CameraServer*	NewL();

	inline CTestBlockController*	CreateTestBlock();

protected:
	inline CT_CameraServer();
	};

#include "T_CameraServer.inl"

#endif /* __T_CAMERA_SERVER_H__ */
