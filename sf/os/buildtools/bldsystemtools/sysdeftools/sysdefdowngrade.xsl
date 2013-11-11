<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
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
	Downgrade a 3.x system definition into the 2.0.1 syntax
-->
	<xsl:output method="xml" indent="yes"/>
	
	<xsl:param name="Path">os/deviceplatformrelease/foundation_system/system_model</xsl:param>
	<!-- $Path is the location of the root system definition XML file. Must not end in /
		This is used to compute the absolute paths the 2.0 syntax needs-->
	<xsl:param name="Root"/> <!-- space separated list of root variables in the form "VAR1=value1 VAR=value2" --> 
	<xsl:variable name="root" select="concat(' ',$Root,' ')"/> <!-- sort of hack to allow absolute paths in downgraded output -->
	<xsl:variable name="srcroot" select="substring-before(substring-after($root,' SRCROOT='),' ')"/> <!-- the default path prefix -->

<xsl:template match="/*">
	<xsl:message terminate="yes">ERROR: Cannot process this document</xsl:message>
</xsl:template>

<!-- can only handle 3.0.0 to 2.0.1 transforms
	Assumes only packages are using href
 -->
<xsl:template match="/SystemDefinition[@schema='3.0.0']"> 
	<!-- process root system definition or package definition-->
	<xsl:call-template name="DTD"/> <!-- insert 2.0.01 DTD -->
  <SystemDefinition name="{*/@name}" schema="2.0.1">
  	<xsl:apply-templates select="*|comment()"/>
  </SystemDefinition>
</xsl:template>

<xsl:template match="/SystemDefinition[@schema='3.0.0' and systemModel]"> 
	<xsl:call-template name="DTD"/> <!-- insert 2.0.01 DTD -->
  <SystemDefinition name="{systemModel/@name}" schema="2.0.1">
  	<xsl:apply-templates select="*|comment()"/>
  </SystemDefinition>
</xsl:template>

<xsl:template match="/SystemDefinition/package[@href]" priority="2">
	<xsl:message terminate="yes">ERROR: Package definition (<xsl:value-of select="@id"/>) cannot link another package</xsl:message>
</xsl:template>

<xsl:template match="/SystemDefinition/package" priority="1">
	<!-- process package definition file--> 
  <systemModel>
  	<layer name="anonymous"> <!-- fake layer -->
		<block>
			<xsl:apply-templates mode="copy" select="@id|@name|@span|@level|@levels"/><!--  valid attribuites for 2.0 -->
		  	<xsl:apply-templates select="*|comment()">
		  		<xsl:with-param name="path" select="concat($Path,'/')"/> <!-- need to keep tack of where the current document is -->
		  	</xsl:apply-templates>			
		</block>  
   	</layer>
  </systemModel>
</xsl:template>



<xsl:template match="/"><xsl:apply-templates select="*"/></xsl:template>
<xsl:template match="@*|comment()"><xsl:copy-of select="."/></xsl:template> 
	<!-- comments are copied verbatim. Attribtues are copied by default -->

<xsl:template match="systemModel">
	<systemModel>
  	<xsl:apply-templates select="*|comment()"> <!-- no attributes -->
  		<xsl:with-param name="path" select="$Path"/> <!-- need to keep tack of where the current document is -->
  	</xsl:apply-templates>
	</systemModel>
</xsl:template>

<xsl:template mode="copy" match="@*">
	<xsl:copy-of select="."/>
</xsl:template>

<xsl:template mode="copy" match="@id"> <!-- id in 3.0 is name in 2.0 -->
	<xsl:attribute name="name"><xsl:value-of select="."/></xsl:attribute>
</xsl:template>

<xsl:template mode="copy" match="@name">  <!-- name in 3.0 is long-name in 2.0.1 -->
	<xsl:if test=".!=../@id"> <!-- don't bother if it will be the same as name -->
		<xsl:attribute name="long-name"><xsl:value-of select="."/></xsl:attribute>
	</xsl:if>
</xsl:template>
	
<xsl:template match="layer"><xsl:param name="path"/>
	<layer>
		<xsl:apply-templates mode="copy" select="@id|@name|@span|@levels"/> <!--  valid attribuites for 2.0 -->
		<xsl:apply-templates select="*|comment()"> 
			<xsl:with-param name="path" select="$path"/> 
		</xsl:apply-templates>
	</layer>
</xsl:template>

