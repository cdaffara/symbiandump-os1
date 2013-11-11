// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmswp.h>
#include <ssm/ssmcommandfactory.h>

#include "ssmdebug.h"
#include "ssmpanic.h"
#include "ssmcommandlistimpl.h"
#include "ssmcommandparameters.h"
#include "ssmcommandutilprovider.h"

#include "cmdwaitforapparcinit.h"
#include "cmdmultiplewait.h"
#include "cmdpublishswp.h"
#include "cmdcustomcommand.h"
#include "cmdpublishsystemstate.h"
#include "cmdpoweroff.h"
#include "cmdfinalisedrives.h"
#include "cmdloadsup.h"
#include "cmdpersisthalattributes.h"
#include "cmdpublishswp.h"
#include "cmdreqswpchange.h"
#include "cmdamastarter.h"
#include "cmdstartprocess.h"
#include "cmdstartapp.h"
#include "cmdcreateswp.h"
#include "cmdsetpandskey.h"

/**
 * Releases the command passed in as a parameter.
 * 
 * Designed for use with a TCleanupItem as follows:
@code
CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
@endcode
 * Where cmd is a MSsmCommand pointer.
 * 
 * @internalComponent
 * @released
*/
static void CleanupCommand(TAny* aCmd)
	{
	MSsmCommand* const cmd = static_cast<MSsmCommand*>(aCmd);
	cmd->Release();
	}

