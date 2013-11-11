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
# parseseq.pl  [<arguments>] <sequence file>
# Arguments:
# -p			Include popups (when clicking on messages etc).
# -r <begin row>,<end row>	Display rows in the range specified.  Row numbers are zero based and inclusive.
# Negative row numbers count backwards from the end row.  <begin row> and/or
# <end row> may be omitted, in which case the beginning and ending rows respectively
# are assumed.  If "r" argument is not specified, a default of the last 400 rows
# is assumed (equivalent to an argument of "-r -400,").
# -R <rows per view>		The maximum number of rows in each "view" (page).  The overall sequence is generated as a number
# of individual HTML/SVG files, each showing a fragment of the sequence.  Each of these is
# termed a "view".  This option indicates the maximum number of rows per view.  Note
# that because the sequence is split into equal sized views, the actual size of each
# view can vary from half values of this parameter to its full value.  This parameter
# only limits the maximum size of each view.
# -H			On "trimmed" displays (i.e. only displaying last <n> lines), don't
# hide object timelines that don't have any messages within the area
# being displayed (but do have messages outside of that area).
# -x <file>		File containing "[DontHideShortName]" section which contains truncated names
# of classes (minus the instance numbering at the end) that should *not* be
# hidden from "trimmed" displays (i.e. displays where only the last <n> lines
# are displayed).
# -M			Exclude messages from the display.  Used for getting an overview of node and activity creation.
# -v			Print on standard output a list of objects which have not been displayed because
# they exchanged no messages or were outside of the displayed view.
# -V			Print on standard output some progress messages (useful when parsing long logs that
# take several minutes).
# -f					Do not display page links in a fixed position window.
# -n                    Print node names vertically as opposed to horizontally.
# -N                    Node to filter available list by
# -A                    Activity to filter available list by
# -h                    This help message
# todo:
# - use findObject() within findOrCreateobject()
# - hide objects whose lifetime extends entirely across the displayed area, but exchange no messages in that area
# 
#

#require 'getopts.pl';
use Getopt::Std;
use HTML::Entities;
use strict;
use Cwd;

my $version = "2.2 (27/02/08)";

getopts("MHpx:N:A:vVr:R:hnfo:");
our($opt_M, $opt_H, $opt_p, $opt_x, $opt_N, $opt_A, $opt_v, $opt_V, $opt_r, $opt_R, $opt_h, $opt_n, $opt_f, $opt_o);

my $outputPath = processPathArgument($opt_o);

if ($opt_h)
{
    print <<HELP;
Usage:
    parseseq.pl  [<arguments>] <sequence file>

 Arguments:

 -p			Include popups (when clicking on messages etc).

 -r <begin row>,<end row>   Display rows in the range specified.  Row numbers are zero based and inclusive.
                            Negative row numbers count backwards from the end row.  <begin row> and/or
                            <end row> may be omitted, in which case the beginning and ending rows respectively
                            are assumed.  If "r" argument is not specified, a default of the last 400 rows
                            is assumed (equivalent to an argument of "-r -400,").

 -R <rows per view>         The maximum number of rows in each "view" (page).  The overall sequence is generated
                            as a number of individual HTML/SVG files, each showing a fragment of the sequence.
			    Each of these is termed a "view".  This option indicates the maximum number of rows
			    per view.  Note that because the sequence is split into equal sized views, the actual
			    size of each view can vary from half values of this parameter to its full value.
			    This parameter only limits the maximum size of each view.

 -H                         On "trimmed" displays (i.e. only displaying last <n> lines), don't
                            hide object timelines that don't have any messages within the area
                            being displayed (but do have messages outside of that area).

 -x <file>		    File containing "[DontHideShortName]" section which contains truncated names
                            of classes (minus the instance numbering at the end) that should *not* be
                            hidden from "trimmed" displays (i.e. displays where only the last <n> lines
                            are displayed).

 -M                         Exclude messages from the display.  Used for getting an overview of node and activity
                            creation.

 -v                         Print on standard output a list of objects which have not been displayed because
                            they exchanged no messages or were outside of the displayed view.

 -V                         Print on standard output some progress messages (useful when parsing long logs that
                            take several minutes).

 -f							Do not display page links in a fixed position window.
							
 -n                         Print node names vertically as opposed to horizontally.
 
 -N                         Node to filter available list by. If set, you will be given a list of node activities
                            which contain the string you passed in, in the node. Select one of these and it will
			    generate the diagram only for the lifetime of that activity.
 
 -A                         Activity to filter available list by. If set, you will be given a list of node
                            activities which match the string you passed in, in the activity.

			    e.g.
			    >parseseq -A start \log.seq
                            Reading sequence file
                            [1] Conn1 :: ConnectionStart
                            [2] IPCPr :: IpCprStart
                            [3] IPProtoCPr :: IPProtoCprStart
                            [4] AgentCPr :: PRStart
                            [5] AgentCPr :: CprDataClientStart
                            [6] AgentSCPr :: AgentSCprStart
                            Select activity to draw:
                            ...

-o <path>					Place all output files into <path>/html/ directory (including log.html).

-h                         This help message
HELP
    exit (0);
}

#
# Flags
#

# remove objects that are created but have no messages
my $trimSilentFlag = 1;

# sort objects left-to-right by type
my $sortObjectsFlag = 1;

my $maxRowsToDisplay = 300;
if ($opt_R) {
	die "Parameter to -R option must be between 100 and 1000\n" if $opt_R < 100 || $opt_R > 1000;
	$maxRowsToDisplay = $opt_R;
}

# whether to output timestamps in the form "hh:mm:ss,dd" where the ",dd" represents the number of seconds since the last timestamp (where possible)
my $outputTimeDeltas = 0;

# constants controlling progress messages
my $sequenceProgressIncrement = 10000;
my $trimProgressTrigger = 1000;
my $trimProgressPercentIncrement = 10;

#
# constants controlling layout
#

my $incrementY = 19;
my $fontWidth = 5;
my $fontHeight = 9;
my $leftMargin = 125;
my $leftSeperatorMargin = 55;
my $screenWidth = 50;
my $rightMargin = 30;
my $topMargin = 20;
my $bottomMargin = 22;
my $objectSpacing = 90;
my $arrowWidth = 10;
my $textArrowSpacing = 1;
my $messageSpacingAboveLine = 1;
my $objectVerticalLineSpacing = 2;
my $objectUnderlineToLifelineGap = 2;
my $objectNameRepeatCount = 35;
my $lifelineBottomOverrun = 5;
my $activitySpacing = 6;
my $textMattSize = $activitySpacing * 2 + 2;	# text is on top of node lifeline and first two activity lines
my $lifelineTextLeftMargin = 8;
my $rotatedTextOffset = 3;
#
# derived constants
#

my $topOffset = $topMargin + $objectVerticalLineSpacing;

#
# colour constants
#

my $objectNameColour1 = "red";
my $objectNameColour2 = "blue";
my $objectNameLightColour1 = "salmon";
my $objectNameLightColour2 = "rgb(50,50,200)";
my $aliveColour = "black";
my $deadColour = "white";
my $seperatorColour = "rgb(245,245,245)";
my $receiveColour = "black";
my $postColour = "green";

my $activityColour = "skyblue";
my $activityEndUnknownColour = "salmon";

# magic value for Creation/Deletion unknown

my $objectUnknown = -2;

#
# changeable global variables that need to be re-initialised per view drawn
# (see initialiseGlobals())
#

my $currentY;

#
# changeable global variables that need to be initialised once before views are drawn
#

my $minOrder = 100;
my $maxOrder = 0;

my $limitedRowText;

# hash for quick lookup of objects
my %objhash = ();
my @silentObjectsTrimmed = ();
my @messages = ();
my @sequences = ();
my @objects = ();
my %dontHideShortName = ();

#
# Data Structures:
#
# "objects" is array of hashes:
#	{ Name => , X =>,   Y =>,  MsgCount =>, Creation =>, Deletion =>, Order => }
#
# "sequences" is an array of hashes:
#	{ Action => , Object => Text =>, SrcObj =>, DestObj =>, Msg => }


# Input file format:
#
#	<action> <arguments> ...
#
# specifically:
#
#	[p|r] <message name> <source object> <destination object>
#	t <object> <text>
#	oc <object> <placement order>
#	od <object>
#	l <left hand label>
#
# actions:
#
# p		Post
# r		Receive
# t		Text
# oc	Object Create
# od	Object Destroyed
# l		Label
#
# All fields are space separated text.  For example:
#
# P StartFlow SCPR Flow
#

readIniFile();
initialiseGlobals();
    
print "Reading sequence file\n" if ($opt_V);
readSequences();

if ($opt_N || $opt_A)
    {
    trimDownToActivity();
    }

if ($trimSilentFlag)
	{
	print "Trimming silent objects\n" if ($opt_V);
	trimSilentObjects();
	}

my $beginRow = 0;
my $endRow = 99999;

# Count the number of rows that would be displayed
# (doesn't actually draw them because of "0" argument)

print "Generating sequence pages\n" if ($opt_V);

my $totalRows = drawSequences($beginRow, $endRow, 0);
fixObjectsWithUnknownCreationDeletion();

my $rowsToDisplay = $totalRows;

if ($opt_r) {
	$rowsToDisplay = processRowArgument($totalRows);
	}

createSubDir();

if ($totalRows <= $maxRowsToDisplay) {
	drawView(0, $totalRows - 1, 0, 0);
} else {
	my $viewsToDisplay = int($rowsToDisplay / $maxRowsToDisplay) + 1;
	my $rowsPerView = int(($rowsToDisplay + $viewsToDisplay - 1) / $viewsToDisplay);
	my $viewNumber = 0;
	createViewMap();
	createIpcMap();
	while ($beginRow < $totalRows) {
		$endRow = $beginRow + $rowsPerView - 1;
		if ($endRow >= $totalRows) {
			drawView($beginRow, $totalRows - 1, $viewNumber, $viewNumber);	# last view
		} else {
			drawView($beginRow, $endRow, $viewNumber, $viewsToDisplay - 1);
		}
		++$viewNumber;
		$beginRow = $endRow + 1;
	}
	closeViewMap($viewNumber);
	closeIpcMap($viewNumber);
}

print "\n" if ($opt_V);

if ($opt_v)
	{
	printArray("\nObjects without any message exchange", \@silentObjectsTrimmed);
	}