<xsl:template match="layer/package"><!-- translates to block -->
	<xsl:param name="path"/>
	<block>
		<xsl:apply-templates mode="copy" select="@id|@name|@span|@level|@levels"/><!--  valid attribuites for 2.0 -->
		<xsl:choose>
			<xsl:when test="@href">
				<xsl:variable name="this" select="."/>
				<xsl:variable name="prefixmap" select="ancestor::SystemDefinition/*/meta[@rel='link-mapping']/map-prefix[starts-with(current()/@href,@link)]"/>
				<xsl:for-each select="document(@href,.)/SystemDefinition/*">
					<xsl:variable name="my-id"><xsl:apply-templates mode="normalize-id" select="@id"/></xsl:variable>
					<xsl:variable name="other-id"><xsl:apply-templates mode="normalize-id" select="$this/@id"/></xsl:variable>
					<xsl:if test="$my-id != $other-id">
						<xsl:message terminate="yes">ERROR: Package IDs do not match: <xsl:value-of select="$my-id"/> vs <xsl:value-of select="$other-id"/></xsl:message>
					</xsl:if>
					<xsl:if test="@name and @name!=@id and not($this/@name and $this/@name=$this/@id)">
						<!-- set long-name only if name is different from the id and not set in child doc -->
						<xsl:attribute name="long-name"><xsl:value-of select="@name"/></xsl:attribute>
					</xsl:if>						
					<xsl:for-each select="@span|@levels|@level">
						<!-- copy only if not set in child doc -->
						<xsl:if test="not(this/@*[name()=name(current())])">
							<xsl:copy-of select="."/>
						</xsl:if>
					</xsl:for-each>
					<xsl:apply-templates select="*|comment()">
	  					<xsl:with-param name="path">
							<xsl:choose>
								<xsl:when test="$prefixmap">
	  								<xsl:call-template name="normpath">
	  									<xsl:with-param name="path">
										<xsl:apply-templates select="$prefixmap/@to"/>
										<xsl:value-of select="substring-after($this/@href,$prefixmap/@link)"/>
									 </xsl:with-param>
	  								</xsl:call-template>
								</xsl:when>
								<xsl:when test="starts-with($this/@href,'/')">  <!-- absolute path -->
	  								<xsl:call-template name="normpath">
	  									<xsl:with-param name="path" select="$this/@href"/>
	  								</xsl:call-template>
								</xsl:when>
								<xsl:when test="contains($this/@href,'://')">  <!-- generic URI -->
	  								<xsl:call-template name="normpath">
	  									<xsl:with-param name="path" select="substring-after($this/@href,'://')"/>
	  								</xsl:call-template>
								</xsl:when>
								<xsl:otherwise>
	  								<xsl:call-template name="normpath">
	  									<xsl:with-param name="path" select="concat($path,'/',$this/@href)"/>
	  								</xsl:call-template>
								</xsl:otherwise>
							</xsl:choose>
	  					</xsl:with-param> 
	  				</xsl:apply-templates>
				</xsl:for-each>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="*|comment()">
  					<xsl:with-param name="path" select="$path"/> 
  				</xsl:apply-templates>
			</xsl:otherwise>
		</xsl:choose>
	</block>
</xsl:template>

<xsl:template match="package/package">	<!-- translates to subblock --><xsl:param name="path"/>
	<subblock>
		<xsl:apply-templates mode="copy" select="@id|@name"/>
		<xsl:apply-templates select="*|comment()">
			<xsl:with-param name="path" select="$path"/> 
		</xsl:apply-templates>
	</subblock>
</xsl:template>

<xsl:template match="package/package/pacakge"> <!-- cannot nest this deep --><xsl:param name="path"/>
	<xsl:message>WARNING: Excessive nesting of packages: Ignoring <xsl:value-of select="@id"/></xsl:message>
	<xsl:apply-templates select="*|comment()">
		<xsl:with-param name="path" select="$path"/> 
	</xsl:apply-templates>
</xsl:template>


<xsl:template match="collection"><xsl:param name="path"/>
	<collection>
		<xsl:apply-templates mode="copy" select="@id|@name|@level"/>
		<xsl:apply-templates select="*|comment()">
			<xsl:with-param name="path" select="$path"/> 
		</xsl:apply-templates>
	</collection>
</xsl:template>


<xsl:template match="component"><xsl:param name="path"/>
	<component>
		<xsl:apply-templates mode="copy" select="@id|@name|@deprecated|@introduced|@filter|@purpose"/>
		<xsl:if test="contains(concat(' ',@class,' '),' plugin ')">
			<xsl:attribute name="plugin">Y</xsl:attribute>
		</xsl:if>
		<xsl:call-template name="class">
			<xsl:with-param name="remove">plugin</xsl:with-param>
			<xsl:with-param name="add">
				<xsl:if test="not(*) and comment()">placeholder</xsl:if>
				<xsl:if test="@target='desktop'"> PC</xsl:if>
			</xsl:with-param>
		</xsl:call-template>
	  	<xsl:apply-templates select="*|comment()">
			<xsl:with-param name="path" select="$path"/> 
		</xsl:apply-templates>
	</component>
</xsl:template>

<xsl:template match="unit[@base and not(@mrp or @bldFile)]"/>

<xsl:template match="unit"><xsl:param name="path"/>
	<unit>
		<xsl:apply-templates select="@mrp|@bldFile|@late">
			<xsl:with-param name="path" select="$path"/> 
		</xsl:apply-templates>
		<xsl:copy-of select="@filter|@root[not(contains($root,concat(' ',.,'=')))]|@version|@prebuilt|@priority"/>
	</unit>
</xsl:template>

<xsl:template match="unit/@late"> <!-- 2.0 uses Y/N, 3.0 uses yes/no -->
	<xsl:attribute name="{name()}">
		<xsl:choose>
			<xsl:when test=".='yes'">Y</xsl:when>
			<xsl:when test=".='no'">N</xsl:when>
		</xsl:choose>
	</xsl:attribute>
