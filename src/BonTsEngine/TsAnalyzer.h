#pragma once

#include <vector>
#include <map>
#include "MediaDecoder.h"
#include "TsStream.h"
#include "TsDescriptor.h"

// EIT の解析を行う
#define TS_ANALYZER_EIT_SUPPORT
// L-EIT[p/f] の解析を行う
#define TS_ANALYZER_L_EIT_SUPPORT


// TS 解析クラス
class CTsAnalyzer : public CMediaDecoder
{
public:
	enum {
		PID_INVALID = 0xFFFF,
		COMPONENTTAG_INVALID = 0xFF,
		LOGOID_INVALID = 0xFFFF
	};

	enum { MAX_SERVICE_NAME = 256 };

	struct EsInfo {
		WORD PID;
		BYTE StreamType;
		BYTE ComponentTag;
		BYTE QualityLevel;
		WORD HierarchicalReferencePID;

		EsInfo()
			: PID(PID_INVALID)
			, StreamType(STREAM_TYPE_INVALID)
			, ComponentTag(COMPONENTTAG_INVALID)
			, QualityLevel(0xFF)
			, HierarchicalReferencePID(PID_INVALID)
		{
		}
	};

	typedef std::vector<EsInfo> EsInfoList;

	struct EcmInfo {
		WORD CaSystemID;
		WORD PID;
	};

	struct ServiceInfo {
		bool bIsUpdated;
		WORD ServiceID;
		WORD PmtPID;
		std::vector<EsInfo> VideoEsList;
		std::vector<EsInfo> AudioEsList;
		std::vector<EsInfo> CaptionEsList;
		std::vector<EsInfo> DataCarrouselEsList;
		std::vector<EsInfo> OtherEsList;
		WORD PcrPID;
		ULONGLONG PcrTimeStamp;
		std::vector<EcmInfo> EcmList;
		BYTE RunningStatus;
		bool bIsCaService;
		TCHAR szServiceName[MAX_SERVICE_NAME];
		BYTE ServiceType;
		WORD LogoID;
	};

	typedef std::vector<ServiceInfo> ServiceList;

	struct SdtServiceInfo {
		WORD ServiceID;
		BYTE RunningStatus;
		bool bFreeCaMode;
		TCHAR szServiceName[MAX_SERVICE_NAME];
		BYTE ServiceType;
	};

	typedef std::vector<SdtServiceInfo> SdtServiceList;

	struct SdtTsInfo {
		WORD TransportStreamID;
		WORD OriginalNetworkID;
		SdtServiceList ServiceList;
	};

	typedef std::vector<SdtTsInfo> SdtTsList;
	typedef std::map<DWORD, SdtTsInfo> SdtTsMap;
	static DWORD GetSdtTsMapKey(WORD NetworkID, WORD TransportStreamID) {
		return ((DWORD)NetworkID << 16) | (DWORD)TransportStreamID;
	}

	typedef CServiceListDesc::ServiceInfo NetworkServiceInfo;

	struct NetworkTsInfo {
		WORD TransportStreamID;
		WORD OriginalNetworkID;
		std::vector<NetworkServiceInfo> ServiceList;
	};

	typedef std::vector<NetworkTsInfo> NetworkTsList;

