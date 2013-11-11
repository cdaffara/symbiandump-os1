// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include <e32std.h>

#include "ctlbstestpsy2requesthandler.h"
  
CT_LbsTestPsy2RequestHandler* CT_LbsTestPsy2RequestHandler::InstanceL()
    {
    CT_LbsTestPsy2RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsTestPsy2RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsTestPsy2RequestHandler;
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(); // self
		Dll::SetTls(reinterpret_cast<TAny*>(self));
        }

	self->iReferenceCounter++;
	return self;
    }

// ---------------------------------------------------------
// ::Release
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy2RequestHandler::Release()
	{
	if (--iReferenceCounter == 0)
		{
        Dll::SetTls(NULL);
		delete this;
		}
	}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy2RequestHandler::CT_LbsTestPsy2RequestHandler() :
    CT_LbsTestPsyBase(NULL)
    {

	}

// Destructor
CT_LbsTestPsy2RequestHandler::~CT_LbsTestPsy2RequestHandler()
    {
    
    for(TInt i=0; i<iThreads.Count(); i++)
       {
       iThreads[i].Terminate(0);
       iThreads[i].Kill(0);
       iThreads[i].Close();
       }
    iThreads.Close();
    iThreads.Reset();

	}

LOCAL_C TInt ThreadFunction(TAny* /*aData*/)
    {
    return 0;
    }

void CT_LbsTestPsy2RequestHandler::NotifyPositionUpdate(TPositionInfo* aPosInfo, TRequestStatus* aStatus)
    {
     _LIT(KThreadName, "Thread %d");

    if (++iRequests == 1)
        {
        RThread thread;
	    TBuf<32> name;
	    name.Format(KThreadName, 1+iThreads.Count());

        TInt err;
        err = thread.Create(name, ThreadFunction, KDefaultStackSize, KMinHeapSize, KMinHeapSize, NULL);
        
        if(err == KErrNone)
        {
        	iThreads.Append(thread);	
        }
        
        }
    
    CT_LbsTestPsyBase::NotifyPositionUpdate(aPosInfo, aStatus);
    }

