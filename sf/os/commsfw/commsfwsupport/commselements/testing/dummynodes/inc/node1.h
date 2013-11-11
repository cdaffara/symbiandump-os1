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

#ifndef SYMBIAN_NODE1_H
#define SYMBIAN_NODE1_H

#include <dummynode.h>

class CNode1Factory;
class CNode1 : public Dummy::CTestNodeBase
    {
public:
	static CNode1* NewL(CNode1Factory& aFactory);

protected:
    CNode1(CNode1Factory& aFactory);
    virtual ~CNode1();
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    virtual TUid ServiceProviderUid() const;
    };

#endif //SYMBIAN_NODE1_H

