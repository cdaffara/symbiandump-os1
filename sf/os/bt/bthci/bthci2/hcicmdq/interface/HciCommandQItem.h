// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HCICOMMANDQITEM_H
#define HCICOMMANDQITEM_H

#include <e32base.h>

class CHCICmdQCompletionTimer;
class CHCICmdQController;
class CHctlCommandFrame;
class MHCICommandAllocator;
class MHCICommandQueueClient;
class CHCICommandBase;


/**
Encapsulates a Command frame so that it can be placed on and handled by
the CHCICmdQController.

The class is designed to be used by the Command Queue Controller

@see CHCICmdQController
*/
NONSHARABLE_CLASS(CHCICommandQItem) : public CBase
	{
friend class CHCICmdQController;
public: // exported
	IMPORT_C static CHCICommandQItem* NewL(MHCICommandAllocator& aCommandAllocator,
										   MHCICommandQueueClient& aCmdProgressRecipient,
										   CHCICommandBase* aCommand);
	IMPORT_C ~CHCICommandQItem();

	IMPORT_C CHCICommandBase& Command() const;

	IMPORT_C MHCICommandQueueClient* Client() const;

	IMPORT_C TAny* QdpData() const;
	IMPORT_C void SetQdpData(TAny* aQdpData);
	
	IMPORT_C TBool ReceivedCmdStatusEvent() const;
	IMPORT_C TBool IsInitialisation() const;
	IMPORT_C TBool IsParent() const;
	IMPORT_C TBool IsPreChild() const;
	IMPORT_C TBool IsPostChild() const;
	IMPORT_C TBool IsNormal() const;
	IMPORT_C TUint SentCount() const;

public:
	/**
	@internalComponent
	*/
	enum TType
		{
		/**
		A CHCICommandQItem is this type until QDP::MhcqdiDoesCommandRequireWorkaround(...) is called on it.
		*/
		EIndeterminate = 0,
		
		/**
		A CHCICommandQItem is this type if QDP::MhcqdiDoesCommandRequireWorkaround(...) returned EFalse for this object.
		*/
		ENormal = 1,
		
		/**
		A CHCICommandQItem is this type if QDP::MhcqdiDoesCommandRequireWorkaround(...) returned ETrue for this object.
		*/
		EParent = 2,
		
		/**
		A CHCICommandQItem is set to this type after it is returned by QDP::GetPreChild.
		*/
		EPreChild = 3,
		
		/**
		A CHCICommandQItem is set to this type after it is returned by QDP::GetPostChild.
		*/
		EPostChild = 4,
		};

public:

	CHctlCommandFrame& Frame() const;

	TUint CommandQId() const;
	void SetCommandQId(TUint aId);
	
	void SetReceivedCmdStatusEvent(TBool aReceivedCmdStatusEvent);
	
	void SetInitialisationCmd();

	TBool DuplicatedOpcode() const;
	void SetDuplicatedOpcode(TBool aDuplicatedOpcode);
	
	void StartCompletionTimer(TUint aMilliseconds, CHCICmdQController& aController);
	void CancelCompletionTimer();

	TType Type() const;
	void SetType(TType aType);
	
	void CommandSent();

	void FormatFrame();
	
	void DetachClient();
	
private:
	CHCICommandQItem(MHCICommandQueueClient& aCmdProgressRecipient, 
						CHCICommandBase* aCommand);
	void ConstructL(MHCICommandAllocator& aCommandAllocator);
	
private:
	/**
	Command state bit flags
	*/
	enum TCommandStateBits 
		{
		/**
		Indicates that a Command Status Event has been processed 
		for the command
		*/
		EReceivedCmdStatusEvent	= 0x01, 
		
		/**
		Indicates that a duplicate command has been detected.
		*/
		EDuplicatedOpcode		= 0x02,
		
		/**
		Indicates that the command was added as an Initialisation command.
		*/
		EInitialisationCmd		= 0x04,
		};
	
private: // owned
	TDblQueLink 				iLink;
	TUint						iCommandQId;
	TType 						iType;
	
	/**
	This is a bit mask of the possible TCommandStateBits states
	*/
	TUint8						iCommandState;
	TUint8						iSentCounter;
	
	/**
	The timer runs between when the command is sent and when it completes. 
	If the timeout value given is 0 then no timer will be run.
	
	NB a Command Status event doesn't stop the timer.
	*/
	CHCICmdQCompletionTimer*	iCompletionTimer;
	
	CHCICommandBase* 			iCommand;	
	CHctlCommandFrame*			iCmdFrame;

private:
	/**
	Owned by this class in the sense that we will delete this pointer (after notifying
	the licensee provided QDP) but this class must not otherwise touch this data. It
	is to be used only by the licensee provided QDP.
	*/
	TAny*						iQdpData;

private: //unowned

	MHCICommandQueueClient*		iCmdProgressRecipient;
	};


#endif // HCICOMMANDQITEM_H 