/**
 * Constructs and returns a new wait for app-arc init command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructWaitForApparcInitCommandLC(TCmdErrorSeverity aSeverity)
	{
	MSsmCommand* cmd = CCmdWaitForApparcInit::NewL(aSeverity);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new multiple wait command.
 * 
 * @param aTimeout Timeout in milliseconds for the multiple wait command to wait for.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructMultipleWaitCommandLC(TInt32 aTimeout)
	{
	MSsmCommand* cmd = CCmdMultipleWait::NewL(aTimeout);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new AMA starter command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aDscId The UID of the DSC that this command should start.
 * 
 * @see KDefaultSymbianDsc
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructAMAStarterCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId)
	{
	MSsmCommand* cmd = CCmdAmaStarter::NewL(aSeverity, aExecutionBehaviour, aDscId);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new start application command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aStartupProperties The properties of the started application. Ownership of this object is not transferred to the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructStartApplicationCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties)
	{
	MSsmCommand* cmd = CCmdStartApp::NewL(aSeverity, aStartupProperties);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new start process command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aStartupProperties The properties of the started process. Ownership of this object is not transferred to the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructStartProcessCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties)
	{
	MSsmCommand* cmd = CCmdStartProcess::NewL(aSeverity, aStartupProperties);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new custom command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aCustomInfo The properties of the custom command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructCustomCommandCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aCustomInfo)
	{
	MSsmCommand* cmd = CCmdCustomCommand::NewL(aSeverity, aExecutionBehaviour, aCustomInfo);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new finalise drives command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructFinaliseDrivesCommandLC(TCmdErrorSeverity aSeverity)
	{
	MSsmCommand* cmd = CCmdFinaliseDrives::NewL(aSeverity);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a load SSM Utility Plugin command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aRetries The number of times to attempt to load the specified SSM Utility Plugin.
 * @param aInfo The properties of the SSM Utility Plugin to load.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructLoadSupCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt aRetries, const TSsmSupInfo& aInfo)
	{
	MSsmCommand* cmd = CCmdLoadSup::NewL(aSeverity, aExecutionBehaviour, aRetries, aInfo);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new persist HAL attributes.
 *
 * This command calls BaflUtils::PersistHAL().
 * 
 * @param aSeverity The severity to use for the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructPersistHalAttributesCommandLC(TCmdErrorSeverity aSeverity)
	{
	MSsmCommand* cmd = CCmdPersistHalAttributes::NewL(aSeverity);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new power off command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aPowerEvent The type of power off event represented by the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructPowerOffCommandLC(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent)
	{
	MSsmCommand* cmd = CCmdPowerOff::NewL(aSeverity, aPowerEvent);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new publish System-wide Property command.
 * 
 * This command should only be used by a System-wide Property policy to 
 * publish the System-wide Property value or values controlled by that policy.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aSwp The System-wide Property information.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructPublishSwpCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp)
	{
	MSsmCommand* cmd = CCmdPublishSwp::NewL(aSeverity, aExecutionBehaviour, aSwp);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new request System-wide Property change command.
 * 
 * This command will request a change to a System-wide Property, using the
 * appropriate policy for the System-wide Property to determine required actions.
 * 
 * This is equivalent to calling RSsmStateManger::RequestSwpChange().
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aSwp The System-wide Property information used in the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructReqSwpChangeCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp)
	{
	MSsmCommand* cmd = CCmdReqSwpChange::NewL(aSeverity, aExecutionBehaviour, aSwp);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new publish system state command.
 * 
 * This command should only be used by the current system state policy 
 * to publish a change in system state value.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aInfo The system state information used in the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructPublishSystemStateCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo)
	{
	MSsmCommand* cmd = CCmdPublishSystemState::NewL(aSeverity, aExecutionBehaviour, aInfo);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new publish system state command.
 * 
 * This command should only be used by the current system state policy 
 * to publish a change in system state value.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aInfo The system state information used in the returned command.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructCreateSwpCommandLC(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwp, TDesC& aFilename)
	{
	MSsmCommand* cmd = CCmdCreateSwp::NewL(aSeverity, aSwp, aFilename);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new set publish and subscribe key command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aCategory The category of the P+S key to be set.
 * @param aKey The key value of the P+S key to be set.
 * @param aValue The value to set the P+S key to.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructSetPAndSKeyCommandLC(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue)
	{
	MSsmCommand* cmd = CCmdSetPAndSKey::NewL(aSeverity, aCategory, aKey, aValue);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
Factory function to create a command from a MSsmCommand object.
@param aSsmCommand is reference of MSsmCommand which needs to be created.
@param aDeferredList List of commands which has execution behaviour as ESsmDeferredWaitForSignal in command list.
@@param aUtilProvider CSsmCommandUtilProvider reference. Only few commands require CSsmCommandUtilProvider reference 
@internalComponent
@panic ECmdBadCommand if the command type is invalid
@see TSsmCommandType
*/
CSsmCommandBase* SsmCommandFactory::ConstructCommandFromCommandLC(const MSsmCommand& aSsmCommand, TArray<MSsmCommand*>& aDeferredList, CSsmCommandUtilProvider* aUtilProvider)
    {
    CSsmCommandBase* cmd = NULL;
    TSsmCommandType const cmdType = aSsmCommand.Type();
    DEBUGPRINT2A("Adding command type in CLE commandlist: %d", cmdType);    
    
    switch(cmdType)
        {
        case ESsmCmdWaitForApparcInit:
            {
            const CCmdWaitForApparcInit& cmdWaitForApparc = dynamic_cast<const CCmdWaitForApparcInit&>(aSsmCommand);
            cmd = CCmdWaitForApparcInit::NewLC(cmdWaitForApparc, aUtilProvider);
            break;
            }
        case ESsmCmdMultipleWait:
            {
            const CCmdMultipleWait& cmdMultipleWait = dynamic_cast<const CCmdMultipleWait&>(aSsmCommand);
            cmd = CCmdMultipleWait::NewLC(cmdMultipleWait, aDeferredList);
            break;
            }
        case ESsmCmdAMAStarter:
            {
            const CCmdAmaStarter& cmdAmaStart = dynamic_cast<const CCmdAmaStarter&>(aSsmCommand);
            cmd = CCmdAmaStarter::NewLC(cmdAmaStart);
            break; 
            }
        case ESsmCmdStartProcess:
            {
            const CCmdStartProcess& cmdProcess = dynamic_cast<const CCmdStartProcess&>(aSsmCommand);
            cmd = CCmdStartProcess::NewLC(cmdProcess, aUtilProvider);
            break;
            }
        case ESsmCmdStartApp:
            {
            const CCmdStartApp& cmdApp = dynamic_cast<const CCmdStartApp&>(aSsmCommand);
            cmd = CCmdStartApp::NewLC(cmdApp, aUtilProvider);
            break;
            }
        case ESsmCmdPublishSystemState:
            {
            const CCmdPublishSystemState& cmdPublishSystemState = dynamic_cast<const CCmdPublishSystemState&>(aSsmCommand);
            cmd = CCmdPublishSystemState::NewLC(cmdPublishSystemState);
            break;
            }
        case ESsmCmdPublishSwp:
            {
            const CCmdPublishSwp& cmdPublishSwp = dynamic_cast<const CCmdPublishSwp&>(aSsmCommand);
            cmd = CCmdPublishSwp::NewLC(cmdPublishSwp);
            break;
            }
        case ESsmCmdReqSwProperty:
            {
            const CCmdReqSwpChange& cmdReqSwpProperty = dynamic_cast<const CCmdReqSwpChange&>(aSsmCommand);
            cmd = CCmdReqSwpChange::NewLC(cmdReqSwpProperty);
            break;
            }
        case ESsmCmdLoadSup:
            {
            const CCmdLoadSup& cmdLoadSup = dynamic_cast<const CCmdLoadSup&>(aSsmCommand);
            cmd = CCmdLoadSup::NewLC(cmdLoadSup);
            break;
            }
        case ESsmCmdPowerOff:
            {
            const CCmdPowerOff& cmdPowerOff = dynamic_cast<const CCmdPowerOff&>(aSsmCommand);
            cmd = CCmdPowerOff::NewLC(cmdPowerOff);
            break;
            }
        case ESsmCmdFinaliseDrives:
            {
            const CCmdFinaliseDrives& cmdFinaliseDrives = dynamic_cast<const CCmdFinaliseDrives&>(aSsmCommand);
            cmd = CCmdFinaliseDrives::NewLC(cmdFinaliseDrives);
            break;
            }
        case ESsmCmdPersistHalAttributes:
            {
            const CCmdPersistHalAttributes& cmdPersistHalAttributes = dynamic_cast<const CCmdPersistHalAttributes&>(aSsmCommand);
            cmd = CCmdPersistHalAttributes::NewLC(cmdPersistHalAttributes);
            break;
            }
        case ESsmCmdCustomCommand:
            {
            const CCmdCustomCommand& cmdCustomCommand = dynamic_cast<const CCmdCustomCommand&>(aSsmCommand);
            cmd = CCmdCustomCommand::NewLC(cmdCustomCommand, aUtilProvider);
            break;
            }
        case ESsmCmdCreateSwp:
            {
            const CCmdCreateSwp& cmdCreateSwp = dynamic_cast<const CCmdCreateSwp&>(aSsmCommand);
            cmd = CCmdCreateSwp::NewLC(cmdCreateSwp);
            break;
            }
        case ESsmCmdSetPAndSKey:
            {
            const CCmdSetPAndSKey& cmdSetPAndSKey = dynamic_cast<const CCmdSetPAndSKey&>(aSsmCommand);
            cmd = CCmdSetPAndSKey::NewLC(cmdSetPAndSKey);
            break;
            }
        default:
            {
            __ASSERT_ALWAYS( EFalse, PanicNow(KPanicCmdList, ECmdBadCommand));
            break;
            }
        }
    
    return cmd;
    }

