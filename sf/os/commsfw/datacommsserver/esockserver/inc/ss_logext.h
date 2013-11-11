// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// - will become published partner soon
// 
//

/**
 @file
 @internalComponent
*/


#ifndef __SS_LOGEXT_H__
#define __SS_LOGEXT_H__

#include <elements/nm_log.h>
#include <elements/mm_log.h>

// Major level of log tagging
_LIT8(KESockComponentTag, "esock");

// Minor levels of log tagging (within "ESOCK" major tag)
_LIT8(KESockConnectionTag, "Conn");
_LIT8(KESockMetaConnectionTag, "MetaConn");
_LIT8(KESockSubConnectionTag, "SubConn");
_LIT8(KCFNodeTag, "CFNode");				// All node creation & destruction (sub-tag gives type)
_LIT8(KESockTierTag, "Tier");
_LIT8(KESockConnServTag, "ConnServ");
_LIT8(KESockFlowTag, "Flow");
_LIT8(KESockDataFactTag, "DatFact");		// data side factories
_LIT8(KESockCtrlFactTag, "CtlFact");		// control side factories
_LIT8(KESockBaseFactTag, "BasFact");		// factory base classes
_LIT8(KESockDCMsgs, "DCMsgs");				// Data/Control messages
_LIT8(KESockMeshMachine, "Mesh");
_LIT8(KESockMessageHook, "Hook");
_LIT8(KESockMessage, "Message");			// For logging in self dispatching messages
_LIT8(KESockStateMachine, "StateMachine");


// Logging for external callers is gated by __FLOG_ACTIVE or _DEBUG - unfortunately there's no polite way to detect running
// in UDEB without CDU logging. But that's very rare

#if defined(_DEBUG) || defined(__FLOG_ACTIVE)
#define ESOCK_EXTLOG_ACTIVE
#define ESOCK_EXTLOG(major_tag, minor_tag, text) ESockLogExternal::Printf(major_tag, minor_tag, text)
#define ESOCK_EXTLOG_VAR(variad) ESockLogExternal::Printf variad
#else
#undef ESOCK_EXTLOG_ACTIVE
#define ESOCK_EXTLOG(major_tag, minor_tag, text)
#define ESOCK_EXTLOG_VAR(variad)
#endif


