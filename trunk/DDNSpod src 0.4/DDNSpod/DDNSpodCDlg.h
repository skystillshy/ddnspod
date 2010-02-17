#pragma once

#include "HyperLink.h"
#include "DDNSpodDlg.h"

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CImage m_png;
	CHyperLink  m_link;

// 实现
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//用CImage加载：流或文件
	HGLOBAL        hGlobal = NULL;
	HRSRC          hSource = NULL;
	int            nSize   = 0;

	hSource = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_PNG1), _T("PNG"));
	if (hSource == NULL)
		return TRUE;
	hGlobal = LoadResource(AfxGetResourceHandle(), hSource);
	if (hGlobal == NULL)
	{
		FreeResource(hGlobal);
		return TRUE;
	}
	IStream* pStream = NULL;

	nSize = (UINT)SizeofResource(AfxGetResourceHandle(), hSource);
	HGLOBAL hGlobal2 = GlobalAlloc(GMEM_MOVEABLE, nSize);
	if(hGlobal2 == NULL)
	{
		FreeResource(hGlobal);
		return TRUE;
	}

	void* pData = GlobalLock(hGlobal2);
	memcpy(pData, (void *)hGlobal, nSize);
	GlobalUnlock(hGlobal2);
	if(CreateStreamOnHGlobal(hGlobal2, TRUE, &pStream) == S_OK)
	{
		m_png.Load(pStream);
		pStream->Release();
	}
	FreeResource(hGlobal2); 
	FreeResource(hGlobal);

	//m_png.Load(_T("res\\DDNSpod.png")); 

	m_link.Attach(GetDlgItem(IDC_STATIC_LINK)->GetSafeHwnd());
	//m_link.SetLink(_T("http://code.google.com/p/ddnspod/"),_T("http://code.google.com/p/ddnspod/"),TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAboutDlg::OnPaint()
{
	CPaintDC dc(this);
	if (m_png.IsNull())
		return;

	for(int i = 0; i < m_png.GetWidth(); ++i)
    {
		for(int j = 0; j < m_png.GetHeight(); ++j)
		{
			unsigned char* pucColor = (unsigned char *)m_png.GetPixelAddress(i , j);
			pucColor[0] = pucColor[0] * pucColor[3] / 255;
			pucColor[1] = pucColor[1] * pucColor[3] / 255;
			pucColor[2] = pucColor[2] * pucColor[3] / 255;
		}
	}
	m_png.AlphaBlend(dc.GetSafeHdc(),8,8); //透明显示
	//m_png.Draw(dc.GetSafeHdc(),10,10,48,48);
}

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//配置对话框
class CSettingDlg : public CDialog
{
public:
	CSettingDlg();
	// 对话框数据
	enum { IDD = IDD_DIALOG_SETTING };

public:
	CConfig m_dlgconf;
	int m_int;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CSettingDlg::CSettingDlg() : CDialog(CSettingDlg::IDD)
,m_int(0)
{
	CDDNSpodDlg *pMainWnd = (CDDNSpodDlg*)::AfxGetApp()->GetMainWnd(); 
	m_dlgconf=pMainWnd->GetConfig();
	if (m_dlgconf.UpdateType==NOTAUTO)
		m_int=0;
	else if (m_dlgconf.UpdateType==AUTO)
		m_int=1;
	else
		m_int=2;
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SETTING_USER, m_dlgconf.User);
	DDX_Text(pDX, IDC_SETTING_PASSWORD,m_dlgconf.Password);
	DDX_Text(pDX, IDC_SETTING_DOMAIN,m_dlgconf.Domain);
	DDX_Text(pDX, IDC_SETTING_A,m_dlgconf.A_Record);
	DDX_Text(pDX, IDC_SETTING_AAAA,m_dlgconf.AAAA_Record);
	DDX_Check(pDX, IDC_SETTING_CHECKA, m_dlgconf.A_Record_enabled);
	DDX_Check(pDX, IDC_SETTING_CHECKAAAA, m_dlgconf.AAAA_Record_enabled);
	DDX_Check(pDX, IDC_SETTING_AUTOSTART, m_dlgconf.AutoStart);
	DDX_Radio(pDX, IDC_SETTING_NOTAUTO, m_int);
}

BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
END_MESSAGE_MAP()

//域名记录树状列表对话框
class CListDlg : public CDialog
{
public:
	CListDlg();
	// 对话框数据
	enum { IDD = IDD_DIALOG_LIST };

protected:
	TiXmlDocument *m_pRecordList;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
public:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonSave();
};

CListDlg::CListDlg() : CDialog(CListDlg::IDD)
{
	m_pRecordList=NULL;
}

BOOL CListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//CTreeCtrl
	CTreeCtrl *tree;
	tree=(CTreeCtrl *)GetDlgItem(IDC_TREE);
	tree->ModifyStyle(0,TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES);
	HTREEITEM hTreeRoot;
	hTreeRoot=tree->InsertItem(_T("Domains"));

	CDDNSpodDlg *pMainWnd = (CDDNSpodDlg*)::AfxGetApp()->GetMainWnd(); 
	//取得RecordList
	m_pRecordList=pMainWnd->RecordList();
	if (m_pRecordList)
	{
		TiXmlHandle hRoot(m_pRecordList);
		TiXmlElement *pElem;
		pElem=hRoot.FirstChild("Domains").FirstChild("Domain").ToElement();
		if (pElem)
		{
			for (const TiXmlElement *sub_tag =pElem; sub_tag; sub_tag = sub_tag->NextSiblingElement())
			{
				CString ItemStr(sub_tag->Attribute("DomainName"));
				CString DomainID(sub_tag->Attribute("DomainID"));
				CString DomainStatus(sub_tag->Attribute("DomainStatus"));
				CString DomainGrade(sub_tag->Attribute("DomainGrade"));
				ItemStr+=_T(" ,DomainID=");
				ItemStr+=DomainID;
				ItemStr+=_T(" ,DomainStatus=");
				ItemStr+=DomainStatus;
				ItemStr+=_T(" ,DomainGrade=");
				ItemStr+=DomainGrade;
				HTREEITEM hTreeChild1;
				hTreeChild1=tree->InsertItem(ItemStr,hTreeRoot);
				for (const TiXmlElement *sub_record =sub_tag->FirstChildElement(); sub_record; sub_record = sub_record->NextSiblingElement())
				{
					CString str(sub_record->FirstChildElement("name")->GetText());
					HTREEITEM hTreeChild2;
					hTreeChild2=tree->InsertItem(str,hTreeChild1);
					str=_T("RecordID:");
					str+=CString(sub_record->FirstChildElement("id")->GetText());
					tree->InsertItem(str,hTreeChild2);
					str=_T("Value:");
					str+=CString(sub_record->FirstChildElement("value")->GetText());
					tree->InsertItem(str,hTreeChild2);
					str=_T("Line:");
					str+=CString(sub_record->FirstChildElement("line")->GetText());
					tree->InsertItem(str,hTreeChild2);
					str=_T("Type:");
					str+=CString(sub_record->FirstChildElement("type")->GetText());
					tree->InsertItem(str,hTreeChild2);
					str=_T("TTL:");
					str+=CString(sub_record->FirstChildElement("ttl")->GetText());
					tree->InsertItem(str,hTreeChild2);
					str=_T("MX:");
					str+=CString(sub_record->FirstChildElement("mx")->GetText());
					tree->InsertItem(str,hTreeChild2);
					str=_T("Enabled:");
					str+=CString(sub_record->FirstChildElement("enabled")->GetText());
					tree->InsertItem(str,hTreeChild2);
				}
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CListDlg, CDialog)
	ON_BN_CLICKED(IDC_LIST_SAVE, &CListDlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()

void CListDlg::OnBnClickedButtonSave()
{
	CFileDialog saveFile(FALSE,_T("*.xml"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,_T("xml文件(*.xml)|*.xml||"));
	if (saveFile.DoModal() == IDOK)
	{
		CStringA FileName(saveFile.GetPathName());
		if (m_pRecordList)
			m_pRecordList->SaveFile(FileName);
	}
}