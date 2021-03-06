/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include <afxadv.h>
#include <atlsync.h>
#include "..\..\subtitles\STS.h"
#include "MediaFormats.h"
#include "fakefiltermapper2.h"

#ifdef UNICODE
#define MPC_WND_CLASS_NAME L"MediaPlayerClassicW"
#else
#define MPC_WND_CLASS_NAME "MediaPlayerClassicA"
#endif

enum 
{
	WM_GRAPHNOTIFY = WM_APP+1,
	WM_REARRANGERENDERLESS,
	WM_RESUMEFROMSTATE
};

#define WM_MYMOUSELAST WM_XBUTTONDBLCLK

///////////////

extern void CorrectComboListWidth(CComboBox& box, CFont* pWndFont);
extern HICON LoadIcon(CString fn, bool fSmall);
extern bool LoadType(CString fn, CString& type);
extern bool LoadResource(UINT resid, CStringA& str, LPCTSTR restype);
extern CString GetContentType(CString fn, CAtlList<CString>* redir = NULL);

/////////////////////////////////////////////////////////////////////////////
// CMPlayerCApp:
// See mplayerc.cpp for the implementation of this class
//

// flags for AppSettings::nCS
enum 
{
	CS_NONE=0, 
	CS_SEEKBAR=1, 
	CS_TOOLBAR=CS_SEEKBAR<<1, 
	CS_INFOBAR=CS_TOOLBAR<<1, 
	CS_STATSBAR=CS_INFOBAR<<1, 
	CS_STATUSBAR=CS_STATSBAR<<1, 
	CS_LAST=CS_STATUSBAR
};

enum
{
	CLSW_NONE=0,
	CLSW_OPEN=1,
	CLSW_PLAY=CLSW_OPEN<<1,
	CLSW_CLOSE=CLSW_PLAY<<1,
	CLSW_STANDBY=CLSW_CLOSE<<1,
	CLSW_HIBERNATE=CLSW_STANDBY<<1,
	CLSW_SHUTDOWN=CLSW_HIBERNATE<<1,
	CLSW_LOGOFF=CLSW_SHUTDOWN<<1,
	CLSW_AFTERPLAYBACK_MASK=CLSW_CLOSE|CLSW_STANDBY|CLSW_SHUTDOWN|CLSW_HIBERNATE|CLSW_LOGOFF,
	CLSW_FULLSCREEN=CLSW_LOGOFF<<1,
	CLSW_NEW=CLSW_FULLSCREEN<<1,
	CLSW_HELP=CLSW_NEW<<1,
	CLSW_DVD=CLSW_HELP<<1,
	CLSW_CD=CLSW_DVD<<1,
	CLSW_ADD=CLSW_CD<<1,
	CLSW_MINIMIZED=CLSW_ADD<<1,
	CLSW_REGEXTVID=CLSW_MINIMIZED<<1,
	CLSW_REGEXTAUD=CLSW_REGEXTVID<<1,
	CLSW_UNREGEXT=CLSW_REGEXTAUD<<1,
	CLSW_STARTVALID=CLSW_UNREGEXT<<2,
	CLSW_NOFOCUS=CLSW_STARTVALID<<1,
	CLSW_FIXEDSIZE=CLSW_NOFOCUS<<1,
	CLSW_MONITOR=CLSW_FIXEDSIZE<<1,	
	CLSW_UNRECOGNIZEDSWITCH=CLSW_MONITOR<<1	
};

enum
{
	VIDRNDT_DS_DEFAULT,
	VIDRNDT_DS_OLDRENDERER,
	VIDRNDT_DS_OVERLAYMIXER,
	VIDRNDT_DS_VMR7WINDOWED,
	VIDRNDT_DS_VMR9WINDOWED,
	VIDRNDT_DS_VMR7RENDERLESS,
	VIDRNDT_DS_VMR9RENDERLESS,
	VIDRNDT_DS_DXR,
	VIDRNDT_DS_NULL_COMP,
	VIDRNDT_DS_NULL_UNCOMP,
};

