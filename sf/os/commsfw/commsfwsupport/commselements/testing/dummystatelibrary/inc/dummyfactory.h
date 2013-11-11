// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DUMMYFACTORY_H__
#define	__DUMMYFACTORY_H__

#include <elements/factory.h>
#include <dummynode.h>

namespace Dummy
{
class CTestFactoryBase;

//
//CTestFactoryContainer
NONSHARABLE_CLASS(CTestFactoryContainer) : public Factories::CFactoryContainerNode
	{
	friend class CTestFactoryBase;

public:
	IMPORT_C static void InstallL();
	IMPORT_C static void UnInstall();

	CTestFactoryBase* FindFactory(TUid aFactoryUid) const;
	CTestFactoryBase& FindOrCreateFactoryL(TUid aFactoryUid);
	CTestFactoryBase* Factory(TInt aIndex) const;

private:
	virtual ~CTestFactoryContainer();
	CTestFactoryContainer();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	};


//
//CTestFactoryBase
class CTestFactoryBase : public Factories::CFactoryBase
	{
public:
	IMPORT_C virtual ~CTestFactoryBase();

	IMPORT_C Dummy::CTestNodeBase& CreateL();
	IMPORT_C Dummy::CTestNodeBase* Find(Factories::MFactoryQuery& aQuery);
	IMPORT_C Dummy::CTestNodeBase& FindOrCreateL(Factories::MFactoryQuery& aQuery);

protected:
	IMPORT_C CTestFactoryBase(TUid aFactoryUid, CTestFactoryContainer& aParentContainer);
	virtual Dummy::CTestNodeBase& DoCreateL() = 0;
	};

} //namespace Dummy

#endif	// __DUMMYFACTORY_H__


