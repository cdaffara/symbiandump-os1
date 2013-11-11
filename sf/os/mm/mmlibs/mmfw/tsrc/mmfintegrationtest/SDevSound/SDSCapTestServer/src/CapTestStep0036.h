// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SECUREDEVSOUNDTESTSTEP0036_H__
#define __SECUREDEVSOUNDTESTSTEP0036_H__

#include "CapTestStep.h"

//class CCapTestInfoServer;

class CSecDevSndTS0036 : public CSimulProcTestStep
	{
public:
	static CSecDevSndTS0036* NewL(TThreadId aClientTid);
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual TVerdict EndProcessingAndReturnResult(TDes8& aMessage);
	virtual ~CSecDevSndTS0036();

protected:
	CSecDevSndTS0036(TThreadId aClientTid);
	void ConstructL();

private:
	//CCapTestInfoServer* iInfoServer;
	TThreadId iClientTid;
	TThreadId iServerTid;
	};


#endif	// __SECUREDEVSOUNDTESTSTEP0036_H__