#
# Generate the SVG and HTML files for a particular "view" (range of rows)
#

sub drawView($$$$) {
	my ($beginRow, $endRow, $viewNumber, $lastViewNumber) = @_;
	print "Generating page $viewNumber of $lastViewNumber\r" if ($opt_V);
	if ($opt_H != 1)
		{
		hideObjectsOutsideOfView($beginRow, $endRow);
		}
	
	my $screenWidth = calculateObjectColumnPositions();
	my $screenHeight = (($endRow - $beginRow + 1) * $incrementY) + $topMargin + $bottomMargin;

	#
	# Filename convention for views: log0.svg, log1.svg, ..., log<n>.svg, log.svg - i.e. log.svg is the last view
	# (considered the one most interesting and hence gets undecorated filename).
	#

	my $fileName = "log";
	if ($viewNumber != $lastViewNumber) {
		$fileName .= $viewNumber;
		}

	my $path = $outputPath . "html/${fileName}.svg";
	open SVG, ">$path" || die "Cannot open $path for writing\n";
	#open RTTTL, ">html/${fileName}.rtttlpre" || die "Cannot open html/${fileName}.rttlpre for writing\n";

	outputDocHeader($screenWidth, $screenHeight);
	updateObjectViewList($viewNumber);
	drawObjectLifelines($beginRow, $endRow, $screenHeight);
	drawObjectNames(0);
	drawActivities($beginRow, $endRow, $screenHeight, $viewNumber);
	my @labelsOnPage = ();
	my @objectsDestroyed = ();
	my @objectsCreated = ();
	my @ipc = ();
	drawSequences($beginRow, $endRow, 1, \@labelsOnPage, \@objectsCreated, \@objectsDestroyed, \@ipc);
	outputViewMap($viewNumber, $lastViewNumber, \@labelsOnPage, \@objectsCreated, \@objectsDestroyed);
	outputIpcMap($viewNumber, $lastViewNumber, \@ipc);
	outputDocFooter();

	close SVG;
	#close RTTTL;

	outputHTMLEmbedder($screenWidth, $screenHeight, $viewNumber, $lastViewNumber, $fileName);
	if ($opt_v) {
		printHiddenObjects("\nObjects outside of displayed view $viewNumber");
	}

	resetForNextView();
}

sub createSubDir()
{
	my $path = $outputPath . "html"; 
	if (! -d $path) {
		mkdir $path || die "Cannot create $path subdirectory\n";
	}
}
sub resetForNextView() {
	# objects to hide must be calculated afresh per view
	unhideAllObjects();
	initialiseGlobals();
}

#
# (re)initialise globals required per view drawn
#
sub initialiseGlobals() {
	$currentY = $topMargin;
	}


####################
# Message routines
####################

