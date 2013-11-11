// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Contains declaration of test 02 that :
// - establishes client server/connection;
// - tranfers 0x10000 bytes long mesage between them 
// 
//

#if (!defined __TEST03IPCLONGDATATRANSFER_H__)
#define __TEST03IPCLONGDATATRANSFER_H__

#include "TestStepIPC.h"

class CTest03IPCLongDataTransfer : public CIPCTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};
#endif //(__TEST03IPCLONGDATATRANSFER_H__)

