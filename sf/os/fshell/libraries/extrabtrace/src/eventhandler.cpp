// eventhandler.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include "eventhandler.h"
#include <fshell/extrabtracek.h>
#include <kern_priv.h>
#include <fshell/btrace_parser_defs.h>

DExtraBtraceEventHandler::DExtraBtraceEventHandler(DObject* aParent)
	: DKernelEventHandler(&Event, this), iParent(aParent)
	{
	// iParent has been opened on our behalf by the caller (or it is null)
	}

DExtraBtraceEventHandler::~DExtraBtraceEventHandler()
	{
	if (iParent)
		{
		NKern::ThreadEnterCS();
		iParent->AsyncClose();
		NKern::ThreadLeaveCS();
		}
	}

TUint DExtraBtraceEventHandler::Event(TKernelEvent aEvent, TAny* a1, TAny* /*a2*/, TAny* /*aPrivateData*/)
	{
	switch (aEvent)
		{
		case EEventKillThread:
			{
			DObjectCon* threads=Kern::Containers()[EThread];
			threads->Wait(); // hold mutex so so we can't interleave with priming data
			DThread* thread = static_cast<DThread*>(a1);
			switch (thread->iExitType)
				{
				case EExitKill:
					BTrace8(KAmTraceCategory, EAmTraceSubCategoryThreadKilled, thread->iId, thread->iExitReason);
					break;
				case EExitTerminate:
					BTrace8(KAmTraceCategory, EAmTraceSubCategoryThreadTerminated, thread->iId, thread->iExitReason);
					break;
				case EExitPanic:
					{
					BTraceN(KAmTraceCategory, EAmTraceSubCategoryThreadPanicked, thread->iId, thread->iExitReason, thread->iExitCategory.Ptr(), thread->iExitCategory.Size());
					break;
					}
				default:
					break;
				}
			threads->Signal();
			break;
			}
		case EEventRemoveProcess:
			{
			DProcess* process = static_cast<DProcess*>(a1);
			switch (process->iExitType)
				{
				case EExitKill:
					BTrace8(KAmTraceCategory, EAmTraceSubCategoryProcessKilled, process->iId, process->iExitReason);
					break;
				case EExitTerminate:
					BTrace8(KAmTraceCategory, EAmTraceSubCategoryProcessTerminated, process->iId, process->iExitReason);
					break;
				case EExitPanic:
					{
					BTraceN(KAmTraceCategory, EAmTraceSubCategoryProcessPanicked, process->iId, process->iExitReason, process->iExitCategory.Ptr(), process->iExitCategory.Size());
					break;
					}
				default:
					break;
				}
			break;
			}
		default:
			break;
		}

	return ERunNext;
	}
