// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ss_flowbinder.cpp
// 
//

#include "ss_flowbinders.h"

using namespace ESock;

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TBinderConfig4, TBinderConfig4::EUid, TBinderConfig4::ETypeId)
// No attributes defined, as no serialisation takes place.
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TBinderConfig6, TBinderConfig6::EUid, TBinderConfig6::ETypeId)
// No attributes defined, as no serialisation takes place.
END_ATTRIBUTE_TABLE()


EXPORT_C  MLowerControl* MFlowBinderControl::GetControlL(const TDesC8& /*aProtocol*/)
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }
    
EXPORT_C  MLowerDataSender* MFlowBinderControl::BindL(const TDesC8& /*aProtocol*/, MUpperDataReceiver* /*aReceiver*/, MUpperControl* /*aControl*/)
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }
EXPORT_C  void MFlowBinderControl::Unbind(MUpperDataReceiver* /*aReceiver*/, MUpperControl* /*aControl*/)
    {
    }

//session binder
EXPORT_C  MSessionControl* MFlowBinderControl::GetControlL(TInt /*aSessionType*/, MSessionControlNotify&)
    {
    User::Leave(KErrNotSupported);    
    return NULL;
    }
EXPORT_C  MSessionData* MFlowBinderControl::BindL(MSessionDataNotify& /*aNotify*/)
    {
    User::Leave(KErrNotSupported);    
    return NULL;
    }
EXPORT_C  void MFlowBinderControl::Unbind()
    {
    }

EXPORT_C CSubConnectionFlowBase& MFlowBinderControl::CloneFlowL()
    {    
    User::Leave(KErrNotSupported);    
    CSubConnectionFlowBase* flow = NULL;
    return *flow;
    }

EXPORT_C TInt MLowerControl::GetName(TDes& /*aName*/)
    {
    return KErrNotSupported;
    }
    
EXPORT_C TInt MLowerControl::BlockFlow(TBlockOption /*aOption*/)
    {
    return KErrNotSupported;
    }
    
EXPORT_C TInt MLowerControl::GetConfig(TBinderConfig& /*aConfig*/)
    {
    return KErrNotSupported;
    }
    
EXPORT_C TInt MLowerControl::Control(TUint /*aLevel*/, TUint /*aName*/, TDes8& /*aOption*/)
    {
    return KErrNotSupported;
    }

