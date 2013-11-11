#ifndef __MNETCONDIALOGPROCACCESS_H__
#define __MNETCONDIALOGPROCACCESS_H__
/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file MNetConDialogProcAccess.h
 @internalComponent
*/


class MDialogProcessorObserver;
class TConnectionPrefs;

class MNetConDialogProcAccess
/**
To allow the requests access to the dialog processor

@internalComponent
*/
	{
public:

	/**
	Display a dialog to the user asking them to select an IAP to connect to

	@param aObserver the object to receive notification when the dialog box completes
	@param aPrefs the connection preferences used to select this IAP
	*/
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs) = 0;

	/**
	Display a dialog to the user asking them to select an IAP to connect to

	@param aObserver the object to receive notification when the dialog box completes
	@param aPrefs the connection preferences used to select this IAP
	@param aLastError if this is not the 1st connection attempt this is the error of the last attempt
	*/
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, TInt aLastError) = 0;

	/**
	Display a dialog to the user asking the user to select a modem and location entry from CommDb

	@param aObserver the object to receive notification when the dialog box completes
	*/
	virtual void SelectModemAndLocation(MDialogProcessorObserver& aObserver) = 0;

	/**
	Display a dialog to the user warning that a new connection is about to be attempted to the specified IAP

	@param aObserver the object to receive notification when the dialog box completes
	@param aPrefs the connection preferences used to select this IAP
	@param aNewIapName the name of the IAP used for the connection.
	@param aNewConnectionNames this parameter is no-longer used.
	@param aLastError if this is not the 1st connection attempt this is the error of the last attempt
	*/
	virtual void WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError) = 0;

	/**
	Display a dialog to the user asking whether to reconnect a failed connection

	@param aObserver the object to receive notification when the dialog box completes
	*/
	virtual void Reconnect(MDialogProcessorObserver& aObserver) = 0;

	/**
	Cancel all outstanding dialog processor requests
	*/
	virtual void CancelEverything() = 0;

	virtual ~MNetConDialogProcAccess() = 0;
	};

#endif /* #ifndef __MNETCONDIALOGPROCACCESS_H__ */

