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

#ifndef SYMBIAN_NODE2_H
#define SYMBIAN_NODE2_H

#include <dummynode.h>

class CNode2Factory;
class CNode2 : public Dummy::CTestNodeBase
    {
public:
	static CNode2* NewL(CNode2Factory& aFactory);

protected:
    CNode2(CNode2Factory& aFactory);
    virtual ~CNode2();
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    virtual TUid ServiceProviderUid() const;
    };

#endif //SYMBIAN_NODE2_H

