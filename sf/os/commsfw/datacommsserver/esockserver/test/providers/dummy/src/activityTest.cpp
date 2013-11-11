// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestDummy implementation file for a TierManager
// 
//

/**
 @file
 @internalComponent
*/

#include <elements/nm_messages_base.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <ss_glob.h>
#include "activityTest.h"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;

void CActivityCancelTest::StoreRequestL(const TSignatureBase& aRequest)
    {
    TBuf8<__Align8(TSignalBase::KMaxInlineMessageSize + TSignalBase::KMaxUnstoredOverhead)> msgBuf;
    aRequest.Store(msgBuf);
    TPtrC8 messageStore(msgBuf);
    TlsGlobals::Get().VirtualCtor()->New(messageStore, iStoredRequest);
    }


DEFINE_SMELEMENT(CActivityCancelTest::TNoTagForwardOrActiveTagBackward, NetStateMachine::MStateFork, TContext)
TInt CActivityCancelTest::TNoTagForwardOrActiveTagBackward::TransitionTag()
    {
    if (message_cast<TEBase::TError>(&iContext.iMessage))
        {
        return KActiveTag | NetStateMachine::EBackward;
        }
    return KNoTag | NetStateMachine::EForward;
    }

DEFINE_SMELEMENT(CActivityCancelTest::TBeginTest, NetStateMachine::MStateTransition, TContext)
void CActivityCancelTest::TBeginTest::DoL()
    {
    ASSERT(iContext.iNodeActivity);
    CActivityCancelTest& act = static_cast<CActivityCancelTest&>(*iContext.iNodeActivity);
    act.StoreRequestL(iContext.iMessage);
    }
    
DEFINE_SMELEMENT(CActivityCancelTest::TBeginLoop, NetStateMachine::MStateTransition, TContext)
void CActivityCancelTest::TBeginLoop::DoL()
    {
    ASSERT(iContext.iNodeActivity);
    CActivityCancelTest& act = static_cast<CActivityCancelTest&>(*iContext.iNodeActivity);    
    act.PostRequestTo(iContext.Node().Id(), act.StoredRequest());  
    act.NextLoop();
    }


DEFINE_SMELEMENT(CActivityCancelTest::TCancel, NetStateMachine::MStateTransition, TContext)
void CActivityCancelTest::TCancel::DoL()
    {
    ASSERT(iContext.iNodeActivity);
    CActivityCancelTest& act = static_cast<CActivityCancelTest&>(*iContext.iNodeActivity);    
    act.PostRequestTo(iContext.Node().Id(), TEBase::TCancel());
    }

DEFINE_SMELEMENT(CActivityCancelTest::TEndTest, NetStateMachine::MStateTransition, TContext)
void CActivityCancelTest::TEndTest::DoL()
    {
    ASSERT(iContext.iNodeActivity);
    CActivityCancelTest& act = static_cast<CActivityCancelTest&>(*iContext.iNodeActivity);   
    act.PostToOriginators(iContext.iMessage);
    ASSERT(act.Error() != KErrNone); //if the activity has done its job, the activity have been invariably errored; 
    act.SetError(KErrNone); //No point reporting this error though.
    }


namespace CancelTestBindToActivity
{
/* This is a largely generic activity written to test cancelling behaviour of another (typically 
 * production code) activity. In this case it is put on top of and to test the TBindTo 
 * activity, by the virtue of awaiting TCFDataClient::TBindTo. In principle though it can be put on
 * top of any other activity by awaiting for something else instead. CancelTestBindToActivity hijacks
 * and stores the original request (in this case TCFDataClient::TBindTo). It then goes to perform a 
 * loop:
 * { 
 * - It forwards the stored request to the overriden activity (in this case the TBindTo activity), 
 * - then follows it with a TCancel 
 * - and awaits for TError. 
 * }
 * Each loop will send the TCancel slightly later, i.e.: at consecutive ticks triggered by messages 
 * received by the local node (and subsequently resulting in AMMNodeBase::SignalActivities). 
 * If you examine CActivityCancelTest::TProgressiveCancel you will see that it is blocked 
 * (as any other synchronised activity) against gradually increasing number of ticks. It has been
 * pointed out that the tick resolution may be too small - i.e.: that we may want to tick the
 * activity with every message sent within the local thread (and thus with every even that can
 * change the state of the activity peers). This can be done by signalling CActivityCancelTest
 * from the transport upon every dispatch (or post). This is so far not done. Instead CancelTestBindToActivity
 * is put against every mm node performing bind to (CPR and SCPR).
 */
DEFINE_CUSTOM_NODEACTIVITY(CActivityCancelTest::ECancelTestActivity, CancelBindTo, TCFDataClient::TBindTo, CActivityCancelTest::NewL)
    FIRST_NODEACTIVITY_ENTRY(CActivityCancelTest::TAwaitingMessageState<TCFDataClient::TBindTo>, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, CActivityCancelTest::TBeginTest, MeshMachine::TTag<KActiveTag>)
    THROUGH_NODEACTIVITY_ENTRY(KActiveTag, CActivityCancelTest::TBeginLoop, MeshMachine::TNoTag)    
    NODEACTIVITY_ENTRY(KNoTag, CActivityCancelTest::TProgressiveCancel, MeshMachine::TAcceptErrorState<MeshMachine::TAwaitingMessageState<TCFDataClient::TBindToComplete>  >, CActivityCancelTest::TNoTagForwardOrActiveTagBackward)
    LAST_NODEACTIVITY_ENTRY(KNoTag, CActivityCancelTest::TEndTest)
NODEACTIVITY_END()
}



