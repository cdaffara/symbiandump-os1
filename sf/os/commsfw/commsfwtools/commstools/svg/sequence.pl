# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#

use strict;

#
# constants
#
my $incrementY = 20;
my $leftMargin = 60;
my $rightMargin = 150;
my $topMargin = 20;
my $topOffset = $topMargin; # is this right Nadeem?
my $objectSpacing = 110;
my $arrowWidth = 10;
my $textArrowSpacing = 1;
my $messageSpacingAboveLine = 1;
my $objectVerticalLineSpacing = 2;
my $objectUnderlineToLifelineGap = 2;

my $objectName = 0;
my $objectX = 1;
my $objectY = 2;
my $objectMsgCount = 3;
my $objectCreationPointKnown = 4;

my $sequenceAction = 0;
my $sequenceObjRef = 1;		# if action == "t" || "oc"
my $sequenceMsgRef = 1;		# if action != "t"
my $sequenceX = 2;
my $sequenceY = 3;

my $trimSilentFlag = 0;
my $objectsDisplayedAtCreationPoint = 1;
#
# global variables
#
my $currentY = $topMargin;

my @sequences = ();
my @objects = ();
my @messages = ();



#
# Data Structures:
#
# "objects" is array of arrays:
#	[ <object name> <object X> <object Y> <object message count> <creation point known flag> ]
#
# "sequences" is an array of arrays:
#	[ "t" \objects[<n>] "text" ]
#	[ "[p|r]" \messages[n] \objects[<source>], \objects[<destination>] ]
#	[ "oc" \objects[<n>] ]


# Input file format:
#
#	<action> <arguments> ...
#
# specifically:
#
#	[P|R] <message name> <source object> <destination object>
#	T <object> <text>
#	OC <object>
#
# actions:
#
# P		Post
# R		Receive
# T		Text
# OC	Object Create
#
# All fields are space separated text.  For example:
#
# P StartFlow SCPR Flow
#

