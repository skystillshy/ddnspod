// DDNSpodDlg.cpp : 实现文件
//
#pragma once

#include "stdafx.h"
#include "DDNSpod.h"
#include "DDNSpodDlg.h"
#include "DDNSpodCDlg.h"
#include "DDNSpodAPI.h"
#include "tinyxml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDDNSpodDlg 对话框

CDDNSpodDlg::CDDNSpodDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDDNSpodDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_conf.User="";
	m_conf.Password="";
	m_conf.URL=_T("www.dnspod.com");
	m_conf.port=80;
	m_conf.Domain="";
	m_conf.DomainID=0;
	m_conf.A_Record="";
	m_conf.A_RecordID=0;
	m_conf.A_Record_enabled=TRUE;
	m_conf.AAAA_Record="";
	m_conf.AAAA_RecordID=0;
	m_conf.AAAA_Record_enabled=TRUE;
	m_conf.AutoStart=FALSE;
	m_conf.UpdateType=ONCE;
	m_bUsingDefault=TRUE;
	m_User="";
	m_Password="";
	ip4="";
	ip6=_T("::1");
	AllRecordList=NULL;
	m_confpath="";
	DNSpodAPIVer=_T("1.5");
	//带命令行参数
	if (AfxGetApp()->m_lpCmdLine[0]!='\0')
	{
		bShow=FALSE;
	}
	else
		bShow=TRUE;
}

void CDDNSpodDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK, m_bUsingDefault);
	DDX_Text(pDX, IDC_EDIT_USER, m_User);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_Password);
}

BEGIN_MESSAGE_MAP(CDDNSpodDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_EXIT, &CDDNSpodDlg::OnFileExit)
	ON_COMMAND(ID_ABOUT, &CDDNSpodDlg::OnAbout)
	ON_COMMAND(ID_FILE_SETTING, &CDDNSpodDlg::OnFileSetting)
	ON_WM_SIZE()
	ON_MESSAGE(WM_TRAY,OnClickTray)
	ON_COMMAND(ID_TRAY_SHOWWINDOW, &CDDNSpodDlg::OnTrayShowwindow)
	ON_COMMAND(ID_TRAY_SETTING, &CDDNSpodDlg::OnTraySetting)
	ON_COMMAND(ID_TRAY_EXIT, &CDDNSpodDlg::OnTrayExit)
	ON_COMMAND(ID_TRAY_ABOUT, &CDDNSpodDlg::OnTrayAbout)
	ON_COMMAND(ID_FILE_REFRESH, &CDDNSpodDlg::OnFileRefresh)
	ON_COMMAND(ID_FILE_SAVE, &CDDNSpodDlg::OnFileSave)
	ON_COMMAND(ID_TRAY_REFRESH, &CDDNSpodDlg::OnTrayRefresh)
	ON_COMMAND(ID_TRAY_SAVE, &CDDNSpodDlg::OnTraySave)
	ON_BN_CLICKED(IDC_CHECK, &CDDNSpodDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDDNSpodDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CDDNSpodDlg::OnBnClickedButtonLogin)
	ON_COMMAND(ID_FILE_SHOWIP, &CDDNSpodDlg::OnFileShowip)
	ON_COMMAND(ID_TRAY_SHOWIP, &CDDNSpodDlg::OnTrayShowip)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CDDNSpodDlg 消息处理程序

BOOL CDDNSpodDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_menu.LoadMenu(IDR_MENU1);
	CDialog::SetMenu(&m_menu);

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//加载配置文件
	TCHAR szFull[_MAX_PATH];
	/*
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	::GetModuleFileName(NULL, szFull, sizeof(szFull)/sizeof(TCHAR));
	_tsplitpath(szFull, szDrive, szDir, NULL, NULL);
	_tcscpy(szFull, szDrive);
	_tcscat(szFull, szDir);
	strPath = CString(szFull);
	*/
	GetModuleFileName(NULL,szFull,_MAX_PATH);
	m_confpath=CString(szFull);
	int pos;
	pos=m_confpath.ReverseFind('\\');
	m_confpath=m_confpath.Left(pos);
	m_confpath+=_T("\\config.xml");

	TiXmlDocument *doc = new TiXmlDocument(CStringA(m_confpath));
	bool loadOK = doc->LoadFile(TIXML_ENCODING_LEGACY);
	//bool loadOK = doc->LoadFile(TIXML_ENCODING_UTF8);
	if (loadOK)
	{
		m_bConfLoaded=LoadFromFile(doc);
		if (!m_bConfLoaded)
		{
			::DeleteFile(m_confpath);
			CreateConfFile();
		}
	}
	else
	{
		CreateConfFile();
	}

	//控件已创建
	GetDlgItem(IDC_EDIT_USER)->SetWindowTextW(m_User);
	GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowTextW(m_Password);

	if (m_conf.UpdateType==ONCE)
		SetTimer(100,100000,NULL); //100秒
	else if (m_conf.UpdateType==AUTO)
		SetTimer(100,1800000,NULL); //30分钟

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

int CDDNSpodDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//最小化启动，参数由外部传入
	if (!bShow)
	{
		m_nd.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
		m_nd.hWnd=this->m_hWnd;
		m_nd.uID=IDR_MAINFRAME;
		m_nd.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
		m_nd.uCallbackMessage=WM_TRAY;            //回调消息
		m_nd.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		_tcscpy_s(m_nd.szTip,_T("DDNSpod"));      //信息提示
		Shell_NotifyIcon(NIM_ADD,&m_nd);          //在托盘区添加图标
		//ShowWindow(SW_HIDE);                    //隐藏主窗口
		//UpdateWindow();
	}

	return 0;
}

void CDDNSpodDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (!bShow)
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialog::OnWindowPosChanging(lpwndpos);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDDNSpodDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CDDNSpodDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDDNSpodDlg::LoadFromFile(TiXmlDocument *xmlfile)
{
	TiXmlHandle hRoot(xmlfile);
	TiXmlElement *pElem;
	TiXmlHandle hXmlHandle(0);
	
	//configure节点
	pElem=hRoot.FirstChildElement().Element();
	if (!pElem) return FALSE;
	CString str(pElem->Value());
	if (str != "configure")
		return FALSE;

	//linkurl节点
	hXmlHandle=TiXmlHandle(pElem);
	pElem=hXmlHandle.FirstChild("linkurl").Element();
	if (!pElem) return FALSE;
	m_conf.URL=pElem->GetText(); //CString(pElem->Value())

	//autostart节点
	pElem=hXmlHandle.FirstChild("autostart").Element();
	if (!pElem) return FALSE;
	str=pElem->GetText();
	if (str.MakeLower()=="true")
		m_conf.AutoStart=TRUE;
	else
		m_conf.AutoStart=FALSE;

	//updatetype节点
	pElem=hXmlHandle.FirstChild("updatetype").Element();
	if (!pElem) return FALSE;
	if (pElem->GetText())
	{
		switch (atoi(pElem->GetText()))
		{
		case 1:
			m_conf.UpdateType=NOTAUTO;
			break;
		case 2:
			m_conf.UpdateType=AUTO;
			break;
		case 3:
			m_conf.UpdateType=ONCE;
			break;
		default:
			m_conf.UpdateType=ONCE;
		}
	}
	else
		m_conf.UpdateType=ONCE;

	//account节点
	pElem=hXmlHandle.FirstChild("account").Element();
	if (!pElem) return FALSE;

	//user节点
	hXmlHandle=TiXmlHandle(pElem);
	pElem=hXmlHandle.FirstChild("user").Element();
	if (!pElem) return FALSE;
	m_conf.User=pElem->GetText();

	//password节点
	pElem=hXmlHandle.FirstChild("password").Element();
	if (!pElem) return FALSE;
	m_conf.Password=pElem->GetText();

	//domain节点
	pElem=hXmlHandle.FirstChild("domain").Element();
	if (!pElem) return FALSE;
	//if (pElem->Attribute("name"))
	m_conf.Domain=pElem->Attribute("name");
	if (pElem->Attribute("DomainID"))
		m_conf.DomainID=atoi(pElem->Attribute("DomainID"));
	else
		m_conf.DomainID=0;

	//A_record节点
	hXmlHandle=TiXmlHandle(pElem);
	pElem=hXmlHandle.FirstChild("A_record").Element();
	if (!pElem) return FALSE;
	//if (pElem->Attribute("name"))
	m_conf.A_Record=pElem->Attribute("name");
	if (pElem->Attribute("RecordID"))
		m_conf.A_RecordID=atoi(pElem->Attribute("RecordID"));
	else
		m_conf.A_RecordID=0;
	str="";
	str=pElem->Attribute("enabled");
	if (str.MakeLower()=="true")
		m_conf.A_Record_enabled=TRUE;
	else
		m_conf.A_Record_enabled=FALSE;

	//AAAA_record节点
	pElem=hXmlHandle.FirstChild("AAAA_record").Element();
	if (!pElem) return FALSE;
	//if (pElem->Attribute("name"))
	m_conf.AAAA_Record=pElem->Attribute("name");
	if (pElem->Attribute("RecordID"))
		m_conf.AAAA_RecordID=atoi(pElem->Attribute("RecordID"));
	else
		m_conf.AAAA_RecordID=0;
	str="";
	str=pElem->Attribute("enabled");
	if (str.MakeLower()=="true")
		m_conf.AAAA_Record_enabled=TRUE;
	else
		m_conf.AAAA_Record_enabled=FALSE;

	/*
	MessageBox(_T("URL:")+m_conf.URL+_T("\nUser:")+m_conf.User+
		_T("\nPassword:")+m_conf.Password+_T("\nDomain:")+m_conf.Domain+
		_T("\nA_Record:")+m_conf.A_Record+_T("\nAAAA_Record:")+m_conf.AAAA_Record);
	*/

	m_User=m_conf.User;
	m_Password=m_conf.Password;

	return TRUE;
}