/**
Factory function to create a command from a stream.
@param aType Type of the command which needs to be created.
@param aReadStream Stream containing the data for the command.
@param aDeferredList List of commands which has execution behaviour as ESsmDeferredWaitForSignal in command list.
@internalComponent
@panic ECmdBadCommand if the command type is invalid
@see TSsmCommandType
@see RReadStream
*/
CSsmCommandBase* SsmCommandFactory::ConstructCommandFromStreamLC(TSsmCommandType aType, RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredList)
	{
	CSsmCommandBase* cmd = NULL;
	
	switch(aType)
		{
	case ESsmCmdWaitForApparcInit:
		cmd = CCmdWaitForApparcInit::NewL(aReadStream);
		break;
	case ESsmCmdMultipleWait:
		cmd = CCmdMultipleWait::NewL(aReadStream, aDeferredList);
		break;
	case ESsmCmdAMAStarter:
		cmd = CCmdAmaStarter::NewL(aReadStream);
		break;
	case ESsmCmdStartApp:
		cmd = CCmdStartApp::NewL(aReadStream);
		break;
	case ESsmCmdStartProcess:
		cmd = CCmdStartProcess::NewL(aReadStream);
		break;
	case ESsmCmdPublishSystemState:
		cmd = CCmdPublishSystemState::NewL(aReadStream);
		break;
	case ESsmCmdPublishSwp:
		cmd = CCmdPublishSwp::NewL(aReadStream);
		break;
	case ESsmCmdReqSwProperty:
		cmd = CCmdReqSwpChange::NewL(aReadStream);
		break;
	case ESsmCmdLoadSup:
		cmd = CCmdLoadSup::NewL(aReadStream);
		break;
	case ESsmCmdPowerOff:
		cmd = CCmdPowerOff::NewL(aReadStream);
		break;
	case ESsmCmdFinaliseDrives:
		cmd = CCmdFinaliseDrives::NewL(aReadStream);
		break;
	case ESsmCmdPersistHalAttributes:
		cmd = CCmdPersistHalAttributes::NewL(aReadStream);
		break;
	case ESsmCmdCustomCommand:
		cmd = CCmdCustomCommand::NewL(aReadStream);
		break;
	case ESsmCmdCreateSwp:
		cmd = CCmdCreateSwp::NewL(aReadStream);
		break;
	case ESsmCmdSetPAndSKey:
		cmd = CCmdSetPAndSKey::NewL(aReadStream);
		break;
	default:
		__ASSERT_ALWAYS( EFalse, PanicNow(KPanicCmdList, ECmdBadCommand));
		break;
		}

	CleanupStack::PushL(cmd);
	return cmd;
	}

