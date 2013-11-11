// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalAll 
*/

#ifndef ISVAO_H
#define ISVAO_H

#include <e32base.h>	
#include <e32std.h>
#include <etel3rdparty.h>

/**
 * Wrapper active object to use with CTelephony.
 */
class CIsvAO : public CActive
	{
public:

	~CIsvAO();
	static CIsvAO* NewL();
	static CIsvAO* NewLC();
	
	enum TIsvOperation
		{
		EIsvNoOperation,
		EIsvDial,
		EIsvHangUp,
		EIsvAnswer,
		EIsvNotifyIncomingCall
		};
	
	void DoOperation(TIsvOperation aOperation);
	
	TRequestStatus Status() { return iStatus; }
	TInt CallId() { return iCallId; }
	CTelephony::TCallStatusV1 LineStatus() { return iLineStatus; }
	
private:

	CIsvAO();
	void ConstructL();

private: // From CActive

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private: // Owned
	
	CTelephony* iTelephony;
	CTelephony::TCallId iCallId;
	
	CActiveScheduler* iScheduler; 
	
	CTelephony::TCallStatusV1 iLineStatus;
    CTelephony::TCallStatusV1Pckg iLineStatusPckg;

    TIsvOperation iCurrentOperation;
	}; // class CIsvAO

#endif // ISVAO_H