/*
#define MESH_LOG_CONTEXT(tag,subTag,context,logText) \
   ESOCK_EXTLOG_VAR((tag, subTag, _L8("%s - [Node=%08x] [Sender=%08x] [Message=%s] [Activity=%s]"), \
      (logText), \
      &((context).iNode), \
      ((context).iCFMessageSig.iSender.Printable()), \
      ((context).iCFMessageSig.Printable()), \
      ((context).iNodeActivity ? (context).iNodeActivity->Printable() : _S8("Undefined"))));

#define MESH_LOG_CONTEXT_DES(tag,subTag,context,logDes) \
   ESOCK_EXTLOG_VAR((tag, subTag, _L8("%S - [Node=%08x] [Sender=%08x] [Message=%s] [Activity=%s]"), \
      (&logDes), \
      ((context).iNode().Printable()), \
      ((context).iCFMessageSig.iSender.Printable()), \
      ((context).iCFMessageSig.Printable()), \
      ((context).iNodeActivity ? (context).iNodeActivity->Printable() : _S8("Undefined"))));


#define MESH_LOG_MESSAGE(tag,subTag,message,activity,logText) \
   ESOCK_EXTLOG_VAR((tag, subTag, _L8("%s - [Sender=%08x] [Message=%s] [Activity=%s]"), \
      (logText), \
      (message.iSender.Printable()), \
      (message.Printable()), \
      (activity? activity->Printable() : _S8("Undefined"))));
#define MESH_LOG_MESSAGEONLY(tag,subTag,message,logText) \
   ESOCK_EXTLOG_VAR((tag, subTag, _L8("%s - [Sender=%08x] [Message=%s] [Activity=%s]"), \
      (logText), \
      (message.iSender.Printable()), \
      (message.Printable()), \
      _S8("Undefined")));


#ifdef ESOCK_EXTLOG_ACTIVE

//
// For more complex macros (i.e. those consisting of more than a simple ESOCK_EXTLOG_VAR statement),
// provide explicitly null urel versions.
//

#define MESH_LOG_TRANSITION(tag,subTag,activity,logText) \
   NetStateMachine::ACore* MESH_LOG_TRANSITION_logging_only_activity = (NetStateMachine::ACore*)activity; \
   ESOCK_EXTLOG_VAR((tag, subTag, _L8("%s - %s"), \
      (logText), \
      (MESH_LOG_TRANSITION_logging_only_activity? MESH_LOG_TRANSITION_logging_only_activity->Printable() : _S8("Undefined"))));

#define MESH_LOG_TRANSITION_DES(tag,subTag,activity,logDes) \
   NetStateMachine::ACore* MESH_LOG_TRANSITION_logging_only_activity2 = (NetStateMachine::ACore*)activity; \
   ESOCK_EXTLOG_VAR((tag, subTag, _L8("%S - %s"), \
      (&logDes), \
      (MESH_LOG_TRANSITION_logging_only_activity2 ? MESH_LOG_TRANSITION_logging_only_activity2->Printable() : _S8("Undefined"))));


//
// All activity logging lines have to have a consistent address printed out, so ensure printed
// addresses are cast to MeshMachine::CNodeActivityBase base.
//

#define MESH_DO_FORMAT(type, text, arg1) \
		TBuf8<80> log; \
		_LIT8(KLog, #type " %08x:\t" text); \
		log.Format(KLog(), static_cast<MeshMachine::CNodeActivityBase*>(arg1));

#define MESH_LOG_CONTEXT_FMT(tag,subTag,context,type,text,arg1) \
		{ \
		MESH_DO_FORMAT(type, text, arg1) \
	  	MESH_LOG_CONTEXT_DES(KESockComponentTag, KESockMeshMachine, aContext, log) \
		}

#define MESH_LOG_TRANSITION_FMT(tag,subTag,activity,type,text,arg1) \
		{ \
		MESH_DO_FORMAT(type, text, arg1) \
	  	MESH_LOG_TRANSITION_DES(KESockComponentTag, KESockMeshMachine, activity, log) \
		}


#define MESH_LOG_SYNC_CALL(tag,subTag,src,dest,fn) \
	{ \
	_LIT8(KLogMsg, "Synchronous call: Sender=%08x, Recipient=%08x, Function=%s"); \
	ESOCK_EXTLOG_VAR((tag,subTag, KLogMsg(), src,dest,fn)); \
	}

#else

#define MESH_LOG_TRANSITION(tag,subTag,activity,logText)
#define MESH_LOG_TRANSITION_DES(tag,subTag,activity,logDes)
#define MESH_LOG_CONTEXT_FMT(tag,subTag,context,type,text,arg1)
#define MESH_LOG_TRANSITION_FMT(tag,subTag,activity,type,text,arg1)
#define MESH_LOG_SYNC_CALL(tag,subTag,src,dest,fn)

#endif	// ESOCK_EXTLOG_ACTIVE
*/



//
// Logging macros for Messages::ANode-derived classes
//
// These macros are used to log Messages::ANode creation and destruction and to display
// class name and corresponding address Messages::ANode base address and real address.
//

#ifdef SYMBIAN_TRACE_ENABLE
#define LOG_NODE_CREATE(subTag, type) \
    { if (ESockLogExternal::WorkerId() == KErrNotFound) { NM_LOG_NODE_CREATE(subTag, type) } else { NM_LOG((subTag, NM_LOG_NODE_CREATE_ARG1(type, " [W%d]", ESockLogExternal::WorkerId()))) } }

#define LOG_NODE_CREATE1(subTag, type, text, arg) \
    { if (ESockLogExternal::WorkerId() == KErrNotFound) { NM_LOG_NODE_CREATE1(subTag, type, text, arg) } else { NM_LOG((subTag, NM_LOG_NODE_CREATE_ARG2(type, text " [W%d]", arg, ESockLogExternal::WorkerId()))) } }

#define LOG_NODE_DESTROY(subTag, type) \
    { if (ESockLogExternal::WorkerId() == KErrNotFound) { NM_LOG_NODE_CREATE(subTag, type) } else { NM_LOG((subTag, NM_LOG_NODE_DESTROY_ARG1(type, " [W%d]", ESockLogExternal::WorkerId()))) } }

#else
// !SYMBIAN_TRACE_ENABLE

#define LOG_NODE_CREATE(subTag, type)
#define LOG_NODE_CREATE1(subTag, type, text, arg)
#define LOG_NODE_DESTROY(subTag, type)
#endif
// SYMBIAN_TRACE_ENABLE


class ESockLogExternal
	{
public:
	IMPORT_C static void Printf(const TDesC8& aMajorTag, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C static void Printf(const TDesC8& aMajorTag, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);

	// Helper for utrace code - do not use this directly
	IMPORT_C static TInt WorkerId();
	};

#endif


