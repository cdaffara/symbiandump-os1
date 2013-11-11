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
//

#include <ecom/ecom.h>

inline
TAssistanceDataSourceModuleConstructionParams::TAssistanceDataSourceModuleConstructionParams(MLbsAssistanceDataSourceObserver& aAssistanceDataSourceObserver)
 :iAssistanceDataSourceObserver(aAssistanceDataSourceObserver)
 /**
     Constructor.
     @param      aAssistanceDataSourceObserver    Reference to an assistance
     											data source observer.
 */
     {
     }


inline CAssistanceDataSourceModuleBase::CAssistanceDataSourceModuleBase()
	{
	}

inline CAssistanceDataSourceModuleBase::~CAssistanceDataSourceModuleBase()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

// Default. Instantiates Supl module
inline CAssistanceDataSourceModuleBase* CAssistanceDataSourceModuleBase::NewL(MLbsAssistanceDataSourceObserver& aObserver)
	{
    // By default, load the SUPL implementation
	// Set up some empty initialisation parameters
	TAssistanceDataSourceModuleConstructionParams initParams(aObserver);
	
	const TUid KDefaultSuplModuleUid = {0x1028225B};//SUPL's implementation UID
	return REINTERPRET_CAST(CAssistanceDataSourceModuleBase*, 
							REComSession::CreateImplementationL(KDefaultSuplModuleUid, 
															   _FOFF(CAssistanceDataSourceModuleBase,iDtor_ID_Key),
															   REINTERPRET_CAST(TAny*,&initParams)));
	}

inline CAssistanceDataSourceModuleBase* CAssistanceDataSourceModuleBase::NewL(TUid aImplementationUid, MLbsAssistanceDataSourceObserver& aObserver)
	{
	TAssistanceDataSourceModuleConstructionParams initParams(aObserver);
	
	return REINTERPRET_CAST(CAssistanceDataSourceModuleBase*, 
							REComSession::CreateImplementationL(aImplementationUid, 
															   _FOFF(CAssistanceDataSourceModuleBase,iDtor_ID_Key),
															   REINTERPRET_CAST(TAny*,&initParams)));
	}

inline CAssistanceDataSourceBase::~CAssistanceDataSourceBase()
{
}