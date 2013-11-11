# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# To Do:
# - excludeByAddr{} and multiple objects using same memory address ?
# - distinguish between threads for context, eg $lastDispatchDestination
# parselog.pl [<arguments>] <log file>
# Arguments:
# -x <ini file>		Specify ini file containing, amongst other things, names of classes and/or messages
# to exclude.  Contains sections named "[ExcludeClass]", "[ExcludeMessage]",
# "[ExcludeShortName]" and "[ExcludeIpc]" each followed by the names
# to exclude, one per line.  Note that "ExcludeShortName" section refers
# to the shortened class name as displayed on the output, whereas "ExcludeClass"
# refers to original full class names.  Also contains a "[MessageArguments]" section
# for describing which arguments to display for specified messages.
# -p			Include posted messages in the output as well as received messages
# -X<n>			Exclude certain information from the display.  <n> is a decimal number which
# is composed of one or more of the following values logical-or'ed together:
# 1	exclude activities being started
# 2	exclude activity transitions
# 4	exclude parking+unparking
# 8	mark source .LOG line milestones in left column
# -s<n>			Include additional information.  <n> is a decimal number which
# is composed of one or more of the following values logical-or'ed together:
# 1	include client IPC requests
# 2	include client IPC completions (async requests only)
# 4	include ESOCK session creation
# 8 display activity name along with each tuple line (e.g. "(IpCprNoBearer) (CoreNetStates::TSendBindTo, CoreNetStates::TAwaitingBindToComplete)")
# 16 display AddClient/RemoveClient operations
# -o <path>			Write HTML and SVG output files into directory <path>

use strict;

require 'getopts.pl';

my $version = "2.3 (16/06/10)";

#
# Internal Options
#
# BUG BEWARE: A timestamp or milestone displayed on lefthand side overwrites any fast count delta that would have been displayed there
my $fastCount = 0;	# Add fast count delta on lefthand side.

my $globalIndex = 1;
my $lifeStage = 0;	# 0 = not running or shutting down, 1 = during boot, 2 = main phase
my $outputPath = "";			# path to output directory specified by "-o"

our($opt_p,$opt_s,$opt_x,$opt_X, $opt_o);

Getopts("ps:x:X:o:");

$outputPath = processPathArgument($opt_o);

### to get 1st line of latest log: type log.txt | perl -e "while(<>){if(/^#Logging started/){$a=$.;print $a.' '}};print $a"

#
# objectNameParse is an array of hashes used to shorten class names and specify left-to-right
# object placement order.  It is an array of:
#
# { SubString => <sub-string>, Replacement =>  <sub-string replacement>, Order => <order> }
#
# For each object name, the array is scanned.  Any occurrences of <sub-string> are
# replaced with <sub-string replacement>.  The first non-zero value of <order> is taken
# to be the placement order.
#
# Note that order of hashes within the array is important - for example, "ConnectionProvider" must appear after all
# entries that specify sub-strings that themselves contain the string "ConnectionProvider".

my @objectNameParse;

#
# Variables/constants concerned with Arguments file.
#

use constant ArgumentSignedDecimal => 0;
use constant ArgumentNode => 1;
use constant ArgumentHex => 2;
my %argumentFormatToConstant = ( "node" => ArgumentNode, "x" => ArgumentHex );	# used to have several other options
my %arguments;

# Support for AddClient/RemoveClient
my %clients;
my %clientTypeToDirection = ( "100" => "<", "200" => "^", "400" => "v", "800" => ">");

# Support for EThreadIdentification
my $pendingProcessCreate;
my %NThreadToName;
my %DProcessToName;
my %processUsingESock = ( "DhcpServ" => 1, "TE_RConnectionSuite" => 1, "dnd" => 1 );
my $sessionGenerationCount = 1;
my %uniqueProcessName;

my $logline = 1;
my $mileStoneInterval = 250;
my $lastMileStone = 1;
my $lastDispatchDestination;
my $excludeAllSingleTuples;
my $lastTimestamp;
my $lastFastCount;

my %excludeByAddr;
my %excludeMessage;
my %excludeClass;
my %excludeShortName;
my %excludeSingleTuple;

my %activityNode;
my %activityName;

my %msgAddrToIpc;
my %msgAddrToExe;
my %symtab;
my %excludeIpc;
my %uniqueNames;
my %deadList;
my %subSessTab;	# maps subsession addr to node addr
my @notableText;
my @mangled;

readIniFile();