</xsl:template>

<xsl:template match="@mrp|@bldFile"><xsl:param name="path"/>
	<xsl:attribute name="{name()}">
		<xsl:choose>
			<xsl:when test="../@root">
				<xsl:variable name="pre" select="substring-before(substring-after($root,concat(' ',../@root,'=')),' ')"/>
				<xsl:value-of select="$pre"/>
				<xsl:if test="$pre!='' and $pre!='/'">/</xsl:if>
			</xsl:when>
			<xsl:when test="$srcroot='/'"> <!-- treat all paths as absolute -->
				<xsl:value-of select="$srcroot"/>
			</xsl:when>
			<xsl:when test="$srcroot!=''">
				<xsl:value-of select="concat($srcroot,'/')"/>
			</xsl:when>
		</xsl:choose>
		<xsl:choose>
		<xsl:when test="starts-with(.,'/')"> <!-- keep absolute paths verbatim (barring the leading / ) -->
			<xsl:value-of select="substring-after(.,'/')"/>
		</xsl:when>
		<xsl:otherwise>	
			<xsl:call-template name="normpath">
  				<xsl:with-param name="path">
  					<xsl:call-template name="before">
						<xsl:with-param name="text" select="$path"/>
					</xsl:call-template>
					<xsl:value-of select="."/>
  				</xsl:with-param>
  			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
	</xsl:attribute>
</xsl:template>

<xsl:template match="meta"/> <!-- strip all meta tags -->

<xsl:template match="meta[info/@contract]"> <!-- except contract -->
	<xsl:copy-of select="info/@contract"/>
</xsl:template>


<xsl:template match="meta[@rel='link-mapping']/map-prefix/@to">
	<xsl:choose>
		<xsl:when test="starts-with(.,'/')"><xsl:value-of select="substring(.,2)"/></xsl:when> <!-- absolute paths in 3.0 are relative in 2.0 -->
		<xsl:otherwise><xsl:value-of select="."/></xsl:otherwise>
	</xsl:choose>
</xsl:template>


<xsl:template match="@id" mode="normalize-id">
	<xsl:choose>
		<xsl:when test="contains(@id,':')"><xsl:value-of select="substring-after(@id,':')"/></xsl:when>
		<xsl:otherwise><xsl:value-of select="@id"/></xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="class"><xsl:param name="remove"/><xsl:param name="add"/>
<!--  returns the value of the class attribute with the space-separated list of names in $remove taken out and those in $add added on (does not check for duplicates) -->
	<xsl:param name="class" select="normalize-space(@class)"/>
	<xsl:variable name="r">
		<xsl:text> </xsl:text>
		<xsl:choose>
			<xsl:when test="contains($remove,' ')"><xsl:value-of select="substring-before($remove,' ')"/></xsl:when>
			<xsl:otherwise><xsl:value-of select="$remove"/></xsl:otherwise>
		</xsl:choose>
		<xsl:text> </xsl:text>
	</xsl:variable>
	<xsl:variable name="c">
		<xsl:choose>
			<xsl:when test="contains(concat(' ',$class,' '),$r)">
				<xsl:value-of select="substring-before(concat(' ',$class,' '),$r)"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="substring-after(concat(' ',$class,' '),$r)"/>
			</xsl:when>
			<xsl:otherwise><xsl:value-of select="$class"/></xsl:otherwise>
		</xsl:choose>
		<xsl:if test="normalize-space($add)!=''"><xsl:value-of select="concat(' ',normalize-space($add))"/></xsl:if>
	</xsl:variable>
	<xsl:choose>
		<xsl:when test="contains($remove,' ')">
			<xsl:call-template name="class">
				<xsl:with-param name="remove" select="substring-after($remove,' ')"/>
				<xsl:with-param name="class" select="$c"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="normalize-space($c)!=''">
			<xsl:attribute name="class">
				<xsl:value-of select="normalize-space($c)"/>
			</xsl:attribute>
		</xsl:when>
	</xsl:choose>
</xsl:template>

<xsl:template name="normpath"><xsl:param name="path"/>
<!-- normalize out any ".." in the path in $path  -->
<xsl:choose>
	<xsl:when test="contains($path,'/../')">
	<xsl:call-template name="normpath">
		<xsl:with-param name="path">
		<xsl:call-template name="before">
			<xsl:with-param name="text" select="substring-before($path,'/../')"/>
		</xsl:call-template>
		<xsl:value-of select="substring-after($path,'/../')"/>
		</xsl:with-param>
		</xsl:call-template>
	</xsl:when>
	<xsl:otherwise><xsl:value-of select="$path"/></xsl:otherwise>
</xsl:choose>
</xsl:template>

<!-- return all text before the last / -->
<xsl:template name="before"><xsl:param name="text"/>
<xsl:if test="contains($text,'/')">
	<xsl:value-of select="substring-before($text,'/')"/>/<xsl:call-template name="before"><xsl:with-param name="text" select="substring-after($text,'/')"/></xsl:call-template>
	</xsl:if>
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
