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
// This file defines the interface to the LBS Assistance Data Source Module. 
// The interface is implemented by a SUPL protocol test module.
// 
//

/**
 @file
 @publishedPartner
 @test
 @released
*/
#include <e32base.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbsposition.h>
#include <lbs/lbsassistancedatabase.h>

#ifndef LBSASSISTANCEDATASOURCEMODULEBASE_H_
#define LBSASSISTANCEDATASOURCEMODULEBASE_H_

const TUid KAssistanceDataSourceModuleUid = {0x101F9421};
/**
MLbsAssistanceDataSourceObserver used to inform the Network Simulator
about completion of a request for Assistance Data.

@see CLbsSuplTestMoudule
*/
class MLbsAssistanceDataSourceObserver
	{
public:
	/** Call-back used to inform the Network Simulator
	about completion of a request to obtain Assistance Data using the SUPL test module.
	@param aError Standard Symbian OS error code
	 */
	virtual void DataSourceNotification(const TInt& aError) = 0;
	};


class TAssistanceDataSourceModuleConstructionParams
/**
This class is defines the construction parameters required by the 
CAssistanceDataSourceModuleBase ECom interface.
@see        CAssistanceDataSourceModuleBase
*/
	{
	 public:

     inline TAssistanceDataSourceModuleConstructionParams(MLbsAssistanceDataSourceObserver& aAssistanceDataSourceObserver);    
 
	 public:
 
     /** Reference to an assistance data source observer
     */
     MLbsAssistanceDataSourceObserver& iAssistanceDataSourceObserver; 
     };

/**
CAssistanceDataSourceBase This class is used in partnership with MLbsAssistanceDataSourceObserver
and CAssistanceDataSourceModuleBase and it represents a session with a remote server (i.e. a
SUPL session).

An instance of a specialization of this class is obtained by the Network Simulator by means
of interface CAssistanceDataSourceModuleBase.

The concrete subclass that gets instantiated will be determined by the implementation of interface
CAssistanceDataSourceModuleBase that the Network Simulator is using (ie SUPL protocol), which is
an ECom plug-in.

The Network Simulator will use this interface to request retrieval of Assistance Data or to cancel
a previous request.

@see CLbsSuplTestMoudule
*/
class CAssistanceDataSourceBase
	{
public:
	/** Call-back used to inform the Network Simulator
	about completion of a request to obtain Assistance Data using the SUPL test module.
	@param aError Standard Symbian OS error code
	 */
	virtual void GetAssistanceData(RLbsAssistanceDataBuilderSet& aBuilderSet, const TLbsAsistanceDataGroup& aAssistanceDataFilter, const TCoordinate& aPosition) = 0;
	virtual void CancelGetAssistanceData() = 0;
	inline virtual ~CAssistanceDataSourceBase ();
	};
	
/**
The CAssistanceDataSourceModuleBase is the abstract interface to a 
assistance data source module (ex: SUPL). It is used by the 
LBS' Network Simulator to gather GPS Assistance Data from
a remote network server. 

The CAssistanceDataSourceModuleBase class is used in partnership with
MLbsAssistanceDataSourceObserver and CAssistanceDataSourceBase. The module
instantiates a CAssistanceDataSourceBase (i.e. a SUPL protocol session)
to gather the data and that object in turn responds to requests from the
Network Simulator via the observer class. 

An actual implementation of an Assistance Data Source module will derive from
CAssistanceDataSourceModuleBase to provide the specific implementation of the interface methods. 

@see MLbsAssistanceDataSourceObserver 
*/

class CAssistanceDataSourceModuleBase : public CBase
	{
public:

	// Instantiate SUPL protocol test module ECOM plug-in
	static CAssistanceDataSourceModuleBase* NewL(MLbsAssistanceDataSourceObserver& aObserver);

	// wraps ECom object instantiation of a implementation UID
	static CAssistanceDataSourceModuleBase* NewL(TUid aImplementationUid, MLbsAssistanceDataSourceObserver& aObserver);

	// wraps ECom object destruction
	~CAssistanceDataSourceModuleBase();
	
	inline CAssistanceDataSourceModuleBase();
	
    //static void ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray);
    
    //Get a new session to obtain assistance data from
	virtual CAssistanceDataSourceBase* NewDataSourceL() = 0;
	
private:
	// Instance identifier key
    TUid iDtor_ID_Key;
	};


#include <lbs/lbsassistancedatasourcemodule.inl>

#endif // LBSASSISTANCEDATASOURCEMODULEBASE_H_
