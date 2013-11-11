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



#ifndef T_DEVVIDEOSERVER_H
#define T_DEVVIDEOSERVER_H

//	EPOC Includes
#include <testserver2.h>

/**
 * This wrapper class extends the test server and creates test server for Video Playback driver
 */
class CT_DevVideoServer : public CTestServer2
	{
private:
	class CT_DevVideoBlock : public CTestBlockController
		{
	public:
		inline CT_DevVideoBlock();
		inline ~CT_DevVideoBlock();
		CDataWrapper*	CreateDataL( const TDesC& aData );
		};

public:
	//~CT_DevVideoServer();
	static CT_DevVideoServer* NewL();
	inline CTestBlockController*	CreateTestBlock();

protected:
	//CT_DevVideoServer();
	};

#include "t_devvideoserver.inl"

#endif // T_DEVVIDEOSERVER_H
