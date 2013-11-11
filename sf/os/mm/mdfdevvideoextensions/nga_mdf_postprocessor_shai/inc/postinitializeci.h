/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef POSTINITIALIZECI_H_
#define POSTINITIALIZECI_H_

#include <e32cmn.h>

static const TUid KUidMmfPostInitializeRequest = { 0x20031dac };  //  allocated properly

class MMmfPostInitializeResponse
	{
public:
	virtual void MmpirPostInitializeResponseL() = 0;
	};

class MMmfPostInitializeRequest
	{
public:
	virtual void MmpirPostInitializeRequest(MMmfPostInitializeResponse& aResponse) = 0;
	};

#endif /* POSTINITIALIZECI_H_ */
