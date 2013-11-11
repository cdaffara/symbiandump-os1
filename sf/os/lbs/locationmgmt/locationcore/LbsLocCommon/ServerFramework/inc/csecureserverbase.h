// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// csesureserverbase.h
// Definition of class used to encapsulate a LBS server 
// (for example,  LbsLocServer.exe)
// 
//

/**
 @file
 @internalTechnology
*/


#ifndef __LBSSECURESERVERBASE_H__
#define __LBSSECURESERVERBASE_H__

#include <e32base.h>

// forward classes
class CShutdownTimer;

/** default server shutdown delay */
const TTimeIntervalMicroSeconds32 KDefaultShutdownDelay = 2*1000*1000;

// NB this and the panic list should be pblisghed all + released...
/** Liternal used as the panic catageory by the server framework */
_LIT(KServerFrameworkPanic, "ServerFramework");
enum TServerFrameWorkPanics
		{
		ESecureSubSessionBaseUnknownSubSessionType = 1 // the sever does not reognise the session type.
		};

/** 
@internalTechnology
@released
@see CShutdownTimer
@see CPolicyServer

CPolicy based class to provde security functionalities to the server base
 */
class CSecureServerBase : public CPolicyServer
	{
public:
	/** Secure server base panic type */
	enum ESecureServerBasePanic 
		{
		EBaseConstructNotCalled = 0,
		EBaseCreateNotCalled = 1
		};
public:
	// creation - called from CPolicyServer
	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	// secondary creation - called by the implementaion of NewSessionL above
	virtual CSession2* DoNewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const = 0;
	virtual TVersion GetServerVersion() const = 0;
	// destruction
	virtual ~CSecureServerBase();
public:
	void IncSession();
	void DecSession();

	TInt GetSessionCount() const;
	TTimeIntervalMicroSeconds32 GetShutdownDelay() const;
	void SetShutdownDelay(const TTimeIntervalMicroSeconds32 aDelay);
	// must be public to the sessions can so this is required.
	void PanicServer(TInt aPanicCode) const;
protected:
	CSecureServerBase(TInt aPriority, const TPolicy& aSecurityPolicy);
	void BaseConstructL(TBool aShutdownTimerRequired = ETrue);
protected:
	// property
	// protected in case a derived class ever has to overload
	/** number of sessions to the server */
	TInt						iSessionCount;
	/** shut down timer for the server */
	CShutdownTimer*				iShutdownTimer;
	/** shut down time delay */
	TTimeIntervalMicroSeconds32 iShutdownDelay;
private:
	// property - really do mean private here
	/** indicate whether the base construct has been called */
	TBool iBaseConstructCalled;
	};


#endif  // __LBSSECURESERVERBASE_H__
