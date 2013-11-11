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
// Contains declaration of test 05 that :
// - creates a thread that fails as it tries to accept unopened socket;
// - creates another thread that fails as it tries to accept connected socket;
// 
//

#if (!defined __TEST05IPCILLEGALOPERATIONS_H__)
#define __TEST05IPCILLEGALOPERATIONS_H__

#include "TestStepIPC.h"

class CTest05IPCIllegalOperations : public CIPCTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		CTest05IPCIllegalOperations();
		virtual enum TVerdict InternalDoTestStepL( void );
	private:
		static TInt fAcceptWithUnopendSocket(TAny*);  //Test thread that shoul fail
		static TInt fAcceptWithConnectedSocket(TAny*);//Test thread that will send data
		
		TInt iThreadStackSize;                 //Stack size of the threads
	};
#endif //(__TEST05IPCILLEGALOPERATIONS_H__)

