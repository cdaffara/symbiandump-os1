// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file commengine.cpp
// This contains CCommEngine which manage access to serial port.
// 

// user include
#include "globalphonemanager.h"
#include "commengine.h"
#include "mslogger.h"
#include "commreader.h"
#include "commwriter.h"
#include "mcommobserver.h"
#include "stringparser.h"

// const define
const TInt KLtsyOneSecondPause = 1000000;

// ---------------------------------------------------------------------------
// CCommEngine::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CCommEngine* CCommEngine::NewL(TInt aBufferSize, 
					           TInt aReadPriority, 
					           TInt aWritePriority, 
					           TPortAccess& aPortAccess)
	{
	CCommEngine *self = CCommEngine::NewLC(aBufferSize, 
										   aReadPriority, 
										   aWritePriority, 
										   aPortAccess);
	CleanupStack::Pop(self);
	
	return self;
	}

// ---------------------------------------------------------------------------
// CCommEngine::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CCommEngine* CCommEngine::NewLC(TInt aBufferSize, 
							    TInt aReadPriority, 
							    TInt aWritePriority, 
							    TPortAccess& aPortAccess)
	{
	CCommEngine* self = new (ELeave) CCommEngine(aPortAccess);
	CleanupStack::PushL(self);
	
	self->ConstructL(aBufferSize, aReadPriority, aWritePriority);
	
	return self;
	}

// ---------------------------------------------------------------------------
// CCommEngine::~CCommEngine
// other items were commented in a header
// ---------------------------------------------------------------------------
CCommEngine::~CCommEngine()
	{
    delete iCommReader;
	delete iCommWriter;
	delete iStringParse;
	delete iCallbackTimer;
	iCommPort.Close();
	iCommServer.Close();
	}

// ---------------------------------------------------------------------------
// CCommEngine::CCommEngine
// other items were commented in a header
// ---------------------------------------------------------------------------
CCommEngine::CCommEngine(TPortAccess& aPortAccess) 
						:iPortAccess(aPortAccess)
	{
	iCommReader = NULL;
	iCommWriter = NULL;
	iEngineObserver = NULL;
	iStringParse = NULL;
	iCommCancel = EFalse;
	}

// ---------------------------------------------------------------------------
// CCommEngine::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::ConstructL(TInt aBufferSize, TInt aReadPriority, TInt aWritePriority)
	{
	iCommReader = new (ELeave) CCommReader(this, aReadPriority);
	iCommWriter = new (ELeave) CCommWriter(this, aWritePriority);	
	iStringParse = CStringParser::NewL(aBufferSize);
	iCallbackTimer = CCallbackTimer::NewL(*this);
	}

// ---------------------------------------------------------------------------
// CCommEngine::ConfigurePort
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CCommEngine::ConfigurePort(TCommConfig aConfiguration)
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::ConfigurePort()"));
	if (iPortAccess == EPortAccessAllowed)
		{
		TInt ret;
		TCommConfig cbuf;
		TCommConfigV01 &cfg = cbuf();
		
		// Get the Configuration from current serial com port
		iCommPort.Config(cbuf);
	    
		// set the configuration according to passed config data
		TCommConfigV01 &newCfg = aConfiguration();
		cfg.iRate = newCfg.iRate;
		cfg.iDataBits = newCfg.iDataBits;
		cfg.iStopBits = newCfg.iStopBits;
		cfg.iParity = newCfg.iParity;
		cfg.iHandshake = newCfg.iHandshake;
		
		// Set the Configuration
		ret = iCommPort.SetConfig(cbuf);
		if(ret != KErrNone)
			{
			LOGTEXT2(_L8("[Ltsy] CCommEngine:\tError %d configuring port"),ret);
			return ret;
			}
		}
	return KErrNone;	
	}

// ---------------------------------------------------------------------------
// CCommEngine::Disconnect
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::Disconnect()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::Disconnect()"));
	
	if (iPortAccess == EPortAccessAllowed)
		{
		TCommConfig cbuf;
		TCommConfigV01 &cfg = cbuf();
		iCommPort.Config(cbuf);
		cfg.iHandshake = KConfigFreeRTS	| KConfigFreeDTR;
		TInt ret = iCommPort.SetConfig(cbuf);
		if (ret == KErrNone)
			{
			iCommPort.SetSignalsToSpace(KSignalRTS | KSignalDTR);
			}
		}
	
	CommClose();
	
	iPortAccess = EPortAccessDenied;	
	}

