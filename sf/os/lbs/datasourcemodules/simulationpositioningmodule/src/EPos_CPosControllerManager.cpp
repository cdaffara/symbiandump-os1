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

#include "EPos_CPosControllerManager.h"
#include <e32debug.h> 
#include <f32file.h>
#include <e32std.h>
#include <centralrepository.h>
#include "EPos_CPosSimulationPositioner.h"
#include "EPos_CPosControllerBase.h"
#include "SimulationPSYInternalCRKeys.h"
#include "EPos_CPosNmeaController.h"
#include "EPos_CPosFixedDataController.h"
#include "EPos_CPosDataSimulationController.h"
_LIT(KDefaultNmeFileName, "\\resource\\default.nme");

// The unit tests (te_lbssimulationsuite.exe) are built as an EXE and cannot 
// use TLS.  The macro below redefines Dll to a new class TestDll that provides 
// an EXE equivalent.
#ifdef SIMPSYTEST
#define Dll TestDll
class TestDll
	{
public:
	static TAny* Tls() { return reinterpret_cast<TAny*>(iInternal); };
	static TInt SetTls(const TAny* aAny) 
		{ 
		iInternal = reinterpret_cast<TUint32>(aAny); 
		return KErrNone;
		};
	static void FreeTls() { iInternal = 0; };
private:
	static TUint32 iInternal;
	};
TUint32 TestDll::iInternal = 0;
#endif

TUint32 HashFunc(const CPosSimulationPositioner& aSimPos)
	{
	return DefaultHash::Integer(reinterpret_cast<TInt>(&aSimPos));
	}

TBool IdentityFunc(const CPosSimulationPositioner& aSimPos1, const CPosSimulationPositioner& aSimPos2)
	{
	return &aSimPos1 == &aSimPos2;
	}

// ---------------------------------------------------------------------------
// Returns the singleton instance.
// ---------------------------------------------------------------------------
//
CPosControllerManager* CPosControllerManager::InstanceL()
	{
	CPosControllerManager* self = NULL;

	TAny* tlsPtr = Dll::Tls();
	if (!tlsPtr)
		{
		self = new (ELeave) CPosControllerManager;
		CleanupStack::PushL(self);
		self->ConstructL();
		User::LeaveIfError(Dll::SetTls(self));
		CleanupStack::Pop(self);
		}
	else
		{
		self = static_cast<CPosControllerManager*> (tlsPtr);
		++self->iReferenceCount;
		}

	return self;
	}

// ---------------------------------------------------------------------------
// Releases the singleton instance.
// ---------------------------------------------------------------------------
//
void CPosControllerManager::ReleaseInstance(const CPosSimulationPositioner& aSimPos)
	{
	//Delete controller only in Independent Data Set mode.
	//when there is more then one controller
	//In TRP mode the only existing controller is deleted in destructor
	if (!iTrp)
		{
		CPosControllerBase* p = iPositionerControllerMap.Find(aSimPos);
		delete p;
		iPositionerControllerMap.Remove(&aSimPos);
		}
	

	TAny* tlsPtr = Dll::Tls();
	__ASSERT_DEBUG( tlsPtr, User::Panic(
					_L( "CPosControllerManager" ), KErrNotFound ) );

	CPosControllerManager* self = static_cast<CPosControllerManager*> (tlsPtr);
	if (--self->iReferenceCount == 0)
		{
		Dll::FreeTls();
		delete self;
		}
	}

CPosControllerManager::CPosControllerManager() :
	iReferenceCount(1),
	iPositionerControllerMap(THashFunction32<CPosSimulationPositioner>(&HashFunc), TIdentityRelation<CPosSimulationPositioner>(&IdentityFunc))
	{
	iLastFixIsPartial = EFalse; 
	}

void CPosControllerManager::ConstructL()
	{
	iTrp = RepositoryTrpValueL();
	}

/**
 * Private destructor so users cannot directly delete this instance.
 * They must call ReleaseInstance()
 */
