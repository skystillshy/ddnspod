// DDNSpodDlg.h : 头文件
//

#pragma once
#include "tinyxml.h"

enum UPDATETYPE{NOTAUTO=1,AUTO=2,ONCE=3};
typedef struct CConfig_tag
{
	CString User;                          //用户名
	CString Password;                      //密码
	CString URL;                           //DNSpod地址
	WORD port;                             //服务器端口
	CString Domain;                        //域名
	int DomainID;                          //域名ID
	CString A_Record;                      //A记录
	int A_RecordID;                        //A记录ID
	CString AAAA_Record;                   //AAAA记录
	int AAAA_RecordID;                     //AAAA记录ID
	BOOL A_Record_enabled;                 //
	BOOL AAAA_Record_enabled;              //
	BOOL AutoStart;                        //
	UPDATETYPE UpdateType;                 //
}CConfig;

// CDDNSpodDlg 对话框
class CDDNSpodDlg : public CDialog
{
// 构造
public:
	CDDNSpodDlg(CWnd* pParent = NULL);	// 标准构造函数
	enum { IDD = IDD_DDNSPOD_DIALOG };  // 对话框数据

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	BOOL CreateConfFile();
	BOOL LoadFromFile(TiXmlDocument *xmlfile);
	BOOL SaveConfFile(BOOL bRegKey);
	BOOL GetRecordList(CString User,CString Password);

// 实现
protected:
	HICON m_hIcon;
	CMenu m_menu;                //菜单
	CString m_confpath;          //配置文件路径
	NOTIFYICONDATA m_nd;         //托盘
	CConfig m_conf;              //配置
	BOOL m_bConfLoaded;          //
	TiXmlDocument *AllRecordList;//
	CString ip4;                 //
	CString ip6;                 //
	//CDNSpodAPI DNSpod;         //
	BOOL m_bUsingDefault;        //使用缺省帐号
	CString m_User;              //
	CString m_Password;          //
	BOOL bShow;
	CString DNSpodAPIVer;        //DNSpod提供的API版本，本程序当前支持版本1.5

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CConfig GetConfig();
	BOOL GetIP();
	TiXmlDocument *RecordList();
	afx_msg void OnFileExit();
	afx_msg void OnAbout();
	afx_msg void OnFileSetting();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnClickTray(WPARAM wParam,LPARAM lParam); //托盘响应
	afx_msg void OnTrayShowwindow();
	afx_msg void OnTraySetting();
	afx_msg void OnTrayExit();
	afx_msg void OnTrayAbout();
	afx_msg void OnFileRefresh();
	afx_msg void OnFileSave();
	afx_msg void OnTrayRefresh();
	afx_msg void OnTraySave();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnFileShowip();
	afx_msg void OnTrayShowip();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};