enum
{
	VIDRNDT_RM_DEFAULT,
	VIDRNDT_RM_DX7,
	VIDRNDT_RM_DX9,
};

enum
{
	VIDRNDT_QT_DEFAULT,
	VIDRNDT_QT_DX7,
	VIDRNDT_QT_DX9,
};

enum
{
	VIDRNDT_AP_SURFACE,
	VIDRNDT_AP_TEXTURE2D,
	VIDRNDT_AP_TEXTURE3D,
};

#define AUDRNDT_NULL_COMP _T("Null Audio Renderer (Any)")
#define AUDRNDT_NULL_UNCOMP _T("Null Audio Renderer (Uncompressed)")

enum
{
	SRC_CDDA=1, 
	SRC_CDXA=SRC_CDDA<<1,
	SRC_VTS=SRC_CDXA<<1,
	SRC_FLIC=SRC_VTS<<1,
	SRC_D2V=SRC_FLIC<<1,
	SRC_DTSAC3=SRC_D2V<<1,
	SRC_MATROSKA=SRC_DTSAC3<<1,
	SRC_SHOUTCAST=SRC_MATROSKA<<1,
	SRC_REALMEDIA=SRC_SHOUTCAST<<1,
	SRC_AVI=SRC_REALMEDIA<<1,
	SRC_RADGT=SRC_AVI<<1,
	SRC_ROQ=SRC_RADGT<<1,
	SRC_OGG=SRC_ROQ<<1,
	SRC_NUT=SRC_OGG<<1,
	SRC_MPEG=SRC_NUT<<1,
	SRC_DIRAC=SRC_MPEG<<1,
	SRC_MPA=SRC_DIRAC<<1,
	SRC_DSM=SRC_MPA<<1,
	SRC_SUBS=SRC_DSM<<1,
	SRC_MP4=SRC_SUBS<<1,
	SRC_FLV=SRC_MP4<<1,
	SRC_LAST=SRC_FLV<<1
};

enum
{
	TRA_MPEG1=1, 
	TRA_MPEG2=TRA_MPEG1<<1,
	TRA_RV=TRA_MPEG2<<1,
	TRA_RA=TRA_RV<<1,
	TRA_MPA=TRA_RA<<1,
	TRA_LPCM=TRA_MPA<<1,
	TRA_AC3=TRA_LPCM<<1,
	TRA_DTS=TRA_AC3<<1,
	TRA_AAC=TRA_DTS<<1,
	TRA_PS2AUD=TRA_AAC<<1,
	TRA_DIRAC=TRA_PS2AUD<<1,
	TRA_VORBIS=TRA_DIRAC<<1,
	TRA_LAST=TRA_VORBIS<<1,
};

enum
{
	DVS_HALF, 
	DVS_NORMAL, 
	DVS_DOUBLE, 
	DVS_STRETCH, 
	DVS_FROMINSIDE, 
	DVS_FROMOUTSIDE
};

typedef enum 
{
	FAV_FILE,
	FAV_DVD,
	FAV_DEVICE
} favtype;

#pragma pack(push, 1)
typedef struct
{
	bool fValid;
	CSize size; 
	int bpp, freq;
} dispmode;

class wmcmd : public ACCEL
{
	ACCEL backup;
	UINT appcmdorg;
	UINT mouseorg;
public:
	CString name;
	UINT appcmd;
	enum {NONE,LDOWN,LUP,LDBLCLK,MDOWN,MUP,MDBLCLK,RDOWN,RUP,RDBLCLK,X1DOWN,X1UP,X1DBLCLK,X2DOWN,X2UP,X2DBLCLK,WUP,WDOWN,LAST};
	UINT mouse;
	CStringA rmcmd;
	int rmrepcnt;
	wmcmd(WORD cmd = 0) {this->cmd = cmd;}
	wmcmd(WORD cmd, WORD key, BYTE fVirt, LPCTSTR name, UINT appcmd = 0, UINT mouse = NONE, LPCSTR rmcmd = "", int rmrepcnt = 5)
	{
		this->cmd = cmd;
		this->key = key;
		this->fVirt = fVirt;
		this->appcmd = appcmdorg = appcmd;
		this->name = name;
		this->mouse = mouseorg = mouse;
		this->rmcmd = rmcmd;
		this->rmrepcnt = rmrepcnt;
		backup = *this;
	}
	bool operator == (const wmcmd& wc) const
	{
		return(cmd > 0 && cmd == wc.cmd);
	}
	void Restore() {*(ACCEL*)this = backup; appcmd = appcmdorg; mouse = mouseorg; rmcmd.Empty(); rmrepcnt = 5;}
	bool IsModified() {return(memcmp((const ACCEL*)this, &backup, sizeof(ACCEL)) || appcmd != appcmdorg || mouse != mouseorg || !rmcmd.IsEmpty() || rmrepcnt != 5);}
};
#pragma pack(pop)

