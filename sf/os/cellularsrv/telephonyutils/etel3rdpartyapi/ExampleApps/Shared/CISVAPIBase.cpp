// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CISVAPIBase.h"

/**
Default constructor.

@param aController  iController is set to point to this object
@param aExampleType The type of the derived class that has extended the 
                    functionality of this class
*/
CISVAPIBase::CISVAPIBase(MExecController* aController, TTelISVExampleType aExampleType)
	: CActive(EPriorityStandard),
	  iController(aController),
	  iExampleType(aExampleType)
	{
	CActiveScheduler::Add(this);
	RetrieveConsole();
	RetrieveTelephonyObject();
	}

/**
Makes a request for notification on changes to specified Etel 3rd party data.
*/
void CISVAPIBase::RequestNotificationL()
	{
	DoRequestNotificationL();
	}

/**
Virtual function for completing the notification request.  Empty method.
*/
void CISVAPIBase::DoRequestNotificationL()
	{
	/*
	TODO Implement CISVAPIBase::DoRequestNotificationL() in any derived
	     classes if required
	*/
	}

/**
Make an Etel 3rd Party request and wait for it to be completed.
*/
void CISVAPIBase::StartRequestL()
	{
	DoStartRequestL();
	}

/**
Virtual function for completing the request to start. Empty method.
*/
void CISVAPIBase::DoStartRequestL()
	{
	/*
	TODO Implement CISVAPIBase::DoStartRequestL() in any derived classes if 
	     required
	*/
	}

/**
As StartRequestL()

@param aCallId A call identifier to perform Etel Third Party operations on
*/
void CISVAPIBase::StartRequestL(CTelephony::TCallId aCallId)
	{
	DoStartRequestL(aCallId);
	}

/**
Virtual function for completing the request to start. Empty method.

@param aCallId A call identifier to perform Etel Third Party operations on
*/
void CISVAPIBase::DoStartRequestL(CTelephony::TCallId /*aCallId*/)
	{
	/*
	TODO Implement CISVAPIBase::DoStartRequestL(CTelephony::TCallId aCallId)
	     in any derived classes if required
	*/
	}

/**
As StartRequestL()

@param aTones A descriptor to perform Etel third party operations with
*/
void CISVAPIBase::StartRequestL(const TDesC& aTones)
	{
	DoStartRequestL(aTones);
	}

/**
Virtual function for completing the request to start. Empty method.

@param aNumber Number to be used
*/
void CISVAPIBase::DoStartRequestL(const TDesC& /*aNumber*/)
	{
	/*
	TODO Implement CISVAPIBase::DoStartRequestL(const TDesC& aNumber) in any 
	     derived classes if required
	*/
	}

/**
As StartRequestL()

@param aCallId1 A call identifier to perform Etel Third Party operations on
@param aCallId2 A call identifier to perform Etel Third Party operations on
*/
void CISVAPIBase::StartRequestL(CTelephony::TCallId aCallId1,
	                               CTelephony::TCallId aCallId2)
	{
	DoStartRequestL(aCallId1, aCallId2);
	}

/**
Virtual function for completing the request to start. Empty method.

@param aCallId1 A call identifier to perform Etel Third Party operations on
@param aCallId2 A call identifier to perform Etel Third Party operations on
*/
void CISVAPIBase::DoStartRequestL(CTelephony::TCallId /*aCallId1*/,
	                              CTelephony::TCallId /*aCallId2*/)
	{
	/*
	TODO Implement CISVAPIBase::DoStartRequestL(CTelephony::TCallId aCallId1, 
	     CTelephony::TCallId aCallId2) in any derived classes if required
	*/
	}

/**
Ends the execution of the application by stopping the active scheduler and 
cancelling any outstanding requests.
*/
void CISVAPIBase::AppTerminate()
	{
	iController->Terminate();
	}

