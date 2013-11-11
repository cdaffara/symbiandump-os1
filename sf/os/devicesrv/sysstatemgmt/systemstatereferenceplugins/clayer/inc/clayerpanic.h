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
//

#ifndef __CLAYERPANIC_H__
#define __CLAYERPANIC_H__

/**
 * Defines the panic category for the SSM compatibility layer
 *
 * @internalComponent
 */
_LIT(KPanicCLayer, "SsmClayer");

/**
 * Defines all possible panic codes within SSM compatibility layer
 * 
 * @internalComponent
 */
enum TClayerPanicCodes
	{
	/**
	 * Inidicates that there is an outstanding request when there was not one expected.
	 */
	EClayerOutstandingRequest,
	/**
	 * Indicates that an attempt was made to complete a null request.
	 */
	ECLayerCompletingNull,
	/**
	 * Indicates that the response for a command was unexpected.
	 */
	ECLayerUnexpectedCommandResponse,
	/**
	 * Indicates that an invalid sub-state was passed to the compatibility layer.
	 */
	ECLayerInvalidSubState,
	/**
	 * Indicates that the dispatch queue tried to grow past the maximum length.
	 */
	ECLayerMaximumQueueSize,
	/**
	 * Inidicates that an unexpect command response was received.
	 */
	ECLayerUnexpectedResponse,
	/**
	 * Inidicates that an unknown command response was received.
	 */
	ECLayerUnknownCommandResponse,
	/**
	 * Indicates that an attempt was made to cancel a command which had not been dispatched.
	 */
	ECLayerInCorrectDispatchedCancel,
	/**
	 * Indicates that an attempt has occured to read the state event buffer, but it is null.
	 */
	ECLayerNullStateEventBuffer,
	/**
	 * Indicates that an attempt has occured to read the SIM event buffer, but it is null.
	 */
	ECLayerNullSimEventBuffer,
		/**
	 * Indicates that the note type is invalid.
	 */
	ECLayerInvalidNoteType,
	/**
	 * Indicates that the resource reader is NULL.
	 */
	ECLayerInvalidResourceReader,
	/**
	 * Indicates that command list has a command with condition info when it is not expected to have any.
	 */
	ECLayerConditionalInfoNotImplemented
	};

#define CLAYER_PANIC(Category) User::Panic(KPanicCLayer, Category)
	
#endif // __CLAYERPANIC_H__
