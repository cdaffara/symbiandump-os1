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
// This is the header file for MultiHoming ResolveName step
// 
//

/**
 @file
*/
 
#if !defined (__TS_RESOLVENAME_H__)
#define __TS_RESOLVENAME_H__

#include "TS_MultiHomingStep.h"
#include "TS_MultiHomingSuite.h"

enum EResolverState
/**
 * Enum for internal state of the resolver
 */
	{	
	 EResReady				//< Resolver ready
	,EWaitingForResult
	,EResComplete			//< Name resolution complete
	};

 
class CTS_ResolveName;

class CResolveName : public CActive
	{
	public:
		
		static CResolveName* NewL();
		virtual ~CResolveName();
		
		// From CActive
		void RunL();
		void DoCancel();
		
		TInt DoResolution(TName aHostName, TConnDetails *aConn, TInt aProtocol, 
			CTS_ResolveName *aOwnerStep, TBool aExplicitResolve);
		TInt GetResult(){return iResolverResult;}
	private:
		CResolveName() : CActive(EPriorityNormal), iOwnerStep(0), iTConnection(0) {} 
		void ConstructL();

	private:
		TNameEntry				iResultingName;
		RHostResolver			iResolver;
		TInt					iResolverResult;
		TName					iHostName;		
		CTS_MultiHomingStep  *iOwnerStep;
		TConnDetails          *iTConnection;  //< The connection that has been started
		EResolverState			iState;			//< Current state of the resolver
		RSocketServ				iResolverSS;	//  Socket server when for implicit resolve.
		TBool					iResolverHasSS;

	};
/******************************************************
 * Resolves a hostname to an ip address
 */
class CTS_ResolveName : public CTS_MultiHomingStep
	{
	public:
		CTS_ResolveName();
		virtual ~CTS_ResolveName();	// final
		virtual enum TVerdict doTestStepL( );

	private:

		enum TVerdict doTestStepPreambleL();
	};


#endif 
		// __TS_RESOLVENAME_H__

