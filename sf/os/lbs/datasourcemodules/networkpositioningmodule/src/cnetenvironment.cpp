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
 @InternalComponent
*/

#include "cnetenvironment.h"
#include "cpositionerq.h"
#include "cnetrequesthandler.h"
#include "cnetresponsehandler.h"
#include "cnetmodstatushandler.h"
#include "psylogging.h"
#include "netpsy.hrh"

/**
 * Destructor
 */
CNETEnvironment::~CNETEnvironment()
	{
    Dll::SetTls(NULL);
    
    delete iPositionerQ;
    delete iRequestHandler;
    delete iResponseHandler;
	delete iModStatusHandler;
	}

/**
 * Constructor
 */
CNETEnvironment::CNETEnvironment()
	{
	}

/**
Close the environment
*/
void CNETEnvironment::CloseInstance()
	{
	CObject::Close();
	}

/**
 * CNETEnvironment::RegisterPSYL
 * 
 * @param aPSY, the MPOsitioner instance to add to the queue.
 */
void CNETEnvironment::RegisterPSYL(MPositioner* aPSY)
	{
	if(iPositionerQ)
		{
		iPositionerQ->RegisterPSYL(aPSY);
		}
	}

/**
 * CNETEnvironment::UnregisterPSYL
 * 
 * @param aPSY, the MPositioner instance to remove from the queue.
 */
void CNETEnvironment::UnregisterPSY(MPositioner* aPSY)
	{
	if(iPositionerQ)
		{
		iPositionerQ->UnregisterPSY(aPSY);
		}
	}


/**
 * CPositionerQ::NewL Two-phased constructor.
 * Creates a new instance of a CNETEnvironment implementation.
 * 
 * @return the CPositionerEnvironment implementation.
 */
CPositionerEnvironment* CNETEnvironment::NewL()
    {
	// Have we loaded the environment already
    if ( !Dll::Tls() ) 
        {
        CNETEnvironment* self = new( ELeave ) CNETEnvironment();
        
        CleanupClosePushL(*self);
        self->ConstructL();
        CleanupStack::Pop();

        Dll::SetTls(self);
		}
	else
		{
		CPositionerEnvironment * theEnv = reinterpret_cast<CPositionerEnvironment*>(Dll::Tls());
		theEnv->Open();
		}
 
    return reinterpret_cast<CPositionerEnvironment*>(Dll::Tls());
    }

void CNETEnvironment::ConstructL()
    {
    TRACESTRING("CNETEnvironment::ConstructL start");
    
    
	// Do not change order of construction!
    CPositionerEnvironment::ConstructL();
	iPositionerQ = CPositionerQ::NewL();
	iRequestHandler = CNETRequestHandler::NewL(iPositionerQ);
	iResponseHandler = CNETResponseHandler::NewL(iPositionerQ, iRequestHandler);
	iModStatusHandler = CNETModStatusHandler::NewL(iPositionerQ);	

    TRACESTRING("CNETEnvironment::ConstructL end");
    }

/**
 * Returns the CNETRequestHandler instance
 *
 * @return the CNETRequestHandler
 */
CRequestHandler* CNETEnvironment::GetRequestHandler()
	{
    return iRequestHandler;
	}
	
/**
 * Returns the CNETResponseHandler instance.
 *
 * @return the CNETResponseHandler
 */
CResponseHandler* CNETEnvironment::GetResponseHandler()
	{
    return iResponseHandler;
	}

void CNETEnvironment::GetEnvCriteria(TPositionCriteria& aCriteria)
	{
	aCriteria = iCriteria;
	}


TPositionCriteria& CNETEnvironment::GetEnvCriteria()
	{
	return iCriteria;
	}
