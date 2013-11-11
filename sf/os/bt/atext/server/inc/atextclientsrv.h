/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Client-Server message passing declarations
*
*/


#ifndef C_ATEXTCLIENTSRV_H
#define C_ATEXTCLIENTSRV_H

/**  Opcodes used in message passing between client and server */
enum TATExtIpc
    {
    EATExtSetExtensionInterface,
    EATExtSynchronousClose,
    EATExtHandleCommand,
    EATExtCancelHandleCommand,
    EATExtGetNextPartOfReply,
    EATExtNumberOfPlugins,
    EATExtReceiveUnsolicitedResult,
    EATExtCancelReceiveUnsolicitedResult,
    EATExtMarkUrcHandlingOwnership,
    EATExtReceiveEcomPluginChange,
    EATExtCancelReceiveEcomPluginChange,
    EATExtReportQuietModeChange,
    EATExtReportVerboseModeChange,
    EATExtReportCharacterChange,
    EATExtReportListenerUpdateReady,
    EATExtBroadcastNvramStatusChange,
    EATExtReportExternalHandleCommandError,
    EATExtReportHandleCommandAbort,
    EATExtGetNextSpecialCommand,
    EInvalidIpc
    };

/**  Message parameter indexes for client side */
enum TATExtClientMessages
    {
    // Parameters for Connect()
    EATExtConnectParamUid                   = 0,
    EATExtConnectParamName                  = 1,
    // Parameters for HandleCommand()
    EATExtHandleCmdParamCmd                 = 0,
    EATExtHandleCmdParamReply               = 1,
    EATExtHandleCmdParamLength              = 2,
    EATExtHandleCmdParamReplyType           = 3,
    // Parameters for GetNextPartOfReply()
    EATExtGetNextPartOfReplyParamReply      = 0,
    EATExtGetNextPartOfReplyParamLength     = 1,
    // Parameters for ReceiveUnsolicitedResult()
    EATExtReceiveUrcCmdParamBuf             = 0,
    EATExtReceiveUrcCmdParamUid             = 1,
    // Parameters for CancelReceiveUnsolicitedResult()
    EATExtCancelReceiveUrcCmdParamUid       = 0,
    // Parameters for MarkUrcHandlingOwnership()
    EATExtMarkUrcHandlingOwnershipParamUid  = 0,
    // Parameters for ReceiveEcomPluginChange()
    EATExtReceiveEcomPluginChangeParamUid   = 0,
    EATExtReceiveEcomPluginChangeParamType  = 1,
    // Parameters for ReportQuietModeChange()
    EATExtReportQuietModeChangeParamMode    = 0,
    // Parameters for ReportVerboseModeChange()
    EATExtReportVerboseModeChangeParamMode  = 0,
    // Parameters for ReportCharacterChange()
    EATExtReportCharacterChangeParamType    = 0,
    EATExtReportCharacterChangeParamChar    = 1,
    // Parameters for ReportListenerUpdateReady()
    EATExtReportListenerUpdateParamUid      = 0,
    EATExtReportListenerUpdateParamType     = 1,
    // Parameters for BroadcastNvramStatusChange()
    EATExtBroadcastNvramChangeParamNvram    = 0,
    // Parameters for ReportHandleCommandAbort()
    EATExtReportHandleCommandAbortParamStop = 0,
    // Parameters for GetNextSpecialCommand()
    EATExtGetNextSpecialCmdParamCmd         = 0,
    EATExtGetNextSpecialCmdParamFirst       = 1
    };

/**  Server name */
_LIT( KATExtSrvName, "atext" );

/**  A version must be specified when creating a session with the server */
const TUint KServerMajorVersionNumber = 1;
const TUint KServerMinorVersionNumber = 0;
const TUint KServerBuildVersionNumber = 0;

#endif  // C_ATEXTCLIENTSRV_H