// ---------------------------------------------------------------------------
// CCommEngine::Read
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::Read()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::Read()"));
	iCommCancel = EFalse;
	if (iPortAccess == EPortAccessAllowed)
		{
		CommReadOneOrMore();
		}
	else
		{
		LOGTEXT(_L8("[Ltsy] Port Not Allow Access"));
		}	
	}

// ---------------------------------------------------------------------------
// CCommEngine::DropDtr
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::DropDtr()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::DropDtr()"));
	
	if (iPortAccess == EPortAccessAllowed)
		{
		LOGTEXT(_L8("[Ltsy] CCommEngine::DropDtr Dropping DTR"));
		
		iCommPort.SetSignals(0,KSignalDTR);
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::RaiseDTR
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::RaiseDTR()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::RaiseDTR()"));
	
	if (iPortAccess == EPortAccessAllowed)
		{
		LOGTEXT(_L8("[Ltsy] CCommEngine::RaiseDTR Raising DTR"));
		
		iCommPort.SetSignals(KSignalDTR,0);
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::DropRTS
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::DropRTS()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::DropRTS()"));
	
	if (iPortAccess == EPortAccessAllowed)
		{
		LOGTEXT(_L8("[Ltsy] Dropping RTS"));
		
		iCommPort.SetSignals(0,KSignalRTS);
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::RaiseRTS
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::RaiseRTS()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::RaiseRTS()"));
	
	if (iPortAccess == EPortAccessAllowed)
		{
		LOGTEXT(_L8("[Ltsy] Raising RTS"));
		
		iCommPort.SetSignals(KSignalRTS,0);
		}
	}
// ---------------------------------------------------------------------------
// CCommEngine::ResetBuffers
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::ResetBuffers()
	{
	iCommPort.ResetBuffers();
	}
// ---------------------------------------------------------------------------
// CCommEngine::Signals
// other items were commented in a header
// ---------------------------------------------------------------------------
TUint CCommEngine::Signals()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::Signals()"));
	
	return iCommPort.Signals();
	}

// ---------------------------------------------------------------------------
// CCommEngine::ResetReadAndWriteBuffers
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::ResetReadAndWriteBuffers()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::ResetReadAndWriteBuffers()"));
	
	iCommPort.ResetBuffers();
	}

// ---------------------------------------------------------------------------
// CCommEngine::GetSizeOfRxBuffer
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CCommEngine::GetSizeOfRxBuffer()
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::GetSizeOfRxBuffer()"));
	
	return iCommPort.QueryReceiveBuffer();
	}