//创建配置文件
BOOL CDDNSpodDlg::CreateConfFile()
{
	TiXmlDocument configdoc;
	TiXmlDeclaration *dec1=new TiXmlDeclaration("1.0","gb2312","");
	//TiXmlDeclaration *dec1=new TiXmlDeclaration("1.0","utf-8","");
	TiXmlElement *configure=new TiXmlElement("configure");

	TiXmlElement *linkurl=new TiXmlElement("linkurl");
	TiXmlText *linkvalue=new TiXmlText("www.dnspod.com");
	linkurl->LinkEndChild(linkvalue);
	configure->LinkEndChild(linkurl);

	TiXmlElement *autostart=new TiXmlElement("autostart");
	TiXmlText *autostartvalue=new TiXmlText("false");
	autostart->LinkEndChild(autostartvalue);
	configure->LinkEndChild(autostart);

	TiXmlElement *updatetype=new TiXmlElement("updatetype");
	TiXmlText *updatetypevalue=new TiXmlText("3");
	updatetype->LinkEndChild(updatetypevalue);
	configure->LinkEndChild(updatetype);

	TiXmlElement *account=new TiXmlElement("account");
	TiXmlElement *user=new TiXmlElement("user");
	TiXmlText *uservalue=new TiXmlText("");
	user->LinkEndChild(uservalue);
	account->LinkEndChild(user);
	TiXmlElement *password=new TiXmlElement("password");
	TiXmlText *passwordvalue=new TiXmlText("");
	password->LinkEndChild(passwordvalue);
	account->LinkEndChild(password);

	TiXmlElement *domain=new TiXmlElement("domain");
	domain->SetAttribute("name","");
	domain->SetAttribute("DomainID",0);
	TiXmlElement *A_record=new TiXmlElement("A_record");
	A_record->SetAttribute("name","");
	A_record->SetAttribute("enabled","true");
	A_record->SetAttribute("RecordID",0);
	domain->LinkEndChild(A_record);
	TiXmlElement *AAAA_record=new TiXmlElement("AAAA_record");
	AAAA_record->SetAttribute("name","");
	AAAA_record->SetAttribute("enabled","true");
	AAAA_record->SetAttribute("RecordID",0);
	domain->LinkEndChild(AAAA_record);
	account->LinkEndChild(domain);
	configure->LinkEndChild(account);

	configdoc.LinkEndChild(dec1);
	configdoc.LinkEndChild(configure);
	configdoc.SaveFile(CStringA(m_confpath));

	return TRUE;
}

