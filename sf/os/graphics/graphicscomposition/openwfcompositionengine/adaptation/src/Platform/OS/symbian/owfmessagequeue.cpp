// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
// 
//
//

#define KInitialIndexValue (-1)

#include "owfmessagequeue.h"
#include "owfdebug.h"
#include <e32base.h>

class CMessageQueue: public CBase
    {
    public:
        virtual ~CMessageQueue();
        static CMessageQueue* NewL();
        static CMessageQueue* NewLC();
        void Send(TInt msg, void* data);
        TInt Fetch(OWF_MESSAGE* msg, TInt timeout);
        TInt Fetch(OWF_MESSAGE* msg);
        
    private:
        CMessageQueue();
        void ConstructL();
        
    private:
        const static TInt iQueueSize = 16;
        OWF_MESSAGE iMessageArray[iQueueSize];
        RSemaphore iFetchSeamaphore;
        RSemaphore iSendSemaphore;
        RFastLock iProtectionMutex;
        TInt iSendIndex;
        TInt iFetchIndex;
    };

CMessageQueue::CMessageQueue():
iSendIndex(KInitialIndexValue),
iFetchIndex(KInitialIndexValue)
    {
    
    }

CMessageQueue::~CMessageQueue()
    {
    iFetchSeamaphore.Close();
    iSendSemaphore.Close();
    iProtectionMutex.Close();
    }

void CMessageQueue::ConstructL()
    {
    User::LeaveIfError(iFetchSeamaphore.CreateLocal(0));
    User::LeaveIfError(iSendSemaphore.CreateLocal(iQueueSize));
    User::LeaveIfError(iProtectionMutex.CreateLocal());
    }

CMessageQueue* CMessageQueue::NewLC()
    {
    CMessageQueue* self = new(ELeave) CMessageQueue();
    CleanupStack::PushL(self);
    self->ConstructL();
    return(self);
    }

CMessageQueue* CMessageQueue::NewL()
    {
    CMessageQueue* self = CMessageQueue::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

void CMessageQueue::Send(TInt msg, void* data)
    {
    iSendSemaphore.Wait();
    iProtectionMutex.Wait();
    iSendIndex = (iSendIndex + 1) % iQueueSize;
    iMessageArray[iSendIndex].id = msg;
    iMessageArray[iSendIndex].data = data;
    iProtectionMutex.Signal();
    iFetchSeamaphore.Signal();
    }

TInt CMessageQueue::Fetch(OWF_MESSAGE* msg, TInt timeout)
    {
    TInt ret = KErrNone;
    if (!msg)
        {
        return KErrArgument;
        }
    
    if ((ret = iFetchSeamaphore.Wait(timeout)) == KErrNone)
        {
        iProtectionMutex.Wait();
        if (iSendIndex == iFetchIndex)
            {
            iProtectionMutex.Signal();
            return KErrTimedOut;
            }
        else
            {
            iFetchIndex = (iFetchIndex + 1) % iQueueSize;
            *msg = iMessageArray[iFetchIndex];
            iProtectionMutex.Signal();
            iSendSemaphore.Signal();
            return KErrNone;
            }
        }
    return ret;
    }

TInt CMessageQueue::Fetch(OWF_MESSAGE* msg)
    {
    if (!msg)
        {
        return KErrArgument;
        }
    
    iFetchSeamaphore.Wait();
    iProtectionMutex.Wait();
    iFetchIndex = (iFetchIndex + 1) % iQueueSize;
    *msg = iMessageArray[iFetchIndex];
    iProtectionMutex.Signal();
    iSendSemaphore.Signal();
    return KErrNone;
    }

OWF_API_CALL void
OWF_MessageQueue_Destroy(OWF_MESSAGE_QUEUE* queue)
{
    if (!queue) 
        {
        return;
        }
    delete (CMessageQueue*)queue->queuePtr;
    queue->queuePtr = NULL;
}

OWF_API_CALL OWFint
OWF_MessageQueue_Init(OWF_MESSAGE_QUEUE* queue)
{
    if (!queue) 
        {
        return MSG_QUEUE_INIT_ERR;
        }
    
    TRAPD(err, queue->queuePtr = CMessageQueue::NewL());
    return (err || !queue->queuePtr) ? MSG_QUEUE_INIT_ERR : MSG_QUEUE_INIT_OK;
}

OWF_API_CALL void
OWF_Message_Send(OWF_MESSAGE_QUEUE* queue,
                 OWFuint msg,
                 void* data)
{
    OWF_ASSERT(queue);
    OWF_ASSERT(queue->queuePtr);
    
    ((CMessageQueue*)(queue->queuePtr))->Send(msg, data);
}

OWF_API_CALL OWFint
OWF_Message_Wait(OWF_MESSAGE_QUEUE* queue,
                 OWF_MESSAGE* msg,
                 OWFint timeout)
{
    OWF_ASSERT(queue);
    OWF_ASSERT(queue->queuePtr);
    OWF_ASSERT(msg);
    
    OWFint ret = 0;
    
    if (WAIT_FOREVER == timeout)
        {
        ret = ((CMessageQueue*)(queue->queuePtr))->Fetch(msg);
        }
    else
        {
        ret = ((CMessageQueue*)(queue->queuePtr))->Fetch(msg, timeout);
        }
    switch (ret)
        {
        case KErrNone:
            return MSG_QUEUE_WAIT_MSG_FETCHED;
        case KErrTimedOut:
            return MSG_QUEUE_WAIT_MSG_TIMEDOUT;
        default:
            return MSG_QUEUE_WAIT_MSG_ERR;
        }
}

