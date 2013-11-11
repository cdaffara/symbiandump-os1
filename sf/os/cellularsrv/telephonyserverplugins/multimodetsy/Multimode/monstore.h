// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MM own number store access Header file.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __MONSTORE_H__
#define __MONSTORE_H__

#include <etelmm.h>
#include "ATSTD.H"
#include "Mownnum.h"


class CATIO;
class CATInit;
class CPhoneGlobals;


/**
 * \class CMobileONStore MONSTORE.H "MMTSY/MULTIMODE/MONSTORE.H"
 * \brief The CMobileONStore class is one of the MULTIMODE TSY's sub-session extensions responsible
 * for handling Client-side calls from the RMobilePhoneONStore class in the Multimode API.
 *
 * CMobileONStore inherits from the CSubSessionExtBase class defined in et_phone.h (which in turn
 * inherits from various mixin classes (MSubSessionExtBaseTSY, MTelObjectTSY). This sub-session
 * extension class implements all the pure virtuals (functions) from the inherited classes
 * including ExtFunc(), CancelService(), ReqModeL() etc.
 */
class CMobileONStore : public CSubSessionExtBase
	{
public:

	static CMobileONStore* NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aMMStatus);						   
	~CMobileONStore();
//
// CTelObject, MTelObject pure virtuals
//
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);		
	virtual TInt RegisterNotification(const TInt aIpc);	
	virtual TInt DeregisterNotification(const TInt aIpc);	
	virtual void Init();								
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aName);
//
// CSubSessionExtBase pure virtuals
//
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,	const TInt aIpc, const TDataPackage& aPackage);


private:
	CMobileONStore(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aMMStatus);
	TInt GetOwnNumberInfo(TTsyReqHandle aTsyReqHandle,TDes8* aInfo);
	TInt Read(TTsyReqHandle aTsyReqHandle,TDes8* aEntry);
	TInt ReadCancel(const TTsyReqHandle aTsyReqHandle);
private:

	void ConstructL();

	CATIO* iIo;
	CATInit* iInit;
	CATOwnNumbers* iATOwnNumbers;
	CPhoneGlobals* iPhoneGlobals;
	};

#endif