BOOL CDDNSpodDlg::SaveConfFile(BOOL bRegKey)
{
	TiXmlDocument configdoc;
	TiXmlDeclaration *dec1=new TiXmlDeclaration("1.0","gb2312","");
	//TiXmlDeclaration *dec1=new TiXmlDeclaration("1.0","utf-8","");
	TiXmlElement *configure=new TiXmlElement("configure");

	TiXmlElement *linkurl=new TiXmlElement("linkurl");
	TiXmlText *linkvalue=new TiXmlText("www.dnspod.com");
	linkurl->LinkEndChild(linkvalue);
	configure->LinkEndChild(linkurl);

	TiXmlElement *autostart=new TiXmlElement("autostart");
	TiXmlText *autostartvalue;
	if (m_conf.AutoStart==TRUE)
		autostartvalue=new TiXmlText("true");
	else
		autostartvalue=new TiXmlText("false");
	if (bRegKey)
	{
		HKEY hKey;
		LPCTSTR lpSubKey=_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\0");
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,lpSubKey,0L,KEY_ALL_ACCESS,&hKey);
		if (m_conf.AutoStart==TRUE)
		{
			//自动启动
			TCHAR AppPathName[MAX_PATH]; //最长260
			GetModuleFileName(NULL, AppPathName, MAX_PATH);
			CString path(AppPathName);
			path += _T(" /hide");
			LPCTSTR lpValue;
			lpValue=(LPCTSTR)path;
			RegSetValueEx(hKey, _T("DDNSpod"), 0L, REG_SZ, (const BYTE *)lpValue, /*MAX_PATH*/wcslen(lpValue)*2 + 1); //unicode
			//MessageBox(lpValue);
		}
		else
		{
			//删除自动启动
			RegDeleteValue(hKey, _T("DDNSpod"));
		}
	}
	autostart->LinkEndChild(autostartvalue);
	configure->LinkEndChild(autostart);

	TiXmlElement *updatetype=new TiXmlElement("updatetype");
	TiXmlText *updatetypevalue;
	switch (m_conf.UpdateType)
	{
	case NOTAUTO:
		updatetypevalue=new TiXmlText("1");
		break;
	case AUTO:
		updatetypevalue=new TiXmlText("2");
		break;
	case ONCE:
		updatetypevalue=new TiXmlText("3");
		break;
	default:
		updatetypevalue=new TiXmlText("3");
	}
	updatetype->LinkEndChild(updatetypevalue);
	configure->LinkEndChild(updatetype);

	TiXmlElement *account=new TiXmlElement("account");
	TiXmlElement *user=new TiXmlElement("user");
	CStringA UserValue(m_conf.User); //16bit unicode -> 8bit ansi
	TiXmlText *uservalue=new TiXmlText(UserValue);
	user->LinkEndChild(uservalue);
	account->LinkEndChild(user);
	TiXmlElement *password=new TiXmlElement("password");
	TiXmlText *passwordvalue=new TiXmlText(CStringA(m_conf.Password));
	password->LinkEndChild(passwordvalue);
	account->LinkEndChild(password);

	TiXmlElement *domain=new TiXmlElement("domain");
	domain->SetAttribute("name",CStringA(m_conf.Domain));
	domain->SetAttribute("DomainID",m_conf.DomainID);
	TiXmlElement *A_record=new TiXmlElement("A_record");
	A_record->SetAttribute("name",CStringA(m_conf.A_Record));
	A_record->SetAttribute("RecordID",m_conf.A_RecordID);
	if (m_conf.A_Record_enabled)
		A_record->SetAttribute("enabled","true");
	else
		A_record->SetAttribute("enabled","false");
	domain->LinkEndChild(A_record);
	TiXmlElement *AAAA_record=new TiXmlElement("AAAA_record");
	AAAA_record->SetAttribute("name",CStringA(m_conf.AAAA_Record));
	AAAA_record->SetAttribute("RecordID",m_conf.AAAA_RecordID);
	if (m_conf.AAAA_Record_enabled)
		AAAA_record->SetAttribute("enabled","true");
	else
		AAAA_record->SetAttribute("enabled","false");
	domain->LinkEndChild(AAAA_record);
	account->LinkEndChild(domain);
	configure->LinkEndChild(account);

	configdoc.LinkEndChild(dec1);
	configdoc.LinkEndChild(configure);
	if (!configdoc.SaveFile(CStringA(m_confpath)))
	{
		::AfxMessageBox(_T("无法保存配置文件！"));
		return FALSE;
	}

	return TRUE;
}
CConfig CDDNSpodDlg::GetConfig()
{
	return m_conf;
}

void CDDNSpodDlg::OnFileExit()
{
	//Shell_NotifyIcon(NIM_DELETE,&m_nd);
	SendMessage(WM_CLOSE);
}

void CDDNSpodDlg::OnAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

//配置对话框
void CDDNSpodDlg::OnFileSetting()
{
	CSettingDlg dlgSetting;
	if (dlgSetting.DoModal()==IDOK)
	{
		switch (dlgSetting.m_int)
		{
		case 0:
			dlgSetting.m_dlgconf.UpdateType=NOTAUTO;
			break;
		case 1:
			dlgSetting.m_dlgconf.UpdateType=AUTO;
			break;
		case 2:
			dlgSetting.m_dlgconf.UpdateType=ONCE;
			break;
		default:
			dlgSetting.m_dlgconf.UpdateType=ONCE;
			break;
		}
		m_conf=dlgSetting.m_dlgconf;
		SaveConfFile(TRUE);
		
		if (m_conf.UpdateType==ONCE)
		{
			SetTimer(100,100000,NULL); //100秒
		}
		else if (m_conf.UpdateType==AUTO)
		{
			SetTimer(100,1800000,NULL); //30分钟
		}
	}
}

void CDDNSpodDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//最小化到托盘
	if (nType==SIZE_MINIMIZED)
	{
		m_nd.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
		m_nd.hWnd=this->m_hWnd;
		m_nd.uID=IDR_MAINFRAME;
		m_nd.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
		m_nd.uCallbackMessage=WM_TRAY;            //回调消息
		m_nd.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		_tcscpy_s(m_nd.szTip,_T("DDNSpod"));      //信息提示
		Shell_NotifyIcon(NIM_ADD,&m_nd);          //在托盘区添加图标
		ShowWindow(SW_HIDE);                      //隐藏主窗口
	}
}

