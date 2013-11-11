/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "clbslocmonitorsession.h"
#include "clbslocmonitorserver.h"
#include "lbsdevloggermacros.h"
#include "lbslocmonitorserverdata.h"
#include "clbsareapositionersubsession.h"
#include "clbsconversionpositionersubsession.h"

// Called by the CServer2 code to complete construction of the session 
void CLbsLocMonitorSession::CreateL()
	{
	LBSLOG(ELogP1,"CLbsLocMonitorSession::CreateL()");
	// MUST call the server framework method
	CSecureSessionSubSessionBase::CreateL();
	}
	
void CLbsLocMonitorSession::ServiceMessageL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"CLbsLocMonitorSession::ServiceMessageL()");
	// Service session level messages in here
	// Only messages are: delete database and cancel delete DB.
	switch(aMessage.Function())
		{
		case EWipeOutDb:
		HandleDatabaseWipeoutL(aMessage);
		break;

		case ECancelDbWipeOut:
		//DB deletion is synchronous internally. Just complete the cancel.
		aMessage.Complete(KErrCompletion);
		break;

		default:
		__ASSERT_DEBUG(EFalse, User::Invariant());
		break;
		}	
	}

void CLbsLocMonitorSession::HandleDatabaseWipeoutL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"CLbsLocMonitorSession::HandleDatabaseWipeout()");
	const CLbsLocMonitorServer* server = static_cast<const CLbsLocMonitorServer*>(Server());
	
	TInt err = server->ReadRequestHandler().DeleteDatabaseL();
		
	aMessage.Complete(err);
	}

void CLbsLocMonitorSession::ServiceMessageError(const RMessage2& /*aMessage*/, const TInt /*aError*/)
	{
	LBSLOG(ELogP1,"CLbsLocMonitorSession::ServiceMessageError()");
		// service leaves from the above in here !!!
	}

// ------------------------------------------------------------------------------------------

/*
Method required by the Server Framework to create an instance
of a subsession of a certain type
*/
MSubSessionImpl* CSubSessionBase::GetImplL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"CSubSessionBase::GetImplL()");
	// Read in the subsession type
	TLbsLocMonitorSubSessionType subSessType;
	TPckg<TLbsLocMonitorSubSessionType> subSessTypeBuf(subSessType);
	MessageUtils::Read(aMessage, 0, subSessTypeBuf); // data is always in slot zero
		
	// Create the subsession, depending on the type requested
	MSubSessionImpl* subSession(NULL);
	switch (subSessType)
		{
		case ELocMonitorSubSessionTypeAreaPositioner:
			{
			subSession = CLbsAreaPositionerSubsession::NewL();
			break;
			}
		case ELocMonitorSubSessionTypeConversionPositiner:
		    {
		    subSession = CLbsConversionPositionerSubsession::NewL();
		    break;
		    }
		case ELocMonitorSubSessionTypeUnknown:
		    {
			_LIT(KPanicUnknownSubSession, "Unknown SubSession Type");
			__ASSERT_DEBUG(0, MessageUtils::PanicClient(aMessage, KPanicUnknownSubSession, 0));
		    break;
		    }
		default:
			{
			_LIT(KPanicSubSessionType, "Unrecognised SubSession Type");
			__ASSERT_DEBUG(0, MessageUtils::PanicClient(aMessage, KPanicSubSessionType, 0));
			break;
			}
		}
	return subSession;
	}
