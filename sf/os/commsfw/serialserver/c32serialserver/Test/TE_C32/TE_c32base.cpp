// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "TE_c32base.h"
#include "TE_c32.h"

//
// Maximum number of characters to output on a line of data.
//
const TInt  KMaxLineLength = 64;


TVerdict CC32TestStep::doTestStepPreambleL( void )
	{	
	TInt res=iCommSession.Connect();
	TESTCHECKL(res, KErrNone);

	res=iCommSession.LoadCommModule(_L("ECUART"));
	TESTCHECKCONDITIONL(res==KErrNone || res==KErrAlreadyExists);
	
	__UHEAP_MARK;
	
    return TestStepResult();
	}	

TVerdict CC32TestStep::doTestStepPostambleL( void )
	{
	__UHEAP_MARKEND;
	
	iCommSession.Close(); // close the comm server.
 	
 	return TestStepResult(); 	 	
	}

void CC32TestStep::ShowReceived(const TPtr8& aBuf)
	{
	TInt offset = 0;

	while (offset < aBuf.Length())
		{
		//
		// Convert to unicode string buffer.  If there are less than KMaxLineLength
		// characters remaining then we should print all of them.  Otherwise print
		// just KMaxLineLength characters.  Also remove any non-printable characters.
		//
		TBuf<KMaxLineLength>  tempBuf;
		TInt  index;

		if (offset + KMaxLineLength > aBuf.Length())
			{
			tempBuf.Copy(aBuf.Mid(offset));
			}
		else
			{
			tempBuf.Copy(aBuf.Mid(offset, KMaxLineLength));
			}

		for (index = 0;  index < tempBuf.Length();  index++)
			{
			if (TChar(tempBuf[index]).IsPrint() == EFalse)
				{
				tempBuf[index] = '.';
				}
			}

		INFO_PRINTF2(_L("%S"), &tempBuf);

		//
		// Move forward KMaxLineLength characters.
		//
		offset += KMaxLineLength;
		}		
	}