	CTsAnalyzer(IEventHandler *pEventHandler = NULL);
	virtual ~CTsAnalyzer();

// CMediaDecoder
	void Reset() override;
	const bool InputMedia(CMediaData *pMediaData, const DWORD dwInputIndex = 0UL) override;

// CTsAnalyzer
	WORD GetServiceNum() const;
	bool GetServiceID(const int Index, WORD *pServiceID) const;
	int GetServiceIndexByID(const WORD ServiceID) const;
	bool IsViewableService(const int Index) const;
	WORD GetViewableServiceNum() const;
	bool GetViewableServiceID(const int Index, WORD *pServiceID) const;
	bool GetFirstViewableServiceID(WORD *pServiceID) const;
	int GetViewableServiceIndexByID(const WORD ServiceID) const;
	bool GetServiceInfo(const int Index, ServiceInfo *pInfo) const;
	bool IsServiceUpdated(const int Index) const;
	bool Is1SegService(const int Index) const;
	bool GetPmtPID(const int Index, WORD *pPmtPID) const;
	WORD GetVideoEsNum(const int Index) const;
	bool GetVideoEsInfo(const int Index, const int VideoIndex, EsInfo *pEsInfo) const;
	bool GetVideoEsList(const int Index, EsInfoList *pEsList) const;
	bool GetVideoEsPID(const int Index, const int VideoIndex, WORD *pVideoPID) const;
	bool GetVideoStreamType(const int Index, const int VideoIndex, BYTE *pStreamType) const;
	BYTE GetVideoComponentTag(const int Index, const int VideoIndex) const;
	int GetVideoIndexByComponentTag(const int Index, const BYTE ComponentTag) const;
	WORD GetAudioEsNum(const int Index) const;
	bool GetAudioEsInfo(const int Index, const int AudioIndex, EsInfo *pEsInfo) const;
	bool GetAudioEsList(const int Index, EsInfoList *pEsList) const;
	bool GetAudioStreamType(const int Index, const int AudioIndex, BYTE *pStreamType) const;
	bool GetAudioEsPID(const int Index, const int AudioIndex, WORD *pAudioPID) const;
	BYTE GetAudioComponentTag(const int Index, const int AudioIndex) const;
	int GetAudioIndexByComponentTag(const int Index, const BYTE ComponentTag) const;
#ifdef TS_ANALYZER_EIT_SUPPORT
	BYTE GetVideoComponentType(const int Index) const;
	BYTE GetAudioComponentType(const int Index, const int AudioIndex) const;
	int GetAudioComponentText(const int Index, const int AudioIndex, LPTSTR pszText, int MaxLength) const;
#endif
	WORD GetCaptionEsNum(const int Index) const;
	bool GetCaptionEsPID(const int Index, const WORD CaptionIndex, WORD *pCaptionPID) const;
	WORD GetDataCarrouselEsNum(const int Index) const;
	bool GetDataCarrouselEsPID(const int Index, const WORD DataCarrouselIndex, WORD *pDataCarrouselPID) const;
	bool GetPcrPID(const int Index, WORD *pPcrPID) const;
	bool GetPcrTimeStamp(const int Index, ULONGLONG *pTimeStamp) const;
	int GetServiceName(const int Index, LPTSTR pszName, const int MaxLength) const;
	BYTE GetServiceType(const int Index) const;
	WORD GetLogoID(const int Index) const;

	bool GetServiceList(ServiceList *pList) const;
	bool GetViewableServiceList(ServiceList *pList) const;
	WORD GetTransportStreamID() const;
	WORD GetNetworkID() const;
	BYTE GetBroadcastingID() const;
	int GetNetworkName(LPTSTR pszName, int MaxLength) const;
	BYTE GetRemoteControlKeyID() const;
	int GetTsName(LPTSTR pszName, int MaxLength) const;
	bool GetSdtServiceList(SdtServiceList *pList) const;
	bool GetSdtTsList(SdtTsList *pList) const;
	bool GetNetworkTsList(NetworkTsList *pList) const;
	bool IsSdtUpdated() const;
	bool IsNitUpdated() const;
	bool IsSdtComplete() const;

	bool SetVideoStreamTypeViewable(const BYTE StreamType, const bool bViewable);
	bool IsVideoStreamTypeViewable(const BYTE StreamType) const;
	void SetRadioSupport(const bool bRadioSupport);
	bool IsRadioSupport() const;

	bool Is1SegStream() const;
	bool Has1SegService() const;
	bool GetFirst1SegServiceID(WORD *pServiceID) const;
	bool Get1SegServiceIDByIndex(const int Index, WORD *pServiceID) const;

#ifdef TS_ANALYZER_EIT_SUPPORT
	struct EventSeriesInfo {
		WORD SeriesID;
		BYTE RepeatLabel;
		BYTE ProgramPattern;
		bool bIsExpireDateValid;
		SYSTEMTIME ExpireDate;
		WORD EpisodeNumber;
		WORD LastEpisodeNumber;
		TCHAR szSeriesName[CSeriesDesc::MAX_SERIES_NAME];
	};

	struct EventVideoInfo {
		enum { MAX_TEXT = 64 };
		BYTE StreamContent;
		BYTE ComponentType;
		BYTE ComponentTag;
		DWORD LanguageCode;
		TCHAR szText[MAX_TEXT];
	};