//托盘响应
LRESULT CDDNSpodDlg::OnClickTray(WPARAM wParam, LPARAM lParam)
{
	if(wParam!=IDR_MAINFRAME)
		return 1;
	
	switch(lParam) //鼠标行为
	{
	case WM_RBUTTONUP://右键
		{
			this->SetForegroundWindow(); //设为前台，当鼠标点击其他地方，消除菜单
			CPoint pt;
			GetCursorPos(&pt);
			CMenu menu;
			menu.LoadMenu(IDR_MENU2);
			CMenu *pSubMenu=menu.GetSubMenu(0);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
          }
		break;
	case WM_LBUTTONDBLCLK://双击左键
		{
			Shell_NotifyIcon(NIM_DELETE,&m_nd); //取消托盘
			bShow=TRUE;
			this->ShowWindow(SW_NORMAL);        //显示主窗口
			this->SetForegroundWindow();        //显示到最前端
		}
		break;
	}
	return 0;
}

void CDDNSpodDlg::OnTrayShowwindow()
{
	Shell_NotifyIcon(NIM_DELETE,&m_nd);
	bShow=TRUE;
	this->ShowWindow(SW_NORMAL);
	this->SetForegroundWindow();
}

void CDDNSpodDlg::OnTraySetting()
{
	CSettingDlg dlgSetting;
	if (dlgSetting.DoModal()==IDOK)
	{
		switch (dlgSetting.m_int)
		{
		case 0:
			dlgSetting.m_dlgconf.UpdateType=NOTAUTO;
			break;
		case 1:
			dlgSetting.m_dlgconf.UpdateType=AUTO;
			break;
		case 2:
			dlgSetting.m_dlgconf.UpdateType=ONCE;
			break;
		default:
			dlgSetting.m_dlgconf.UpdateType=ONCE;
			break;
		}
		m_conf=dlgSetting.m_dlgconf;
		SaveConfFile(TRUE);
		
		if (m_conf.UpdateType==ONCE)
		{
			SetTimer(100,100000,NULL); //100秒
		}
		else if (m_conf.UpdateType==AUTO)
		{
			SetTimer(100,1800000,NULL); //30分钟
		}
	}
}

void CDDNSpodDlg::OnTrayExit()
{
	Shell_NotifyIcon(NIM_DELETE,&m_nd);
	SendMessage(WM_CLOSE);
}

void CDDNSpodDlg::OnTrayAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

BOOL CDDNSpodDlg::GetIP()
{
	//ipv4
	//偷懒ing
	DWORD dwFlags = 0;
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulOutBufLen = 0;
	InternetGetConnectedState(&dwFlags, 0); //得到上网方式

	if(dwFlags & INTERNET_CONNECTION_MODEM) //拨号上网
	{
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
		ulOutBufLen  = sizeof(IP_ADAPTER_INFO);

		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
		{
			free(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen); 
		}

		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
		{
			while (pAdapterInfo != NULL)
			{    
				//ppp拨号上网
				if (pAdapterInfo->Type == MIB_IF_TYPE_PPP) 
				{
					ip4 = CString(pAdapterInfo->IpAddressList.IpAddress.String);
				}
				/*
				//以太网
				if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET)
				{
					ip4=CString(pAdapterInfo->IpAddressList.IpAddress.String);
				}
				*/
				pAdapterInfo = pAdapterInfo->Next;
			}
		}
	}
	else
	{
		hostent *name;
		char Name[255];
		memset(Name,0,255);
		gethostname(Name,255);
		name=gethostbyname(Name);
		ip4=CString(inet_ntoa(*(in_addr *)name->h_addr_list[0]));
		//MessageBox(ip4);
	}


	//ipv6
	PIP_ADAPTER_ADDRESSES pAddresses;
	pAddresses = (IP_ADAPTER_ADDRESSES*) malloc(sizeof(IP_ADAPTER_ADDRESSES));
	ULONG outBufLen = 0;
	DWORD dwRetVal = 0;
	TCHAR buff[100];
	DWORD bufflen=100;
	//TCHAR *IPAddr;
	//int addrLen = sizeof(SOCKADDR_STORAGE);

	if (GetAdaptersAddresses(AF_INET6, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*) malloc(outBufLen);
	}
	if ((dwRetVal = GetAdaptersAddresses(AF_INET6, 0 , NULL, pAddresses, &outBufLen)) == NO_ERROR) 
	{
		PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
		while (pCurrAddresses) 
		{
			if(pCurrAddresses->FirstUnicastAddress !=NULL)
			{
				PIP_ADAPTER_UNICAST_ADDRESS pCurrIP=pCurrAddresses->FirstUnicastAddress;
				while(pCurrIP)
				{
					WSAAddressToString(pCurrIP->Address.lpSockaddr,
						pCurrIP->Address.iSockaddrLength,NULL,buff,&bufflen);
					if (_wcsnicmp(buff,_T("fe80"),4)==0||wcscmp(buff,_T("::1")) ==0)
					{
						//::AfxMessageBox(buff);
						/*
						CString tmp;
						tmp.Format(_T("%d"),pCurrAddresses->Ipv6IfIndex);
						::AfxMessageBox(tmp);
						*/
					}
					else
					{
						//wmemcpy(IPAddr,buff,bufflen);
						//::AfxMessageBox(buff);
						ip6=CString(buff);
						//return TRUE;
					}
					pCurrIP=pCurrIP->Next;
				}
			}
			pCurrAddresses = pCurrAddresses->Next;
		}
	}
	//::AfxMessageBox(ip6);

	return TRUE;
}

