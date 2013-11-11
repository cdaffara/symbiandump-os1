<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common"  exclude-result-prefixes="exslt">
<!--Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
	All rights reserved.
	This component and the accompanying materials are made available
	under the terms of "Eclipse Public License v1.0"
	which accompanies this distribution, and is available
	at the URL "http://www.eclipse.org/legal/epl-v10.html".

	Initial Contributors:
	Nokia Corporation - initial contribution.
	Contributors:
	Description:
	Filter a sysdef in the 2.0 or 3.0 syntax
	The functionality of these filters is not supported outside of this tool. 
	The filtering concepts here may not be carried forward in future system definition processing tools
-->
<xsl:output method="xml" indent="yes"/>

<xsl:param name="filter-type">only</xsl:param> <!-- only, has or with -->

<!--
"only" = every component/unit can only have zero or more of these specified filters. ie, it can only have filters from this list, no other filters are allowed. This covers the common use case of "I want anything with gt and or techview, but no other filters" that was the first step in all old symbian.com builds. 

"has" =  every component/unit must have all of these filters. ie it can have any other filters, but all specified filters must all be present. This covers the case where filter="test" identifies tests and there is no special filter to identify things that are not tests. So a filter for "!test" will strip out anything with filter="test" (plus any other filters) for a production build, and a filter of "test" will strip out everything that does not have filter="test" (plus any other filters) for a test build. Opposite filters will generate sets that are exactly opposite to each other.

"with" =  components/units that have the opposite filter are removed. This covers the old symbian.com case of java, !java and "don't care" components. In other words we have a global feature that every item can be built only when the feature is set, only when the feature is not set, or it does not care and will always be built. So for a !java build only items containing filter="java" are stripped out. For a java build, only items with filter="!java" are stripped out. Anything which does not explicitly mention java are always unaffected. Opposite filters will generate sets which overlap.
-->


<xsl:param name="filter"/> <!-- comma-separated list of filters-->

<xsl:param name="addbuild" select="0"/> <!-- add a system build section that accepts everything. Needed for genxml processing -->


<xsl:template match="node()|@*"><xsl:copy-of select="."/></xsl:template>
<xsl:template match="*"><xsl:param name="data"/>
	<xsl:copy>
		<xsl:copy-of select="@*"/>
		<xsl:apply-templates select="node()">
			<xsl:with-param name="data" select="$data"/>
		</xsl:apply-templates>
	</xsl:copy>
</xsl:template>

<xsl:template match="component|unit"><xsl:param name="data"/> <!-- filterable items -->
	 <xsl:variable name="display">
	 	<xsl:apply-templates select="$data" mode="filter">
			<xsl:with-param name="item" select="current()"/>
		</xsl:apply-templates>
	 </xsl:variable>
	 
	 <xsl:if test="$display != 'hide' "> <!-- if hide, remove completely from the output-->
		<xsl:copy>
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates select="node()">
				<xsl:with-param name="data" select="$data"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:if>
</xsl:template>

<xsl:template match="/SystemDefinition">
	<xsl:variable name="f">
		<xsl:element name="filter-{$filter-type}">
			<xsl:call-template name="filter-list">
				<xsl:with-param name="f" select="$filter"/>
			</xsl:call-template>
		</xsl:element>
	</xsl:variable>
	<xsl:choose>
		<xsl:when test="starts-with(@schema,'3.0')"/> <!-- no DTD needed for 3.0 sysdef -->
		<xsl:when test="$addbuild">
			<xsl:call-template name="DTD-bld"/> <!-- insert 2.0.1 DTD w/sysbuild parts-->
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="DTD"/> <!-- insert 2.0.1 DTD -->
		</xsl:otherwise>
	</xsl:choose>
	<xsl:copy>
		<xsl:copy-of select="@*"/>
		<xsl:variable name="content"> <!-- save content for potential further processing -->
			<xsl:apply-templates select="node()">
				<xsl:with-param name="data" select="exslt:node-set($f)/*"/>
			</xsl:apply-templates>
		</xsl:variable>
		<!-- just output content as is. We could create a template to remove any filtered out items, but so far there's no compelling need -->
		<xsl:copy-of select="$content"/>  

		<xsl:if test="$addbuild">
			<SystemBuild schema="1.0.0">
				<configuration name="any" description="text">
					<xsl:attribute name="filter">
						<xsl:for-each select="//@filter[not(.=following::*/@filter)]">
							<xsl:value-of select="."/><xsl:if test="position()!=last()">,</xsl:if>
						</xsl:for-each>
					</xsl:attribute>
					<xsl:for-each select="exslt:node-set($content)/systemModel/layer[descendant::unit]">
						<!-- only include layers we know have units -->
						<ref item="{@name}"/>
					</xsl:for-each>
				</configuration>
			</SystemBuild>
		</xsl:if>
	</xsl:copy>
</xsl:template>


<xsl:include href="filter-module.xsl"/>

