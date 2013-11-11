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
// Inline functions for CSmsPDUProcessor.
// 
//

/**
 @file
 @internalAll 
*/

inline TBool CSmsPDUProcessor::IsClass0Message() const
    {
    return iIsClass0Message;
    }

inline TBool CSmsPDUProcessor::IsClass2Message() const
    {
    return iIsClass2Message;
    }

inline TBool CSmsPDUProcessor::IsPIDType0() const
    {
    return iIsPIDType0;
    }

inline TBool CSmsPDUProcessor::IsForwardMessageToClient() const
    {
    return iIsForwardMessageToClient;
    }

inline void CSmsPDUProcessor::SetApplyAutoDeletionToClass2(TBool aSetting)
    {
    iOptionApplyAutoDeletionToClass2 = aSetting;
    }

inline void CSmsPDUProcessor::SetDiscardType0Class0(TBool aSetting)
    {
    iOptionDiscardType0Class0 = aSetting;
    }

inline TBool CSmsPDUProcessor::IsDiscardType0Class0() const
    {
    return iOptionDiscardType0Class0;
    }

inline void CSmsPDUProcessor::SetDiscardType0Class2(TBool aSetting)
    {
    iOptionDiscardType0Class2 = aSetting;
    }

inline TBool CSmsPDUProcessor::IsDiscardType0Class2() const
    {
    return iOptionDiscardType0Class2;
    }

inline TPtrC CSmsPDUProcessor::DeliverReportBuffer() const
    {
    return iDeliverReportBuffer;
    }

inline CSmsMessage* CSmsPDUProcessor::SmsMessage() const
    {
    return iSmsMessage;
    }

inline CSmsMessage* CSmsPDUProcessor::OriginalSmsMessage() const
    {
    return iOriginalSmsMessage;
    }

inline const TLogSmsPduData* CSmsPDUProcessor::SmsPDUData() const
    {
    return &iSmsPDUData;
    }