void CDDNSpodDlg::OnFileRefresh()
{
	GetIP();

	CDNSpodAPI DNSpod(m_conf.URL,m_conf.port,m_conf.User,m_conf.Password,_T("xml"));
	TiXmlDocument DomainList=DNSpod.DomainList();
	if (!DNSpod.bLogin())
	{
		::AfxMessageBox(_T("登录失败!"));
		//DomainList.SaveFile("Error.xml");
		return;
	}
	//DomainList.SaveFile("domian.xml");

	m_conf.DomainID=DNSpod.GetDomainID(DomainList,m_conf.Domain);
	if (m_conf.DomainID==0)
	{
		::AfxMessageBox(_T("域名不存在!"));
		//DomainList.SaveFile("domain.xml");
		return;
	}
	
	TiXmlDocument RecordList=DNSpod.RecordList(m_conf.DomainID);
	if (!DNSpod.bLogin())
	{
		::AfxMessageBox(_T("记录解析失败!"));
		//RecordList.SaveFile("Error.xml");
		return;
	}
	//RecordList.SaveFile("record.xml");
	
	if (m_conf.A_Record_enabled)
		m_conf.A_RecordID=DNSpod.GetRecordID(RecordList,m_conf.A_Record,_T("A"));
	if (m_conf.AAAA_Record_enabled)
		m_conf.AAAA_RecordID=DNSpod.GetRecordID(RecordList,m_conf.AAAA_Record,_T("AAAA"));
	if ((m_conf.A_Record_enabled&&(m_conf.A_RecordID==0))||(m_conf.AAAA_Record_enabled&&(m_conf.AAAA_RecordID==0)))
	{
		::AfxMessageBox(_T("记录不存在!"));
		//RecordList.SaveFile("record.xml");
		return;
	}

	DNSpod.GetAPIVersion();
	//版本不匹配
	if (!DNSpod.APIVerMatch(DNSpodAPIVer))
	{
		AfxMessageBox(_T("DNSpod API版本不匹配,程序当前支持版本为")+DNSpodAPIVer+_T(",请下载新版本\nhttp://code.google.com/p/ddnspod/"));
		return;
	}

	//静默处理
	if (m_conf.A_Record_enabled&&m_conf.A_RecordID!=0)
	{
		DNSpod.RecordModify(m_conf.DomainID,m_conf.A_RecordID,m_conf.A_Record,_T("A"),_T("default"),ip4,1,1);
	}
	if (m_conf.AAAA_Record_enabled&&m_conf.AAAA_RecordID!=0)
	{
		DNSpod.RecordModify(m_conf.DomainID,m_conf.AAAA_RecordID,m_conf.AAAA_Record,_T("AAAA"),_T("default"),ip6,1,1);
	}
	SaveConfFile(FALSE); //保存各ID
	return;
}