while (<>) {
	die unless s/^(\w+)\s+//;
	my $action = $1;
	if ($action eq "t")
		{
		# Text
		# t <object> <text>
		my $pos;
		if (s/^(\w+)\s+//) { $pos = $1; }
		my $objRef = addObject($pos);
		chomp;
		my $text = $_;
		push @sequences, [$action, $objRef, $text];
		}
	elsif ($action eq "oc")
		{
		# Object Create
		# oc <object>
		my $objName;
		if (s/^(\w+)\s+//) { $objName = $1; }
		my $objRef = addObject($objName);
		${$objRef}->[$objectCreationPointKnown] = 1;
		chomp;
		push @sequences, [$action, $objRef];
		}
	else
		{
		# Post/Receive
		# [P|R] <message> <source object> <destination object>
		split;
		my $msgRef = addMessage(shift @_);
		my $srcRef = addObject(shift @_);
		my $destRef = addObject(shift @_);
		${$srcRef}->[$objectMsgCount]++;
		${$destRef}->[$objectMsgCount]++;
		push @sequences, [$action, $msgRef, $srcRef, $destRef];
		}
	}

if ($trimSilentFlag)
	{
	trimSilentObjects();
	}
calculateObjectRowPositions();

my $screenWidth = $objects[$#objects]->[$objectX] + $rightMargin;
my $screenHeight = scalar(@sequences) * ($incrementY + 2) + $topMargin;


outputDocHeader($screenWidth, $screenHeight);

drawObjectsAtTop();

drawObjectsRepeatedly();

drawSequences();

outputDocFooter();

####################
# Message routines
####################

sub addMessage()
	{
	my $obj = $_[0];
	my $i;
	for ($i = 0 ; $i < scalar(@messages) ; ++$i) {
		if ($messages[$i] eq $obj) {
			return \$messages[$i];
			}
		}
	$messages[$i] = $obj;
	return \$messages[$i];
	}

sub printMessages()
	{
	print "Messages (", scalar(@messages), ") : ";
	foreach my $msg (@messages) {
		print $msg, " ";
		}
	print "\n";
	}

###################
# Object routines
###################

sub addObject()
	{
	my $objName = $_[0];
	my $i;
	for ($i = 0 ; $i < scalar(@objects) ; ++$i) {
		if ($objects[$i]->[$objectName] eq $objName) {
			return \$objects[$i];
			}
		}
		$objects[$i] = [ $objName, 0, 0, 0, 0 ];
		return \$objects[$i];
	}

sub printObjects()
	{
	print "Objects (", scalar(@objects), "): ";
	foreach my $obj (@objects) {
		print $obj->[0], "(", $obj->[1], ") ";
		}
	print "\n";
	}

sub calculateObjectRowPositions()
	{
	# first object position is at left margin
	$objects[0][$objectX] = $leftMargin;
	my $i;
	for ($i = 1 ; $i < scalar(@objects) ; ++$i) {
		$objects[$i][$objectX] = $objects[$i-1][$objectX] + $objectSpacing;
		}
	}

sub trimSilentObjects()
	{
	# get rid of objects that didn't end up with any messages sent to/from them
	my $i;
	for ($i = 0 ; $i < scalar(@objects) ; ) {
		if ($objects[$i][$objectMsgCount] == 0)
			{
			my $j;
			for ($j = 0 ; $j < scalar(@sequences) ; )
				{
				my $action = $sequences[$j][$sequenceAction];
				if ( (($action eq "t") || ($action eq "oc")) &&
				     ($sequences[$j][$sequenceObjRef] == \$objects[$i]) )
				    {
					splice @sequences, $j, 1;
#print "delete sequence $j\n";
#++$j;
					}
				else
					{
					++$j;
					}
				}
			splice @objects, $i, 1;
#print "delete object $i $objects[$i][$objectName]\n";
#++$i;
			}
		else
			{
			++$i;
			}
		}
	}

sub drawObject($$$)
	{
	my ($x,$y,$name) = @_;
	outputText($x, $name, $y, "middle", "underline");
	outputVerticalLine($x, $y + $objectUnderlineToLifelineGap, $screenHeight - $topOffset);
	}
	
sub drawObjectsAtTop()
	{
	my $i;
	foreach $i (@objects) {
		if ($objectsDisplayedAtCreationPoint == 1 && $i->[$objectCreationPointKnown] == 1)
			{
			next;
			}
		outputText($i->[$objectX], $i->[$objectName], $currentY, "middle", "underline");
		} 
	incrementY();
	drawObjectLifelines();
	}

sub drawObjectsRepeatedly()
	{
		my $i;
		foreach $i (@objects)
		{
			for( my $j = 400 ; $j < $screenHeight ; $j += 400 )
			{
				outputText($i->[$objectX], $i->[$objectName], $j, "middle", "","silver");
			}
		}
	}

sub drawSequences()
	{
	foreach my $ref (@sequences) {
		my $action = $ref->[$sequenceAction];
		if ($action eq "t")
			{
			my $objX = ${$ref->[$sequenceObjRef]}->[$objectX];
			my $text = $ref->[2];
			outputText($objX, $text, $currentY, "middle");
			incrementY();
			}
		elsif ($action eq "oc") 
			{
			if ($objectsDisplayedAtCreationPoint == 1)
				{
				my $objX = ${$ref->[$sequenceObjRef]}->[$objectX];
				my $objName = ${$ref->[$sequenceObjRef]}->[$objectName];
				drawObject($objX, $currentY, $objName);
				incrementY();
				}
			}
		else
			{
			my $msg = ${$ref->[$sequenceObjRef]};
			my $srcX = ${$ref->[$sequenceX]}->[$objectX];
			my $destX = ${$ref->[$sequenceY]}->[$objectX];
			my $align;
			if ($action eq "p") {
				$align = "tail";
				}
			elsif ($action eq "r") {
				$align = "head";
				}
			outputLabelledLine($srcX, $destX, $msg, $align);
			incrementY();
			}
		}
	}

sub drawObjectLifelines()
	{
	my $topOffset = $topMargin + $objectVerticalLineSpacing;
	foreach my $i (@objects) {
		if ($objectsDisplayedAtCreationPoint == 0 || $i->[$objectCreationPointKnown] == 0)
			{
			outputVerticalLine($i->[$objectX], $topOffset, $screenHeight - $topOffset);
			}
		}
	}

#######################
# SVG output routines
#######################

sub outputDocHeader()
	{
	my ($width,$height) = @_;
	print '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',"\n";
	print '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">',"\n";
	print "<svg height=\"$height\" width=\"$width\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
	outputDefs();
	}

sub outputDefs()
	{
	print "<defs>\n";
	print qq{<marker id="arr" markerHeight="10" markerUnits="strokeWidth" markerWidth="10" orient="auto" refX="10" refY="5" viewBox="0 0 10 10">\n};
	print qq{\t<path d="M 0 0 L 10 5 L 0 10 " />\n};
	print "</marker>\n";
	print "</defs>\n";
	}

sub outputDocFooter()
	{
	print "</svg>\n";
	}

sub outputLabelledLine()
	{
	my ($x1,$x2,$text,$alignment) = @_;
	outputLine($x1,$x2);
	my $textx;
	my $anchor;
	my $colour = 'grey';
	if (!$alignment || $alignment eq "mid") {
		$textx = (($x1 + $x2) / 2);
		$anchor = "middle";
		}
	else {
		if ($alignment eq "head") {
			$colour='red';
			if ($x1 < $x2) {
				$anchor = "end";
				$textx = $x2 - $arrowWidth - $textArrowSpacing;
				}
			else {
				$anchor = "start";
				$textx = $x2 + $arrowWidth + $textArrowSpacing;
				}
		} else {	# "tail"
			$colour='green';
			$textx = $x1;
			if ($x1 < $x2) {
				$anchor = "start";
			} else {
				$anchor = "end";
				}
			}
		}

	outputText($textx,$text,$currentY - $messageSpacingAboveLine, $anchor,'',$colour);
	}

sub outputText()
	{
	my ($x,$text,$y,$anchor,$decoration,$colour) = @_;
	
	my $attrs = qq{ x="$x" y="$y" };
	
	if ($decoration)
		{
		$attrs .= qq{ text-decoration="$decoration" };
		}
	
	if ($colour)
		{
		$attrs .= qq{ stroke="$colour" };
		}
	
	if ($anchor)
		{
		$attrs .= qq { text-anchor="$anchor" };
		}

	print "<text $attrs>$text</text>\n";
	}

sub outputLine()
	{
	my ($x1,$x2) = @_;
	print qq{<line stroke="black" marker-end="url(#arr)" x1="$x1" y1="$currentY" x2="$x2" y2="$currentY" />\n};
	}

sub outputVerticalLine()
	{
	my ($x,$y1,$y2) = @_;
	print qq{<line stroke="black" x1="$x" y1="$y1" x2="$x" y2="$y2" />\n};
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
