///*|-----------------------------------------------------------------------------
// *|            This source code is provided under the Apache 2.0 license      --
// *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
// *|                See the project's LICENSE.md for details.                  --
// *|           Copyright Thomson Reuters 2016. All rights reserved.            --
///*|-----------------------------------------------------------------------------

package com.thomsonreuters.ema.examples.training.niprovider.series100.example110__MarketByOrder__Streaming;

import java.nio.ByteBuffer;

import com.thomsonreuters.ema.access.EmaFactory;
import com.thomsonreuters.ema.access.FieldList;
import com.thomsonreuters.ema.access.Map;
import com.thomsonreuters.ema.access.MapEntry;
import com.thomsonreuters.ema.access.OmmException;
import com.thomsonreuters.ema.access.OmmNiProviderConfig;
import com.thomsonreuters.ema.access.OmmProvider;
import com.thomsonreuters.ema.access.OmmReal.MagnitudeType;
import com.thomsonreuters.ema.access.OmmState;
import com.thomsonreuters.ema.rdm.EmaRdm;

public class NiProvider {

	public static void main(String[] args)
	{
		OmmProvider provider = null;
		try
		{
			OmmNiProviderConfig config = EmaFactory.createOmmNiProviderConfig();
			
			provider = EmaFactory.createOmmProvider(config.host("localhost:14003").username("user"));
			
			long itemHandle = 5;
			
			FieldList mapSummaryData = EmaFactory.createFieldList();
			mapSummaryData.add(EmaFactory.createFieldEntry().enumValue(15,  840));
			mapSummaryData.add(EmaFactory.createFieldEntry().enumValue(53,  1));
			mapSummaryData.add(EmaFactory.createFieldEntry().enumValue(3423,  1));
			mapSummaryData.add(EmaFactory.createFieldEntry().enumValue(1709,  2));
			
			FieldList mapKeyAscii = EmaFactory.createFieldList();
			mapKeyAscii.add(EmaFactory.createFieldEntry().realFromDouble(3427, 7.76, MagnitudeType.EXPONENT_NEG_2));
			mapKeyAscii.add(EmaFactory.createFieldEntry().realFromDouble(3429, 9600));
			mapKeyAscii.add(EmaFactory.createFieldEntry().enumValue(3428, 2));
			mapKeyAscii.add(EmaFactory.createFieldEntry().rmtes(212, ByteBuffer.wrap("Market Maker".getBytes())));
			
			Map map = EmaFactory.createMap();
			map.summaryData(mapSummaryData);
			map.add(EmaFactory.createMapEntry().keyAscii("100", MapEntry.MapAction.ADD, mapKeyAscii));
			
			provider.submit( EmaFactory.createRefreshMsg().domainType(EmaRdm.MMT_MARKET_BY_ORDER).serviceName("NI_PUB").name("AAO.V")
					.state(OmmState.StreamState.OPEN, OmmState.DataState.OK, OmmState.StatusCode.NONE, "UnSolicited Refresh Completed")
					.payload(map).complete(true), itemHandle);
			
			Thread.sleep(1000);
			
			for( int i = 0; i < 60; i++ )
			{
				mapKeyAscii = EmaFactory.createFieldList();
				mapKeyAscii.add(EmaFactory.createFieldEntry().realFromDouble(3427, 7.76 + i * 0.1, MagnitudeType.EXPONENT_NEG_2));
				mapKeyAscii.add(EmaFactory.createFieldEntry().realFromDouble(3429, 9600));
				mapKeyAscii.add(EmaFactory.createFieldEntry().enumValue(3428, 2));
				mapKeyAscii.add(EmaFactory.createFieldEntry().rmtes(212, ByteBuffer.wrap("Market Maker".getBytes())));
				
				map = EmaFactory.createMap();
				map.add(EmaFactory.createMapEntry().keyAscii("100", MapEntry.MapAction.ADD, mapKeyAscii));

				provider.submit( EmaFactory.createUpdateMsg().serviceName("NI_PUB").name("AAO.V").domainType(EmaRdm.MMT_MARKET_BY_ORDER).payload( map ), itemHandle );
				Thread.sleep(1000);
			}
		} 
		catch (InterruptedException | OmmException excp)
		{
			System.out.println(excp.getMessage());
		}
		finally 
		{
			if (provider != null) provider.uninitialize();
		}
	}
}