sub findOrCreateMessage($)
	{
	my $msgName = $_[0];
	my $msg;
	foreach $msg (@messages) {
		if ($msg eq $msgName) {
			return \$msg;
			}
		}
	push @messages, $msgName;
	return \$messages[$#messages];
	}

###################
# Object routines
###################

sub readSequences()
{
	my $line = 0;
	while (<>) {
		die unless s/^(\w+)\s+//;
		my $action = $1;
		if ($action eq "t")
			{
			# Text
			# "t <object> <text>"
			# Place text item on the timeline of a particular object
			my $pos;
			if (s/^([^\s]*)\s+//) { $pos = $1; }
			my $objRef = findOrCreateObject($pos);
			chomp;
			my $text = $_;
			push @sequences, { Action => $action, Object => $objRef, Text => $text };
			}
		elsif ($action eq "oc")
			{
			# Object Create
			# oc <object name> <display order> <addr>
			split;
			my $objName = shift @_;
			my $order = shift @_;

			my $objRef = findOrCreateObject($objName);
			$objRef->{Order} = $order;
			if ($order < $minOrder)
				{
				$minOrder = $order;
				}
			if ($order > $maxOrder)
				{
				$maxOrder = $order;
				}
			chomp;
			push @sequences, { Action => $action, Object => $objRef };
			}
		elsif ($action eq "od")
			{
			# Object Destroyed
			# od <object name> <addr>
			split;
			my $objName = shift @_;
			my $objRef = findOrCreateObject($objName);
			chomp;
			push @sequences, { Action => $action, Object => $objRef };
			}
		elsif ($action eq "ac")
			{
			# Activity Created
			# ac <object name> <activity addr> <activity name>
			split;
			my $objRef = findObjectByName(shift @_);
			if ($objRef != 0)
				{
				my $addr = shift @_;
				my $name = shift @_;
				push @sequences, { Action => $action, Object => $objRef, Addr => $addr, Name => $name };
				}
			chomp;
			}
		elsif ($action eq "ad")
			{
			# Activity Destroyed
			# ad <addr>
			split;
			my $addr = shift @_;
			chomp;
			push @sequences, { Action => $action, Addr => $addr };
			}
		elsif ($action eq "p" || $action eq "r" || $action eq "sc")
			{
			# Post/Receive/SynchronousCall
			# [P|R|SC] <message> <source object> <destination object>
			my $msg;
			if (/^"/)
				{
				s/^"([^"]+)"\s+//;
				$msg = $1;
				}
			else
				{
				s/^(\S+)\s+//;
				$msg = $1;
				}
			my $msgRef = findOrCreateMessage($msg);
			split;
			my $srcRef = findOrCreateObject(shift @_);
			my $destRef = findOrCreateObject(shift @_);
			$srcRef->{MsgCount}++;
			$destRef->{MsgCount}++;
			if (!$opt_M) {
				push @sequences, { Action => $action, Msg => $msgRef, SrcObj => $srcRef, DestObj => $destRef };
				}
			}
		elsif ($action eq "l" || $action eq "pn" || $action eq "ts") {
			# Label (on lefthand side - for time stamp)
			# l <text>
			#
			# Popup - associated with [n]ext message
			# pn <text>
			chomp;
			push @sequences, { Action => $action, Text => $_ };
		}
		++$line;
		if ($opt_V) {
			if ($line >= $sequenceProgressIncrement && ($line % $sequenceProgressIncrement == 0)) {
				print "line $line\r";
			}
		}
	}
	if ($opt_V && $line >= $sequenceProgressIncrement) {
		print "line $line (complete)\n";
	}
}

sub findObjectByName()
	{
	my $objName = $_[0];
	if (exists($objhash{$objName})) {
	    return $objhash{$objName}
	}
	return 0;
	}

sub findOrCreateObject()
	{
	my $objName = $_[0];
	my $obj = &findObjectByName($objName);
	if ($obj != 0) {
	    return $obj;
	}
	
	push @objects, { Name => $objName, X => 0, Y => 0, MsgCount => 0, Order => 0, Creation => $objectUnknown, Deletion => $objectUnknown, Hide => 0 };

	$obj = $objects[$#objects];
	$objhash{$objName} = $obj;
	
	return $obj;
	}

sub printArray($$)
	{
	my ($title, $arrayRef) = @_;
	print $title, ":\n";
	foreach my $i (@$arrayRef)
		{
		print "$i\n";
		}
	}


sub printHiddenObjects($)
	{
	print $_[0], ":\n";
	foreach my $i (@objects)
		{
		if ($i->{Hide} == 1)
			{
			print $i->{Name}, "\n";	
		}
		}
	}

sub processRowArgument($)
#
# Process the "-r <begin>,<end>" argument.  If either <begin> and/or <end> are missing,
# then assume that the beginning and ending row are the first and last row respectively.
#
# Input argument:	Total number of rows in sequence
# Outputs:			Beginning and ending row to display (in $beginRow, $endRow)
# Return value:		Total number of rows to display
	{
	my $totalRows = $_[0];
	die unless $totalRows;

	if (! $opt_r) {
		$opt_r = "$maxRowsToDisplay,";
	}
		
	$opt_r =~ m/(-?[0-9]*),(-?[0-9]*)/;
		
	if (! $1)
		{
		$beginRow = 0;
		}
	else
		{
		$beginRow = $1;
		if ($beginRow < 0) {
			$beginRow += $totalRows;
			if ($beginRow < 0) {
				$beginRow = 0;
				}
			}
		}
	if (! $2)
		{
		$endRow = $totalRows - 1;
		}
	else
		{
		$endRow = $2;
		if ($endRow < 0) {
			$endRow += $totalRows;
			if ($endRow < 0) {
				$endRow = 0;
				}
			}
		}
	
	die "Invalid row range specification ($beginRow, $endRow)" if ($beginRow > $endRow);

	print "Total rows = $totalRows\n";

	my $newTotalRows = $endRow - $beginRow + 1;
	if ($newTotalRows < $totalRows) {
	        my $endRowText = ($endRow == $totalRows - 1) ? "the end (row $endRow)" : $endRow;
		SetLimitedRowText("Only displaying $newTotalRows rows - row $beginRow to $endRowText");
		print limitedRowText(), "\n";
		return $newTotalRows;
		}
	else {
		return $totalRows;
		}
	}

sub SetLimitedRowText($)
	{
	$limitedRowText = $_[0];
	}
	
sub limitedRowText()
	{
	return $limitedRowText;
	}

#
# Set X position for each object.  Return final screen width.
#

sub calculateObjectColumnPositions()
	{
	# loop through each order
	my $x = $leftMargin;
	my $colno = 1;
	my $order;
	
	#
	# Objects can either be placed by sorting left-to-right by type, or else
	# placing objects as they are created.
	#
	
	if ($sortObjectsFlag == 1)
		{
		for ($order = 0 ; $order <= $maxOrder ; $order++)
			{
			foreach my $obj (@objects)
				{
				if ($obj->{Hide} == 1)
						{
						next;
						}
				if ($obj->{Order} == $order)
					{
					$obj->{X} = $x;
					$x += $objectSpacing;

					$obj->{colno} = $colno;
					$colno++;

# print "$obj->{Name}, order $obj->{Order}, X $obj->{X}\n";
					}
				}
			}
		}
	else
		{
		foreach my $obj (@objects)
			{
			if ($obj->{Hide} == 1)
				{
				next;
				}
# print "$obj->{Name}, order $obj->{Order}, X $obj->{X}\n";
			$obj->{X} = $x;
			$x += $objectSpacing;

			$obj->{colno} = $colno;
			$colno++;
			}
		}
	return $x;	
	}

sub hideObjectsOutsideOfView($$)
	{
	my ($beginRow, $endRow) = @_;

	my $obj;
	foreach my $obj (@objects) {
		# Don't need this as we now track process creation/destruction properly
		#if ($obj->{Name} =~ /^!/)
		#	{
		#	next;
		#	}

		if (IsCrossingRange($beginRow, $endRow, $obj->{Creation}, $obj->{Deletion}) != 1)
			{
			$obj->{Hide} = 1;
			}
		}
	}

sub unhideAllObjects()
	{
	foreach my $obj (@objects) {
		$obj->{Hide} = 0;
		}
	}

sub trimDownToActivity()
{
    my @activities = undef;
    my $i = 0;
    
    for ($i = 0; $i < $#sequences; $i++)
    {
	my $s = $sequences[$i];

	if ($s->{Action} eq "ac" && ($opt_N eq "" || $s->{Object}{Name} =~ /$opt_N/i) && ($opt_A eq "" || $s->{Name} =~ /$opt_A/i))
	{
	    push @activities, { Index => $i, Addr => $s->{Addr} };
	    my $ai = $#activities;
	    print "[$ai] ".$s->{Object}{Name}." :: ".$s->{Name}."\n";
	}
    }
    print "Select activity to draw: ";
    my $selected = <STDIN>;
    chomp($selected);
    $selected += 0;


    my $adfound = 0;

    my $selectedact = $activities[$selected];
    my $selectedseq = $sequences[$selectedact->{Index}];
    
    my %usednodes = ();
    $usednodes{$selectedseq->{Object}} = 1;
    $selectedseq->{Object}{MsgCount}++;
    for ($i = 0; $i < $#sequences ; $i++)
    {
	if ($i >= $selectedact->{Index} && $adfound == 0)
	{
	    my $s = $sequences[$i];
	    my $action = $s->{Action};
	    if ($action eq "p" || $action eq "r" || $action eq "sc")
	    {
		if ($usednodes{$s->{SrcObj}} == 1)
		{
		    $usednodes{$s->{DestObj}} = 1;
		}
		else
		{
		    $s->{KillMe} = 1;
		}
	    }
	    if ($action eq "ad" && $s->{Addr} eq $selectedact->{Addr})
	    {
		$adfound = 1;
	    }
	}
	else
	{
	    # remove message posts
	    my $action = $sequences[$i]->{Action};
	    if ($action eq "p" || $action eq "r" || $action eq "sc" || $action eq "t" || $action eq "ac" || $action eq "ad")
	    {
		$sequences[$i]->{KillMe} = 1;
	    }
	}
    }

    for ($i = $#sequences; $i >= 0; $i--)
    {
	if ($sequences[$i]->{KillMe} == 1)
	{
	    my $action = $sequences[$i]->{Action};
	    if ($action eq "p" || $action eq "r" || $action eq "sc")
	    {
		$sequences[$i]->{SrcObj}{MsgCount}--;
		$sequences[$i]->{DestObj}{MsgCount}--;
	    }
	    splice @sequences, $i, 1;
	}
    }

}

sub trimSilentObjects()
	{
	# get rid of objects that didn't end up with any messages sent to/from them.
	# Exceptions are objects beginning with "!", which are forced to be displayed (for example,
	# exe timelines displaying socket/host resolver operations).
	my $i;
	my %trimObjects = ();
	# NOTE: in the following "for" loop, "i" is incremented in the body.  Beware if using "next" etc.
	#

	my $objectCount = scalar(@objects);
	my $percentIncrement = 0;

	if ($opt_V && $objectCount > $trimProgressTrigger) {
		$percentIncrement = $objectCount/$trimProgressPercentIncrement;
	}

	for ($i = 0 ; $i <  $objectCount ; ) {	# we are splicing @objects in the loop, so do we need to recalculate scalar() every time?
		my $objectName = $objects[$i]->{Name};
		if ($objectName =~ /^!/)
			{
			++$i;
			next;
			}

		$objectName =~ s/[0-9]+$//;
		if ($dontHideShortName{$objectName})
			{
			++$i;
			next;
			}

		if ($objects[$i]->{MsgCount} == 0)
			{
			push @silentObjectsTrimmed, $objects[$i]->{Name} if ($opt_v);

			$trimObjects{$objects[$i]} = 1;
			$objects[$i]->{KillMe} = 1;
#			deleteObject($i);
			$objectCount = scalar(@objects);		# is this needed given we aren't incrementing i?
			$percentIncrement = $objectCount/$trimProgressPercentIncrement if ($percentIncrement);
			# Don't increment "i" here
			++$i;
			}
		else
			{
			++$i;
			}
		if ($percentIncrement && $i > $percentIncrement && ($i % $percentIncrement) == 0) {
			print int($i * 100 / $objectCount), "% complete\r";
			}
		}

	for (my $i = 0; $i < scalar(@sequences); $i++) {
	    my $action = $sequences[$i]->{Action};
	    my $obj = $sequences[$i]->{Object};
	    if ( ($action eq "t" || $action eq "oc") && exists($trimObjects{$obj})) {
		$sequences[$i]->{KillMe} = 1;
	    } 
	}

	for (my $i = $#sequences; $i >= 0; $i--) {
	    if ($sequences[$i]->{KillMe} == 1) {
		splice @sequences, $i, 1;
	    }
	}

	for (my $i = $#objects; $i >= 0; $i--) {
	    if ($objects[$i]->{KillMe} == 1) {
		splice @objects, $i, 1;
	    }
	}

	print "100% complete\n" if ($percentIncrement);		# last percentage printed in loop may be, say, "88%" so make it look 100% complete
	}

sub deleteObject($)
	{
	my ($i) = @_;			# object index to delete
	my $j;

	for ($j = 0 ; $j < scalar(@sequences) ; )
		{
		my $action = $sequences[$j]->{Action};
		if ( (($action eq "t") || ($action eq "oc")) &&
		     ($sequences[$j]->{Object} == $objects[$i]) )
			{
			splice @sequences, $j, 1;
			}
		else
			{
			++$j;
			}
		}
		splice @objects, $i, 1;
	}

sub drawObjectNames($)
	{
	my ($repeatedNameFlag) = @_;
	my $i;
	my $colour;
	foreach $i (@objects) {
		if ($i->{Hide} == 1)
			{
			next;
			}
		my $colour;
		if ($repeatedNameFlag == 1)
			{
			if ($i->{Order} == 0)
				{
				$colour = "!silver";
				}
			else
				{
				$colour = (($i->{Order} % 2) == 0) ? $objectNameLightColour1 : $objectNameLightColour2;
				$colour = "!" . $colour;
				}

			}
		else
			{
			if ($i->{Order} == 0)
				{
				$colour = "!";
				}
			else
				{
				$colour = (($i->{Order} % 2) == 0) ? $objectNameColour1 : $objectNameColour2;
				$colour = "!" . $colour;
				}
			}
		my $name = $i->{Name};
		$name =~ s/^!//;
		# **** REMOVE THIS HACK for debugEvent() ****
		if ($opt_n)
		    {
		    my $x = $i->{X} + $rotatedTextOffset;
		    my $transform="rotate(90, $x, $currentY)";
			# If node names are displayed vertically, then the text matt which creates a break in the
			# node lifeline for the node name text is not required (a "!" in front of the colour name).
			$colour =~ s/^!//;
		    outputText($x + $rotatedTextOffset, $name, $currentY, "begin", $colour, qq{id="$name" onclick="debugEvent(evt)" transform="$transform" font-size="12" font-family="sans-serif"});
		    }
		else
		    {
			my $stagger = ($i->{colno} & 1);
		    outputText($i->{X}, $name, $currentY + ($stagger * $incrementY), "middle", $colour, qq{id="$name" onclick="debugEvent(evt)"});
		    }
		} 
	incrementY(2);
	}

#
# Test whether a value is within a certain (start,end] range inclusive.
#
# Arguments: value, start, end
#
# If start and/or end are -1, then this is a "wildcard" match 
#

sub IsInRangeInclusive($$$)
	{
	my ($value, $start, $end) = @_;
	return ($start == -1 || $value >= $start) && ($end == -1 || $value <= $end);
	}

#
# Test whether one range crosses another.
#
# if $start and/or $end are $objectUnknown, this means 1 and infinity respectively.
#

sub IsCrossingRange($$$$)
	{
	my ($startRange, $endRange, $start, $end) = @_;
	if ($start == $objectUnknown)
		{
		return ($end == $objectUnknown || $end >= $startRange);
		}
	elsif ($end == $objectUnknown)
		{
		return ($start == $objectUnknown || $start <= $endRange);
		}
	else
		{
		return ($start <= $endRange && $end >= $startRange);
		}
	}

#
# Activity related routines
#

# Create an activity hash associated with the object in question.  Parameters:
# <object reference>		Reference to the object
# <activity address>		Unique activity address
# Returns: reference to the new activity hash.
#
sub	createObjectActivity($$)
	{
	my ($objRef, $addr) = @_;
	my $actRef;
	if (($actRef = findObjectActivity($objRef, $addr)) != 0)
		{
		print "Duplicate activity creation - $addr\n";
		return 0;
		}
	else 
		{
		my $newActRef = { Addr => $addr, End => -1 };
		push @{$objRef->{Activities}}, $newActRef;
		return $newActRef; 
		}
	}

# Find an existing activity hash associated with the object in question.  Parameters:
# <object reference>		Reference to the object
# <activity address>		Unique activity address (string)
# Returns: reference to the new activity hash.
#

sub	findObjectActivity($$)
	{
	my ($objRef, $addr) = @_;
	
	if (defined($objRef->{Activities}))
		{
		my $actRef;
		foreach $actRef (@{$objRef->{Activities}})
			{
			if ($actRef->{Addr} eq $addr)
				{
				return $actRef;
				}
			}
		}
	return 0;
	}

# Find the activity and object associated with an activity address.  Parameters:
# <activity address>				Unique activity address (string)
# <object reference reference>		Reference to a variable that will receive the object reference (out)
# Returns: reference to the activity.

sub findActivity($$)
	{
	my ($addr, $returnObj) = @_;
	my $objRef;
	my $actRef;
	foreach $objRef (@objects)
		{
		if (defined($objRef->{Activities}))
			{
			$actRef = findObjectActivity($objRef, $addr);
			if ($actRef != 0)
				{
				${$returnObj} = $objRef;
				return $actRef;
				}
			}
		}
	return 0;
	}

# Allocate a free vertical activity line associated with the object.
# Returns: vertical activity line (number starting from 1).
sub allocateObjectActivityLine($)
	{
	my ($objRef) = @_;
	if (! defined ($objRef->{LineMap}))
		{
		# LineMap is actually a bitmap.  Allocate first line.
		$objRef->{LineMap} = 1;
		return 1;
		}
	else {
		my $lineMap = $objRef->{LineMap};
		my $freeBit = ~($lineMap) & ($lineMap + 1);
		die "Zero allocated bit - $lineMap\n" if ($freeBit == 0);
		$objRef->{LineMap} |= $freeBit;
		my $bitNumber = 1;
		while ($freeBit >>= 1)
			{
			++$bitNumber;
			}
		return $bitNumber;
		}
	}

sub freeObjectActivityLine($$)
	{
	my ($objRef,$line) = @_;
	die "Freeing undefined/empty LineMap\n" if (!defined($objRef->{LineMap}) || $objRef->{LineMap} == 0);
	$objRef->{LineMap} &= ~(1 << ($line-1));
	}

sub closeActivity($$$)
	{
	my ($actRef, $objRef, $endRow) = @_;
	$actRef->{End} = $endRow;
	$actRef->{Addr} = -1;		# address can be re-used, so have to clear it
	freeObjectActivityLine($objRef, $actRef->{Line});
	}

#
# Draw message sequences
#
# Args: drawFlag  1 = do draw, 0 = don't draw
# Return: 	number of rows in diagram for sequences
#
# ToDo:
#	Partitioning: how do popups work ?
#

sub drawSequences($$$$$$)
	{
	my ($startRow, $endRow, $drawFlag, $labelsOnPageRef, $objectsCreatedRef, $objectsDestroyedRef, $ipcRef) = @_;
	my $nextLabel = "";

	# startRow/endRow = start/end row of displayed area (inclusive)
	# absoluteRow	  = increments through all rows that have an object to display, whether in displayed area or not.
	# rows			  = Increments for each row that contains an object to display, but only when $absoluteRow moves
	#					into the displayed area.

	my $rows = 0;
	my $absoluteRow = 0;
	my $drawDone = 0;
	my $inRange = 0;
	my $lastPopupText = "";		# text of last "pn" action
	foreach my $ref (@sequences) {
		my $action = $ref->{Action};

		$inRange = IsInRangeInclusive($absoluteRow, $startRow, $endRow);
		if ($action eq "t") {
			# text
			if ($inRange) {
				++$rows;
				if ($drawFlag == 1) {
					my $objX = $ref->{Object}->{X};
					my $text = $ref->{Text};
					# Text matt no longer required because "t" primitives are not drawn centred on the node lifeline anymore.
					# my $colour = "!";
					my $colour = "";
					if ($text =~ /\s*\{(\d+,\d+,\d+)\}\s*(.+)/) {
						$colour .= "rgb($1)";
						$text = $2;
					}
					my $name = $ref->{Object}->{Name};
					my $attrs = generatePopupAttrs(\$lastPopupText, qq{id="$name"});
					outputText($objX + $lifelineTextLeftMargin, $text, $currentY, "begin", $colour, $attrs);
					$drawDone = 1;
					# support for putting threads into the view map
					$text = $ref->{Text};
					if ($text =~ m/^\(Thread "(.*)" (\w+)\)$/) {
						if ($2 eq "Created") {
							push @{$objectsCreatedRef}, $1;
						} elsif ($2 eq "Destroyed") {
							push @{$objectsDestroyedRef}, $1;
						}
					}
					if ($name =~ s/^!//) {
						push @{$ipcRef}, $ref;
					}
				} else {
					fakeUpCreationDeletionIfRequired($ref->{Object}, $absoluteRow);
					}
				}
			# displayable row, so increment absoluteRow.
			++$absoluteRow;
		} elsif ($action eq "ts") {
			# text
			if ($inRange) {
				++$rows;
				if ($drawFlag == 1) {
					my $text = $ref->{Text};
					outputTextSeperator($text, $currentY);
					$drawDone = 1;
					}
				}
			# displayable row, so increment absoluteRow.
			++$absoluteRow;
			}
		elsif ($action eq "oc") {
			# object created

			# ensure that $objectCreation contains the very first creation point and
			# is not subsequently overwritten if we happen to come across another "oc".
			# This would only happen if an object was deleted and another object of
			# the same type was created with the same address, so it looks to us like the
			# same object being created/destroyed in several cycles.

			if ($drawFlag == 0) {
				if ($ref->{Object}->{Creation} == $objectUnknown) {
					$ref->{Object}->{Creation} = $absoluteRow;
					}
				}
			else {
				if ($inRange) {
					push @{$objectsCreatedRef}, $ref->{Object};
					}
				}
			}
		elsif ($action eq "od")
			{
			# object destroyed
			#
			# Note: okay for objectDeletion to be overwritten by subsequent "od" actions such
			# that it is left containing the last "od" in the sequence for that object.  See comment
			# in "oc" about this circumstance.

			if ($drawFlag == 0)
				{
				$ref->{Object}->{Deletion} = $absoluteRow;
				}
			else {
				if ($inRange) {
					push @{$objectsDestroyedRef}, $ref->{Object};
				}
			}
			}
		elsif ($action eq "l")
			{
			# left label
			if ($outputTimeDeltas && $nextLabel =~ m/^(\d\d):(\d\d):(\d\d)(,(\d+))?/)	# hh:mm:ss(dur)
				{
				use integer my $dur;
				my ($h1,$m1,$s1,$dur) = ($1,$2,$3,$5);
				if ($ref->{Text} =~ m/^(\d\d):(\d\d):(\d\d)/)
					{
					my ($h2,$m2,$s2) = ($1,$2,$3);
					$dur += ($h2 - $h1) * 3600 + ($m2 - $m1) * 60 + ($s2 - $s1);
					$nextLabel = $h2 . ":" . $m2 . ":" . $s2 . "," . $dur;
					}
				}
			else
				{
				$nextLabel = $ref->{Text};
				}
			if ($drawFlag == 1 && $inRange) {
				push @{$labelsOnPageRef}, $nextLabel;
				}
			}
		elsif ($action eq "pn")
			{
			# popup
			if ($drawFlag == 1 && $opt_p && $inRange)
				{
				$lastPopupText = $ref->{Text};
				# not setting $drawDone=1 because popups do not occupy a row
				}
			}
		elsif ($action eq "ac")
			{
			# Activity Created
			# (does not occupy a row, hence no need to set $drawDone)
			# - add activity to list of activities in object and set start row attribute
			# - determine free vertical position to draw activity line
			if ($drawFlag == 0)			# just do it once on first invocation of drawSequences()
				{
				my $actRef = findObjectActivity($ref->{Object}, $ref->{Addr});
				if ($actRef != 0)
					{
					# We have encountered a duplicate "ac" without an intervening "ad".
					# In other words, we've come across two "activity create" tags with the
					# same memory address <xxx> without an intervening "activity delete" tag
					# closing off the first <xxx> memory address.  The same <xxx> memory address
					# has been re-used.  Mark the first of the activities as having ended here
					# (at the point where the second activity starts) and mark its activity line
					# in red - it's the best we can do.
					$actRef->{EndUnknown} = 1;
					closeActivity($actRef, $ref->{Object}, $absoluteRow);
					}

				my $newActRef = createObjectActivity($ref->{Object}, $ref->{Addr});
				$newActRef->{Start} = $absoluteRow;
				$newActRef->{Line} = allocateObjectActivityLine($ref->{Object});
				$newActRef->{Name} = $ref->{Name};
				}
			}
		elsif ($action eq "ad")
			{
			# Activity Destroyed
			# (does not occupy a row, hence no need to set $drawDone)
			# - find corresponding activity and set end row attribute
			# - update our list of free vertical activity lines associated with the object
			if ($drawFlag == 0)			# just do it once on first invocation of drawSequences()
				{
				my $objRef;
				my $actRef = findActivity($ref->{Addr}, \$objRef);
				if ($actRef != 0)
					{
					closeActivity($actRef, $objRef, $absoluteRow);
					}
				else
					{
#					print "Destroyed activity not found - $ref->{Addr}\n";
					}
				}
			}
		else
			{
			# post or receive
			if ($inRange)
				{
				++$rows;
				if ($drawFlag == 1)
					{
					my $msg = ${$ref->{Msg}};
					my $srcX = $ref->{SrcObj}->{X};
					my $destX = $ref->{DestObj}->{X};
					my $align;
					my $colour;
					my $endType;
					if ($action eq "p") {
						$align = "tail";
						$colour = $postColour;
						$endType = "postArr";
						}
					elsif ($action eq "r") {
						$align = "head";
						$colour = $receiveColour;
						$endType = "recvArr";
						}
					elsif ($action eq "sc") {
						$align = "head";
						$colour = $receiveColour;
						$endType = "callArr";
						}
					else {
						die("Unknown action");
						}
					my $lineName = $ref->{SrcObj}->{Name} . "_" . $ref->{DestObj}->{Name};
					my $attrs = generatePopupAttrs(\$lastPopupText, qq{id="$lineName"});
					outputLabelledLine($srcX, $destX, $msg, $colour, $endType, $align, $attrs);

					#print RTTTL ($ref->{SrcObj}->{colno}.",".$ref->{DestObj}->{colno}."\n");
					$drawDone = 1;
					}
				else
					{
					# If the object has no creation point specified, just take the first message as the
					# creation point.  This is to ensure that such objects (generally with hex numbers as object names
					# due to fragmental logs) do not appear on every single SVG page.
					fakeUpCreationDeletionIfRequired($ref->{SrcObj}, $absoluteRow);
					fakeUpCreationDeletionIfRequired($ref->{DestObj}, $absoluteRow);
					}
				}
			++$absoluteRow;
			}

		if ($drawDone == 1) {
			if ($nextLabel)
				{
				outputLeftLabel($nextLabel);
				$nextLabel = "";
				}
			incrementY();
			$drawDone = 0;
			}

		if ($absoluteRow != 0 && (($absoluteRow) % $objectNameRepeatCount) == 0)
			{
			# Note: have (possibly) incremented absoluteRow above, so re-calculate $inRange
			$inRange = IsInRangeInclusive($absoluteRow, $startRow, $endRow);
			if ($inRange)
				{
				# drawObjectNames() increments Y twice (for stagger), and we increment Y once for
				# spacing, so we occupy three rows here.
				$rows += 3;
				if ($drawFlag == 1)
					{
					drawObjectNames(1);
					incrementY();
					}
				}
			$absoluteRow += 3;		# 2 + 1 for stagger
			}
		}
	return $rows;
	}

sub fakeUpCreationDeletionIfRequired($$)
	{
	my ($ref,$row) = @_;
	if ($ref->{Name} =~ /[0-9a-f]{8}/)
		{
		if ($ref->{Creation} == $objectUnknown)
			{
			$ref->{Creation} = $row;
			#print "Object $srcObjRef->{Name} fake creation $absoluteRow\n";
			}
		if ($ref->{Deletion} == $objectUnknown)
			{
			$ref->{FakeDeletion} = $row;
			#print "Object $ref->{SrcObj}->{Name} fake deletion $absoluteRow\n";
			}
		}
	}
	
sub fixObjectsWithUnknownCreationDeletion()
	{
	foreach my $ref (@objects) {
		if ($ref->{Hide} == 1) {
			next;
			}
		if ($ref->{Deletion} == $objectUnknown && $ref->{FakeDeletion}) {
			$ref->{Deletion} = $ref->{FakeDeletion};
			#print "Object $ref->{Name} fixing deletion to $ref->{FakeDeletion}\n";
			}
		}
	}

#
# Generate SVG attributes suitable for an element with a popup.
#
# inputs:
# textRef	Reference to a string variable which, when non-null, indicates
#			that the attributes should additionally contain an onclick popup.
# attrs		Initial attribute values.
#

sub generatePopupAttrs($$)
	{
	my ($textRef,$attrs) = @_;
	if ($opt_p && ${$textRef} ne "")
		{
		# non-null popup text, so append popup attribute
		$attrs .= qq{ onclick="popup(evt,'${$textRef}')"};
		# nullify the text - important not to leave behind stray popup text
		# that can inadvertantly be utilised in a later seq primitive that does
		# not specify a "pn" seq primitive.
		${$textRef} = "";
		}
	return $attrs;
	}

sub relativeRowToY($)
	{
	return $topOffset + ($_[0] + 1) * $incrementY;
	}

#
# Draw the vertical object lifelines between the object creation and destruction points
#
# main inputs: global $screenHeight

# Partitioning: need to modify this to take into account the beginning and ending row of the partition being
# displayed.

sub drawObjectLifelines($$$)
	{
	my ($startRow, $endRow, $screenHeight) = @_;
	my $x;
	my $lifelineBottomOfScreenY = $screenHeight - $topOffset + $lifelineBottomOverrun;
	my $afterLastRow = $endRow + 1;
	my $beforeFirstRow = $startRow - 1;

#print "\ndrawObjectLifelines(startRow $startRow, endRow $endRow, screenHeight $screenHeight)\n";

	foreach my $i (@objects)
		{
		if ($i->{Hide} == 1)
			{
			next;
			}

		$x = $i->{X};

		my $creationRow = $i->{Creation};
		my $deletionRow = $i->{Deletion};
			
#		print "Before: $i->{Name}, creationRow $creationRow, deletionRow $deletionRow\n";

		if ($creationRow == $objectUnknown)
			{
			$creationRow = $beforeFirstRow;
			}

		if ($deletionRow == $objectUnknown)
			{
			$deletionRow = $afterLastRow;
			}

		my $deletionY;
		my $creationY;

#		print "After: $i->{Name}, creationRow $creationRow, deletionRow $deletionRow\n";

		my $idAttr = qq{id="$i->{Name}"};
		if ($deletionRow < $startRow || $creationRow > $endRow)
			{
			outputVerticalLine($x, $topOffset, $lifelineBottomOfScreenY, $deadColour, "");
			}
		elsif ($creationRow < $startRow && $deletionRow > $endRow)
			{
			outputVerticalLine($x, $topOffset, $lifelineBottomOfScreenY, $aliveColour, $idAttr);
			}
		elsif ($creationRow < $startRow && IsInRangeInclusive($deletionRow, $startRow, $endRow))
			{
#			$deletionY = $topOffset + ($deletionRow - $startRow + 1) * $incrementY;
			$deletionY = relativeRowToY($deletionRow - $startRow);
			outputVerticalLine($x, $topOffset, $deletionY, $aliveColour, $idAttr);
			outputVerticalLine($x, $deletionY, $lifelineBottomOfScreenY, $deadColour, "");
			}
		elsif ($deletionRow > $endRow && IsInRangeInclusive($creationRow, $startRow, $endRow))
			{
			$creationY = relativeRowToY($creationRow - $startRow);
#			$creationY = $topOffset + ($creationRow - $startRow + 1) * $incrementY;
			outputVerticalLine($x, $topOffset, $creationY, $deadColour, "");
			outputVerticalLine($x, $creationY, $lifelineBottomOfScreenY, $aliveColour, $idAttr);
			}
		else
			{
#			$creationY = $topOffset + ($creationRow - $startRow + 1) * $incrementY;
#			$deletionY = $topOffset + ($deletionRow - $startRow + 1) * $incrementY;
			$creationY = relativeRowToY($creationRow - $startRow);
			$deletionY = relativeRowToY($deletionRow - $startRow);
			outputVerticalLine($x, $topOffset, $creationY, $deadColour, "");
			outputVerticalLine($x, $creationY, $deletionY, $aliveColour, $idAttr);
			outputVerticalLine($x, $deletionY, $lifelineBottomOfScreenY, $deadColour, "");
			}
		}
	}

sub drawActivities($$$$)
	{
	my ($startRow, $endRow, $screenHeight, $viewNumber) = @_;
#	print "drawActivities(startRow $startRow, endRow $endRow, screenHeight $screenHeight)\n";
	my $x;
	my $beforeFirstRow = $startRow - 1;
	my $afterLastRow = $endRow + 1;
	my $bottomOfScreenY = $screenHeight - $topOffset + $lifelineBottomOverrun;
	foreach my $obj (@objects)
		{
		if ($obj->{Hide} == 1)
			{
			next;
			}

		if (defined ($obj->{Activities}))
			{
			foreach my $act (@{$obj->{Activities}})
				{
#				print "\t$obj->{Name} : ", $act->{Start}, " -> ", $act->{End}, ", $act->{Line}, $act->{Addr}\n";
				my $startAct = $act->{Start};
				my $endAct = $act->{End};
				my $startY;
				my $endY;
				my $startContinuation;
				my $endContinuation;
				
				# "EndUnknown" indicates that the activity endpoint is not known, so mark
				# it in a different colour to highlight this.  Note that {End} may still
				# be set in this case - it just means that is a hint as to the last possible place
				# where it could have been terminated, not necessarily that it actually terminated
				# at that point.  Also, {End} may be -1, which means we don't even have a hint
				# as to where it terminated, so just assume the node termination point.
				my $colour = $act->{EndUnknown} == 1 ? $activityEndUnknownColour : $activityColour;
				if ($endAct == -1)		# unknown end point
					{
					# don't know activity end (logging missing).  Trim it to the object destruction
					# point (if known).
					if ($obj->{Deletion} != $objectUnknown)
						{
						$endAct = $obj->{Deletion};
						}
					else
						{
						$endAct = $afterLastRow;
						}
					$colour = $activityEndUnknownColour;
					}
				if ($startAct > $endRow || $endAct < $startRow)
					{
					# not visible at all in this page
					next;
					}
				if ($startAct < $startRow)
					{
					$startContinuation = 1;
					$startY = $topOffset;
					}
				else
					{
					$startContinuation = 0;
					$startY = relativeRowToY($startAct - $startRow);
					}
					
				if ($endAct > $endRow) {
					$endContinuation = 1;
					$endY = $bottomOfScreenY;
					}
				else
					{
					$endContinuation = 0;
					$endY = relativeRowToY($endAct - $startRow);
					}
#				print "\t\tstartY = $startY, endY = $endY\n";
				$x = $obj->{X};
				my $activityX = $x + ($act->{Line} * $activitySpacing);
				my $topCornerSize = 5;
				my $endUpwardsOffset = 10;
				
				# For activities that span multiple pages, ensure the name of the activity pop up box (i.e. when you
				# click on the activity line) includes the page where the activity started (e.g. "PRStart (p11)").
				# Just do this for pages after the one where the activity starts.
				my $pages = "";
				if ($startContinuation == 0)			# note page where activity started
					{
					$act->{StartPage} = $viewNumber;
					}
				else
					{
					$pages = " (p$act->{StartPage})";
					}
		
				my $event = qq{ onclick="popup(evt, '$act->{Name}$pages')" };
				if ($startContinuation == 1 && $endContinuation == 1)
					{
					outputVerticalLine($activityX, $startY, $endY, $colour, $event);
					}
				elsif ($startContinuation == 1 && $endContinuation == 0)
					{
					$endY -= $endUpwardsOffset;
					outputVerticalLine($activityX, $startY, $endY - $topCornerSize, $colour, $event);
					outputLine($activityX, $endY - $topCornerSize, $x, $endY, $colour, "");
					}
				elsif ($startContinuation == 0 && $endContinuation == 1)
					{
					outputLine($x, $startY, $activityX, $startY + $topCornerSize, $colour, "");
					outputVerticalLine($activityX, $startY + $topCornerSize, $endY, $colour, $event);
					}
				else
					{
					# $startContinuation == 0 && $endContinuation == 0
					$endY -= $endUpwardsOffset;
					outputLine($x, $startY, $activityX, $startY + $topCornerSize, $colour, "");
					outputVerticalLine($activityX, $startY + $topCornerSize, $endY - $topCornerSize, $colour, $event);
					outputLine($activityX, $endY - $topCornerSize, $x, $endY, $colour, "");
					}
				}
			}
		}
	}

sub readIniFile()
	{
	if ($opt_x) {
	        my $sectionName;
		open (INI, $opt_x) || die "Cannot open ini file $opt_x\n";
		while (<INI>) {
			chomp;
			# remove leading and trailing blanks and ignore blank lines
			s/^\s+//;
			s/\s+$//;
			if (! $_)
				{ next;	};

			if (/^\[(.+)\]/)
				{
				# parse section name from "[<section>]" lines
				$sectionName = $1;
				}
			else
				{
				if ($sectionName eq "DontHideShortName")
					{
					$dontHideShortName{$_} = 1;
					}
				}
			}
		close INI;
		}
	}

##########################
# HTML Embedder routines #
##########################

sub createViewMap()
{
	my $path = $outputPath . "html/logmap.html";
	open MAP, ">$path" || die "Cannot open $path for writing\n";
	print MAP "<html>\n<body>\n";
	
	# Begin page label/time table
	print MAP qq { <table border="1" style="text-align: center">\n };
	print MAP qq { <tr><th>Page</th><th colspan="2">Lines</th><th colspan="2">Times</th><th>Objects Created</th><th>Objects Destroyed</th></tr>\n };
}

sub closeViewMap($)
{
	# End page label/time table
	print MAP "</table>\n<p>\n";
	
	# begin object page lifetime table
	my $pageCount = $_[0];
	print MAP qq{ <table border="1">\n };
	print MAP qq{ <tr><th colspan="4">Object Lifetimes per Page</th></tr><tr><th>Object</th><th colspan="2">Start/End</th><th>Lifetime</th></tr>\n };
	foreach my $obj (@objects) {
		print MAP qq { <tr style="font-family: courier new">\n };
		my $last;
		my $name = $obj->{Name};
		my $style = "";
		if ($name =~ s/^!//) {
			$style = "font-weight: bold";
		}
		if ($style) {
			print MAP qq {<td style="$style">$name</td>};
		} else {
			print MAP "<td>$name</td>";
		}
		if (!defined($obj->{LastPage})) {
			print MAP qq {<td colspan=\"2\">$obj->{FirstPage}</td>};
			$last = $obj->{FirstPage};
		} else {
			print MAP qq {<td>$obj->{FirstPage}</td><td>$obj->{LastPage}</td>};
			$last = $obj->{LastPage};
		}
		print MAP qq{ <td><table style="font-family: courier new" border="1" cellpadding="0" cellspacing="0"><tr>\n };
		for (my $i = 0 ;$i < $pageCount ; ++$i) {
			my $anchor = ($i != $pageCount - 1) ? "html/log$i.html" : "log.html";
			if ($i < $obj->{FirstPage} || $i > $last) {
				print MAP "<td>&nbsp;</td>";
			} else {
				print MAP qq{ <td style="background-color: blue"><a href=$anchor>&nbsp;</a></td> };
			}
		}
		print MAP "</tr></table></td>\n";
	}
	print MAP "</table>\n";
	print MAP "<body>\n<\html>\n";
	close MAP;
}

sub outputViewMap($$$$$)
{
	my ($viewNumber, $lastViewNumber, $labelsOnPageRef, $objectsCreatedRef, $objectsDestroyedRef) = @_;
	my $firstLine;
	my $lastLine;
	my $firstTime;
	my $lastTime;
	foreach my $i (@{$labelsOnPageRef}) {
		if ($i =~ s/^#//) {
			if (! $firstLine) {
				$firstLine = $i;
			} else {
				$lastLine = $i;
			}
		} elsif ($i =~ m/^\d\d:\d\d:\d\d\.\d/) {
			if (! $firstTime) {
				$firstTime = $i;
			} else {
				$lastTime = $i;
			}
		}
	}
	$firstTime = "&nbsp;" if (!$firstTime);
	$firstLine = "&nbsp;" if (!$firstLine);
	$lastLine = "&nbsp;" if (!$lastLine);
	$lastTime = "&nbsp;" if (!$lastTime);
	my $anchor = $viewNumber != $lastViewNumber ? "html/log$viewNumber.html" : "log.html";
	print MAP qq { <tr><td><a href=\"$anchor\">$viewNumber</a></td><td>$firstLine</td><td>$lastLine</td><td>$firstTime</td><td>$lastTime</td>\n };
	print MAP qq { <td style="text-align: left ; font-family: courier new">\n };
	my $name = "";
	outputViewObjects($objectsCreatedRef);
	print MAP qq { </td><td style="text-align: left ; font-family: courier new"> \n };
	outputViewObjects($objectsDestroyedRef);
	print MAP "</td></tr>\n";
}

sub outputViewObjects($)
{
	my $objectsRef = $_[0];
	my $name = "";
	foreach my $obj (@{$objectsRef}) {
		my $style = "";
		if (ref($obj) eq "HASH") {
			$name = $obj->{Name};		# obj = reference to (SVG) object
			if ($name =~ s/^!//) {
				$style = "font-weight: bold";
			}
		} else {
			$name = $obj;				# obj = thread name
			$style = "font-style: italic; font-weight: bold";
		}
		if ($style) {					# highlight processes/threads
			print MAP "<span style=\"$style\">$name</span> ";
		} else {
			print MAP "$name ";
		}
	}
	if (!$name) {
		print MAP "&nbsp;";
	}
}

sub updateObjectViewList($)
{
	my ($viewNumber) = @_;
	foreach my $obj (@objects) {
		if ($obj->{Hide} == 1) {
			next;
		}
		if (! defined($obj->{FirstPage})) {
			$obj->{FirstPage} = $viewNumber;
		}
		elsif (! defined($obj->{LastPage}) || $viewNumber > $obj->{LastPage}) {
			$obj->{LastPage} = $viewNumber;
		}
	}
}

sub createIpcMap()
{
	my $path = $outputPath . "html/ipcmap.html";
	open IPC, ">$path" || die "Cannot open $path for writing\n";
	print IPC "<html><head>\n";
	print IPC qq{<style type="text/css">};
	print IPC "td,th { text-align: left; font-family: courier new; white-space: nowrap ; font-size: smaller}\n";
	print IPC "</style></head><body>\n";
	
	# Begin page label/time table
	print IPC qq { <table border="1" >\n };
	print IPC qq { <tr><th style="text-align: center">Page</th><th style="text-align: center">EXEs...</th></tr>\n };
}

sub closeIpcMap()
{
	print IPC "</table></body></html>\n";
	close IPC;
}

my @IpcExeColumns = ();

sub addToArrayUniquely($$)
{
	my ($arrRef, $element) = @_;
	foreach my $i ( @{$arrRef} ) {
		if ($i eq $element) {
			return;
		}
	}
	push @{$arrRef}, $element;
}

sub outputIpcMap($$$)
{
	my ($viewNumber, $lastViewNumber, $ipcRef) = @_;
	my %ipcExeUsed = ();
	my $exe;
	my $ipc;
	foreach $ipc (@{$ipcRef}) {
		$exe = $ipc->{Object}->{Name};
		addToArrayUniquely(\@IpcExeColumns, $exe);
		$ipcExeUsed{$exe} = 1;
	}
	
	# Page number in first column (two rows)
	#my $style = qq{style="text-align: left; font-family: courier new; white-space: nowrap ; font-size: smaller"};
	print IPC qq {<tr><td rowspan="2" style="text-align: center">$viewNumber</td>\n};
	
	# Second and subsequent columns showing the exe names
	foreach $exe ( @IpcExeColumns ) {
		my $exe2 = $exe;			# why do things go wrong if I operate directly on $exe???
		$exe2 =~ s/^!//;
		if (!defined($ipcExeUsed{$exe})) {
			print IPC qq{<td style="text-align: center ; color: gray">$exe2</td>};
		} else {
			print IPC qq{<td style="text-align: center">$exe2</td>};
		}
	}

	# next row
	print IPC qq{</tr>\n<tr>};
	# for each exe there is column...
	my $rr;
	foreach $rr ( @IpcExeColumns ) {
		print IPC qq{<td>};
		# ...showing the IPCs
		foreach my $ref ( @{$ipcRef} ) {
			if ($ref->{Object}->{Name} eq $rr) {
				print IPC qq{$ref->{Text}<br>};
			} else {
				print IPC qq{<br>};
			}
		}
		print IPC qq{</td>};
	}
	print IPC qq {</tr>\n};
}

#
# Output the "0 1 2 3 4 ..." anchors representing the different views
#
sub outputHTMLPageLinks($$$$) {
	my ($viewNumber, $lastViewNumber, $htmlPathNotLastView, $htmlPathLastView) = @_;
	my $i;
	for ($i = 0 ; $i <= $lastViewNumber ; ++$i) {
		if ($i == $viewNumber) {
			print HTML qq{&nbsp; <b>$i</b>\n};
		} else {
			if ($i != $lastViewNumber) {
				print HTML qq{&nbsp;<a href="${htmlPathNotLastView}log$i.html">$i</a>\n};
			} else {
				print HTML qq{&nbsp;<a href="${htmlPathLastView}log.html">$i</a>\n};
			}
		}
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

sub outputHTMLEmbedder($$$$$)
{
	my ($width, $height, $viewNumber, $lastViewNumber, $fileName) = @_;
	#
	# leave log.html in the current directory, everything else goes into "html/" subdirectory
	#
	my $htmlPath;
	my $svgPathInHtmlFile;
	my $htmlLinkPathLastView;
	my $htmlLinkPathNotLastView;
	my $mainPath;
	if ($viewNumber != $lastViewNumber) {
		$htmlPath = "html/" . $fileName;
		$htmlLinkPathLastView = "../";
		$htmlLinkPathNotLastView = "";
		$svgPathInHtmlFile = $fileName;
		$mainPath = "../";
	} else {
		$htmlPath = $fileName;
		$htmlLinkPathLastView = "html/";
		$htmlLinkPathNotLastView = "html/";
		$svgPathInHtmlFile = "html/" . $fileName;
		$mainPath = "";
	}
	my $path = $outputPath . $htmlPath . ".html";
	open HTML, ">$path" || die "Cannot open $path for writing\n";

	if (! $opt_f) {
		# DOCTYPE needed for "position: fixed" to work in IE 
		print HTML "<!--[if IE]>\n";
		print HTML qq{<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n};
		print HTML "<![endif]-->\n";
		print HTML qq{<html>\n<head>\n<style type="text/css">\n};
		print HTML qq{#pagelinks { position: fixed; padding-right: 1px; top: 0px ; left: 0px; height: auto; background-color: lightyellow; border: 1px solid black }\n};
		# body "background-color: white": On machines with the system wide window background set to grey rather than white, using "...wmode="transparent"..." in the <embed>
		# shows up the (normally invisible) small white textual matt blocks that are written over the node lifeline to produce a break in the lifeline before the node
		# name is written over the top.  Set the background to white to hide these.
		print HTML qq{body { background-color: white }\n};
		print HTML qq{</style>\n</head>\n<body onLoad="loadPosition()" onUnload="savePosition()">\n};
		# Output a duplicate hidden set of pagelinks in the same position as the fixed ones.  The hidden pagelinks serve to
		# reserve vertical space at the top of the display for the fixed links to sit on top of and prevent them obscuring
		# the top of the SVG image.  Also, if the main window width is made smaller than the width of the fixed page links,
		# the latter will resize downward and start obscuring the top of the SVG image.  The hidden pagelinks prevent this
		# by resizing downward at the same time and so, not being fixed, will automatically reserve the right amount of space
		# at the top of the display for the fixed pagelinks to reside in.  A better way would be nice, but I don't know it.
		print HTML qq{<div style="visibility: hidden">};
		outputHTMLPageLinks($viewNumber, $lastViewNumber, $htmlLinkPathNotLastView, $htmlLinkPathLastView);
		print HTML qq{</div><div id="pagelinks">\n};
	} else {
		print HTML "<html>\n<body>\n";
	}
	outputHTMLPageLinks($viewNumber, $lastViewNumber, $htmlLinkPathNotLastView, $htmlLinkPathLastView);
	my $embedExtraArgs = "";
	if (! $opt_f) {
		print HTML "</div>\n";
		# wmode="transparent": In IE the "position: fixed" pagelinks will otherwise be hidden underneath the SVG image.
		$embedExtraArgs = qq{ wmode="transparent"};
	}
	print HTML qq{<embed src="$svgPathInHtmlFile.svg" width="$width" height="$height" type="image/svg+xml" pluginspage="http://www.adobe.com/svg/viewer/install/" name="embedder"$embedExtraArgs>\n};
	my $limitedText = limitedRowText();
	if ($limitedText)
		{
		print HTML "<small>$limitedText</small>&nbsp;\n";
		}

	print HTML qq{<input type="button" value="Symbols" onclick="popupSymbols()">\n};
	print HTML qq{<input type="button" value="Relations" onclick="popupRelations()">\n};
	print HTML qq{<input type="button" value="Map" onclick="popupMap()">\n};
	print HTML qq{<input type="button" value="IPC Map" onclick="popupIpcMap()">\n};
	print HTML qq{&nbsp;Zoom%\n<input type="text" size=3 maxlength=3 onkeypress="zoom(this,event)" />\n};

	if ($opt_f) {
		# don't need the bottom page links if the fixed position ones are at the top
		outputHTMLPageLinks($viewNumber, $lastViewNumber, $htmlLinkPathNotLastView, $htmlLinkPathLastView);
	}

	print HTML qq{<script type="text/javascript">\n};
	print HTML qq{var mainPath = "$mainPath";\n};
	print HTML qq{var width = $width;\nvar height = $height;\n};
	print HTML qq{var cookieName = "PagePos$viewNumber";\n};

#################################
# Begin interpolated JavaScript #
#################################
	print HTML<<'EOT'

var relationsDefined = 0;
var zoomPercent = 100;

function zoom(obj,event) {
	var keycode;
	if (window.event) {
		keycode = window.event.keyCode;
	} else
	if (event) {
		keycode = event.which;
	} else
		return true;
	if (keycode == 13) {		// <return>
		var percent = obj.value;
		if (percent > 0 && percent <= 100) {
			if (percent != zoomPercent) {
				zoomPercent = percent;
				setViewBox();
			}
		} else {
			alert("Zoom value must be between 1 and 100");
		}
	}
}

function setViewBox() {
	var viewBoxSize = "0 0 " + width*100/zoomPercent + " " + height*100/zoomPercent;
	document.embeds[0].getSVGDocument().documentElement.setAttribute("viewBox", viewBoxSize);
}

// Functions for preserving original scroll position on each page

function cookieToHash() {
	var cookie = document.cookie;
	var bits = cookie.split('&');
	var hash = new Array();
	for (b in bits) {
	    if (bits[b].length > 0)
	       {
	       var keypair = bits[b].split('=');
	       var key = unescape(keypair[0]);
	       var value = unescape(keypair[1]);
	       hash[key] = value;
	       }
	}
	return hash;
}

function hashToCookie(hash) {
	var str = "";
	for (h in hash) {
	    str = str + escape(h) + "=" + escape(hash[h]) + "&";
	}
	document.cookie = str;
}

function savePosition() {
	var hash = cookieToHash();
	var x = window.pageXOffset ? window.pageXOffset : document.documentElement.scrollLeft;
	var y = window.pageYOffset ? window.pageYOffset : document.documentElement.scrollTop;
	hash[cookieName] = x + "," + y;
	hashToCookie(hash);
}

function loadPosition() {
	var hash = cookieToHash();
	var cookie = hash[cookieName];
	if (cookie) {
		var position = cookie.split(',');
		window.scrollTo(position[0], position[1]);
	}
}

//

function popupSymbols() {
	symbols = window.open(mainPath + "html/logsym.html", "_blank", "width=400,resizable=yes,scrollbars=yes");
}

function popupRelations() {
	relations = window.open(mainPath + "html/relations.html", "_blank", "resizable=yes,scrollbars=yes,status=yes");
	relationsDefined = 1;
}

function popupMap() {
	relations = window.open(mainPath + "html/logmap.html", "_blank", "resizable=yes,scrollbars=yes,status=yes");
}

function popupIpcMap() {
	relations = window.open(mainPath + "html/ipcmap.html", "_blank", "resizable=yes,scrollbars=yes,status=yes");
}

//

function objectHighlightForwarder(id) {
	if (relationsDefined && !relations.closed) {
		relations.relationsObjectHighlightPtr(id);
	}
}

function messageHighlightForwarder(id) {
	if (relationsDefined && !relations.closed) {
		relations.relationsMessageHighlightPtr(id);
	}
}

function display(str, obj)
	{
	var s = "Browser::" + str + ": ";
	var t = "";
	for (i in obj)
		{
		t = typeof obj[i];
		s += i + " = ";
		if (t != "unknown") {
			var u = "";
			u += obj[i];
			if (u.indexOf("function") < 0)
				{
				s += u;
				}
			else
				{
				s += "(fn)";
				}
			}
		else
			{
			s += "(unk)";
			}
		s += ",   ";
		}
	alert(s);
	}

</script>
</body>
</html>
EOT
;
###############################
# End interpolated JavaScript #
###############################
	close HTML;
}
	


#######################
# SVG output routines
#######################

sub outputDocHeader()
	{
	my ($width,$height) = @_;
	print SVG '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',"\n";
	print SVG '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">',"\n";
	print SVG "<svg height=\"$height\" width=\"$width\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
	outputDefs();
	}

sub outputDefs()
	{
	print SVG "<defs>\n";
	print SVG qq{<marker id="recvArr" markerHeight="10" markerUnits="strokeWidth" markerWidth="10" orient="auto" refX="10" refY="5" viewBox="0 0 10 10">\n};
	print SVG qq{\t<path d="M 0 0 L 10 5 L 0 10 " />\n};
	print SVG "</marker>\n";
	if ($opt_p) {
		print SVG qq{<marker id="postArr" stroke="$postColour" fill="white" markerHeight="10" markerUnits="strokeWidth" markerWidth="10" orient="auto" refX="10" refY="5" viewBox="0 0 10 10">\n};
		print SVG qq{\t<path d="M 0 0 L 10 5 L 0 10 Z" />\n};
		print SVG "</marker>\n";
	}

	print SVG qq{<marker id="callArr" stroke="black" fill="white" markerHeight="10" markerUnits="strokeWidth" markerWidth="10" orient="auto" refX="10" refY="5" viewBox="0 0 10 10">\n};
	print SVG qq{\t<path d="M 0 5 L 10 5 L 0 0 L 10 5 L 0 10 L 10 5" />\n};
	print SVG "</marker>\n";
	
	print SVG "</defs>\n";
	}

#
# Output label at lefthand side - used for timestamp
#

sub outputLeftLabel($)
	{
	my ($text) = @_;
	outputText(0, $text, $currentY, "begin","black", "");
	}

sub outputTextSeperator($$)
	{
	my ($text, $y) = @_;
	outputRect($leftSeperatorMargin, $y - $fontHeight, $screenWidth, $fontHeight, $seperatorColour, $seperatorColour);
	outputText($leftSeperatorMargin, $text, $y, "begin", "black", "");
	}


sub outputLabelledLine()
	{
	my ($x1,$x2,$text,$colour,$endType,$alignment, $otherAttrs) = @_;
	outputHorizontalLine($x1,$x2,$colour,$endType,$otherAttrs);
	my $textx;
	my $anchor;

	if (! $colour) {
		$colour = "black";
	}

	if (!$alignment || $alignment eq "mid") {
		$textx = (($x1 + $x2) / 2);
		$anchor = "middle";
		}
	else {
		if ($alignment eq "head") {
			if ($x1 < $x2) {
				$anchor = "end";
				$textx = $x2 - $arrowWidth - $textArrowSpacing;
				}
			else {
				$anchor = "start";
				$textx = $x2 + $arrowWidth + $textArrowSpacing;
				}
		} else {	# "tail"
			$textx = $x1;
			if ($x1 < $x2) {
				$anchor = "start";
			} else {
				$anchor = "end";
				}
			}
		}

	outputText($textx,$text,$currentY - $messageSpacingAboveLine, $anchor, $colour, $otherAttrs);
	}

sub outputText()
	{
	my ($x,$text,$y,$anchor,$colour, $otherAttrs) = @_;
	$text = encode_entities($text);

	my $attrs = qq{ x="$x" y="$y" $otherAttrs };

	if ($anchor)
		{
		$attrs .= qq { text-anchor="$anchor" };
		}

	if ($colour)
		{
		if ($colour =~ s/^!//)
			{
			outputRect($x - 1, $y - $fontHeight, $textMattSize, $fontHeight, "white", "white");
			}
		if ($colour)
			{
			$attrs .= qq { fill="$colour" };
			}
		}

	print SVG "<text $attrs>$text</text>\n";
	}

sub outputLine()
	{
	my ($x1,$y1,$x2,$y2,$colour,$otherAttrs) = @_;

	if (! $colour) {
		$colour = "black";
	}
	
	print SVG qq{<line stroke="$colour" x1="$x1" y1="$y1" x2="$x2" y2="$y2" $otherAttrs />\n};
	}

sub outputHorizontalLine()
	{
	my ($x1,$x2,$colour,$endType,$otherAttrs) = @_;

	if (! $colour) {
		$colour = "black";
	}
	
	print SVG qq{<line stroke="$colour" marker-end="url(#$endType)" x1="$x1" y1="$currentY" x2="$x2" y2="$currentY" $otherAttrs />\n};
	}

sub outputVerticalLine()
	{
	my ($x,$y1,$y2,$colour,$otherAttrs) = @_;
	if (! $colour) { $colour = "black"; }
	print SVG qq{<line stroke="$colour" x1="$x" y1="$y1" x2="$x" y2="$y2" $otherAttrs />\n};
	}

sub outputRect($$$$)
	{
	my ($x,$y,$width,$height,$fill,$stroke) = @_;
	print SVG qq {<rect fill="$fill" stroke="$stroke" x="$x" y="$y" width="$width" height="$height" />\n};
	}

sub incrementY()
	{
	my $amount;
	if ($_[0]) {
		$amount = $_[0];
	} else {
		$amount = 1;
	}
	$currentY += $incrementY * $amount;
	}

sub outputDocFooter()
	{
###################################
#  Begin Interpolated JavaScript  #
###################################
	print SVG <<'EOT'
<script type="text/ecmascript"> <![CDATA[

var savedAttributes = new Object();
var nodeListDefined = 0;
var freePopups = new Array();

createBindingToChild();

function relationsObjectHighlight(nodeName)
	{
	parent.objectHighlightForwarder(nodeName);
	}

function relationsMessageHighlight(nodeName)
	{
	parent.messageHighlightForwarder(nodeName);
	}

function createBindingToChild()
	{
	parent.sequenceMessageHighlightPtr = messageHighlight;
	parent.sequenceObjectHighlightPtr = objectHighlight;
	}

function popup(evt, popupText) {
	var documentElement = document.documentElement;
	var trans = documentElement.currentTranslate;	// in case canvas moved with "the hand"

	var group = allocPopup(popupText);
	var margin = 2;

	var rect = group.firstChild;
	var text = rect.nextSibling;

	if (group.parentNode == null) {
		documentElement.appendChild(group);
	}
	var x = evt.clientX - trans.x;
	var y = evt.clientY - trans.y;

	// Compensate for svg viewBox attribute being different size to svg width/height attributes - as a result of "zoom" functionality
	var viewBox = documentElement.getAttribute("viewBox");
	if (viewBox) {
		var viewBoxArray = viewBox.split(" ", 4);
		var viewBoxWidth = parseInt(viewBoxArray[2]);
		var viewBoxHeight = parseInt(viewBoxArray[3]);
		var svgWidth = parseInt(documentElement.getAttribute("width"));
		var svgHeight = parseInt(documentElement.getAttribute("height"));
		if (viewBoxWidth != svgWidth) {
			x = Math.floor(x * viewBoxWidth / svgWidth);
		}
		if (viewBoxHeight != svgHeight) {
			y = Math.floor(y * viewBoxHeight / svgHeight);
		}
	}
	// text
	text.setAttribute("x", x + margin);
	text.setAttribute("y", y);
	var textNode = text.firstChild;
	textNode.nodeValue = popupText;

	// background rectangle
	var bbox = text.getBBox();
	var textYOffset = bbox.y - y;
	rect.setAttribute("width", bbox.width + margin * 2);
	rect.setAttribute("height", bbox.height + margin * 2);
	rect.setAttribute("x", x);
	rect.setAttribute("y", y + textYOffset - margin);

	// make popup visible
	group.setAttribute("display", "inherit");

	if (evt.shiftKey)
		{
		messageHighlight(evt.currentTarget.id);
		relationsMessageHighlight(evt.currentTarget.id);
		}
}

function popdown(evt) {
	var element = evt.target.parentNode;		// "g" element
	element.setAttribute("display", "none");
	freePopup(element);
}

function freePopup(element) {
	var i;
	// Popups that have been dismissed become invisible and placed into a free pool from which
	// they can be re-allocated and re-used when new popups are required.  The only reason to
	// do this rather than doing documentElement.removeChild(group), deleting the group element,
	// and re-creating the group element afresh next time a popup is required is that the
	// removeChild() seems to take a visible period of time to execute (i.e. about a second) before
	// the popup disappears, whereas just making the group element invisible and avoiding the
	// removeChild() looks instantaneous.

	for (i = 0 ; i < freePopups.length ; ++i) {
		if (freePopups[i] == null) {
			freePopups[i] = element;	// add to existing slot in free list
			return;
		}
	}
	freePopups.push(element);			// add to end of free list
}

function allocPopup(popupText) {
	var i;
	for (i = 0 ; i < freePopups.length ; ++i) {
		if (freePopups[i] != null) {
			var popup = freePopups[i];	// use existing popup
			freePopups[i] = null;
			// navigate from popup ("g") -> "rect" -> "text" -> textNode
			popup.firstChild.nextSibling.firstChild.nodeValue = popupText;
			return popup;
		}
	}

	var group = document.createElementNS("http://www.w3.org/2000/svg", "g");			// create group
	group.setAttribute("onclick", "popdown(evt)");

	var rect = document.createElementNS("http://www.w3.org/2000/svg", "rect");			// create rect
	rect.setAttribute("fill", "yellow");

	// create <text> and text within it
	var text = document.createElementNS("http://www.w3.org/2000/svg", "text");			// create text
	var textNode = document.createTextNode(popupText);
	text.appendChild(textNode);

	group.appendChild(rect);	// rect first...
	group.appendChild(text);	// ...text on top of it

	return group;
}

// save current attribute setting of a node and set new attribtue value.
//
// savedAttributes is used to store all the attributes and original values of elements
// whose attributes have been changed.  It is an object whose properties are attribute names.
// Each of these attribute names are, in turn, objects whose properties are attribute values.
// Each of these attribute values are, in turn, an array of elements.
// For example:
//	savedAttributes = { "stroke" : { "red"   : [element, ...], "blue" : [element, ...] },
//						"fill"   : { "green" : [element, ...], "pink" : [element, ...], "yellow" : [element, ...] }

function saveAndSetAttribute(el, attrName, newVal)
{
	if (savedAttributes[attrName] == null) {
		savedAttributes[attrName] = new Object();
	}

	var val = el.getAttribute(attrName);
	if (savedAttributes[attrName][val] == null) {
		savedAttributes[attrName][val] = new Array();
		}
	savedAttributes[attrName][val].push(el);

	el.setAttribute(attrName, newVal);
}

// restore all saved node attributes
function restoreAttributes()
{
	for (var attrName in savedAttributes) {
		for (var val in savedAttributes[attrName]) {
			while (savedAttributes[attrName][val].length > 0) {
				savedAttributes[attrName][val].pop().setAttribute(attrName, val);
			}
		}
	}
}

// highlight a node, saving its changed attributes to restore later
function highlightNode(el)
	{
	if (el.tagName == "line")
		{
		saveAndSetAttribute(el, "stroke", "orange");
		}
	else
	if (el.tagName == "text")
		{
		saveAndSetAttribute(el, "fill", "orange");
		}
	}

function messageHighlight(nodeNamesString)
	{
	restoreAttributes();

	// given a string "x_y" highlight all nodes with tags "x", "y", "x_y" or "y_x"

	var nodeNames = nodeNamesString.split("_", 2);

	if (nodeNames.length == 2)
		{	
		var nodeList = getNodeList();
		var count = nodeList.length;
		var el;
		var reverseNodeNamesString = nodeNames[1] + "_" + nodeNames[0];
		for (i = 0 ; i < count ; i++)
			{
			el = nodeList.item(i);
			if (el.id == nodeNames[0] || el.id == nodeNames[1] || el.id == nodeNamesString || el.id == reverseNodeNamesString)
				{
				highlightNode(el);
				}
			}
		}

	}

// retrieve the node list, creating it (once) if necessary
function getNodeList()
{
	if (!nodeListDefined) {
		nodeList = document.getElementsByTagName("*");
		nodeListDefined = 1;
		}
	return nodeList;
}

function objectHighlight(nodeName)
{
	restoreAttributes();

	// given a string "x" highlight all nodes with tags "x", "x_<y>", "<y>_x", where <y> is any node name.  Then,
	// highlight all nodes with tag <y> for all values of <y> that were found.

	var nodeNameLen = nodeName.length;
	var nodeList = getNodeList();

	var count = nodeList.length;
	var el;
	var otherNodes = new Object();

	for (i = 0 ; i < count ; i++)
		{
		el = nodeList.item(i);
		if (el.id == nodeName)						// nodes with tag "x"
			{
			highlightNode(el);
			}
		else {
			var underlinePos = el.id.indexOf("_");
			var nodePos;
			if (underlinePos != -1) {
				nodePos = el.id.indexOf(nodeName);
				if (nodePos != -1) {
					if (nodePos == 0 && underlinePos == nodeNameLen) {	// nodes with tag "x_<y>"
						highlightNode(el);
						otherNodes[el.id.substr(underlinePos+1)] = 1;
					} else
					if (nodePos == underlinePos + 1 && el.id.length == nodeNameLen + underlinePos + 1)	{	// nodes with tag "<y>_x"
						highlightNode(el);
						otherNodes[el.id.substr(0, underlinePos)] = 1;
					}
				}
			}
		}
	}

	// nodes with tag "<y>"
	for (i = 0 ; i < count ; ++i) {
		el = nodeList.item(i);
		for (j in otherNodes) {
			if (el.id != nodeName && el.id == j) {
				highlightNode(el);
			}
		}
	}
}

function debugEvent(evt)
	{
	objectHighlight(evt.currentTarget.id);
	relationsObjectHighlight(evt.currentTarget.id);
	}

function display(str, obj)
	{
	var s = "SVG::" + str + ": ";
	var t = "";
	for (i in obj)
		{
		t = typeof obj[i];
		s += i + " = ";
		if (t != "unknown") {
			var u = "";
			u += obj[i];
			if (u.indexOf("function") < 0)
				{
				s += u;
				}
			else
				{
				s += "(function)";
				}
			}
		else
			{
			s += "(unknown)";
			}
		s += ",   ";
		}
	alert(s);
	}

]]> </script>
</svg>
EOT
#################################
#  End Interpolated JavaScript  #
#################################
	}
