/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2015. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "GenericMsg.h"
#include "GenericMsgDecoder.h"
#include "GenericMsgEncoder.h"
#include "Utilities.h"
#include "GlobalPool.h"
#include "RdmUtilities.h"

using namespace thomsonreuters::ema::access;
using namespace thomsonreuters::ema::rdm;

extern const EmaString& getDTypeAsString( DataType::DataTypeEnum dType );

GenericMsg::GenericMsg() :
 Msg(),
 _toString()
{
}

GenericMsg::~GenericMsg()
{
	if ( _pEncoder )
		g_pool._genericMsgEncoderPool.returnItem( static_cast<GenericMsgEncoder*>( _pEncoder ) );

	if ( _pDecoder )
		g_pool._genericMsgDecoderPool.returnItem( static_cast<GenericMsgDecoder*>( _pDecoder ) );
}

GenericMsg& GenericMsg::clear()
{
	if ( _pEncoder )
		_pEncoder->clear();

	return *this;
}

Data::DataCode GenericMsg::getCode() const
{
	return Data::NoCodeEnum;
}

DataType::DataTypeEnum GenericMsg::getDataType() const
{
	return DataType::GenericMsgEnum;
}

const EmaString& GenericMsg::toString() const
{
	return toString( 0 );
}

const EmaString& GenericMsg::toString(  UInt64 indent ) const
{
	const GenericMsgDecoder* pTempDecoder = static_cast<const GenericMsgDecoder*>( _pDecoder );

	addIndent( _toString.clear(), indent++ ).append( "GenericMsg" );
	addIndent( _toString, indent, true ).append( "streamId=\"" ).append( pTempDecoder->getStreamId() ).append( "\"" );
	addIndent( _toString, indent, true ).append( "domain=\"" ).append( rdmDomainToString( getDomainType() ) ).append( "\"" );

	if ( pTempDecoder->getComplete() ) addIndent( _toString, indent, true ).append( "MessageComplete" );

	if ( pTempDecoder->hasSeqNum() )
		addIndent( _toString, indent, true ).append( "seqNum=\"" ).append( pTempDecoder->getSeqNum() ).append( "\"" );

	if ( pTempDecoder->hasSecondarySeqNum() )
		addIndent( _toString, indent, true ).append( "secondarySeqNum=\"" ).append( pTempDecoder->getSecondarySeqNum() ).append( "\"" );

	if ( pTempDecoder->hasPartNum() )
		addIndent( _toString, indent, true ).append( "partNum=\"" ).append( pTempDecoder->getPartNum() ).append( "\"" );

	if ( pTempDecoder->hasPermissionData() )
	{
		EmaString temp;
		hexToString( temp, pTempDecoder->getPermissionData() );
		addIndent( _toString, indent, true ).append( "permissionData=\"" ).append( temp ).append( "\"" );
	}

	indent--;
	if ( pTempDecoder->hasMsgKey() )
	{
		indent++;
		if ( pTempDecoder->hasName() )
			addIndent( _toString, indent, true ).append( "name=\"" ).append( pTempDecoder->getName() ).append( "\"" );

		if ( pTempDecoder->hasNameType() )
			addIndent( _toString, indent, true ).append( "nameType=\"" ).append( pTempDecoder->getNameType() ).append( "\"" );

		if ( pTempDecoder->hasServiceId() )
			addIndent( _toString, indent, true ).append( "serviceId=\"" ).append( pTempDecoder->getServiceId() ).append( "\"" );

		if ( pTempDecoder->hasFilter() )
			addIndent( _toString, indent, true ).append( "filter=\"" ).append( pTempDecoder->getFilter() ).append( "\"" );

		if ( pTempDecoder->hasId() )
			addIndent( _toString, indent, true ).append( "id=\"" ).append( pTempDecoder->getId() ).append( "\"" );
			
		indent--;
		if ( pTempDecoder->hasAttrib() )
		{
			indent++;
			addIndent( _toString, indent, true ).append( "Attrib dataType=\"" ).append( getDTypeAsString( pTempDecoder->getAttribData().getDataType() ) ).append( "\"\n" );

			indent++;
			_toString.append( pTempDecoder->getAttribData().toString( indent ) );
			indent--;

			addIndent( _toString, indent, true ).append( "AttribEnd" );
			indent--;
		}
	}

	if ( pTempDecoder->hasExtendedHeader() )
	{
		indent++;
		addIndent( _toString, indent, true ).append( "ExtendedHeader\n" );

		indent++;

		addIndent( _toString, indent );
		hexToString( _toString, pTempDecoder->getExtendedHeader() );

		indent--;

		addIndent( _toString, indent, true ).append( "ExtendedHeaderEnd" );
		indent--;
	}

	if ( pTempDecoder->hasPayload() )
	{
		indent++;
		addIndent( _toString, indent, true ).append( "Payload dataType=\"" ).append( getDTypeAsString( pTempDecoder->getPayloadData().getDataType() ) ).append( "\"\n" );

		indent++;
		_toString.append( pTempDecoder->getPayloadData().toString( indent ) );
		indent--;

		addIndent( _toString, indent, true ).append( "PayloadEnd" );
		indent--;
	}
			
	addIndent( _toString, indent, true ).append( "GenericMsgEnd\n" );

	return _toString;
}

