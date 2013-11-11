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

#include "cagpsenvironment.h"
#include "cpositionerq.h"
#include "cagpsrequesthandler.h"
#include "cagpsresponsehandler.h"
#include "cagpsmodstatushandler.h"
#include "lbsqualityprofile.h"
#include "psylogging.h"
#include "agpspsy.hrh"

/**
 * Destructor
 */
CAgpsEnvironment::~CAgpsEnvironment()
	{
    Dll::SetTls(NULL);
    delete iRequestHandler;
    delete iResponseHandler;
    delete iModStatusHandler;
    delete iPositionerQ; //Should only be deleted after iRequestHandler
	}


/**
 * Constructor
 */
CAgpsEnvironment::CAgpsEnvironment()
	{
	}

/**
Close the environment
*/
void CAgpsEnvironment::CloseInstance()
	{
	CObject::Close();
	}
	
/**
 * CAgpsEnvironment::RegisterPSYL
 * 
 * @param aPSY, the MPOsitioner instance to add to the queue.
 */
void CAgpsEnvironment::RegisterPSYL(MPositioner* aPSY)
	{
	if(iPositionerQ)
		{
		iPositionerQ->RegisterPSYL(aPSY);
		}
	}

/**
 * CAgpsEnvironment::UnregisterPSYL
 * 
 * @param aPSY, the MPositioner instance to remove from the queue.
 */
void CAgpsEnvironment::UnregisterPSY(MPositioner* aPSY)
	{
	if(iPositionerQ)
		{
		iPositionerQ->UnregisterPSY(aPSY);
		}
	}


/**
 * CPositionerQ::NewL Two-phased constructor.
 * Creates a new instance of a CAgpsEnvironment implementation.
 * 
 * @return the CPositionerEnvironment implementation.
 */
CPositionerEnvironment* CAgpsEnvironment::NewL()
    {
	// Have we loaded the environment already
    if ( !Dll::Tls() ) 
        {
        CAgpsEnvironment* self = new( ELeave ) CAgpsEnvironment();
        
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

void CAgpsEnvironment::ConstructL()
    {
    TRACESTRING("CAgpsEnvironment::ConstructL start");
    
	// Do not change order of construction!
    CPositionerEnvironment::ConstructL();
	iPositionerQ = CPositionerQ::NewL();
	iRequestHandler = CAgpsRequestHandler::NewL(iPositionerQ);
	iResponseHandler = CAgpsResponseHandler::NewL(iPositionerQ, iRequestHandler);
	iModStatusHandler = CAgpsModStatusHandler::NewL(iPositionerQ);	
	InitFinalNetPositionLagFromAdminProfile();
	
    TRACESTRING("CAgpsEnvironment::ConstructL end");
    }

/**
 * Returns the CAgpsRequestHandler instance
 *
 * @return the CAgpsRequestHandler
 */
CRequestHandler* CAgpsEnvironment::GetRequestHandler()
	{
    return iRequestHandler;
	}
	
/**
 * Returns the CAgpsResponseHandler instance.
 *
 * @return the CAgpsResponseHandler
 */
CResponseHandler* CAgpsEnvironment::GetResponseHandler()
	{
    return iResponseHandler;
	}

/**
 * Returns the final network position lag value
 * 
 * @return iFinalNetPositionLag
 */
TTimeIntervalMicroSeconds& CAgpsEnvironment::GetFinalNetPositionLag()
	{
	return iFinalNetPositionLag;
	}

/**
 * Called from ConstructL
 * to populate iFinalNetPositionLag
 **/
void CAgpsEnvironment::InitFinalNetPositionLagFromAdminProfile()
	{

	// Get the profile Id for self locate requests
	TLbsQualityProfileId profileId;
	TInt err = Admin().Get(KLbsSettingQualityProfileSelfLocate, profileId);

	if (err == KErrNone)
		{
		// Retrieve the quality profile that belongs to the given Id
		TQualityProfile quality;
		err = LbsQualityProfile::GetQualityProfileById(profileId, quality);
		if (err == KErrNone)
			{
			iFinalNetPositionLag = quality.FinalNetPositionLag();
			}
		else
			{
			iFinalNetPositionLag = 0;
			}
		}
	else
		{
		iFinalNetPositionLag = 0;
		}
	}

