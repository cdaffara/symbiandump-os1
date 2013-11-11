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
// - transfer small amound of data with partial reading/writing 
// 
//

#if (!defined __TEST02IPCSIMPLEDATATRANSFER_H__)
#define __TEST02IPCSIMPLEDATATRANSFER_H__

#include "TestStepIPC.h"

class CTest02IPCSimpleDataTransfer : public CIPCTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};
#endif //(__TEST02IPCSIMPLEDATATRANSFER_H__)