const EmaBuffer& GenericMsg::getAsHex() const
{
	return static_cast<const GenericMsgDecoder*>( _pDecoder )->getHexBuffer();
}

bool GenericMsg::hasSeqNum() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->hasSeqNum();
}

bool GenericMsg::hasSecondarySeqNum() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->hasSecondarySeqNum();
}

bool GenericMsg::hasPartNum() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->hasPartNum();
}

bool GenericMsg::hasPermissionData() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->hasPermissionData();
}

UInt32 GenericMsg::getSeqNum() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->getSeqNum();
}

UInt32 GenericMsg::getSecondarySeqNum() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->getSecondarySeqNum();
}

UInt16 GenericMsg::getPartNum() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->getPartNum();
}

const EmaBuffer& GenericMsg::getPermissionData() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->getPermissionData();
}

bool GenericMsg::getComplete() const
{
	return static_cast<const GenericMsgDecoder*>(_pDecoder)->getComplete();
}

Decoder& GenericMsg::getDecoder()
{
	if ( !_pDecoder )
		setDecoder( g_pool._genericMsgDecoderPool.getItem() );

	return *_pDecoder;
}

GenericMsg& GenericMsg::name( const EmaString& name )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->name( name );
	return *this;
}

GenericMsg& GenericMsg::nameType( UInt8 nameType )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->nameType( nameType );
	return *this;
}

GenericMsg& GenericMsg::serviceId( UInt32 serviceId )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->serviceId( serviceId );
	return *this;
}

GenericMsg& GenericMsg::id( Int32 id )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->identifier( id );
	return *this;
}

GenericMsg& GenericMsg::filter( UInt32 filter )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->filter( filter );
	return *this;
}

GenericMsg& GenericMsg::streamId( Int32 streamId )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->streamId( streamId );
	return *this;
}

GenericMsg& GenericMsg::domainType( UInt16 domainType )
{
	if ( domainType > 255 )
	{
		EmaString temp( "Passed in DomainType is out of range." );
		throwDtuException( domainType, temp );
		return *this;
	}

	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->domainType( (UInt8)domainType );
	return *this;
}

GenericMsg& GenericMsg::seqNum( UInt32 seqNum )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	static_cast<GenericMsgEncoder*>(_pEncoder)->seqNum( seqNum );
	return *this;
}

GenericMsg& GenericMsg::secondarySeqNum( UInt32 seqNum )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	static_cast<GenericMsgEncoder*>(_pEncoder)->secondarySeqNum( seqNum );
	return *this;
}

GenericMsg& GenericMsg::partNum( UInt16 partNum )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	static_cast<GenericMsgEncoder*>(_pEncoder)->partNum( partNum );
	return *this;
}

GenericMsg& GenericMsg::permissionData( const EmaBuffer& permissionData )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	static_cast<GenericMsgEncoder*>(_pEncoder)->permissionData( permissionData );
	return *this;
}

GenericMsg& GenericMsg::complete( bool complete )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	static_cast<GenericMsgEncoder*>(_pEncoder)->complete( complete );
	return *this;
}

GenericMsg& GenericMsg::attrib( const ComplexType& data )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->attrib( data );
	return *this;
}

GenericMsg& GenericMsg::payload( const ComplexType& data )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	_pEncoder->payload( data );
	return *this;
}

GenericMsg& GenericMsg::extendedHeader( const EmaBuffer& Buffer )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._genericMsgEncoderPool.getItem();

	static_cast<GenericMsgEncoder*>(_pEncoder)->extendedHeader( Buffer );
	return *this;
}