CPosControllerManager::~CPosControllerManager()
	{
	iPositionerControllerMap.Close();
	if (iTrpController && iTrp)
		{
		delete iTrpController;
		}
	}

/**
 * Check in central repository if time relative playback (TRP) in on.
 * @return ETrue if TRP is on, EFalse if it's off.
 */
TBool CPosControllerManager::RepositoryTrpValueL()
	{
	TInt val = 0;
	CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
	User::LeaveIfError( repository->Get(KCRKeySimPSYCommonDataSet, val) );
	CleanupStack::PopAndDestroy(repository);
	return static_cast<TBool>(val);
	}

CPosControllerBase& CPosControllerManager::ControllerL(
		const CPosSimulationPositioner& aSimPos)
	{
	CPosControllerBase* p = NULL;
	if(!iTrp)
		{
		p = iPositionerControllerMap.Find(aSimPos);
		}
	else
		{
		p = iTrpController;
		}
	//if there is no controller associated with give positioner
	//just create new one
	if(!p)
		{
		p = CreateControllerL(aSimPos);
		//in Time Relative Playback we should have only one controller
		//in old mode we need to create each time new one.
		if (iTrp)
			{
			iTrpController = p;
			}
		}
	return *p;
	}

/**
 * Create controller and pairs it with given CPosSimulationPositioner 
 * Controller type is determined by filename given in central repository.
 * @param aSimPos Pointer to positioner that will be connecter with new controller
 */
CPosControllerBase* CPosControllerManager::CreateControllerL(const CPosSimulationPositioner& aSimPos)
	{
	CPosControllerBase* retController = NULL;
	//getting file name from repository
	TFileName fileName;
	CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
	User::LeaveIfError(repository->Get(KCRKeySimPSYSimulationFile, fileName));
	CleanupStack::PopAndDestroy(repository);

	fileName.LowerCase();
	TParsePtrC fileNamePtr(fileName);

	//based on file extension creating correct type controller
	TInt err = KErrNone;
	if (fileNamePtr.Ext().Compare(KPosNMEAFileExtension) == 0)
		{
			TRAP(err, retController =
					CPosNmeaController::NewL(fileName, iTrp));
		}
	else if (fileNamePtr.Ext().Compare(KPosSimulationFileExtension) == 0)
		{
			TRAP(err, retController =
					CPosDataSimController::NewL(fileName, iTrp));
		}
	else if (fileName.Length() == 0)
		{
		// No file selected so use the default.nme file from
		// \resource directory
		TFileName DllName;
		TParse nmeFileName;
#ifndef SIMPSYTEST
		Dll::FileName(DllName);
#else
		_LIT(KTestPathTxt, "Z:\\resource\\");
		DllName.Copy(KTestPathTxt);
#endif

		nmeFileName.Set(KDefaultNmeFileName, &DllName, NULL);

			TRAP(err, retController =
					CPosNmeaController::NewL(nmeFileName.FullName(), iTrp));
		if (err == KErrPathNotFound || err == KErrNotFound 
				|| err == KErrCorrupt)
			{
				TRAP(err, retController = CPosFixedDataController::NewL());
			}
		}
	else
		{
		err = KErrNotSupported;
		}
	User::LeaveIfError(err);
	//after valid controller creation adding it to map of positioners 
	//and controllers to be able then properly delete it when it will be unneeded
	if(!iTrp)
		{
		iPositionerControllerMap.InsertL(&aSimPos, retController);
		}
	return retController;
	}

TPositionInfo& CPosControllerManager::GetLastFix()
	{
	return iLastFix;
	}

void CPosControllerManager::SetLastFix(TPositionInfo& aPosInfo)
	{
	iLastFix = aPosInfo;
	}

TBool CPosControllerManager::GetLastFixIsPartial()
	{
	return iLastFixIsPartial;
	}

void CPosControllerManager::SetLastFixIsPartial(TBool aPartial)
	{
	iLastFixIsPartial = aPartial;
	}
