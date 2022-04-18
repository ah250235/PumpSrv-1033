#ifndef _OLA_SERVICE2_EVENTS_HANDLER_
#define _OLA_SERVICE2_EVENTS_HANDLER_

#import "OLA/OLAStoreLine.exe" named_guids //using namespace OLASTORELINELib;

class COLAService2EventsHandler: 

			/*public IDispEventImpl <
			0,
			COLAService2EventsHandler,
			&__uuidof(OLASTORELINELib::_IOLAServiceEvents),
			&__uuidof(OLASTORELINELib::IOLAService),
			1,
			0>,
			
			public CComObjectRootEx<CComMultiThreadModel> 

*/
//4.0.21.11
			public IDispEventImpl <
			0,
			COLAService2EventsHandler,
			&OLASTORELINELib::DIID__IOLAServiceEvents,
			&OLASTORELINELib::LIBID_OLASTORELINELib,
			1,
			0>,
			
			public CComObjectRootEx<CComMultiThreadModel> 


{
public:
	 
					  
	HRESULT __stdcall StatusEvent(BSTR sInBuffer);	
	
//4.0.21.11	
	BEGIN_SINK_MAP(COLAService2EventsHandler)		
		SINK_ENTRY_EX(0,OLASTORELINELib::DIID__IOLAServiceEvents /*__uuidof(OLASTORELINELib::_IOLAServiceEvents)*/,1,StatusEvent)
		
	END_SINK_MAP()  

};



#endif//_OLA_SERVICE2_EVENTS_HANDLER_

