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



#ifndef T_DEVSOUND_SERVER_H
#define T_DEVSOUND_SERVER_H

#include <testserver2.h>

class CT_DevSoundServer : public CTestServer2
	{
private:
	class CT_DevSoundBlock : public CTestBlockController
		{
	public:
		inline CT_DevSoundBlock();
		inline ~CT_DevSoundBlock();
		CDataWrapper* CreateDataL( const TDesC& aData );

		};

public:
	static CT_DevSoundServer* NewL();
	inline CTestBlockController* CreateTestBlock();
	void DoCmdutilStartApp();
	};

#include "t_devsoundserver.inl"

#endif // T_DEVSOUND_SERVER_H