/**
Factory function to create a command from a resource.
@param aType Type of the command which needs to be created.
@param aCommandParameters Object data from a resource file
@internalComponent
@panic ECmdBadCommand if the command type is invalid
@see TSsmCommandType
@see TSsmCommandParameters
*/
CSsmCommandBase* SsmCommandFactory::ConstructCommandFromResourceLC(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters)
	{
	CSsmCommandBase* cmd = NULL;
	
	switch(aType)
		{
	case ESsmCmdWaitForApparcInit:
		cmd = CCmdWaitForApparcInit::NewL(aCommandParameters);
		break;
	case ESsmCmdMultipleWait:
		cmd = CCmdMultipleWait::NewL(aCommandParameters);
		break;
	case ESsmCmdAMAStarter:
		cmd = CCmdAmaStarter::NewL(aCommandParameters);
		break;
	case ESsmCmdStartApp:
		cmd = CCmdStartApp::NewL(aCommandParameters);
		break;
	case ESsmCmdStartProcess:
		cmd = CCmdStartProcess::NewL(aCommandParameters);
		break;
	case ESsmCmdPublishSwp:
		cmd = CCmdPublishSwp::NewL(aCommandParameters);
		break;
	case ESsmCmdPublishSystemState:
		cmd = CCmdPublishSystemState::NewL(aCommandParameters);
		break;
	case ESsmCmdReqSwProperty:
		cmd = CCmdReqSwpChange::NewL(aCommandParameters);
		break;
	case ESsmCmdLoadSup:
		cmd = CCmdLoadSup::NewL(aCommandParameters);
		break;
	case ESsmCmdPowerOff:
		cmd = CCmdPowerOff::NewL(aCommandParameters);
		break;
	case ESsmCmdFinaliseDrives:
		cmd = CCmdFinaliseDrives::NewL(aCommandParameters);
		break;
	case ESsmCmdPersistHalAttributes:
		cmd = CCmdPersistHalAttributes::NewL(aCommandParameters);
		break;
	case ESsmCmdCustomCommand:
		cmd = CCmdCustomCommand::NewL(aCommandParameters);
		break;
	case ESsmCmdCreateSwp:
		cmd = CCmdCreateSwp::NewL(aCommandParameters);
		break;
	case ESsmCmdSetPAndSKey:
		cmd = CCmdSetPAndSKey::NewL(aCommandParameters);
		break;
	default:
		__ASSERT_ALWAYS( EFalse, PanicNow(KPanicCmdList, ECmdBadCommand));
		break;
		}

	cmd->SetConditionalInformation(aCommandParameters.ConditionalInformation());
	CleanupStack::PushL(cmd);
	return cmd;
	}


