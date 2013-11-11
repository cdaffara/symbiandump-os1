/* Copyright (c) 2009 The Khronos Group Inc.
 * Portions copyright (c) 2009-2010  Nokia Corporation and/or its subsidiary(-ies)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef OWF_MESSAGEQUEUE_H_
#define OWF_MESSAGEQUEUE_H_

#include "owftypes.h"
#include "owfsemaphore.h"
#include "owfmutex.h"


#ifdef __cplusplus
extern "C" {
#endif

#define WAIT_FOREVER            -1
#define MSG_QUEUE_WAIT_MSG_FETCHED      0
#define MSG_QUEUE_WAIT_MSG_ERR          -1
#define MSG_QUEUE_WAIT_MSG_TIMEDOUT     -2
#define MSG_QUEUE_INIT_OK               0
#define MSG_QUEUE_INIT_ERR              -1


typedef struct {
    OWFuint                 id;
    void*                   data;
} OWF_MESSAGE;

typedef struct _MSGQUE {
    void*                   queuePtr;
} OWF_MESSAGE_QUEUE;

/*
 *  Destroy message queue
 *
 *  \param queue Message queue to destroy
 */
OWF_API_CALL void
OWF_MessageQueue_Destroy(OWF_MESSAGE_QUEUE* queue);

/*
 *  Initialize message queue
 *
 *  \param queue Message queue to initialize
 *  \return 0 if initialization succeeded, < 0 otherwise
 *
 */
OWF_API_CALL OWFint
OWF_MessageQueue_Init(OWF_MESSAGE_QUEUE* queue);

/*
 *  Insert message into message queue (send it to
 *  THE other side)
 *
 *  \param queue Message queue
 *  \param msg Message to send
 *  \param data Message contents
 *
 */
OWF_API_CALL void
OWF_Message_Send(OWF_MESSAGE_QUEUE* queue,
                 OWFuint msg,
                 void* data);
/*
 *  Wait for message
 *
 *  \param queue Message queue
 *  \param msg Where to store the received message
 *  \param timeout Time to wait for the message (microseconds)
 *
 *  \return < 0 if error occurred, 0 if no message was received within
 *  given period of time, > 0 otherwise; received message is stored into
 *  OWF_MESSAGE structure pointed by the msg param
 *
 */
OWF_API_CALL OWFint
OWF_Message_Wait(OWF_MESSAGE_QUEUE* queue,
                 OWF_MESSAGE* msg,
                 OWFint timeout);


#ifdef __cplusplus
}
#endif


#endif