#include <afxsock.h>

class CRemoteCtrlClient : public CAsyncSocket
{
protected:
	CCritSec m_csLock;
	CWnd* m_pWnd;
	enum {DISCONNECTED, CONNECTED, CONNECTING} m_nStatus;
	CString m_addr;

	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);

	virtual void OnCommand(CStringA str) = 0;

	void ExecuteCommand(CStringA cmd, int repcnt);

public:
	CRemoteCtrlClient();
	void SetHWND(HWND hWnd);
	void Connect(CString addr);
	int GetStatus() {return(m_nStatus);}
};

class CWinLircClient : public CRemoteCtrlClient
{
protected:
	virtual void OnCommand(CStringA str);

public:
	CWinLircClient();
};

class CUIceClient : public CRemoteCtrlClient
{
protected:
	virtual void OnCommand(CStringA str);

public:
	CUIceClient();
};

extern void GetCurDispMode(dispmode& dm);
extern bool GetDispMode(int i, dispmode& dm);
extern void SetDispMode(dispmode& dm);

class CMPlayerCApp : public CWinApp
{
	ATL::CMutex m_mutexOneInstance;

	CAtlList<CString> m_cmdln;
	void PreProcessCommandLine();
	void SendCommandLine(HWND hWnd);

public:
	CMPlayerCApp();

	void ShowCmdlnSwitches();

	bool StoreSettingsToIni();
	bool StoreSettingsToRegistry();
	CString GetIniPath();
	bool IsIniValid();

	bool GetAppDataPath(CString& path);
	
