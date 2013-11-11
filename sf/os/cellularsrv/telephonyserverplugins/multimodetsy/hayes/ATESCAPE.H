// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Escape to command (online) mode
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __ATESCAPE_H__
#define __ATESCAPE_H__

#include "ATBASE.H"

class CATSetToOnlineCommandMode : public CATBase
/**
@internalTechnology
*/
	{
public:
	static CATSetToOnlineCommandMode* NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	~CATSetToOnlineCommandMode();
	void StartEscapeSequence(CATCommands* aPendingCommand, TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void StopEscapeSequence(TTsyReqHandle aTsyReqHandle);
protected:
	CATSetToOnlineCommandMode(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
private:
	CATCommands* iPendingCommand; // Which command to set going after escaping to command mode
	TAny* iParams;
	enum
		{
		EWaitForATSilence,
		EWaitForWriteCompletion,
		EWaitForATEscapeOK
		} iState;
	};

#endif
