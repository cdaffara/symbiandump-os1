// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Conditional includes
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __SMSPPARA_H__
#define __SMSPPARA_H__

#include "smspcomm.h"
#include <etelmm.h>
#include "smsuaddr.H"
#include "smsuact.h"

//
// Forward class declarations
//

class TSmsSettings;
class RFs;
class CRetrieveMobilePhoneSmspList;

//
// Class declarations
//


/**
 *  Base class for CSmsReadParams and CSmsWriteParams classes.
 *  @internalComponent
 */
class CSmsParamsBase : public CSmsuActiveBase
	{
protected:
	CSmsParamsBase(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging);
	virtual void ConstructL();
protected:
	MSmsComm& iSmsComm; 										//< Reference to notification interface
	const TSmsSettings& iSmsSettings;							//< Reference to object that contains timeout settings
	RMobileSmsMessaging& iSmsMessaging; 						//< Reference to ETEL SMS messaging subsession
	MSmsMessageObserver* iSmsMessageObserver;					//< Pointer to the read parameters observer
	CMobilePhoneSmspList* iMobilePhoneSmspList; 				//< Pointer to the CMobilePhoneSmspList object
	};


/**
 *  Used by CSmsProtocol and CSmsWriteParams to read SMS parameters from phone memory.
 *  @internalComponent
 */
class CSmsReadParams : public CSmsParamsBase
	{
public:
	static CSmsReadParams* NewL(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging);
	~CSmsReadParams();
	void Start(MSmsMessageObserver& aObserver);
	void Start(TRequestStatus& aStatus);
	void CheckListValidityL(CMobilePhoneSmspList& aMobilePhoneSmspList);
	void StoreParameters(CMobilePhoneSmspList* aMobilePhoneSmspList);

private:
	void DoRunL();
	void DoCancel();
	CSmsReadParams(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging);
	void ConstructL();
	void Complete(TInt aStatus);

	CRetrieveMobilePhoneSmspList* iRetrieveMobilePhoneSmspList; //< Pointer to object for retrieving SMS parameters
	};


/**
 *  Used by CSmsProtocol to write SMS parameters to phone memory.
 *  @internalComponent
 */
class CSmsWriteParams : public CSmsParamsBase
	{
public:
	static CSmsWriteParams* NewL(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging,CSmsReadParams& aSmsReadParams);
	~CSmsWriteParams();
	void Start(MSmsMessageObserver& aObserver,CMobilePhoneSmspList* aMobilePhoneSmspList);
protected:
	void DoRunL();
	void DoCancel();
private:
	CSmsWriteParams(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging,CSmsReadParams& aSmsReadParams);
	void ConstructL();
	void Complete(TInt aStatus);
private:
	enum TSmsWriteParamsState
		{
		ESmsParamsIdle,
		ESmsParamsStoringList,
		ESmsParamsStoringListToInternal
		};
private:
	TSmsWriteParamsState iState;			//< Data member indicating current state of the WriteParams
	CSmsReadParams& iSmsReadParams; 		//< Reference to object for reading parameters from phone memory
	};

#endif
