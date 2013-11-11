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
// mesh machine logging
// Note: some of these log outputs are used by SVG tools - careful when changing them
// 
//

/**
 @file
 @internalTechnology
*/

#include "mm_log.h"
#include <elements/mm_context.h>
#include <elements/mm_activities.h>

using namespace Messages;
namespace MeshMachine
{
EXPORT_C void Logging::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...)
    {
#ifdef SYMBIAN_TRACE_ENABLE
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aSubTag, aFmt, list);
	VA_END(list);
#else
	(void)aSubTag;
	(void)aFmt;
#endif
    }

    
EXPORT_C void Logging::Printf(const TDesC8& /*aSubTag*/, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
#ifdef SYMBIAN_TRACE_ENABLE
    TLogTextBuf buf;
	buf.AppendFormatIgnoreOverflow(aFmt, aList);
	UTracePfAny(KPrimaryFilter, MeshMachine::Logging::ELogInfo, ETrue, EFalse, buf.Length(), buf.Ptr(), buf.Length());
#else
	//(void)aSubTag;
	(void)aFmt;
	(void)aList;
#endif
    }


EXPORT_C void Logging::LogContext(const TDesC8& aSubTag, const TNodeContextBase& aContext)
    {
#ifdef SYMBIAN_TRACE_ENABLE
    LogContext(aSubTag, aContext, KNullDesC8);    
#else
	(void)aSubTag;
	(void)aContext;
#endif
    }


EXPORT_C void Logging::LogContext(const TDesC8& aSubTag, const TNodeContextBase& aContext, const TDesC8& aText)
    {
#ifdef SYMBIAN_TRACE_ENABLE
    NM_LOG_START_BLOCK(aSubTag, _L8("Ctx"));
    if (aText.Length() > 0)
        {
        Printf(aSubTag, aText);
        }
    NM_LOG_NODE_EXT(aSubTag, aContext.NodeId().Node(), KNullDesC8);
    NM_LOG_ADDRESS_EXT(aSubTag, aContext.iSender, _L8("From:"));
    NM_LOG_ADDRESS_EXT(aSubTag, aContext.iRecipient, _L8("To:"));
   	LogActivity(aSubTag, aContext.iNodeActivity, NULL);
    NM_LOG_END_BLOCK(aSubTag, KNullDesC8);
#else
	(void)aSubTag;
	(void)aContext;
	(void)aText;
#endif
    }


EXPORT_C void Logging::LogActivity(const TDesC8& aSubTag, const CNodeActivityBase* aActivity, const TNodeContextBase* aContext)
    {
    LogActivity(aSubTag, aActivity, aContext, KNullDesC8);
    }
    

EXPORT_C void Logging::LogActivity(const TDesC8& aSubTag, const CNodeActivityBase* aActivity, const TNodeContextBase* aContext, const TDesC8& aText)
    {
#ifdef SYMBIAN_TRACE_ENABLE
    const ANode* node = NULL;
    if (aContext)
    	{
    	node = &aContext->NodeId().Node();
    	}

	if (aActivity)
		{
		if (node)
			{
			Printf(aSubTag, _L8("%S [ANode=0x%08x] [Activity=%s] [Triple=%s]"), &aText, node, aActivity->ActivityName(), aActivity->CurrentTripleName());
			}
		else
			{
			Printf(aSubTag, _L8("%S [Activity=%s] [Triple=%s]"), &aText, aActivity->ActivityName(), aActivity->CurrentTripleName());
			}
		}
	else
	if (aText.Length())
		{
		Printf(aSubTag, _L8("%S "), &aText);
		}
#else
	(void)aSubTag;
	(void)aActivity;
	(void)aContext;
	(void)aText;
#endif
    }

} //MeshMachine


