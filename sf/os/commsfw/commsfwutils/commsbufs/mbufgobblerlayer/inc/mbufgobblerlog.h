// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Helper macros that wrap the ones from ESock.
//
//
//  This is a 3-plane comms layer implementation example, which has been customised to be a test layer which gobbles and releases ESOCK MBUFs.
//  The MBuf gobbling functionality can be disabled by undefining the macro SYMBIAN_COMMSFW_MBUF_GOBBLER which is specified in mbufgobblerproviders.mmp.
//  When SYMBIAN_COMMSFW_MBUF_GOBBLER is undefined, the source code specified by mbufgobblerproviders.mmp becomes a pass through layer i.e. it passes the data
//  through to the layer above or below without altering it. This makes it useful as a starting point for implementing your own layers / providers;
//  useful documentation on how to customise your own passthrough layer can be found in ..\docs\MbufGobblerLayer.doc
//

/**
 @file
 @internalComponent
*/

#ifndef MBUFGOBBLERLOG_H_
#define MBUFGOBBLERLOG_H_

#include <e32def.h>
#include <comms-infras/ss_logext.h> //defines LOG_NODE_CREATE and LOG_NODE_DESTROY

#include <cflog.h> //defines __CFLOG_* macros

#include "mbufgobblerhelper.h"

/*
 * The component tag
 * 
 * In commsdbg.ini add line LOG MbufGobblerLayer * to enbale these logs 
 */
_LIT8(KMbufGobblerComponentTag, "MbufGobblerLayer");

#define KMbufGobblerMcprTag KESockMetaConnectionTag
_LIT8(KMbufGobblerMcprSubTag, "MbufGobblerMCpr");

#define KMbufGobblerMCprFactoryTag KESockMetaConnectionTag
_LIT8(KMbufGobblerMCprFactorySubTag, "MbufGobblerMCprFactory");

#define KMbufGobblerCprFactoryTag KESockConnectionTag
_LIT8(KMbufGobblerCprFactorySubTag, "MbufGobblerCprFactory");

#define KMbufGobblerCprTag KESockConnectionTag
_LIT8(KMbufGobblerCprSubTag, "MbufGobblerCpr");

#define KMbufGobblerScprFactoryTag KESockSubConnectionTag
_LIT8(KMbufGobblerScprFactorySubTag, "MbufGobblerSCprFactory");

#define KMbufGobblerScprTag KESockSubConnectionTag
_LIT8(KMbufGobblerScprSubTag, "MbufGobblerSCpr");

#define KMbufGobblerFlowTag KESockFlowTag
_LIT8(KMbufGobblerFlowSubTag,"MbufGobblerFlow");

#define KMbufGobblerTierTag KESockTierTag
_LIT8(KMbufGobblerTierSubTag,"MbufGobblerTierM");

#define KMbufGobblerTierFactoryTag KESockTierTag
_LIT8(KMbufGobblerTierFactorySubTag,"MbufGobblerTierMFactory");

/*
 * LOG_NODE_CREATE - this logs out to utrace and is needed for SVG viewer
 * __CFLOG_VAR - this logs to standard comms dbg utils. Must include cflog.mmh in mmp file
 */

#define MBUFGOBBLER_LOG_NODE_CREATE(subTag, type, nodename, nodeptr) \
		LOG_NODE_CREATE(subTag, type);		 \
		MbufGobblerHelper::LogCreateDestroy(subTag, nodename, nodeptr, 1); 
	
#define MBUFGOBBLER_LOG_NODE_DESTROY(subTag, type, nodename, nodeptr) \
		LOG_NODE_DESTROY(subTag, type);		 \
		MbufGobblerHelper::LogCreateDestroy(subTag, nodename, nodeptr, 0);  
			
#define LOGMESSAGE(subtag, nodeclass, nodeptr, sender, recipient, msg)	MbufGobblerHelper::LogMessage(subtag, nodeclass, nodeptr, sender, recipient, msg)
	
#define LOG_NODE_INFO(subtag, nodename, node, clientcount) MbufGobblerHelper::PrintClientNodes(subtag, nodename, node, clientcount);

#define LOG(subTag, ARGS...) __CFLOG_VAR((KMbufGobblerComponentTag, subTag, ##ARGS));

#endif /* MBUFGOBBLERLOG_H_ */