void CDDNSpodDlg::OnTrayRefresh()
{
	GetIP();

	CDNSpodAPI DNSpod(m_conf.URL,m_conf.port,m_conf.User,m_conf.Password,_T("xml"));
	TiXmlDocument DomainList=DNSpod.DomainList();
	if (!DNSpod.bLogin())
	{
		::AfxMessageBox(_T("登录失败!"));
		//DomainList.SaveFile("Error.xml");
		return;
	}
	//DomainList.SaveFile("domian.xml");

	m_conf.DomainID=DNSpod.GetDomainID(DomainList,m_conf.Domain);
	if (m_conf.DomainID==0)
	{
		::AfxMessageBox(_T("域名不存在!"));
		//DomainList.SaveFile("domain.xml");
		return;
	}
	
	TiXmlDocument RecordList=DNSpod.RecordList(m_conf.DomainID);
	if (!DNSpod.bLogin())
	{
		::AfxMessageBox(_T("记录解析失败!"));
		//RecordList.SaveFile("Error.xml");
		return;
	}
	//RecordList.SaveFile("record.xml");
	
	if (m_conf.A_Record_enabled)
		m_conf.A_RecordID=DNSpod.GetRecordID(RecordList,m_conf.A_Record,_T("A"));
	if (m_conf.AAAA_Record_enabled)
		m_conf.AAAA_RecordID=DNSpod.GetRecordID(RecordList,m_conf.AAAA_Record,_T("AAAA"));
	if ((m_conf.A_Record_enabled&&(m_conf.A_RecordID==0))||(m_conf.AAAA_Record_enabled&&(m_conf.AAAA_RecordID==0)))
	{
		::AfxMessageBox(_T("记录不存在!"));
		//RecordList.SaveFile("record.xml");
		return;
	}

	DNSpod.GetAPIVersion();
	//版本不匹配
	if (!DNSpod.APIVerMatch(DNSpodAPIVer))
	{
		AfxMessageBox(_T("DNSpod API版本不匹配,程序当前支持版本为")+DNSpodAPIVer+_T(",请下载新版本\nhttp://code.google.com/p/ddnspod/"));
		return;
	}

	//静默处理
	if (m_conf.A_Record_enabled&&m_conf.A_RecordID!=0)
	{
		DNSpod.RecordModify(m_conf.DomainID,m_conf.A_RecordID,m_conf.A_Record,_T("A"),_T("default"),ip4,1,1);
	}
	if (m_conf.AAAA_Record_enabled&&m_conf.AAAA_RecordID!=0)
	{
		DNSpod.RecordModify(m_conf.DomainID,m_conf.AAAA_RecordID,m_conf.AAAA_Record,_T("AAAA"),_T("default"),ip6,1,1);
	}
	SaveConfFile(FALSE); //保存各ID
	return;
}

BOOL CDDNSpodDlg::GetRecordList(CString User,CString Password)
{
	if (AllRecordList)
	{
		delete AllRecordList;
		AllRecordList=NULL;
	}

	CDNSpodAPI DNSpod(m_conf.URL,m_conf.port,User,Password,_T("xml"));
	TiXmlDocument DomainList=DNSpod.DomainList();
	if (!DNSpod.bLogin())
	{
		return FALSE;
	}

	TiXmlHandle hRoot(&DomainList);
	TiXmlElement *pElem;
	pElem=hRoot.FirstChild("dnspod").FirstChild("domains").ToElement();
	if (!pElem)
	{
		return FALSE;
	}
	TiXmlElement *pSubElem;
	//存在域名
	pSubElem=pElem->FirstChildElement("domain");
	if (!pSubElem)
	{
		return FALSE;
	}

	AllRecordList = new TiXmlDocument();
	TiXmlDeclaration *dec1=new TiXmlDeclaration("1.0","gb2312","");
	AllRecordList->LinkEndChild(dec1);
	//Domians节点
	TiXmlElement *pDomains=new TiXmlElement("Domains");

	//遍历
	for(const TiXmlElement *sub_tag =pElem->FirstChildElement(); sub_tag; sub_tag = sub_tag->NextSiblingElement())
	{
		int DomainID,RecordNum;
		DomainID=atoi(sub_tag->FirstChildElement("id")->GetText());
		//DomainID=DNSpod.GetDomainID(DomainList,DomainName);
		RecordNum=atoi(sub_tag->FirstChildElement("records")->GetText());

		//Domain节点
		TiXmlElement *pDomainElem=new TiXmlElement("Domain");
		pDomainElem->SetAttribute("DomainName",sub_tag->FirstChildElement("name")->GetText());
		pDomainElem->SetAttribute("DomainID",DomainID);
		pDomainElem->SetAttribute("RecordNumber",RecordNum);
		pDomainElem->SetAttribute("DomainGrade",sub_tag->FirstChildElement("grade")->GetText());
		pDomainElem->SetAttribute("DomainStatus",sub_tag->FirstChildElement("status")->GetText());

		//获取域名下的记录
		TiXmlDocument RecordList=DNSpod.RecordList(DomainID);
		TiXmlHandle hRecordRoot(&RecordList);
		TiXmlElement *pRecords;
		pRecords=hRecordRoot.FirstChild("dnspod").FirstChild("records").ToElement();
		if (pRecords)
		{
			for(const TiXmlElement *sub_record =pRecords->FirstChildElement(); sub_record; sub_record = sub_record->NextSiblingElement())
			{
				//pDomainElem->LinkEndChild(sub_record); //这是错误的
				pDomainElem->LinkEndChild(sub_record->Clone());
			}
		}
		pDomains->LinkEndChild(pDomainElem);
	}
	AllRecordList->LinkEndChild(pDomains);

	return TRUE;
}

