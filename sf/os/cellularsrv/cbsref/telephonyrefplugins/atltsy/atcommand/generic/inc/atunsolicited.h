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
// @file atunsolicited.h
// 

#ifndef ATUNSOLICITED_H_
#define ATUNSOLICITED_H_

//system include

//user include
#include "atcommandbase.h"
#include "unsolicitedparams.h"

class CATUnsolicited : public CAtCommandBase
	{
public:
	/**
	 * 
	 * 
	 */
	static CATUnsolicited* NewL(CGlobalPhonemanager& aGloblePhone,
			                    CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * 
	 * 
	 */
	static CATUnsolicited* NewLC(CGlobalPhonemanager& aGloblePhone,
			                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
			                     
	
	/**
	 * 
	 * 
	 */
	virtual ~CATUnsolicited();
	
	/**
	 * 
	 * 
	 * 
	 */
	virtual void ExecuteCommand(){};
	
	/**
	 * 
	 * 
	 */
	virtual void CancelCommand(){};
	
	/**
	 * 
	 * 
	 */
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	
	/**
	 * 
	 * 
	 */
	TInt GetParserResult() const;	
	
	/**
	 * 
	 * 
	 */
	const TDesC8& GetUnsolicitedParams() const;
protected:
	/**
	 * 
	 * 
	 */
	CATUnsolicited(CGlobalPhonemanager& aGloblePhone,
				   CCtsyDispatcherCallback& aCtsyDispatcherCallback);	
	
	/**
	 * 
	 * 
	 */
	void ConstructL();
	
private:
	/**
	 * 
	 * 
	 */
	TUnsolicitedParams::TUnsolicitedParamsPckgBuf iPckgBuf;
	
	/**
	 * 
	 * 
	 * 
	 */
	TInt iResult;
	};

#endif /*ATUNSOLICITED_H_*/