while (<>) {
	if(!($opt_X & 8) && ($logline - $lastMileStone) > $mileStoneInterval)
	{
		$lastMileStone += $mileStoneInterval;
		print "l #$lastMileStone\n";
	}

	print stderr "WARNING: Probable deleted heap at $logline:\n$_\n" if(m/-555819298|dededede/i);
	print stderr "WARNING: Probable uninitialised stack at $logline:\n$_\n" if(m/-858993460|cccccccc|690563369|29292929/i);

#OLD:
#esock	Mesh	a	47	W6: TCFSignatureBase:	DispatchL(): Sender=0c181b9c, Recipient=0c181b9c, Message=ECFStateChange, Activity=ECFActivityNull [0000] 1000,0
#NEW:
#12362,194,2,3745950947,0,0x00a1c488,ENodeMessages: [STARTBLOCK=TransportReceiver::DispatchMessage]  aSender:  [Address=[iPtr=0x0d9fa138] ]  aRecipient:  [Address=[iPtr=0x0db79148] [iNodeCtx=ActivityNoBearer] ]  aMessage:  [Signature=TSigAddrUpdate] [iMessageId=0x10285a57:DataClientRouted] [iAddrUpdate=[iSrcSockAddr=] [iDestSockAddr=] [iProtocolId=0x00000011] [iIapId=0x00000001] ]  [ENDBLOCK=TransportReceiver::DispatchMessage]

	if (/.*BLOCK=Dispatch/) {
		doFastCount() if ($fastCount);
		parsePostDispatchMessageLine("BLOCK=Dispatch", "r");
	}
#OLD:
#esock	DCMsgs	a	47	W6: RClientII:	PostMessage(): Sender=0c18241c, Recipient=0c182374, Message=ECFSelect, Activity=ECFActivityNull [0000]
#NEW:
#5072,194,2,4009228503,0,0x00a1b628,ENodeMessages: [STARTBLOCK=TransportSender::PostMessage]  aPostFrom:  [Address=[iPtr=0x0d925558] ]  aPostTo:  [Address=[iPtr=0x0d925558] ]  args:  [Signature=TSigStateChange] [Message=StateChange] [iMessageId=[iMessageId=EStateChange] [iRealmId=0x10285a56] ] [iStateChange=[iStage=0x000009c5] [iError=0x00000000] ]  [ENDBLOCK=TransportSender::PostMessage]
	elsif ($opt_p == 1 && (/.*BLOCK=Post/)) {
		doFastCount() if ($fastCount);
		parsePostDispatchMessageLine("BLOCK=Post", "p");
		}
#OLD:
#CFNode	MetaConn	a	47	W6: CIpTierManagerFactory 0c1822d0:	created [MCFNode 0c1822d8] [96]
#NEW:
#250,194,1,1864213751,0,0x00f3e114,ENodeMessages: CFactoryContainerNode 0e571204:	created [ANode 0e571220] [44]
	elsif (/.*created \[ANode=0x([0-9A-Fa-f]{8})\]/) {
		my ($name,$realaddr,$nodeaddr) = m/(\w+) ([0-9A-Fa-f]{8}):\tcreated \[ANode=0x([0-9A-Fa-f]{8})\]/;
		$realaddr =~ s/^0x//;
		$nodeaddr =~ s/^0x//;
		my $doExclude = 1;
		if (! defined $excludeClass{$name})
			{
			my $order = parseObjectName(\$name);
			$name = uniquifyName($name);
			if (!defined $excludeShortName{$name})
				{
#				if (defined $symtab{$nodeaddr})
#					{
#					print "**** SYMTAB already defined - $nodeaddr, $name, $symtab{$nodeaddr}\n";
#					}
				$symtab{$nodeaddr} = $name;
				doFastCount() if ($fastCount);
				print "oc $name $order $nodeaddr\n";
				print "t $name ($nodeaddr created)\n";
				$doExclude = 0;
				delete $deadList{$nodeaddr} if defined($deadList{$nodeaddr});
				$subSessTab{$realaddr} = $nodeaddr;
				}
			}
		if ($doExclude)
			{
			$excludeByAddr{$nodeaddr} = 1;
			}
		}
	elsif (/:\t~.*\[ANode=0x([0-9A-Fa-f]{8})\]/) {
		my ($name,$realaddr,$nodeaddr) = m/(\w+) ([0-9A-Fa-f]{8}):\t~.*\[ANode=0x([0-9A-Fa-f]{8})\]/;
		$realaddr =~ s/^0x//;
		$nodeaddr =~ s/^0x//;
		if (! defined $excludeClass{$name} && defined($symtab{$nodeaddr}) && !defined($deadList{$nodeaddr}))
			{
			$deadList{$nodeaddr} = 1;
			$name = $symtab{$nodeaddr};
			doFastCount() if ($fastCount);
			print "od $name $nodeaddr\n";
			print "t $name ($nodeaddr destroyed)\n";
			delete $subSessTab{$realaddr};
			}
		}
	elsif (/^#Time = (.*)$/)
		{
		print "l $1\n";
		}
	elsif (/W0: SocketServer::InitL\(\) Done/)
		{
		print "l ^Booting\n";
		$lifeStage = 1;
		}
	elsif ($lifeStage == 1 && /CDealer::ProcessConfigurationComplete\(\)/)
		{
		print "l ^Booted\n";
		$lifeStage = 2;
		}
	elsif ($lifeStage == 2 && /CFUnbindMessageReceived/)
		{
		print "l ^Shutting-down\n";
		$lifeStage = 0;
		}
#1061,194,2,1212990838,0,0x00a1b488,ENodeMessages: [STARTBLOCK=Context]  CNodeActivityBase 0d922584:	StartL->starting activity Node:  [ANode=0x0d9229e0]  Sender:  [Address=[iThread=0x0006] [iPtr=0x0d922338] ]  Recipient:  [Address=[iThread=0x0006] [iPtr=0x0d9229e0] ]  aContext.iMessage:  [Signature=0x00000000:0xfdfdfdfd]  [STARTBLOCK=Activity]   [Activity=MCprControlClientJoin] [CurrentTriple=Idle]  [ENDBLOCK=Activity]  [ENDBLOCK=Context]
	elsif (/starting activity.*?\[ANode=0x([0-9A-Fa-f]{8})\].*\[Activity=(\w*)\]/)
		{
		if (!($opt_X & 1))
			{
			my $dest = $1;
			my $activity = $2;

			if ($activity eq "Undefined") {
				# An activity name of "Undefined" may mean that the non-debug macros for defining
				# the activity and triples have been used, as these do not compile-in the activity name.
				$activity = "Activity name unknown"
			}

			if (! defined $excludeByAddr{$dest})
				{
				if (m/C\w+ ([0-9a-fA-F]{8}):/) {
					my $activityAddr = $1;
					$activityNode{$activityAddr} = $dest;
					$activityName{$activityAddr} = $activity;
					# emit activity create tag
					doFastCount() if ($fastCount);
					print "ac ", objectName($dest), " $activityAddr $activity\n";
					print "t ", objectName($dest), " {0,200,0} ($activity)\n";
					}
				else {
					# OLD OLD OLD
					# Hackery - "Node=" in the "starting activity" line actually contains the
					# address of the ACFNodeBase rather than the MCFNode - the latter of which we work
					# with in this script - hence the use of $lastDispatchDestination.  However, sanity check
					# whether the two addresses are at least within 8 bytes of each other.
					#
					#if (abs(hex($dest) - hex ($lastDispatchDestination)) <= 8)
					#	{
					#	print "t ", objectName($lastDispatchDestination), " {0,200,0} ($activity)\n";
					#	}
					}
				}
			else
				{
#				print "Ignored $logline - $_\n";
				}
			}
		}
	elsif ($opt_s != 0 && (/CSockSession[\(\s]/ || /CSockSubSession\(.*\):/ || /CWorkerSubSession\(/ ||
						   /RSafeMessage\(/) || /RMessage2::Complete/)
		{
		if (($opt_s & 1) && /CSockSession\(.{8}\):\s+ServiceL, Message\((.{8})\) \[(\w+)\] "(.*)" int3=(.{8})/)
			{
			my $msgAddr = $1;
			my $exe = $3;
			my $ipc = $2;
			my $subSessNode = $subSessTab{$4};
			$ipc .= "($symtab{$subSessNode})" if defined $subSessNode;	# append the name of the node
			$msgAddrToIpc{$msgAddr} = $ipc;
			$msgAddrToExe{$msgAddr} = $exe;
			if (! excludeIpc($ipc))
				{
				$exe = truncateExeName($exe);
				$exe = uniqueProcessName($exe);
				# adding "!" to beginning of object name forces the parseseq.pl script to display
				# the object despite the fact that it would normally hide objects from the final display
				# that have no actual messages to/from them.
				doFastCount() if ($fastCount);
				print "pn line $logline\n";
				print "t !$exe $ipc\n";
				}
			}
		elsif (($opt_s & 4) && /CSockSession\s*(.{8}):\s+ConstructL.*"(.*)"/)
			{
			# W0: CSockSession 0be839a8:	ConstructL() pid=4c "Te_Cap_RConnDHCP_sc.exe"
			my $session = $1;
			my $exe = $2;
			$exe = truncateExeName($exe);
			$exe = uniqueProcessName($exe);
#			print "oc !$exe\n";
			if ($opt_s & 32) {
				print "ac !$exe $session Session-$session-$sessionGenerationCount\n";
				++$sessionGenerationCount;
				}
			print "t !$exe CSockSession $session\n";
			$processUsingESock{$exe} = 1;
			}
		elsif (($opt_s & 4) && /~CSockSession\s*\((.{8})\):\s*"(.*)"/)
			{
			#W0: ~CSockSession(00682bf0):	"Te_RConnectionSuite.exe"
			my $session = $1;
			my $exe = $2;
			$exe = truncateExeName($exe);
			$exe = uniqueProcessName($exe);
			print "t !$exe ~CSockSession $session\n";
			if ($opt_s & 32) {
				print "ad $session\n";
				}
			}
		elsif (($opt_s & 2) &&
			(/CWorkerSubSession\(.{8}\):\s*CompleteMessage\((.{8})\) with (.+), session .{8}/ ||
			 /~CESockClientActivityBase..{8}.\s*RMessage2::Complete \((.{8})\) with ([-\d]+)\./ ||
			 /RSafeMessage\(.{8}\)::Complete\((.{8})\) with ([-\d]+)/))
			{
#			 /ProcessMessageL, session=.{8}, RMessage2::Complete \((.{8})\) with ([-\d]+)\./ ||
			# W6: CPlayer:	ProcessMessageL, session=0be839a8, RMessage2::Complete (00de5538) with 0.
			# W0: CWorkerSubSession(0c941bc4):	CompleteMessage(00de1a6c) with -3, session 0be839a8.
			# W6: ~CESockClientActivityBase=0c941f8c, RMessage2::Complete (00de5538) with 0.
			# RSafeMessage(00e01590)::Complete(cd21cb80) with -36

			my $msgAddr = $1;
			my $ret = $2;
			my $exe = $msgAddrToExe{$msgAddr};
			my $ipc = $msgAddrToIpc{$msgAddr};
			if($ipc =~ /^E.+Create$/)
				{
				# For subsession creation try to add the generated name of the object (which necessarily wasn't available when the request arrived)
				my $subSessNode = $subSessTab{$4};
				$ipc .= "($symtab{$subSessNode})" if defined $subSessNode;	# append the name of the node
				}

			$exe = truncateExeName($exe);
			$exe = uniqueProcessName($exe);
			if ($ipc ne "")
				{
				doFastCount() if ($fastCount);
				print "pn line $logline\n";
				print "t !$exe $ipc = $ret\n";
				$msgAddrToIpc{$msgAddr} = "";
				$msgAddrToExe{$msgAddr} = "";
				}
			 if (/~CESockClientActivityBase.(.{8})/) {
				print "ad $1\n";
				ClearActivity($1);
				}
			}
		}
	elsif (/stray message received.*\[iMessageId=.*?:(.*)\]/i)
		{
		print "t ", objectName($lastDispatchDestination), " (STRAY $1)\n";
		}
#1077,194,2,1212991263,0,0x00a1b488,ENodeMessages: [STARTBLOCK=Context]  CNodeActivityBase 0d922584:	StartL->activity started Node:  [ANode=0x0d9229e0]  Sender:  [Address=[iThread=0x0006] [iPtr=0x0d922338] ]  Recipient:  [Address=[iThread=0x0006] [iPtr=0x0d9229e0] ]  aContext.iMessage:  [Signature=0x00000000:0xfdfdfdfd]  [STARTBLOCK=Activity]   [Activity=MCprControlClientJoin] [CurrentTriple=MCprStates::TDecrementBlockingDestoryAndAddControlClientAndSendJoinCompleteIfRequest->NULL]  [ENDBLOCK=Activity]  [ENDBLOCK=Context]
#843,195,1,1212618492,0,0x00a1b488,EMeshMachine: CNodeActivityBase:	Accept->First transition: ConnStates::TProcessStateChange->MeshMachine::TAwaitingStateChange [ANode=0x0b1ff948]
#897,194,2,1212884309,0,0x00a1b488,ENodeMessages: [STARTBLOCK=Activity]  CNodeActivityBase 0d921eb8:	Next->transition happened [ANode=0x0b1ff948] [Activity=ConnectionStart] [CurrentTriple=ConnStates::TSelectMetaPlane->TECABState<CoreNetStates::TAwaitingBindTo>]  [ENDBLOCK=Activity]
#1237,195,1,2340065279,0,0x00dfff38,EMeshMachine: CNodeActivityBase 0cb427ec:	Next->match [ANode=0x0cb42c48] [Activity=MCprBinderRequest] [Triple=PRStates::TCreateDataClient->CoreNetStates::TAwaitingDataClientJoin]

	elsif (($opt_X & 2) == 0 && (/(StartL)->activity started.*?\[ANode=0x([0-9a-f]{8})\].*\[Activity=(\S*)\].*\[Triple=(\S*)\]/ ||
				     /(Accept)->first transition.*?\[ANode=0x([0-9a-f]{8})\].*\[Activity=(\S*)\].*\[Triple=(\S*)\]/) ||
				     /(Next)->transition\s*\[ANode=0x([0-9a-f]{8})\].*\[Activity=(\S*)\].*\[Triple=(\S*)\]/ ||
					 /(Next)->match\s*\[ANode=0x([0-9a-f]{8})\].*\[Activity=(\S*)\].*\[Triple=(\S*)\]/)
		{
		# $2 = ANode, $3 = Activity, $4 = Triple
		my $matchType = $1;
		my $text;
		my $objAddr;
		my $prefix = "";
		if ($matchType eq "Accept") {
			if ($excludeAllSingleTuples || $excludeSingleTuple{$3}) {
				# skip any excluded single tuples
				next;
			}
			$objAddr = $2;
			$text = $4;
			$prefix = "($3) ";
			# for first transitions, the normal activity creation/idle log lines don't appear
			# so prefix the transition line with the activity name: "activity: (transition, state)"
		} else {
			$objAddr = $2;
			$text = $4;
			if ($opt_s & 8) {
				$prefix = "($3) ";
			}
		}
		if ($text ne "" && $text ne "Idle" && $text ne "Undefined")
			{
			$text =~ s/->/, /;
			if ($objAddr ne "")
				{
				doFastCount() if ($fastCount);
				print "t ", objectName($objAddr), " $prefix", "($text)\n";
				}
			else
				{
				print "t ", objectName($lastDispatchDestination), " $prefix","($text)\n";
				}
			}

		if (m/C\w+ ([0-9a-fA-F]{8}):/) {
			my $actAddr = $1;
#			if (defined $activityToNode{$actAddr})
#				{
				# map activity address to object address for "t" tag
#				$objAddr = $activityToNode{$actAddr};
#				}
			if ($text eq "Idle" || $text =~ m/, NULL/)
				{
				print "ad $actAddr\n";
				ClearActivity($actAddr);
				}
			}
		}
#1044,195,1,322007694,0,0x00a1c5e4,EMeshMachine: CNodeActivityBase 0dbe2584:	SetIdle
#1044,195,1,322007694,0,0x00a1c5e4,EMeshMachine: CNodeActivityBase 0dbe2584:	Abort
# CAREFUL: this catches all "CNodeActivityBase" log lines not captured earlier - carefulof positioning of this!
	elsif (($opt_X & 2) == 0 && /CNodeActivityBase ([0-9a-fA-F]{8}):\s*(\S+)\s*/) {
		my $actAddr = $1;
		my $action = $2;
		doFastCount() if ($fastCount);
		if ($action eq "SetIdle") {
			print "ad $actAddr\n";
			ClearActivity($actAddr);
			}
		elsif ($action eq "Abort") {
			print "t ", objectName($activityNode{$actAddr}), " ($activityName{$actAddr}) Abort()\n";
			}
		elsif ($action =~ m/Cancel\(\)/) {
			m/iPostedToId ([0-9a-f]{8})/;
			my $postedToId = $1;
			print "t ", objectName($activityNode{$actAddr}), " ($activityName{$actAddr}) Cancel().  Posted to ", objectName($postedToId), "\n";
			}
	}
	elsif (($opt_X & 4) == 0 && /UnparkState->unparked\s+\[ANode=0x([0-9a-fA-F]{8})\].*\[Activity=(\w+)\].*\[Triple=(.*?)\]/)
 	            {
	            print "t ", objectName($1), " {50,200,50} (Unparked $2, $3)\n";
 	            }
	elsif (($opt_X & 4) == 0 && /ParkState->parked\s+\[ANode=0x([0-9a-fA-F]{8})\].*\[Activity=(\w+)\].*\[Triple=(.*?)\]/) {
	            print "t ", objectName($1), " {50,200,50} (Parked $2, $3)\n";
 	            }
	elsif (/Synchronous call:/) {
		my ($src,$dest,$msg) = m/Synchronous call: From=(\S+)\s*To=(\S+) Func=(.+)$/;
		$lastDispatchDestination = $dest;
		if ((! defined $excludeByAddr{$src}) && (! defined $excludeByAddr{$dest}) && !defined($excludeMessage{$msg}))
			{
			doFastCount() if ($fastCount);
			print "pn line $logline\n";
			print "sc \"$msg ()\" ", objectName($src), " ", objectName($dest), "\n";
			}
		}
	elsif (($opt_s & 16) && (/AddClient/ || /RemoveClient/)) {
# ... ANodeBase 00983774:	AddClientL(00982c74, flags=0000, type=0200)
		if (/ANodeBase ([0-9a-f]{8}):\s*(\w*)\(([0-9a-f]{8}), flags=([0-9a-f]{4}), type=([0-9a-f]{4})\)/) {
			my ($node,$op,$client,$flags,$type) = ($1,$2,$3,$4,$5);
			$flags =~ s/^0+(\d)/\1/;			# remove leading zeroes
			$type =~ s/^0+(\d)/\1/;				# remove leading zeroes			
			$node = objectName($node);
			$client = objectName($client);
			if ($op eq "AddClientL") {
				$op = "added";
				$clients{$node}{$client} = $type;
			}
			elsif ($op eq "RemoveClient") {
				$op = "removed";
				delete $clients{$node}{$client};
			}
			my $clientList = "";
			foreach $client (keys %{$clients{$node}}) {
				my $direction = $clientTypeToDirection{$clients{$node}{$client}};
				$clientList .= " $client$direction";
				}
			if ($clientList) {
				$clientList = ": $clientList";
				}
			print "t ", $node, " {35,60,110} (${op} client ", $client, ", flags $flags, type $type)$clientList\n";
			}
		}
	elsif (($opt_s & 32) && /EThreadIdentification/) {
		if (/Process Create:\s*\[DProcess=0x(.{8})\]/) {
			# Process Create: [DProcess=0xc8057f38]
			$pendingProcessCreate = $1;
			}
		elsif (/Process Name:\s*\[NThread=0x(.{8})\] \[DProcess=0x(.{8})\] \[Name=(.*)\]/) {
			# Process Name: [NThread=0xc805b050] [DProcess=0xc8057f38] [Name=centralrepositorysrv.exe[10202be9]0001]
			my $NThread = $1;
			my $DProcess = $2;
			my $processName = $3;
			$processName =~ s/\[[0-9a-f]{8}\][0-9a-f]{4}$//;
			$processName = truncateExeName($processName);
			if ($pendingProcessCreate && processUsingESock($processName)) {
				#print STDERR "Process Name: NThread $NThread (($NThreadToName{$NThread})), DProcess $DProcess ($processName)\n";
				$processName = createUniqueProcessName($processName);
				print "oc !$processName 0 $DProcess\n";
				print "t !$processName (Process Created)\n";
			}
			$pendingProcessCreate = "";
			$DProcessToName{$DProcess} = $processName;
			}
		elsif (/Thread Create: \[NThread=0x(.{8})\] \[DProcess=0x(.{8})\] \[Name=(.*)\]\s*$/) {
			# Thread Create: [NThread=0xc805b050] [DProcess=0xc8057f38] [Name=Main] 
			if ($3 ne "Main" && DProcessUsingESock($2)) {
				#print STDERR "Thread Create: NThread $1 ($3), DProcess $2 ($DProcessToName{$2})\n";
				my $name = $DProcessToName{$2};
				if ($name) {
					print "t !$name (Thread \"$3\" Created)\n";
					}

				}
			$NThreadToName{$1} = $3;
			}
		elsif (/Thread Destroy: \[NThread=0x(.{8})\] \[DProcess=0x(.{8})\]/) {
			# Thread Destroy: [NThread=0xc8057140] [DProcess=0xc80561d0] 
			if (DProcessUsingESock($2)) {
				#print STDERR "Thread Destroy: NThread $1 ($NThreadToName{$1}), DProcess $2 ($DProcessToName{$2})\n";
				my $processName = $DProcessToName{$2};
				my $threadName = $NThreadToName{$1};
				if ($processName && $threadName ne "Main") {
					print "t !$processName (Thread \"$threadName\" Destroyed)\n";
					}
				}
			delete $NThreadToName{$1};
			}
		elsif (/Process Destroy: \[DProcess=0x(.{8})\]/) {
			# Process Destroy: [DProcess=0xc80561d0] 
			if (DProcessUsingESock($1)) {
				#print STDERR "Process Destroy: $1 ($DProcessToName{$1})\n";
				my $name = $DProcessToName{$1};
				if ($name) {
					$name = uniqueProcessName($name);
					print "od !$name $1\n";
					print "t !$name (Process Destroyed)\n";
					}
				}
			delete $DProcessToName{$1};
			# do not delete the $processUsingESock{} entry - it identifies processes that use ESock and whose
			# process/thread activity is worth display.  It is both statically initialised and dynamically built up.
			}
		}
	else {
		my $text = matchNotableText($_);
		if ($text ne "") {
			print "t ", objectName($lastDispatchDestination), " $text\n";
		}
	}
} continue {
	++$logline;
}

sub uniqueProcessName($)
{
	my $name = $_[0];
	if (! ($opt_s & 32)) {
		return $name;
	}
	my $unique = $uniqueProcessName{$name};
	if (!$unique || $unique == 1) {
		return $name;
	} else {
		return $name . "-" . $uniqueProcessName{$name};
	}
}

sub createUniqueProcessName($)
{
	my $name = $_[0];
	if (! ($opt_s & 32)) {
		return $name;
	}
	if (!defined($uniqueProcessName{$name})) {
		$uniqueProcessName{$name} = 1;
		return $name;
	} else {
		return $name . "-" . ++$uniqueProcessName{$name};
	}
}

sub DProcessUsingESock($)
{
	return processUsingESock($DProcessToName{$_[0]});
}

sub processUsingESock($)
{
	return $processUsingESock{$_[0]};
}

outputSymbols();

sub doFastCount()
{
	if (m/^\d+,\d+,\d+,(\d+),/) {
		if ($lastFastCount) {
			print "l ", $1 - $lastFastCount, "\n";
			$lastFastCount = $1;
		} else {
			$lastFastCount = $1;
		}
	}
}

#
# Parse log lines related to posting and dispatching messages and output appropriate command in log.seq file.
#
# Arguments:
#	$type		String at beginning of line to match (either "TransportReceiver::DispatchMessage" or "TransportSender::PostMessage")
#	$cmd		Type of command to output to log.seq file (either "r" for dispatch or "p" for post).

sub parsePostDispatchMessageLine($$)
{
	my ($type, $cmd) = @_;
	my ($mark,$src, $srcAfter, $dest, $rest) = m/$type\s*(.*?)iPtr=0x([^\]]+)(.*?)iPtr=0x([^\]]+)(.*)/;
	$_ = $rest;
	my ($destAfter, $msgRealm, $msg, $args) = m/(.*?)iMessageId=(.+?):(.+?)\]\s*(.*)/;
	if ($msgRealm eq "" || ($msgRealm eq "0x00000000" && $msg eq "0x0000")) {
		# If message is not available, try extracting the signature, which can be decoded or not
		# (e.g. "... aMessage:  [Signature=0x102822da:0x00000003] ..." or "... aMessage: [Signature=TCprRetrieveProgress] ...")
		# Use the signature as the message.
		($destAfter, $msg, $args) = m/(.*?)\[Signature=(.+?)\]\s*(.*)/;
		if ($msg =~ m/(.+?):(.+)/) {
			# undecoded signature - arrange to display hex values
			$msgRealm = $1;
			$msg = $2;
		}
	}

	# For decoded signatures with undecoded messages
	# (e.g. "... aMessage:  [Signature=TSigNumber] [iMessageId=0x10281ded:0x0001] ... ")
	# display message realm and id in hex.
	if ($msg =~ s/^0x//) {
		$msgRealm =~ s/^0x//;
		$msg = "$msgRealm:$msg";
	}
	my $activity = parseActivityIds(\$srcAfter, \$destAfter);
	$lastDispatchDestination = $dest;

	# Deal with any markers (which are added to the popup after the line number) and timestamps
	# (which are displayed on the left).
	my $popup;
	if ($mark =~ s/(w\d+\:\d+)//) {
		$popup = ", $1";
		if ($mark =~ m/(\d\d:\d\d:\d\d.\d)/) {
			# remove duplicate times
			if ($lastTimestamp ne $1) {
				$lastTimestamp = $1;
				print "l $1\n";
			}
		}
	}
	outputMessageSequenceCommand($cmd, $msg, $src, $dest, $activity, \$args, $popup);
}

#
# Output command to log.seq file
#
# Arguments:
#	$cmd		Command to output to log.seq ("r" or "p")
#	$msg		Message name (e.g. "Stop")
#	$src		Source node address in hex
#	$dest		Destination node address in hex
#	$activity	Activity name (e.g. "CprBindToRequest")
#	$lineRef	Reference to original log line.
#	$popup		Additional string to add to popup

sub outputMessageSequenceCommand($$$$$$)
{
	my ($cmd,$msg,$src,$dest,$activity,$lineRef,$popup) = @_;
	if ((! defined $excludeByAddr{$src}) && (! defined $excludeByAddr{$dest}) && !defined($excludeMessage{$msg})) {
		print "pn line $logline$popup\n";
		# extract any arguments from the end of the log line
		my $args = getArguments($lineRef, $msg);
		if ($args ne "") {
			if ($activity) {
				$cmd .= " \"$msg ($activity, $args)\"";
			} else {
				$cmd .= " \"$msg ($args)\"";
			}
		} else {
			$cmd .= " \"$msg ($activity)\"";
		}
		print "$cmd ", objectName($src), " ", objectName($dest), "\n";
	}
}

#
# Extract message arguments from the log and return them in a formatted string ready to be displayed
#
# Arguments:
#	$lineRef	Reference to original log line
#	$msg		Name of message (e.g. "Start").
# Return:
#	String containing comma-seperated arguments of the message (e.g. "-3, 0")

sub getArguments($$)
{
	my $args;
	my ($lineRef, $msg) = @_;
	# Check if arguments are to be displayed for this message.
	if (exists($arguments{$msg})) {
		# Extract arguments:
		# "... args:  [Signature=TSigStateChange] [iMessageId=0x10285a56:StateChange] <arguments>  [ENDBLOCK=TransportSender::PostMessage]"
#		if (${$lineRef} =~ m/(.*?)\s*\[ENDBLOCK=/) {
		if (${$lineRef} =~ m/(.*)\s*\[ENDBLOCK=/) {
			# place message argument values into a hash
			my $valuesHashRef = convertDecoderArgumentsToPerlHash($1);
			if ($valuesHashRef) {
				$args = formatArguments($arguments{$msg}, $valuesHashRef);
			} else {
				handleMangleError($logline, $lineRef, $msg);
			}
			# hash should be deleted when $valuesHashRef goes out of scope (?)
		}
	}
	return $args;
}

#
# Parse the "[iNodeCtx=...]" strings to extract activity ids.  Return a string of the form:
#
# [<src activity>] "->" [<dest activity>]
#
# Source and/or destination activity may not be present.  Examples: "Start->", "->NoBearer", "Start->NoBearer"
#

sub parseActivityIds($$)
{
	my ($srcRef,$destRef) = @_;
	my $activity;
	if (${$srcRef} =~ m/\[iNodeCtx=(\S*)\]/) {
		$activity = $1;
		$activity =~ s/^Activity//;
	}

	if (${$destRef} =~ m/\[iNodeCtx=(\S*)\]/) {
		my $ctx = $1;
		$ctx =~ s/^Activity//;
		$activity .= "->$ctx";
	}
	elsif ($activity) {
		$activity .= "->";
	}
	return $activity;
}

#
# Return a string containing containing comma-seperated arguments of the message (e.g. "-3, 0")
#
# Arguments:
#	$formatRef	Reference to a hash containing the arguments to be displayed for each message  (see processArgumentLine())
#	$h		Reference to a hash containing the argument values (see convertDecoderArgumentsToPerlHash())
#
# Returns:
#	String containing message arguments

sub formatArguments($$)
{
	my ($formatRef, $h) = @_;
	my $index = 0;
	my $buf;
	while ($index < scalar(@{$formatRef})) {
		# extract the argument value from the hash
		if ($index > 0) {
			$buf .= ", ";
		}
		my $hashAccess = "\$h->" . $formatRef->[$index++]; 	# "$h" refers to argument variable passed in
		my $value  = eval $hashAccess;
		my $format = $formatRef->[$index++];
		if ($format == ArgumentNode) {
			# for ":node" arguments, convert node address to node name via symbol table
			my $hexval;
			if ($value =~ s/^0x//) {
				$hexval = $value;
			} else {
				$hexval = sprintf "%08x", $value;
			}
			if (defined $symtab{$hexval}) {
				$buf .= $symtab{$hexval};
			} else {
				# if we couldn't convert node address to node name, just show hex value
				# (display "00000000" as "0".  We could also do "$hexval = sprintf "%x" $value")
				if ($value == 0) {
					$hexval = "0";
				}
				$buf .= $hexval;
			}
		} elsif ($value =~ m/^0x[0-9a-fA-F]+$/ || $value =~ m/^[\-\d]\d+$/) {
			# hex or decimal number - convert to signed decimal
			if ($value =~ m/^0x/) {
				$value = hex($value);
			}
			if ($format == ArgumentHex) {
				$buf .= sprintf "0x%x", $value;
			} else {
				$buf .= sprintf "%d", $value;
			}
		} else {
			$buf .= $value;
		}
	}
	return $buf;
}

sub uniquifyName()
	{
	my ($name) = @_;
	my $index = 0;
	my $proposedName = $name;
	while (defined $uniqueNames{$proposedName})
		{
		$proposedName = $name . ++$index;
		}

	$uniqueNames{$proposedName} = 1;
	return $proposedName;
	}

#
# Shorten object name and return its placement order
#

sub parseObjectName($)
{
	my ($nameRef) = @_;
	my $ref;
	my $order = 0;
	foreach $ref (@objectNameParse) {
		if (${$nameRef} =~ s/$ref->{SubString}/$ref->{Replacement}/) {
			if ($order == 0 && $ref->{Order} != 0) {
				$order = $ref->{Order};
			}
		}
	}
	${$nameRef} =~ s/^C//;			# remove leading 'C', if any
	return $order;
}

sub objectName($) {
	my ($name) = @_;
	if (defined ($symtab{$name})) {
		return $symtab{$name};
	} else {
		return $name;
	}
}

sub readIniFile()
{
	my $sectionName;
	if ($opt_x) {
		open (INI, $opt_x) || die "Cannot open ini file $opt_x\n";
		while (<INI>) {
			chomp;
			# remove leading and trailing blanks and ignore blank lines
			s/^\s+//;
			s/\s+$//;
			if (! $_|| m/^#/)
				{ next;	};

			if (/^\[(.+)\]/) {
				# parse section name from "[<section>]" lines
				$sectionName = $1;
			}
			else {
				if ($sectionName eq "ExcludeClass") {
					$excludeClass{$_} = 1;
				}
				elsif ($sectionName eq "ExcludeMessage") {
					$excludeMessage{$_} = 1;
				}
				elsif ($sectionName eq "ExcludeShortName") {
					$excludeShortName{$_} = 1;
				}
				elsif ($sectionName eq "ExcludeIpc") {
					$excludeIpc{$_} = 1;
				}
				elsif ($sectionName eq "ExcludeSingleTuple") {
					if ($_ eq "*") {
						$excludeAllSingleTuples = 1;
					} else {
						$excludeSingleTuple{$_} = 1;
					}
				}
				elsif ($sectionName eq "MessageArguments") {
					processArgumentLine($_);
				}
				elsif ($sectionName eq "ObjectNameTruncateAndOrder") {
					print $_;
					processObjectNameTruncateAndOrder($_);
				}
				elsif ($sectionName eq "NotableTextToDisplay") {
					processNotableText($_);
				}
			}
		}
		close INI;
	}
}

sub excludeIpc($)
	{
	my $ipc = $_[0];
	$ipc =~ s/\(.*\)$//;		# "ESoGetOpt(Socket1)" => "ESoGetOpt"
	return $excludeIpc{$ipc};
	}

printMangleErrors();

sub printMangleErrors()
	{
	if (scalar(@mangled) > 0) {
		print STDERR "\n", scalar(@mangled), " possibly mangled log lines:\n";
		foreach my $i (@mangled) {
			print STDERR "$i->{num} ";
			}
		}
		print STDERR "\n\n";
		foreach my $i (@mangled) {
			my $line = $i->{line};
			# decode node and activity names when printing out the mangled log lines
			$line =~ s/\[Address=\[iPtr=0x([0-9a-f]{8})\]/&objectName($1)/ge;
			$line =~ s/CNodeActivityBase ([0-9a-f]{8})/"CNodeActivityBase " . $activityName{$1}/ge;
			print STDERR $line, "\n\n";
		}
	}

sub handleMangleError($$$)
	{
	my ($logline, $lineRef, $msg) = @_;
	push @mangled, { num => $logline, line => "($msg @ $logline): ..." . ${$lineRef} };
	}
	
sub truncateExeName($)
	{
	my ($exeName) = @_;
	$exeName =~ s/\.exe$//;
	$exeName =~ s/\.EXE$//;
	$exeName =~ s/ /_/g;
	return $exeName;
	}

sub outputSymbols()
	{
	# Should have already created $outputPath directory, create html/ subdirectory.
	my $path = $outputPath . "html";
	if (! -d $path) {
		mkdir $path;
	}
	$path .= "/logsym.html";
	open SYM, ">$path" || die "Cannot open $path for writing\n";
	print SYM "<html>\n<body><code>\n";
	my @keys = sort keys %symtab;
	for my $i (@keys) {
		print SYM "$i\t$symtab{$i}<br>\n";
		}
	print SYM "</code></body></html>\n";
	close SYM;
	}

#
# Process a single line from the "[Arguments]" section of the config file.
# This section contains a list of messages and the corresponding message arguments to display.
#
# Format:
#	<message> <argument> [<argument>]*
# where:
#	<argument> := <member>[.<member>]*[:<output format>]
#	<output format> := "node"
#	<member> := class data member name (e.g. iStateChange.iError)
#
# Output format values:
#	node		take the hex address as a node address and display the human readable node name (e.g. "IPCPR2")
#			By default, display as signed decimal.
#
# For example:
#
# [Arguments]
# StateChange iStateChange.iStage iStateChange.iError
# BindTo iCommsBinder.iCommsId.iPtr:node iProviderInfo.iAPId
# SelectComplete iNodeId.iPtr:node
#
# Each line is read into the hash %argument which is keyed on message name and contains a (mixed type) array
# giving each argument (in corresponding Perl hash access format) and output format (constant ArgumentSignedDecimal,
# ArgumentNode).  For example:
#
#	%arguments:	StateChange => [ "{iStateChange}->{iStage}", ArgumentSignedDecimal, "{iStateChange}->{iError}", ArgumentSignedDecimal ]
#			BindTo => [ "{iCommsBinder}->{iCommsId}->{iPtr}", ArgumentNode, "{iProviderInfo}->{iAPId}", ArgumentSignedDecimal ]


sub processArgumentLine($)
{
	my $line = $_[0];
	if (!($line =~ s/^(\w+)\s*//)) {
		die "Incorrectly formatted arguments file\n";
	}
	my $msg = $1;
	if (exists $arguments{$msg}) {
		die "Duplicate line for message $msg in arguments file\n";
	}
	my @args = split ' ', $line;
	my $format;
	foreach my $i (@args) {
		# extract the output format specifier after the ":" (if any) - e.g. "iPtr:node".
		# Default to signed decimal, if not specified.
		if ($i =~ s/:(\w+)//) {
			$format = $argumentFormatToConstant{$1};
		} else {
			$format = ArgumentSignedDecimal;
		}
		# convert from period seperated fields to Perl hash access format, e.g. "a.b.c" to "$h->{a}->{b}->{c}"
		$i =~ s/(\w+)/{\1}/g;
		$i =~ s/\./->/g;
		# add hash access string and output format to array associated with the message
		push @{$arguments{$msg}}, ($i,$format);
	}
#	printArguments();
}

#
# Convert the nested bracketted field/value format used by the decoder into a Perl hash.
# For example:
#	"[A=[B=[C=x1][D=x2]][E=x3]][F=[G=x4]]"
# becomes:
#	"{A => { B=C => x1, D => x2, }, E => x3, }, F => {G => x4, }, }"
# Note: extraneous commas are harmless and hence not removed.
#
# This is then passed to "eval" to become an actual hash.  Accessing any field is then via standard Perl hash access,
# e.g. "$hash->{A}->{B}->{D}" for accessing A.B.D.
#
# Arguments:
#	- decoder nested bracketted format string for arguments
# Return:
#	- reference to hash

sub convertDecoderArgumentsToPerlHash($)
{
	my $arg = "{" . $_[0];
	# Remove any empty fields, e.g. "[Bad=]" as these will result in us generating hash initialisers without a value
	# (e.g. "..., Good => Value,  Bad => , Good2 => Value2, ...") which causes undefined behaviour when initialising
	# the hash.

	$arg =~ s/\[\w+=\]\s*//g;

	# NOTE: order in which the substitutions happens below is important

	# "[X=[" -> "X => { "
	$arg =~ s/\[(\w+)=\[/\1 => { /g;

	# "X=["	-> "X => { "
	$arg =~ s/(\w+)=\[/\1 => { /g;

	# "[X=Y]" -> "X => Y, "
	# Note: allows for empty values, e.g. "[A=]"
#	$arg =~ s/\[(\w+)=(\w*)\]/\1 => \2, /g;
	$arg =~ s/\[([^\]=]+)=([^\]=]*)\]/\1 => \2, /g;

	# "X=Y]" -> "X => Y, "
	# Note: allows for empty values, e.g. "[A=]"
#	$arg =~ s/[^[](\w+)=(\w*)\]/\1 => \2, /g;
#	$arg =~ s/[^\[]([^\]\=]+)\=([^\>][^\]\=]*)\]/\1 \=> \2, /g;
	$arg =~ s/([\w\d_]+)\s*\=([^\>][^\]\=]*)\]/\1 \=> \2, /g;
	
	# "]" -> "}, "
	$arg =~ s/\]/}, /g;

	# "[" -> "{ "
	$arg =~ s/\[/{ /g;

	# KLUDGE: " , " becomes " } ".  This is mainly to deal with placing the correct number
	# of "}" at the end.
	$arg =~ s/ , / } /g;
	
	$arg .= "}";

	# we must quote any non-numeric values.  Not an ideal expression - just looks at first character.
#	$arg =~ s/=> ([^{][a-zA-Z]+),/=> "\1",/g;
	$arg =~ s/=> ([^{,]+),/=> "\1",/g;
	return eval $arg;
}

#
# Process Object name truncation and ordering lines from ini file.
#
# Each line contains:
# 	<sub string>	<replacement>	<order>
#
# each occurrence of <string> in any object name is replaced with <replacement>.
# <order> indicates how objects containing <string> are placed on the display.
#

sub processObjectNameTruncateAndOrder($_)
{
	my $line = $_[0];
	my ($substr, $replacement, $order) = split ' ', $line;
	push @objectNameParse, { SubString => $substr, Replacement => $replacement, Order => $order };
}

sub printRecursiveHash($)
{
	print "Printing hash: ";
	printRecursiveHashWorker($_[0]);
	print "\n\n";
}

sub printRecursiveHashWorker($)
{
	my $hashRef = $_[0];
	foreach my $i (keys %{$hashRef}) {
		print "$i => ";
		my $type = ref($hashRef->{$i});
		if ($type eq "HASH") {
			print "{ ";
			printRecursiveHashWorker($hashRef->{$i});
			print "}, ";
		} else {
			print $hashRef->{$i}, ", ";
		}
	}
}

sub printArguments()
{
	print "Arguments:\n";
	foreach my $key (keys %arguments) {
		print "$key: @{$arguments{$key}}\n";
	}
}

sub processNotableText($)
{
	my $text = $_[0];
	my ($matchText,$displayText) = m/"(.*)"\s*"(.*)"/;
	push @notableText, [ $matchText, $displayText ];
}

sub matchNotableText($)
{
	my $text = $_;
	foreach my $ref (@notableText) {
		if ($text =~ m/$ref->[0]/) {
			return $ref->[1];
		}
	}
	return "";
}

sub ClearActivity($)
{
	my $actAddr = $_[0];
	if (defined $activityNode{$actAddr}) {
		delete $activityNode{$actAddr};
	}
	if (defined $activityName{$actAddr}) {
		delete $activityName{$actAddr};
	}
}

sub processPathArgument($)
{
	my $path = $_[0];
	if ($path) {
		# ensure "/" at the end
		if ($path !~ /\/$/) {
			$path .= "/";
		}
		mkdirp($path);
		return $path;
	} else {
		return "";
	}
}

sub mkdirp($) 
{ 
    my $dirName = @_[0]; 
    if ($dirName =~ m/^(.*)\//i) { 
        if ($1 ne "") { 
            mkdirp($1); 
        } 
    }
	if (! -d $dirName) {
		mkdir($dirName); 
	}
}