void CDDNSpodDlg::OnFileSave()
{
	CFileDialog saveFile(FALSE,_T("*.xml"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,_T("xml文件(*.xml)|*.xml||"));
	if (saveFile.DoModal() == IDOK)
	{
		CStringA FileName(saveFile.GetPathName());
		if (GetRecordList(m_conf.User,m_conf.Password))
			AllRecordList->SaveFile((LPCSTR)FileName);
		else
			::AfxMessageBox(_T("获取记录失败!"));
	}
}

void CDDNSpodDlg::OnTraySave()
{
	CFileDialog saveFile(FALSE,_T("*.xml"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,_T("xml文件(*.xml)|*.xml||"));
	if (saveFile.DoModal() == IDOK)
	{
		CStringA FileName(saveFile.GetPathName());
		if (GetRecordList(m_conf.User,m_conf.Password))
			AllRecordList->SaveFile((LPCSTR)FileName);
		else
			::AfxMessageBox(_T("获取记录失败!"));
	}
}

void CDDNSpodDlg::OnBnClickedCheck()
{
	m_bUsingDefault=!m_bUsingDefault;
	if (m_bUsingDefault==TRUE)
	{
		GetDlgItem(IDC_EDIT_USER)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_USER)->SetWindowTextW(m_conf.User);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowTextW(m_Password);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(0);
	}
	else
	{
		GetDlgItem(IDC_EDIT_USER)->EnableWindow();
		GetDlgItem(IDC_EDIT_USER)->SetWindowTextW(m_conf.User);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow();
		GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowTextW(m_Password);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();
	}
}

void CDDNSpodDlg::OnBnClickedButtonSave()
{
	GetDlgItem(IDC_EDIT_USER)->GetWindowTextW(m_User);
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowTextW(m_Password);
	m_conf.User=m_User;
	m_conf.Password=m_Password;
	this->SaveConfFile(FALSE);
}

void CDDNSpodDlg::OnBnClickedButtonLogin()
{
	GetDlgItem(IDC_EDIT_USER)->GetWindowTextW(m_User);
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowTextW(m_Password);
	
	if (GetRecordList(m_User,m_Password))
	{
		CListDlg dlgList;
		dlgList.DoModal();
	}
	else
		::AfxMessageBox(_T("登录失败或域名为空!"));
}

TiXmlDocument *CDDNSpodDlg::RecordList()
{
	return AllRecordList;
}

void CDDNSpodDlg::OnFileShowip()
{
	GetIP();
	CString tmpStr;
	tmpStr=_T("IPv4:");
	tmpStr+=ip4;
	tmpStr+=_T("\nIPv6:");
	if (ip6!=_T("::1"))
		tmpStr+=ip6;
	else
		tmpStr+=_T("NULL");
	MessageBox(tmpStr,MB_OK);
}

void CDDNSpodDlg::OnTrayShowip()
{
	GetIP();
	CString tmpStr;
	tmpStr=_T("IPv4:");
	tmpStr+=ip4;
	tmpStr+=_T("\nIPv6:");
	if (ip6!=_T("::1"))
		tmpStr+=ip6;
	else
		tmpStr+=_T("NULL");
	MessageBox(tmpStr,MB_OK);
}

void CDDNSpodDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==100)
	{
		if ((m_conf.UpdateType==ONCE)||(m_conf.UpdateType==AUTO))
		{
			GetIP();
			CDNSpodAPI DNSpod(m_conf.URL,m_conf.port,m_conf.User,m_conf.Password,_T("xml"));
			DNSpod.GetAPIVersion();
			if (DNSpod.bLogin())
			{
				//假如用户名和密码都正确，会检查API版本
				if (!DNSpod.APIVerMatch(DNSpodAPIVer))
				{
					AfxMessageBox(_T("DNSpod API版本不匹配,程序当前支持版本为")+DNSpodAPIVer+_T(",请下载新版本\nhttp://code.google.com/p/ddnspod/"));
				}
			}
			DNSpod.SetModeNoChecking(TRUE);  //直接使用配置中的参数更新，不进行DomainID/RecordID检查
			if (m_conf.A_Record_enabled&&m_conf.A_RecordID!=0)
			{
				DNSpod.RecordModify(m_conf.DomainID,m_conf.A_RecordID,m_conf.A_Record,_T("A"),_T("default"),ip4,1,1);
			}
			if (m_conf.AAAA_Record_enabled&&m_conf.AAAA_RecordID!=0)
			{
				DNSpod.RecordModify(m_conf.DomainID,m_conf.AAAA_RecordID,m_conf.AAAA_Record,_T("AAAA"),_T("default"),ip6,1,1);
			}
			//::AfxMessageBox(_T("Refresh Done!"));
		}
		//更新后退出程序
		if (m_conf.UpdateType==ONCE)
		{
			Shell_NotifyIcon(NIM_DELETE,&m_nd);
			SendMessage(WM_CLOSE);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDDNSpodDlg::OnDestroy()
{
	CDialog::OnDestroy();
	KillTimer(100);
}
