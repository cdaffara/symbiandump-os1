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


#ifndef CPOSCONTROLLERMANAGER_H
#define CPOSCONTROLLERMANAGER_H

#include <e32base.h>
#include <e32hashtab.h>
#include <lbspositioninfo.h>

class CPosControllerBase;
class CPosSimulationPositioner;

/**
 *  Singleton class that manages positioning controllers
 */
NONSHARABLE_CLASS( CPosControllerManager ) : public CBase
	{
public:
	/**
	 * Returns the singleton instance. Each call to InstanceL()
	 * should be matched witch a call to ReleaseInstance()
	 * @return Instance of class CPosControllerManager
	 * @leave System wide error code
	 */
	static CPosControllerManager* InstanceL();
	/**
	 * Releases the singleton instance.
	 * @param aSimPos CPosSimulationPositioner which releases manager
	 */
	void ReleaseInstance(const CPosSimulationPositioner& aSimPos);
	/**
	 * Gets or creates new controller for given CPosSimulationPositioner
	 * @param aSimPos CPosSimulationPositioner which requesting for controller
	 * @return CPosControllerBase instance. Be careful the owner of this instance
	 * is still CPosControllerManager. To prevent memory leaks you have to call 
	 * ReleaseInstance(CPosSimulationPositioner&) function when You finish using controller.
	 */
	CPosControllerBase& ControllerL(const CPosSimulationPositioner& aSimPos);
	/**
	 * Returns iLastFix
	 * @return iLastFix
	 */
	TPositionInfo& GetLastFix();
	/**
	 * Sets value of iLastFix
	 * @param aPosInfo Position info which will set iLastFix
	 */
	void SetLastFix(TPositionInfo& aPosInfo);
	/**
	 * Returns iLastFixIsPartial
	 * @return iLastFixIsPartial
	 */
	TBool GetLastFixIsPartial();
	/**
	 * Sets value of iLastFixIsPartial
	 * @param aPartial defines value to which iLastFixIsPartial will be set
	 */
	void SetLastFixIsPartial(TBool aPartial);

private:
	CPosControllerManager();
	virtual ~CPosControllerManager();
	void ConstructL();
	CPosControllerBase* CreateControllerL(const CPosSimulationPositioner& aSimPos);
	TBool RepositoryTrpValueL();

private:
	/**
	 * Amount of references to the singleton instance.
	 * Instance will be deleted when count reaches zero.
	 */
	TUint iReferenceCount;
	/**
	 * Pointer to single controller object which will be used
	 * by all positioners. Works only when central repository key
	 * time relative playback is on.
	 */
	CPosControllerBase* iTrpController;
	/**
	 * Map between positioners and controllers. 
	 * This array is used for proper creation and deletion of controllers.
	 */
	RPtrHashMap<CPosSimulationPositioner, CPosControllerBase> iPositionerControllerMap;
	/**
	 * Defines if time relative playback is on or off.
	 */
	TBool iTrp;
	/**
	 * Stores last valid fix, 
	 */
	TPositionInfo	iLastFix;
	/**
	 * Indicates if last valid fix was partial
	 */
	TBool			iLastFixIsPartial;

	};

#endif //CPOSCONTROLLERMANAGER_H
