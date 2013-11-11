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
// node message logging
// 
//

/**
 @file
 @internalTechnology
*/

#include <elements/nm_log.h>
#include <elements/nm_address.h>
#include <elements/nm_signals.h>


namespace Messages
{

EXPORT_C void Logging::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aSubTag, Logging::ELogInfo, aFmt, list);
	VA_END(list);
    }

    
EXPORT_C void Logging::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
    Printf(aSubTag, Logging::ELogInfo, aFmt, aList);
    }


EXPORT_C void Logging::Printf(const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aSubTag, aType, aFmt, list);
	VA_END(list);
    }

    
EXPORT_C void Logging::Printf(const TDesC8& /*aSubTag*/, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
    TLogTextBuf buf;
	buf.AppendFormatIgnoreOverflow(aFmt, aList);
	UTracePfAny(KPrimaryFilter, aType, ETrue, EFalse, buf.Length(), buf.Ptr(), buf.Length());
    }


EXPORT_C void Logging::LogAddress(const TDesC8& /*aSubTag*/, const TRuntimeCtxId& aAddress, const TDesC8& aText)
    {
    TBuf8<KMaxLogTextLength + TRuntimeCtxId::KMaxInlineAddressSize> buf;
    if (aText.Length() > 0)
        {
        TPtrC8 subStr(aText.Ptr(), aText.Length() > KMaxLogTextLength ? KMaxLogTextLength : aText.Length());
        buf.Copy(subStr);
        }
    buf.Append(reinterpret_cast<const TUint8*>(&aAddress), aAddress.Size());
    
    
    UTracePfAny(KPrimaryFilter, Logging::ELogAddress, ETrue, EFalse, aText.Length(), buf.Ptr(), buf.Length());
    }


EXPORT_C void Logging::LogAddress(const TDesC8& aSubTag, const TRuntimeCtxId& aAddress)
    {
    LogAddress(aSubTag, aAddress, KNullDesC8);
    }


EXPORT_C void Logging::LogMessage(const TDesC8& /*aSubTag*/, const TDesC8& aMessage, const TDesC8& aText)
    {
    if (aText.Length() > 0)
        {
        // Putting all the logging data into one event keeps them tied together
        // making the utrace decoders life easier and saves us from having to
        // use additional logging statements to group the message and text
        TBuf8<KMaxLogTextLength + TSignalBase::KMaxInlineMessageSize> buf;
        TPtrC8 subStr(aText.Ptr(), aText.Length() > KMaxLogTextLength ? KMaxLogTextLength : aText.Length());
        buf.Copy(subStr);
        buf.Append(aMessage.Ptr(), aMessage.Length());
        UTracePfAny(KPrimaryFilter, Logging::ELogMessage, ETrue, EFalse, subStr.Length(), buf.Ptr(), buf.Length());
        }
    else
        {
        // Log only the message
        UTracePfAny(KPrimaryFilter, Logging::ELogMessage, ETrue, EFalse, 0, aMessage.Ptr(), aMessage.Length());
        }
    }
    

EXPORT_C void Logging::LogMessage(const TDesC8& aSubTag, const TDesC8& aMessage)
    {
    LogMessage(aSubTag, aMessage, KNullDesC8);
    }


EXPORT_C void Logging::LogMessage(const TDesC8& /*aSubTag*/, const TSignalBase& aMessage, const TDesC8& aText)
    {
    TBuf8<KMaxLogTextLength + TSignalBase::KMaxInlineMessageSize> buf;
    TPtrC8 subStr(aText.Ptr(), aText.Length() > KMaxLogTextLength ? KMaxLogTextLength : aText.Length());
    buf.Copy(subStr);
    
    TPtr8 messageBuf(const_cast<TUint8*>(buf.Ptr() + buf.Length()), buf.MaxLength() - buf.Length());
    aMessage.Store(messageBuf);
    buf.SetLength(buf.Length() + messageBuf.Length());
    UTracePfAny(KPrimaryFilter, Logging::ELogMessage, ETrue, EFalse, subStr.Length(), buf.Ptr(), buf.Length());
    }


EXPORT_C void Logging::LogMessage(const TDesC8& aSubTag, const TSignalBase& aMessage)
    {
    LogMessage(aSubTag, aMessage, KNullDesC8);
    }


EXPORT_C void Logging::LogNode(const TDesC8& aSubTag, const ANode& aNode)
    {
    LogNode(aSubTag, aNode, KNullDesC8);
    }
    
    
EXPORT_C void Logging::LogNode(const TDesC8& /*aSubTag*/, const ANode& aNode, const TDesC8& aText)
    {
    TBuf8<KMaxLogTextLength + sizeof(ANode*)> buf;
    TPtrC8 subStr(aText.Ptr(), aText.Length() > KMaxLogTextLength ? KMaxLogTextLength : aText.Length());
    buf.Copy(subStr);
    const ANode* ptr = &aNode;
    TPckgC<const ANode*> pckg(ptr);
    buf.Append(pckg);
    UTracePfAny(KPrimaryFilter, Logging::ELogNode, ETrue, EFalse, subStr.Length(), buf.Ptr(), buf.Length());
    }

}
// namespace Messages


