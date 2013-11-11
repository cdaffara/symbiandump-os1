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
// CCISVAPIBase.h
// 
//


#ifndef __CISVAPIBASE_H__
#define __CISVAPIBASE_H__

#include <e32base.h>
#include <etel3rdparty.h>

/**
Enumeration allocating numbers to different types of classes derived from 
CISVBase.
*/
enum TTelISVExampleType
	{
	KNotType = -1,
	KPhoneId,
	KSubscriberId,
	KBatteryInfo,
	KSignalInfo,
	KGetIndicator,
	KLockInfo,
	KNetworkInfo,
	KNetworkName,
	KOperatorName,
	KCallInfo,
	KLineStatus,
	KCallStatus,
	KFlightModeInfo,
	KNetworkRegInfo,
	KAnswerIncomingCall,
	KDialCall,
	KDynamicCaps,
	KHold,
	KResume,
	KSwap,
	KSendDTMF,
	KHangup,
	KCallWaitingStatus,
	KCallForwardingStatus,
	KCallBarringStatus,
	KIdentityServiceStatus
	};

/**
Enumerations for the potential states of a CMainMenu class.
*/
enum TState
	{
	EStart,
	EEnd,
	EDummyAnswer,

	EGetPhoneId,
	EGetSubscriberId,
	EGetCallInfo,
	EGetBatteryInfo,
	EGetIndicator,
	EGetLockInfo,
	EGetSignalInfo,
	EGetNetworkInfo,
	EGetNetworkName,
	EGetOperatorName,

	EGetFlightModeInfo,
	EGetNetworkRegStatus,

	EGetLineStatus,
	EGetCallStatus,
	EAnswerIncomingCall,
	EDialCall,
	EGetDynamicCaps,
	EHold,
	EResume,
	ESwap,
	ESendDTMF,
	EHangup,

	ECallForwarding,
	ECallWaiting,
	ECallBarring,
	EIdentityService,

	EWaitingForKeyPress,

	ESetNotifier
	};

/**
Base interface providing functionality for a member active object to interact
with its owning class.
*/
class MExecController
	{
public:
	/**
	End execution of the program, stop the active scheduler making sure all 
	outstanding requests have been cancelled.
	*/
	virtual void Terminate() = 0;

	/**
	Returns a pointer to the CTelephony Instance so member objects can make
	calls to the Etel 3rd Party API through the same object.
	
	@return CTelephony pointer
	*/
	virtual CTelephony* GetTelObj() const = 0;

	/**
	Returns a pointer to the console instance so
	member objects can print to the console as required.
	
	@return CConsoleBase pointer
	*/
	virtual CConsoleBase* GConsole() const = 0;

	};

/**
Base class providing functionality for objects to make Etel 3rd party function
calls and interact with their owning class.
*/
class CISVAPIBase : public CActive
	{

public:
	CISVAPIBase(MExecController* aController,
			    TTelISVExampleType aExampleType);

	void StartRequestL();
	void StartRequestL(CTelephony::TCallId aCallId);
	void StartRequestL(CTelephony::TCallId aCallId1,
	                   CTelephony::TCallId aCallId2);
	void StartRequestL(const TDesC& aTones);
	void RequestNotificationL();
	void AppTerminate();

	inline void RetrieveTelephonyObject();
	inline void RetrieveConsole();

	inline TTelISVExampleType GetExampleType() const;

protected:
	virtual void DoStartRequestL();
	virtual void DoStartRequestL(CTelephony::TCallId aCallId);
	virtual void DoStartRequestL(CTelephony::TCallId aCallId1,
	                             CTelephony::TCallId aCallId2);
	virtual void DoStartRequestL(const TDesC& aNumber);
	virtual void DoRequestNotificationL();

protected:
	/**
	An instance of the MExecController interface allowing objects derived from
	this class to make calls back to a class implementing this interface. (In 
	this case CMainMenu).
	*/
	MExecController* iController;
	/**
	A CTelephony instance allowing derived classes to make Etel 3rd party
	calls.
	*/
	CTelephony* iTelephony;
	/**
	A CConsoleBase instance allowing derived classes to peform console 
	operations such as printing.
	*/
	CConsoleBase* iConsole;

private:
	/**
	Specifies the type of the derived class that has extended the functionality
	of this class.
	*/
	const TTelISVExampleType iExampleType;

	};

/**
Gives the type of the derived class.

@return TTelISVExampleType enumeration value stating the derived type of this
                           object
*/
inline TTelISVExampleType CISVAPIBase::GetExampleType() const
	{
	return iExampleType;
	};

/**
Retrieves a CTelephony instance from the MExecController implementing class by
calling the GetTelObj() method of the interface. Sets iTelephony to point to this
instance. Etel 3rd party calls can be made through this pointer.
*/
inline void CISVAPIBase::RetrieveTelephonyObject()
	{
	iTelephony = iController->GetTelObj();
	}

/**
Retrieves a CConsoleBase instance from the MExecController implementing class by
calling GConsole() method of the interface. Sets iConsole to point to this
instance. A console can now be printed to through this pointer.
*/
inline void CISVAPIBase::RetrieveConsole()
	{
	iConsole = iController->GConsole();
	}


#endif // __CISVAPIBASE_H__
