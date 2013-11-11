// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the factory class which is used to instantiate the RAW IP.
// 
//

/**
 @file
*/

#ifndef RAWIPFLOWFACTORY_H__
#define RAWIPFLOWFACTORY_H__

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_protflow.h>


/**
RawIP Flow Implementation UID
*/
const TInt KRawIpFlowImplUid = 0x10281DF2;


class CRawIPFlowFactory : public ESock::CSubConnectionFlowFactoryBase
/**
RawIP Flow Factory
@internalComponent
*/
	{
public:
	static CRawIPFlowFactory* NewL(TAny* aConstructionParameters);

protected:
	CRawIPFlowFactory(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer);
	virtual ESock::CSubConnectionFlowBase* DoCreateFlowL(ESock::CProtocolIntfBase* aProtocol, ESock::TFactoryQueryBase& aQuery);

};

#endif