<xsl:template name="DTD-bld">
<xsl:text disable-output-escaping="yes"><![CDATA[<!DOCTYPE SystemDefinition [
 <!ELEMENT SystemDefinition ( systemModel?, SystemBuild? )>
 <!ATTLIST SystemDefinition
  name CDATA #REQUIRED
  schema CDATA #REQUIRED>
 <!ELEMENT systemModel (layer+)>
 <!ELEMENT layer (block* | collection*)*>
 <!ATTLIST layer
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED
  levels NMTOKENS #IMPLIED
  span CDATA #IMPLIED>
 <!ELEMENT block (subblock* | collection*)*>
 <!ATTLIST block
  levels NMTOKENS #IMPLIED
  span CDATA #IMPLIED
  level NMTOKEN #IMPLIED
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED>
 <!ELEMENT subblock (collection)*>
 <!ATTLIST subblock
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED>
 <!ELEMENT collection (component)*>
 <!ATTLIST collection
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED
  level NMTOKEN #IMPLIED>
 <!ELEMENT component (unit)*>
 <!ATTLIST component
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED
  deprecated CDATA #IMPLIED
  introduced CDATA #IMPLIED
  contract CDATA #IMPLIED
  plugin (Y|N) "N"
  filter CDATA #IMPLIED
  class NMTOKENS #IMPLIED
  supports CDATA #IMPLIED
  purpose ( optional | mandatory | development ) "optional">
 <!ELEMENT unit EMPTY>
 <!ATTLIST unit
  mrp CDATA #IMPLIED
  filter CDATA #IMPLIED
  bldFile CDATA #IMPLIED
  root CDATA #IMPLIED
  version NMTOKEN #IMPLIED
  prebuilt NMTOKEN #IMPLIED
  late (Y|N) #IMPLIED
  priority CDATA #IMPLIED>
 <!ELEMENT SystemBuild (option* | target+ | targetList+ | list+ | configuration+)*>
 <!ATTLIST SystemBuild schema CDATA #REQUIRED>
 <!ELEMENT list (ref+)>
 <!ATTLIST list
  name ID #REQUIRED
  description CDATA #REQUIRED>
 <!ELEMENT ref EMPTY>
 <!ATTLIST ref item CDATA #REQUIRED>
 <!ELEMENT targetList EMPTY>
 <!ATTLIST targetList
  name ID #REQUIRED
  description CDATA #REQUIRED
  target IDREFS #REQUIRED>
 <!ELEMENT target EMPTY>
 <!ATTLIST target
  name ID #REQUIRED
  abldTarget CDATA #REQUIRED
  description CDATA #REQUIRED>
 <!ELEMENT option EMPTY>
 <!ATTLIST option
  name ID #REQUIRED
  abldOption CDATA #REQUIRED
  description CDATA #REQUIRED
  enable (Y | N ) #REQUIRED>
 <!ELEMENT configuration (listRef+ | ref+ | task+)*>
 <!ATTLIST configuration
  name ID #REQUIRED
  description CDATA #REQUIRED
  filter CDATA #REQUIRED>
 <!ELEMENT task (listRef* , (buildLayer | specialInstructions))>
 <!ELEMENT listRef EMPTY>
 <!ATTLIST listRef list CDATA #REQUIRED>
 <!ELEMENT buildLayer EMPTY>
 <!ATTLIST buildLayer
  command CDATA #REQUIRED
  targetList IDREFS #IMPLIED
  unitParallel (Y | N ) #REQUIRED
  targetParallel (Y | N ) "N">
 <!ELEMENT specialInstructions EMPTY>
 <!ATTLIST specialInstructions
  name CDATA #REQUIRED
  cwd CDATA #REQUIRED
  root CDATA #IMPLIED
  command CDATA #REQUIRED>
]>
]]></xsl:text>
</xsl:template>

<xsl:template name="DTD">
<xsl:text disable-output-escaping="yes"><![CDATA[<!DOCTYPE SystemDefinition [
<!ELEMENT SystemDefinition ( systemModel )>
<!ATTLIST SystemDefinition
  name CDATA #REQUIRED
  schema CDATA #REQUIRED
>
<!-- all paths are relative to the environment variable specified by the root attribute, or SOURCEROOT if not.  -->

<!-- System Model Section of DTD -->
<!ELEMENT systemModel (layer+)>

<!ELEMENT layer (block* | collection*)*>
<!-- Kernel Services, Base Services, OS Services, Etc -->
<!ATTLIST layer
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED
  levels NMTOKENS #IMPLIED
  span CDATA #IMPLIED
>

<!ELEMENT block (subblock* | collection*)*>
 <!-- Generic OS services, Comms Services, etc -->
<!ATTLIST block
  levels NMTOKENS #IMPLIED
  span CDATA #IMPLIED
  level NMTOKEN #IMPLIED
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED
>

<!ELEMENT subblock (collection)*>
<!-- Cellular Baseband Services, Networking Services, etc -->
<!ATTLIST subblock
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED
>

<!ELEMENT collection (component)*>
<!-- Screen Driver, Content Handling, etc -->
<!ATTLIST collection
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED
  level NMTOKEN #IMPLIED
>

<!ELEMENT component (unit)*>
<!-- contains units or is a  package or prebuilt -->
<!ATTLIST component
  name CDATA #REQUIRED
  long-name CDATA #IMPLIED
  deprecated CDATA #IMPLIED
  introduced CDATA #IMPLIED
  contract CDATA #IMPLIED
  plugin (Y|N) "N"
  filter CDATA #IMPLIED
  class NMTOKENS #IMPLIED
  supports CDATA #IMPLIED
  purpose ( optional | mandatory | development ) "optional"
>

<!ELEMENT unit EMPTY >
<!-- must be buildable (bld.inf) -->
<!-- bldFile  may someday be removed in favour of mrp -->
<!ATTLIST unit
  mrp CDATA #IMPLIED
  filter CDATA #IMPLIED
  bldFile CDATA #IMPLIED
  root CDATA #IMPLIED
  version NMTOKEN #IMPLIED
  prebuilt NMTOKEN #IMPLIED
  late (Y|N) #IMPLIED
  priority CDATA #IMPLIED
>
]>
]]></xsl:text>
</xsl:template>
</xsl:stylesheet>	