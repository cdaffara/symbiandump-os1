// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 22
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection22.h"


// Test step 22.1
const TDesC& CEsockTest22_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test22.1");
	return ret;
	}

CEsockTest22_1::~CEsockTest22_1()
	{
	appendixInArray.Close();
	appendixOutArray.Close();
	}

enum TVerdict CEsockTest22_1::easyTestStepL()
	{
	TInetAddr mask, addrOut;
	
	mask.Input(_L("ffff:ffff::"));
	
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	if (!GetAppendixC3())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		appendixInArray[i].SubNet(addrOut, mask);
		if (addrOut.Match(appendixOutArray[i]))
			{
			return EFail;
			}
		}
	
	appendixInArray.Reset();
	appendixOutArray.Reset();
	return EPass;
	}

