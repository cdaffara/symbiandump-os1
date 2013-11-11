// ping.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
using namespace IoUtils;

#include "ping_misc.h"
#include "pingmodel.h"



class CCmdPing : public CCommandBase, public CPingContainer
	{
public:
	static CCommandBase* NewLC();
	~CCmdPing();
private:
	CCmdPing();
	void InitModelL();

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);

public:
	virtual void WriteHostL(TDes& aHostname);
	virtual void UpdateStatisticsL();
	virtual void WriteLine(const TDesC& abuf);
	virtual void OnEnd();
	
private:
	CActiveSchedulerWait* iActiveWait;
	CPing *iPingModel;	//contains all Ping related Data
	TInt iOptTotalPackets;		//total packets
	TInt iOptSecInterval;		//interval between packets
	TInt iIap;
	HBufC* iAddress;
	};

CCommandBase* CCmdPing::NewLC()
	{
	CCmdPing* self = new(ELeave) CCmdPing();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdPing::CCmdPing()
	{
	iActiveWait = new (ELeave) CActiveSchedulerWait;
	}


CCmdPing::~CCmdPing()
	{
	delete iAddress;
	delete iPingModel;
	delete iActiveWait;
	}

void CCmdPing::WriteHostL(TDes& aHostname)
	{
	Printf(_L("Host: %S \r\n"), &(aHostname) );
	}

void CCmdPing::WriteLine(const TDesC& abuf)
	{
	Printf(_L("%S\r\n"), &(abuf) );
	}

void CCmdPing::UpdateStatisticsL()
	{
	}

void CCmdPing::OnEnd()
	{
	TBool bWait = iActiveWait->IsStarted(); 
	if (bWait)
		iActiveWait->AsyncStop();
	}

const TDesC& CCmdPing::Name() const
	{
	_LIT(KName, "ping");
	return KName;
	}

void CCmdPing::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iAddress, _L("address"));
	}

void CCmdPing::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendIntL(iOptTotalPackets, _L("num-packets"));
	aOptions.AppendIntL(iOptSecInterval, _L("time"));
#ifdef IAPSETTING
	aOptions.AppendIntL(iIap, _L("iap"));
#endif
	}

void CCmdPing::DoRunL()
	{
	InitModelL();
		
	iPingModel->SetConsole(this);
	
	//set host name, if it is specified
	if (iAddress && iAddress->Length()>0 )
		{
		iPingModel->SetHostName(*iAddress);
		}
	
	//start to ping
    if (!(iPingModel->IsRunning()))
    	{
        iPingModel->BeginL();
        TBool bPingIsRunning = iPingModel->IsRunning();
        
        if (bPingIsRunning)
            iActiveWait->Start();
        else
	        LeaveIfErr(KErrGeneral, _L("Could not connect to socket server or connect to internet"));
    	}
	}

void CCmdPing::InitModelL()
	{
	TPreferences param;
	CPing::DefaultPreferences(param);
    
	if (iOptTotalPackets > 0)
		param.iTotalPackets = iOptTotalPackets; 
		
	if (iOptSecInterval > 0)
		param.iSecWait = iOptSecInterval; 

#ifdef IAPSETTING
	if (iIap > 0)
		param.iIAP = iIap;
#endif	

	iPingModel= new (ELeave) CPing();
	iPingModel->ConstructL(param);
	}


EXE_BOILER_PLATE(CCmdPing)

