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



#if (!defined __T_IMAGE_ENCODER_SERVER_H__)
#define __T_IMAGE_ENCODER_SERVER_H__

//	EPOC Includes
#include <testblockcontroller.h>
#include <testserver2.h>

class CT_ImageEncoderServer : public CTestServer2
	{
private:
	class CT_ImageEncoder : public CTestBlockController
		{
	public:
		inline CT_ImageEncoder();
		inline ~CT_ImageEncoder();

		inline CDataWrapper* CreateDataL(const TDesC& aData);
		};

public:
	inline ~CT_ImageEncoderServer();

	inline void	DeleteActiveSchedulerL();

	static CT_ImageEncoderServer* NewL();

	inline CTestBlockController*	CreateTestBlock();

protected:
	inline CT_ImageEncoderServer();
	};

#include "T_ImageEncoderServer.inl"

#endif /* __T_IMAGE_ENCODER_SERVER_H__ */