	struct EventAudioInfo {
		enum { MAX_TEXT = 64 };
		BYTE StreamContent;
		BYTE ComponentType;
		BYTE ComponentTag;
		BYTE SimulcastGroupTag;
		bool bESMultiLingualFlag;
		bool bMainComponentFlag;
		BYTE QualityIndicator;
		BYTE SamplingRate;
		DWORD LanguageCode;
		DWORD LanguageCode2;
		TCHAR szText[MAX_TEXT];
	};

	typedef std::vector<EventAudioInfo> EventAudioList;

	struct EventContentNibble {
		int NibbleCount;
		CContentDesc::Nibble NibbleList[7];
	};

	struct EventInfo {
		WORD EventID;
		bool bValidStartTime;
		SYSTEMTIME StartTime;
		DWORD Duration;
		BYTE RunningStatus;
		bool bFreeCaMode;
		LPTSTR pszEventName;
		int MaxEventName;
		LPTSTR pszEventText;
		int MaxEventText;
		LPTSTR pszEventExtendedText;
		int MaxEventExtendedText;
		EventVideoInfo Video;
		EventAudioList Audio;
		EventContentNibble ContentNibble;
	};

	typedef CComponentGroupDesc::GroupInfo EventComponentGroupInfo;
	typedef std::vector<EventComponentGroupInfo> EventComponentGroupList;

	WORD GetEventID(const int ServiceIndex, const bool bNext = false) const;
	bool GetEventStartTime(const int ServiceIndex, SYSTEMTIME *pSystemTime, const bool bNext = false) const;
	DWORD GetEventDuration(const int ServiceIndex, const bool bNext = false) const;
	bool GetEventTime(const int ServiceIndex, SYSTEMTIME *pTime, DWORD *pDuration, const bool bNext = false) const;
	int GetEventName(const int ServiceIndex, LPTSTR pszName, int MaxLength, const bool bNext = false) const;
	int GetEventText(const int ServiceIndex, LPTSTR pszText, int MaxLength, const bool bNext = false) const;
	int GetEventExtendedText(const int ServiceIndex, LPTSTR pszText, int MaxLength, const bool bUseEventGroup = true, const bool bNext = false) const;
	bool GetEventSeriesInfo(const int ServiceIndex, EventSeriesInfo *pInfo, const bool bNext = false) const;
	bool GetEventVideoInfo(const int ServiceIndex, EventVideoInfo *pInfo, const bool bNext = false) const;
	bool GetEventAudioInfo(const int ServiceIndex, const int AudioIndex, EventAudioInfo *pInfo, bool bNext = false) const;
	bool GetEventAudioList(const int ServiceIndex, EventAudioList *pList, const bool bNext = false) const;
	bool GetEventContentNibble(const int ServiceIndex, EventContentNibble *pInfo, const bool bNext = false) const;
	bool GetEventInfo(const int ServiceIndex, EventInfo *pInfo, const bool bUseEventGroup = true, const bool bNext = false) const;
	int GetEventComponentGroupNum(const int ServiceIndex, const bool bNext = false) const;
	bool GetEventComponentGroupInfo(const int ServiceIndex, const int GroupIndex, EventComponentGroupInfo *pInfo, const bool bNext = false) const;
	bool GetEventComponentGroupList(const int ServiceIndex, EventComponentGroupList *pList, const bool bNext = false) const;
	int GetEventComponentGroupIndexByComponentTag(const int ServiceIndex, const BYTE ComponentTag, const bool bNext = false) const;
#endif	// TS_ANALYZER_EIT_SUPPORT

	bool GetTotTime(SYSTEMTIME *pTime) const;

	struct SatelliteDeliverySystemInfo {
		WORD TransportStreamID;
		DWORD Frequency;
		WORD OrbitalPosition;
		bool bWestEastFlag;
		BYTE Polarization;
		BYTE Modulation;
		DWORD SymbolRate;
		BYTE FECInner;
	};

	struct TerrestrialDeliverySystemInfo {
		WORD TransportStreamID;
		WORD AreaCode;
		BYTE GuardInterval;
		BYTE TransmissionMode;
		std::vector<WORD> Frequency;
	};

	typedef std::vector<SatelliteDeliverySystemInfo> SatelliteDeliverySystemList;
	typedef std::vector<TerrestrialDeliverySystemInfo> TerrestrialDeliverySystemList;