	static bool	IsVistaOrAbove();
	static bool	IsVSFilterInstalled();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMPlayerCApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	class Settings
	{
		friend class CMPlayerCApp;

		bool fInitialized;

		class CRecentFileAndURLList : public CRecentFileList
		{
		public:
			CRecentFileAndURLList(UINT nStart, LPCTSTR lpszSection,
				LPCTSTR lpszEntryFormat, int nSize,
				int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);

			virtual void Add(LPCTSTR lpszPathName); // we have to override CRecentFileList::Add because the original version can't handle URLs
		};

	public:
		// cmdline params
		int nCLSwitches;
		CAtlList<CString> slFiles, slDubs, slSubs, slFilters;
		__int64 rtStart;
		CSize fixedWindowSize;
		bool HasFixedWindowSize() {return fixedWindowSize.cx > 0 || fixedWindowSize.cy > 0;}
		// int iFixedWidth, iFixedHeight;
		int iMonitor;

		void ParseCommandLine(CAtlList<CString>& cmdln);

		bool fXpOrBetter;
		int iDXVer;

		int nCS;
		bool fHideCaptionMenu;
		int iDefaultVideoSize;
		bool fKeepAspectRatio;
		bool fCompMonDeskARDiff;

		CRecentFileAndURLList MRU;
		CRecentFileAndURLList MRUDub;

		CAutoPtrList<FilterOverride> filters;

		int iDSVideoRendererType;
		int iRMVideoRendererType;
		int iQTVideoRendererType;
		int iAPSurfaceUsage;
		bool fVMRSyncFix;
		int iDX9Resizer;
		bool fVMR9MixerMode;
		bool fVMR9MixerYUV;

		int nVolume;
		int nBalance;
		bool fMute;
		int nLoops;
		bool fLoopForever;
		bool fRewind;
		int iZoomLevel;
		// int iVideoRendererType; 
		CStringW AudioRendererDisplayName;
		bool fAutoloadAudio;
		bool fAutoloadSubtitles;
		bool fBlockVSFilter;
		bool fEnableWorkerThreadForOpening;
		bool fReportFailedPins;

		bool fAllowMultipleInst;
		int iTitleBarTextStyle;
		bool fTitleBarTextTitle;
		int iOnTop;
		bool fTrayIcon;
		bool fRememberZoomLevel;
		bool fShowBarsWhenFullScreen;
		int nShowBarsWhenFullScreenTimeOut;
		dispmode dmFullscreenRes;
		bool fExitFullScreenAtTheEnd;
		bool fRememberWindowPos;
		bool fRememberWindowSize;
		bool fSnapToDesktopEdges;
		CRect rcLastWindowPos;
		UINT lastWindowType;
		CSize AspectRatio;
		bool fKeepHistory;

		CString sDVDPath;
		bool fUseDVDPath;
		LCID idMenuLang, idAudioLang, idSubtitlesLang;
		bool fAutoSpeakerConf;

		STSStyle subdefstyle;
		bool fOverridePlacement;
		int nHorPos, nVerPos;
		int nSPCSize;
		int nSPCMaxRes;
		int nSubDelayInterval;
		bool fSPCPow2Tex;
		bool fEnableSubtitles;

		bool fDisabeXPToolbars;
		bool fUseWMASFReader;
		int nJumpDistS;
		int nJumpDistM;
		int nJumpDistL;
		bool fFreeWindowResizing;
		bool fNotifyMSN;
		bool fNotifyGTSdll;

		bool fEnableAudioSwitcher;
		bool fDownSampleTo441;
		bool fAudioTimeShift;
		int tAudioTimeShift;
		bool fCustomChannelMapping;
		DWORD pSpeakerToChannelMap[18][18];
		bool fAudioNormalize;
		bool fAudioNormalizeRecover;
		float AudioBoost;

		bool fIntRealMedia;
		// bool fRealMediaRenderless;
		int iQuickTimeRenderer;
		float RealMediaQuickTimeFPS;

		CStringArray m_pnspresets;

		CList<wmcmd> wmcmds;
		HACCEL hAccel;

		bool fWinLirc;
		CString WinLircAddr;
		CWinLircClient WinLircClient;
		bool fUIce;
		CString UIceAddr;
		CUIceClient UIceClient;

		CMediaFormats Formats;

		UINT SrcFilters, TraFilters;

		CString logofn;
		UINT logoid;
		bool logoext;

		bool fHideCDROMsSubMenu;

		DWORD priority;
		bool launchfullscreen;

		BOOL fEnableWebServer;
		int nWebServerPort;
		bool fWebServerPrintDebugInfo;
		bool fWebServerUseCompression;
		bool fWebServerLocalhostOnly;
		CString WebRoot, WebDefIndex;
		CString WebServerCGI;

		CString SnapShotPath, SnapShotExt;
		int ThumbRows, ThumbCols, ThumbWidth;

		CString ISDb;

		struct Shader {CString label, target, srcdata;};
		CAtlList<Shader> m_shaders;
		CString m_shadercombine;
		
		CString	strShaderList;

	public:
		Settings();
		virtual ~Settings();
		void UpdateData(bool fSave);

		void GetFav(favtype ft, CAtlList<CString>& sl);
		void SetFav(favtype ft, CAtlList<CString>& sl);
		void AddFav(favtype ft, CString s);
	} m_s;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
	afx_msg void OnFileExit();
	afx_msg void OnHelpShowcommandlineswitches();
};

#define AfxGetMyApp() ((CMPlayerCApp*)AfxGetApp())
#define AfxGetAppSettings() ((CMPlayerCApp*)AfxGetApp())->m_s
#define AppSettings CMPlayerCApp::Settings
