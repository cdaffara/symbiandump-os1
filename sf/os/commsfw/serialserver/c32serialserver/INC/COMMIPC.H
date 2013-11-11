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
//


#ifndef COMMIPC_H
#define COMMIPC_H

/** 
@file
 
Defines the IPC numbers used for C32 Client/Server communications
@internalAll
*/

/**
unique numbers used for message passing between
client and C32 comms server.
@internalAll
*/
enum TCommMess
{
	ECommLoadCommModule,            //< 0 Load a CSY comms module
	ECommCloseCommModule,           //< 1 Unload a CSY comms module
	ECommPortInfo,
	ECommPortInfoByName,            //< 3 Get information on serial ports by name
	ECommPortInfoByNumber,          //< 4 Get information on serial ports by number
	ECommNumPorts,                  //< 5 Get how many CSY's are loaded
	ECommStartServerThread,         //< 6 Start another thread in the C32 process
	ECommOpen,                      //< 7 Open a comm port
	ECommRead,                      //< 8 Read from the comm port
	ECommReadCancel,                //< 9 Cancel an outstanding read
	ECommQueryReceiveBuffer,        //< 10 Get number of bytes in received buffer

	ECommResetBuffers,              //< 11 Reset the serial port buffers
	ECommWrite,                     //< 12 Write to the comm port
	ECommWriteCancel,               //< 13 Cancel an outstanding write
	ECommBreak,                     //< 14 Send a break
	ECommBreakCancel,               //< 15 Cancel sending of break
	ECommCancel,                    //< 16 Cancel any outstanding reads or writes
	ECommConfig,                    //< 17 Get comm port configuration
	ECommSetConfig,                 //< 18 Set comm port configuration
	ECommCaps,                      //< 19 Get the capabilities of the comm port
	ECommSetMode,                   //< 20 Set server buffering mode

	ECommGetMode,                   //< 21 Get server buffering mode
	ECommSignals,                   //< 22 Read serial port control lines
	ECommSetSignalsToMark,          //< 23 Set control lines high (1)
	ECommSetSignalsToSpace,         //< 24 Set control line low (0)
	ECommReceiveBufferLength,       //< 25 Get the size of the receive buffer
	ECommSetReceiveBufferLength,    //< 26 Set the size of the receive buffer
	ECommClose,                     //< 27 Close the comm port
	ECommDbgMarkHeap,               //< 28 Set a heap mark in the Comms server
	ECommDbgCheckHeap,              //< 29 Check the heap mark in the Comms server
	ECommDbgMarkEnd,                //< 30 Set the heap mark end in the Comms server

	ECommDbgFailNext,               //< 31 Emulate fail next heap allocation in the comm server
	ECommDbgSetDebugPrintMask,      //< 32 Set the debug print mask
	ECommDbgDoDumpDebugInfo,        //< 33 Dump debug info

	// Here starts version 02 extensions
	ECommGetRole,                   //< 34 Get the current role
	ECommNotifySignals,             //< 35 Notify when signals change
	ECommNotifySignalsCancel,       //< 36 Cancel an outstanding signals notify
	ECommNotifyFlowControl,         //< 37 Notify when flow control changes
	ECommNotifyFlowControlCancel,   //< 38 Cancel an outstanding flow control notify
	ECommGetFlowControl,            //< 39 Get the current status of flow control
	ECommNotifyConfigChange,        //< 40 Notify when config change

	ECommNotifyConfigChangeCancel,  //< 41 Cancel an outstanding config notify
	ECommNotifyBreak,               //< 42 Send break
	ECommNotifyBreakCancel,	        //< 43 Cancel an outstanding sending of break
	ECommNotifyDataAvailable,       //< 44 Notify when data available in Rx buffer
	ECommNotifyDataAvailableCancel, //< 45 Cancel an outstanding notify data available
	ECommNotifyOutputEmpty,         //< 46 Notify when Tx buffer is empty
	ECommNotifyOutputEmptyCancel,   //< 47 Cancel an outstanding output empty notify
	// Here ends version 02 extensions

	ECommSetAccess,                 //< 48 Set the access mode defined by TCommAccess
	ECommDebugState,                //< 49 Get the debug state
	ECommOpenWhenAvailable,         //< 50 Open port when it is available
	ECommOpenWhenAvailableCancel    //< 51 Cancel open port when it is available
	};

#endif // COMMIPC_H