// ---------------------------------------------------------------------------
// CCommEngine::GetPortShutdownTimeout
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CCommEngine::GetPortShutdownTimeout()
	{
	if (iPortAccess == EPortAccessAllowed)
		{
		TCommConfig2 cbuf;
		TCommConfigV02& cfg = cbuf();
		// Get the Configuration
		iCommPort.Config(cbuf);				

		return (cfg.iTxShutdownTimeout);
		}
	else
		{
		return (0);
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::SetPortShutdownTimeout
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CCommEngine::SetPortShutdownTimeout(TInt aTimeout)
	{
	if (iPortAccess == EPortAccessAllowed)
		{
		TCommConfig2 cbuf;
		TCommConfigV02 &cfg = cbuf();
		
		//Get the Configuration
		iCommPort.Config(cbuf);
	
		//Set the Configuration
		cfg.iTxShutdownTimeout = aTimeout;
		return (iCommPort.SetConfig(cbuf));
		}
	else
		{
		return KErrAccessDenied;
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommOpen
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CCommEngine::CommOpen(const TDesC& aDll, const TDesC& aName, TCommAccess aMode)
	{
	TInt err;
	if (err = iCommServer.Connect(), err!=KErrNone)
		return err;
		
	// load serial comm module
	if (aDll.Length()>0)
		{
		if (err = iCommServer.LoadCommModule(aDll), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	
	// start to open the serial com with ECommShared model
	if (aMode==ECommShared)
		{
		if (err = iCommPort.Open(iCommServer, aName, aMode), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
		
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommClose
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommClose()
	{
	iCommReader->Cancel();
	iCommWriter->Cancel();
	iCommPort.Close();
	iCommServer.Close();
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommWrite
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommWrite(const TDesC8& aDes)
	{
    LOGTEXT(_L8("[Ltsy] Starting CCommEngine::CommWrite()"));
    LOGTEXTREL2(_L8("Tx:\t%S"),&aDes);
    
	iCommPort.Write(iCommWriter->StatusRef(), aDes);
	iCommWriter->Activate();
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommRead
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommRead()
	{
	iCommPort.Read(iCommReader->StatusRef(), iRxBuf, iRxBuf.Length());
	iCommReader->Activate();
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommReadOneOrMore
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommReadOneOrMore()
	{
	if(!iCommCancel)
		{
		iCommPort.ReadOneOrMore(iCommReader->StatusRef(), iRxBuf);
		iCommReader->Activate();
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommCancel
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommCancel()
	{
	iCommCancel = ETrue;
	if(iCallbackTimer->IsActive())
	    {
		iCallbackTimer->Cancel();
		}
		
	if (NULL != iCommWriter)
		{
		iCommWriter->Cancel();
		}
	
	if (NULL != iCommReader)
		{
		iCommReader->Cancel();
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommWriteCancel
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommWriteCancel()
	{
	if(iCallbackTimer->IsActive())
	    {
		iCallbackTimer->Cancel();
		}
		
	if (NULL != iCommWriter)
		{
		iCommWriter->Cancel();
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommReadCancel
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommReadCancel()
	{
	if(iCallbackTimer->IsActive())
	    {
		iCallbackTimer->Cancel();
		}
		
	if (NULL != iCommReader)
		{
		iCommReader->Cancel();
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommReadComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommReadComplete(TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::CommReadComplete()"));
	LOGTEXT2(_L8("[Ltsy] aStatus = %d"), aStatus);
	LOGTEXT2(_L8("Received %d"),iRxBuf.Length());
	LOGTEXTREL2(_L8("Rx:\t%S"),&iRxBuf);
		
	if (KErrNone == aStatus)
		{
		if (NULL != iStringParse)
			{
			iStringParse->ProcessReadString(iRxBuf);
			}
			
		CommReadOneOrMore();	
		}
	else
		{
		if (NULL != iEngineObserver)
			{
			iEngineObserver->ReadOrWriteComplete(EReadCompletion, aStatus);
			}
		}
	LOGTEXT(_L8("[Ltsy] End CCommEngine::CommReadComplete()"));
	}

// ---------------------------------------------------------------------------
// CCommEngine::CommWriteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::CommWriteComplete(TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy] Starting CCommEngine::CommWriteComplete()"));
	LOGTEXT2(_L8("[Ltsy] aStatus = %d"), aStatus);
	if(iCallbackTimer->IsActive())
	    {
		iCallbackTimer->Cancel();
		}
		
	if (NULL != iEngineObserver)
		{
		iEngineObserver->ReadOrWriteComplete(EWriteCompletion, aStatus);
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::GetStringParse
// other items were commented in a header
// ---------------------------------------------------------------------------
CStringParser* CCommEngine::GetStringParse() const
	{
	return iStringParse;
	}

// ---------------------------------------------------------------------------
// CCommEngine::SetCommReadLineNotify
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::SetCommReadLineNotify(MCommReadLineNotifier *aReadLineNotify)
	{
	iStringParse->SetReadLineNotify(aReadLineNotify);
	}

// ---------------------------------------------------------------------------
// CCommEngine::SetCommEngineObserver
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::SetCommEngineObserver(MCommEngineObserver *aEngineObserver)
	{
	iEngineObserver = aEngineObserver;
	}

// ---------------------------------------------------------------------------
// CCommEngine::TimerRun
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::TimerRun(TInt aError)
    {
	if(aError == KErrNone)
	    {
		iEngineObserver->ReadOrWriteComplete(ETimeoutCompletion, KErrTimedOut);
		}
	}

// ---------------------------------------------------------------------------
// CCommEngine::StartTimer
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::StartTimer(TInt aSeconds)
    {
    if (iCallbackTimer->IsActive())
    	{
    	iCallbackTimer->Cancel();
    	}
	iCallbackTimer->After(aSeconds*KLtsyOneSecondPause);
	}

// ---------------------------------------------------------------------------
// CCommEngine::StopTimer
// other items were commented in a header
// ---------------------------------------------------------------------------
void CCommEngine::StopTimer()	
    {
	iCallbackTimer->Cancel();
	}

// End of file
