#ifndef __CNETCONDLGPROCESSOR_H__
#define __CNETCONDLGPROCESSOR_H__
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <agentdialog.h>

#include "MNetConDialogProcAccess.h"

/**
Wrap the interface to the Dialog Processor implementation in a non-virtual interface class.

@internalComponent
*/
NONSHARABLE_CLASS(CNetConDlgProcessor) : public CBase
	{
public:
	static CNetConDlgProcessor* NewL();
	static CNetConDlgProcessor* NewL( MNetConDialogProcAccess *aImpl, TBool aOwnImpl );

	virtual ~CNetConDlgProcessor();

	void CancelEverything();
	void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs);
	void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, TInt aLastError );
	void SelectModemAndLocation(MDialogProcessorObserver& aObserver);
	void WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError );
	void Reconnect(MDialogProcessorObserver& aObserver);

protected:
	explicit CNetConDlgProcessor( MNetConDialogProcAccess* aImpl );

private:
	MNetConDialogProcAccess* GetImpl() const;

private:	
	MNetConDialogProcAccess* const iDialogProc; //< The actual implementation of the dialog processor to use
	};

#endif /* #ifndef __CNETCONDLGPROCESSOR_H__ */