	bool GetSatelliteDeliverySystemList(SatelliteDeliverySystemList *pList) const;
	bool GetTerrestrialDeliverySystemList(TerrestrialDeliverySystemList *pList) const;

	enum EventType {
		EVENT_PAT_UPDATED,
		EVENT_PMT_UPDATED,
		EVENT_SDT_UPDATED,
		EVENT_NIT_UPDATED,
		EVENT_PCR_UPDATED,
		EVENT_TOT_UPDATED,
		EVENT_INVALID,
		NUM_EVENTS = EVENT_INVALID,
		EVENT_LAST = NUM_EVENTS - 1
	};

	class IAnalyzerEventHandler
	{
	public:
		IAnalyzerEventHandler();
		virtual ~IAnalyzerEventHandler();
		virtual void OnEvent(CTsAnalyzer *pAnalyzer, EventType Type) = 0;
		virtual void OnReset(CTsAnalyzer *pAnalyzer);
	};

	bool AddEventHandler(IAnalyzerEventHandler *pHandler);
	bool RemoveEventHandler(IAnalyzerEventHandler *pHandler);

protected:
	void CallEventHandler(EventType Type);
	void NotifyResetEvent();

#ifdef TS_ANALYZER_EIT_SUPPORT
	const class CEitPfTable *GetEitPfTableByServiceID(const WORD ServiceID, int *pIndex = NULL) const;
	const CDescBlock *GetHEitItemDesc(const int ServiceIndex, const bool bNext = false) const;
#ifdef TS_ANALYZER_L_EIT_SUPPORT
	const CDescBlock *GetLEitItemDesc(const int ServiceIndex, const bool bNext = false) const;
#endif
	const CAudioComponentDesc *GetAudioComponentDescByComponentTag(const CDescBlock *pDescBlock, const BYTE ComponentTag) const;
#endif

	CTsPidMapManager m_PidMapManager;

	WORD m_TransportStreamID;
	WORD m_NetworkID;

	bool m_bSdtUpdated;
	bool m_bNitUpdated;

	ServiceList m_ServiceList;
	SdtServiceList m_SdtServiceList;
	SdtTsMap m_SdtTsMap;
	NetworkTsList m_NetworkTsList;

	struct NitInfo {
		BYTE BroadcastingFlag;
		BYTE BroadcastingID;
		BYTE RemoteControlKeyID;
		TCHAR szNetworkName[32];
		TCHAR szTSName[32];

		void Clear() { ::ZeroMemory(this, sizeof(*this)); }
	};
	NitInfo m_NitInfo;

	std::vector<IAnalyzerEventHandler*> m_EventHandlerList;
	EventType m_DecoderEvent;

	enum {
		VIDEO_STREAM_MPEG1	= 0x0001U,
		VIDEO_STREAM_MPEG2	= 0x0002U,
		VIDEO_STREAM_MPEG4	= 0x0004U,
		VIDEO_STREAM_H264	= 0x0008U,
		VIDEO_STREAM_H265	= 0x0010U
	};

	unsigned int m_ViewableVideoStreamTypes;
	bool m_bRadioSupport;

	struct StreamTypeMap {
		unsigned int Flag;
		BYTE StreamType;
	};
	static const StreamTypeMap m_VideoStreamTypeMap[];

private:
	static void CALLBACK OnPatUpdated(const WORD wPID, CTsPidMapTarget *pMapTarget, CTsPidMapManager *pMapManager, const PVOID pParam);
	static void CALLBACK OnPmtUpdated(const WORD wPID, CTsPidMapTarget *pMapTarget, CTsPidMapManager *pMapManager, const PVOID pParam);
	static void CALLBACK OnSdtUpdated(const WORD wPID, CTsPidMapTarget *pMapTarget, CTsPidMapManager *pMapManager, const PVOID pParam);
	static void CALLBACK OnNitUpdated(const WORD wPID, CTsPidMapTarget *pMapTarget, CTsPidMapManager *pMapManager, const PVOID pParam);
	static void CALLBACK OnPcrUpdated(const WORD wPID, CTsPidMapTarget *pMapTarget, CTsPidMapManager *pMapManager, const PVOID pParam);
	static void CALLBACK OnTotUpdated(const WORD wPID, CTsPidMapTarget *pMapTarget, CTsPidMapManager *pMapManager, const PVOID pParam);
};
