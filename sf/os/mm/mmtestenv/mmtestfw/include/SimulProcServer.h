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

#ifndef __SIMULPROCSERVER_H__
#define __SIMULPROCSERVER_H__ 

#include <e32base.h>
#include <testframework.h>

enum 
	{
	EMMTSOpenTestStep,
	EMMTSStartProcessing,
	EMMTSStopProcessing,
	EMMTSClose
	};
	
class CSimulProcTestStep 
	{
public:
	virtual void StartProcessing(TRequestStatus& aStatus)= 0;
	virtual TVerdict EndProcessingAndReturnResult(TDes8& aMessage) = 0;		
	IMPORT_C virtual ~CSimulProcTestStep();
protected:
	IMPORT_C CSimulProcTestStep();
	};
	
	
	
class CSimulProcServer : public CServer2
	{
public:
	IMPORT_C virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
//	IMPORT_C virtual void ConstructL(const TDesC& aName);
	
	virtual CSimulProcTestStep* CreateTestStep(const TDesC& aStepName) const = 0;
	IMPORT_C virtual void SessionClosed();
	IMPORT_C virtual ~CSimulProcServer();
	
protected:
	IMPORT_C CSimulProcServer();
	
private:
	
	TInt	iSessionCount;
	};




class CSimulProcSession : public CSession2
	{
public:
	class CActiveCallback : public CActive
		{
	public:
		CActiveCallback(const RMessage2& aMessage);
		
		void RunL();
		
		void DoCancel();
		
		TRequestStatus& ActiveStatus();
		
	private:		
		const RMessage2& iMessage;
		};
		
public:
	IMPORT_C CSimulProcSession();
	IMPORT_C virtual ~CSimulProcSession();
	IMPORT_C virtual void ServiceL(const RMessage2& aMessage);
private:
	CSimulProcTestStep* iTestStep;
	CActiveCallback* iActiveCallback;
	
	};
	

#endif