#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
 * Constructs and returns a new wait for app-arc init command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructWaitForApparcInitCommandLC(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdWaitForApparcInit::NewL(aSeverity, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new AMA starter command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aDscId The UID of the DSC that this command should start.
 * @param aPriority The priority of the command in the list
 * 
 * @see KDefaultSymbianDsc
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructAMAStarterCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdAmaStarter::NewL(aSeverity, aExecutionBehaviour, aDscId, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new start application command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aStartupProperties The properties of the started application. Ownership of this object is not transferred to the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructStartApplicationCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdStartApp::NewL(aSeverity, aStartupProperties, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new start process command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aStartupProperties The properties of the started process. Ownership of this object is not transferred to the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructStartProcessCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdStartProcess::NewL(aSeverity, aStartupProperties, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new custom command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aCustomInfo The properties of the custom command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructCustomCommandCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aCustomInfo, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdCustomCommand::NewL(aSeverity, aExecutionBehaviour, aCustomInfo, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a load SSM Utility Plugin command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aRetries The number of times to attempt to load the specified SSM Utility Plugin.
 * @param aInfo The properties of the SSM Utility Plugin to load.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructLoadSupCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt aRetries, const TSsmSupInfo& aInfo, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdLoadSup::NewL(aSeverity, aExecutionBehaviour, aRetries, aInfo, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new publish System-wide Property command.
 * 
 * This command should only be used by a System-wide Property policy to 
 * publish the System-wide Property value or values controlled by that policy.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aSwp The System-wide Property information.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructPublishSwpCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdPublishSwp::NewL(aSeverity, aExecutionBehaviour, aSwp, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new request System-wide Property change command.
 * 
 * This command will request a change to a System-wide Property, using the
 * appropriate policy for the System-wide Property to determine required actions.
 * 
 * This is equivalent to calling RSsmStateManger::RequestSwpChange().
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aSwp The System-wide Property information used in the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructReqSwpChangeCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdReqSwpChange::NewL(aSeverity, aExecutionBehaviour, aSwp, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new publish system state command.
 * 
 * This command should only be used by the current system state policy 
 * to publish a change in system state value.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aInfo The system state information used in the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructPublishSystemStateCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdPublishSystemState::NewL(aSeverity, aExecutionBehaviour, aInfo, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new publish system state command.
 * 
 * This command should only be used by the current system state policy 
 * to publish a change in system state value.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aInfo The system state information used in the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructCreateSwpCommandLC(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwp, TDesC& aFilename, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdCreateSwp::NewL(aSeverity, aSwp, aFilename, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new set publish and subscribe key command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aCategory The category of the P+S key to be set.
 * @param aKey The key value of the P+S key to be set.
 * @param aValue The value to set the P+S key to.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructSetPAndSKeyCommandLC(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdSetPAndSKey::NewL(aSeverity, aCategory, aKey, aValue, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

/**
 * Constructs and returns a new multiple wait command.
 * 
 * @param aTimeout Timeout in milliseconds for the multiple wait command to wait for.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructMultipleWaitCommandLC(TInt32 aTimeout, const TUint16 aPriority)
	{
	MSsmCommand* cmd = CCmdMultipleWait::NewL(aTimeout, aPriority);
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

#else //SYMBIAN_SSM_FLEXIBLE_MERGE
/**
 * Constructs and returns a new wait for app-arc init command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructWaitForApparcInitCommandLC(TCmdErrorSeverity /*aSeverity*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new AMA starter command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aDscId The UID of the DSC that this command should start.
 * @param aPriority The priority of the command in the list
 * 
 * @see KDefaultSymbianDsc
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructAMAStarterCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, const TUid& /*aDscId*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new start application command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aStartupProperties The properties of the started application. Ownership of this object is not transferred to the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructStartApplicationCommandLC(TCmdErrorSeverity /*aSeverity*/, CSsmStartupProperties* /*aStartupProperties*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new start process command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aStartupProperties The properties of the started process. Ownership of this object is not transferred to the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructStartProcessCommandLC(TCmdErrorSeverity /*aSeverity*/, CSsmStartupProperties* /*aStartupProperties*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new custom command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aCustomInfo The properties of the custom command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructCustomCommandCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, CSsmCustomCommandInfo& /*aCustomInfo*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a load SSM Utility Plugin command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the command.
 * @param aRetries The number of times to attempt to load the specified SSM Utility Plugin.
 * @param aInfo The properties of the SSM Utility Plugin to load.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructLoadSupCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, TInt /*aRetries*/, const TSsmSupInfo& /*aInfo*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new publish System-wide Property command.
 * 
 * This command should only be used by a System-wide Property policy to 
 * publish the System-wide Property value or values controlled by that policy.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aSwp The System-wide Property information.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructPublishSwpCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, const TSsmSwp& /*aSwp*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}
	
/**
 * Constructs and returns a new request System-wide Property change command.
 * 
 * This command will request a change to a System-wide Property, using the
 * appropriate policy for the System-wide Property to determine required actions.
 * 
 * This is equivalent to calling RSsmStateManger::RequestSwpChange().
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aSwp The System-wide Property information used in the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructReqSwpChangeCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, const TSsmSwp& /*aSwp*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new publish system state command.
 * 
 * This command should only be used by the current system state policy 
 * to publish a change in system state value.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aInfo The system state information used in the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructPublishSystemStateCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, TSsmPublishSystemStateInfo& /*aInfo*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new publish system state command.
 * 
 * This command should only be used by the current system state policy 
 * to publish a change in system state value.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aExecutionBehaviour The execution behaviour of the returned command.
 * @param aInfo The system state information used in the returned command.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructCreateSwpCommandLC(TCmdErrorSeverity /*aSeverity*/, const TSsmSwp& /*aSwp*/, TDesC& /*aFilename*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new set publish and subscribe key command.
 * 
 * @param aSeverity The severity to use for the returned command.
 * @param aCategory The category of the P+S key to be set.
 * @param aKey The key value of the P+S key to be set.
 * @param aValue The value to set the P+S key to.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructSetPAndSKeyCommandLC(TCmdErrorSeverity /*aSeverity*/, const TUid& /*aCategory*/, TUint /*aKey*/, TInt /*aValue*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}

/**
 * Constructs and returns a new multiple wait command.
 * 
 * @param aTimeout Timeout in milliseconds for the multiple wait command to wait for.
 * @param aPriority The priority of the command in the list
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmCommand* SsmCommandFactory::ConstructMultipleWaitCommandLC(TInt32 /*aTimeout*/, const TUint16 /*aPriority*/)
	{
	User::Leave(KErrNotSupported);
	return NULL; //will never reach here; keeping compiler happy
	}
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE

