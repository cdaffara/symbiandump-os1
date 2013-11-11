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
// Server-side representation of a target client, which may have multiple 
// server-side sessions open
// 

/**
 @file
 @internalComponent
*/

#ifndef MESSAGESENDOBSERVER_H
#define MESSAGESENDOBSERVER_H

class MRemConMessageSendObserver
	{
public:

	/**
	Called by CRemConServer to provide the result of an attempt to send a given message to a single remote.
	This should be used in cases where a command can only be delivered to zero or one remotes (e.g. a notify command)
	AND the observer does not require notification in advance of the number of remotes.
	The observer should complete its client request with the error given here.
	@param aMessage The CRemConMessage whose send was attempted.
	@param aError The result of the send attempt (KErrNone in success).
	*/
	virtual void MrcmsoMessageSendResult(const CRemConMessage& aMessage, TInt aError)=0;

	/**
	Called by CRemConServer to notify the observer that an attempt is about to be made to send a particular message
	to one or more remotes.
	This allows CRemConServer to notify the session in advance of the number of remotes to which the send will be attempted.
	If the number of remotes is 1, this method can still be used to provide the session with notification of the number of remotes.
	MrcmsoMessageSendOneOrMoreResult() will be invoked for each remote to which an attempt is made to send the message.
	The observer should keep track of the number of remotes to which the message is still to be sent and should complete its client
	request once all remotes have been tried.
	@param aMessage The CRemConMessage whose send is about to be attempted.
	@param aNumRemotes The number of remotes that the message is to be sent to.
	*/
	virtual void MrcmsoMessageSendOneOrMoreAttempt(const CRemConMessage& aMessage, TUint aNumRemotes)=0;

	/**
	Called by CRemConServer to notify the observer that an attempt is about to be made to send a particular message to n 
	further remotes.
	This can be used when the exact number of remotes is not known at first. This should not be invoked after the first
	attempt to send the message has been made.
	The observer should keep track of the number of remotes to which the message is still to be sent and should complete its client
	request once all remotes have been tried.
	@param aMessage The CRemConMessage whose send is about to be attempted.
	@param aNumRemotes The number of additional remotes that this message is to be sent to.
	*/
	virtual void MrcmsoMessageSendOneOrMoreIncremental(const CRemConMessage& aMessage, TUint aNumRemotes)=0;

	/**
	Called by CRemConServer to indicate that a message send attempt to one or more remotes could not be made.
	For example, the send attempt may have failed because as the message could not be addressed.
	The observer should complete its client	request immediately with the given error.
	@param aMessage The CRemConMessage whose send was attempted.
	@param aError An error preventing the attempt from being made.
	*/
	virtual void MrcmsoMessageSendOneOrMoreAttemptFailed(const CRemConMessage& aMessage, TInt aError)=0;

	/**
	Called by CRemConServer to provide the result of an attempt to send a given message to one of a number of remotes.
	A call is made to this method for each remote for which an attempted send was made.
	The observer should keep track of the number of remotes to which the message is still to be sent and should complete its client
	request once all remotes have been tried.
	If the message send result is unexpected (i.e. the observer believes that no remotes should have been tried), then the observer
	may ignore this result. For example, the observers client may have cancelled its send request for this message.
	@param aMessage The CRemConMessage whose send was attempted.
	@param aError The result of the send attempt (KErrNone in success).
	*/
	virtual void MrcmsoMessageSendOneOrMoreResult(const CRemConMessage& aMessage, TInt aError)=0;

	/**
	Called by CRemConServer to indicate that the sending of a given message to a remote was abandoned.
	This provides a mechanism whereby CRemConServer may drop a message without sending an error back
	to the client. For example, if the message is not permitted by the TSP for the remote, or fails to match a
	command, then the attempt for this remote should be abandoned but the client may still be completed
	without error. If a message cannot be sent because of an error, then MrcmsoMessageSendResult() should be used.
	The observer should keep track of the number of remotes to which the message is still to be sent and should complete its client
	request once all remotes have been tried.
	If the message send result is unexpected (i.e. the observer believes that no remotes should have been tried), then the observer
	may ignore this result. For example, the observers client may have cancelled its send request for this message.
	@param aMessage The CRemConMessage whose send was attempted.
	*/
	virtual void MrcmsoMessageSendOneOrMoreAbandoned(const CRemConMessage& aMessage)=0;

	};

#endif // MESSAGESENDOBSERVER_H
