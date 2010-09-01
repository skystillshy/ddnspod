//DNSpod API的封装类实现

#pragma once

#include <afxinet.h>
#include "stdafx.h"
#include "tinyxml.h"
#include "DDNSpodAPI.h"

//dnspod支持HTTPS 
//HTTPS通信参考http://support.microsoft.com/kb/168151/

/*
由于DNSpod提供的API要求的参数是domain id，即域名在DNSpod中的序号
因此必须首先获得域名对应的domain id
域名在Dnspod并非唯一性，查找domain id仅依据域名名称是不足够的
record_id同样也是记录对应的id，也需要查找返回的记录列表才能得到
*/

CDNSpodAPI::CDNSpodAPI()
{
	ServerURL=_T("www.dnspod.com");
	nPort=80;
	login_email="";
	login_password="";
	bLoginSuccess=FALSE;
	return_format=_T("xml");
	APIVersion="";
	tmp = new TiXmlDocument();
}

CDNSpodAPI::CDNSpodAPI(CString URL,WORD port,CString account, CString password, CString format)
{
	ServerURL=URL;
	nPort=port;
	login_email=account;
	login_password=password;
	bLoginSuccess=FALSE;
	return_format=_T("xml"); //仅支持xml
	APIVersion="";
	tmp = new TiXmlDocument();
}

CDNSpodAPI::~CDNSpodAPI()
{
	delete tmp;
}

//是否匹配最新版本API，在检查请应当首先调用一次GetAPIVersion()
BOOL CDNSpodAPI::APIVerMatch(CString version)
{
	if (APIVersion==version)
		return TRUE;
	else
		return FALSE;
}

/********获取API版本号*******/
/*
URL: /API/Info.Version
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
响应代码:
-1 登陆失败
1 成功
*/
/***************************/
TiXmlDocument CDNSpodAPI::GetAPIVersion()
{
	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Info.Version");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);

		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;                                  //
	}

	TiXmlHandle hRoot(temp);
	TiXmlElement *pElem;

	pElem=hRoot.FirstChild("dnspod").FirstChild("status").FirstChild("code").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	if (atoi(pElem->GetText())!=1)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	TiXmlElement *pAPIVersionNode;
	pAPIVersionNode=hRoot.FirstChild("dnspod").FirstChild("status").FirstChild("message").ToElement();
	if (!pAPIVersionNode)
	{
		bLoginSuccess=FALSE;
		return *temp;
	}

	APIVersion=CString(pAPIVersionNode->GetText());
	bLoginSuccess=TRUE;
	//temp->SaveFile("result.xml");
	return *temp;
}

/**********创建域名*********/
/*
URL: /API/Domain.Create
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
domain - 域名, 没有 WWW, 如 dnspod.com
响应代码:
-1 登陆失败
1 成功
2 只允许POST方法
3 未知错误
4 域名无效
5 域名已存在
*/
/***************************/
TiXmlDocument CDNSpodAPI::DomainCreate(CString Domain)
{
	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Domain.Create");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	strFormData+=_T("&domain=");
	strFormData+=Domain;
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;                                  //
	}

	TiXmlHandle hRoot(temp);
	TiXmlElement *pElem;

	pElem=hRoot.FirstChild("dnspod").FirstChild("status").FirstChild("code").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	if (atoi(pElem->GetText())!=1)
	{
		bLoginSuccess=FALSE;                           //创建失败
		return *temp;
	}

	bLoginSuccess=TRUE;
	//temp->SaveFile("result.xml");
	return *temp;
}

/********返回域名列表********/
/*
URL: /API/Domain.List
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
grade - 用户分类，共分为Free，Express，Extra，Ultra四类 //注：无需发送该参数
响应代码:
-1 登陆失败
1 成功
2 只允许POST方法
3 未知错误
4 没有域名
*/
/***************************/
TiXmlDocument CDNSpodAPI::DomainList()
{
	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Domain.List");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		return *tmp;                                   //null xml document
	}
	//temp->SaveFile("domain.xml");
	TiXmlHandle hRoot(temp);
	TiXmlElement *pElem;
	/*
	TiXmlHandle hXmlHandle(0);
	pElem=hRoot.FirstChildElement("dnspod").Element(); //<dnspod>
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	hXmlHandle=TiXmlHandle(pElem);
	pElem=hXmlHandle.FirstChild("status").Element();   //<status>
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	hXmlHandle=TiXmlHandle(pElem);
	pElem=hXmlHandle.FirstChild("code").Element();     //<code>
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	if (atoi(pElem->GetText())!=1)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	*/
	pElem=hRoot.FirstChild("dnspod").FirstChild("status").FirstChild("code").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	if (atoi(pElem->GetText())!=1)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}

	bLoginSuccess=TRUE;
	return *temp;
}

/**********删除域名*********/
/*
URL: /API/Domain.Remove
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
domain_id - 域名ID, 如 12345
响应代码:
-1 登陆失败
1 成功
2 只允许POST方法
3 未知错误
4 域名ID错误
5 域名不存在
6 非域名所有者
 */
/***************************/
TiXmlDocument CDNSpodAPI::DomainRemove(int DomainID)
{
	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Domain.Remove");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	strFormData+=_T("&domain_id=");
	CString domianid;
	domianid.Format(_T("%d"),DomainID);
	strFormData+=domianid;
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;                                  //
	}

	TiXmlHandle hRoot(temp);
	TiXmlElement *pElem;

	pElem=hRoot.FirstChild("dnspod").FirstChild("status").FirstChild("code").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	if (atoi(pElem->GetText())!=1)
	{
		bLoginSuccess=FALSE;                           //删除失败
		return *temp;
	}

	bLoginSuccess=TRUE;
	//temp->SaveFile("result.xml");
	return *temp;
}

/********设置域名状态*******/
/*
URL: /API/Domain.Status
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
domain_id - 域名ID, 如 12345
status {enable,disable} - 状态
响应代码:
-1 登陆失败
1 成功
2 只允许POST方法
3 未知错误
4 域名ID错误
5 域名不存在
6 非域名所有者
*/
/***************************/
TiXmlDocument CDNSpodAPI::DomainStatus(int DomainID,BOOL DomainEnabled)
{
	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Domain.Status");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	strFormData+=_T("&domain_id=");
	CString domianid;
	domianid.Format(_T("%d"),DomainID);
	strFormData+=domianid;
	strFormData+=_T("&status=");
	if (DomainEnabled)
		strFormData+=_T("enable");
	else
		strFormData+=_T("disable");
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;                                  //
	}

	TiXmlHandle hRoot(temp);
	TiXmlElement *pElem;

	pElem=hRoot.FirstChild("dnspod").FirstChild("status").FirstChild("code").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	if (atoi(pElem->GetText())!=1)
	{
		bLoginSuccess=FALSE;                           //域名状态设置失败
		return *temp;
	}

	bLoginSuccess=TRUE;
	//temp->SaveFile("result.xml");
	return *temp;
}

/**********创建记录*********/
/*
URL: /API/Record.Create
方法: POST
参数:
 login_email - 用户帐号
 login_password - 用户密码
 format {json,xml} - 返回的数据格式，支持json和xml.
 domain_id - 域名ID, 如 12345
 sub_domain - 主机记录, 如 www
 record_type {'A','CNAME','MX','URL','NS','TXT','AAAA'} - 记录类型
 record_line {'default', 'tel', 'cnc', 'edu', 'cmc', 'foreign'} - 记录线路依次为：默认、电信、网通、教育网、移动、国外（cmc/foreign仅对VIP有效）
 value - 记录值, 如 IP:200.200.200.200, CNAME: cname.dnspod.com., MX: mail.dnspod.com.
 mx {1-20} - MX优先级, 当记录类型是 MX 时有效，范围1-20
 ttl {1-604800} - TTL，范围1-604800
响应代码:
 -1 登陆失败
 1 成功
 2 只允许POST方法
 3 未知错误
 4 域名ID错误
 5 域名不存在
 6 非域名所有者
 7 缺少参数或者参数错误
 8 主机记录错误
 9 记录值错误
 10 不能为@记录设置NS类型
 11 TTL 必须大于 1
 12 MX 必须大于 1
 12 记录重复或者和 CNAME 记录冲突
 */
/***************************/
TiXmlDocument CDNSpodAPI::RecordCreate(int DomainID,CString SubDomain,CString record_type,CString record_line,CString RecordValue,int MX,int TTL)
{
	if (!bLoginSuccess)
		return *tmp;

	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Record.Create");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	strFormData+=_T("&domain_id=");
	CString tmpStr;
	tmpStr.Format(_T("%d"),DomainID);
	strFormData+=tmpStr;
	strFormData+=_T("&sub_domain=");
	strFormData+=SubDomain;
	strFormData+=_T("&record_type=");
	strFormData+=record_type;
	strFormData+=_T("&record_line=");
	strFormData+=record_line;
	strFormData+=_T("&value=");
	strFormData+=RecordValue;
	strFormData+=_T("&mx=");
	tmpStr.Format(_T("%d"),MX);
	strFormData+=tmpStr;
	strFormData+=_T("&ttl=");
	tmpStr.Format(_T("%d"),TTL);
	strFormData+=tmpStr;
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		//return *temp;                                //
	}
	
	return *temp;
}

/********返回域名列表*******/
/*
URL: /API/Record.List
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
domain_id - 域名ID, 如 12345
响应代码:
-1 登陆失败
1 成功
2 只允许POST方法
3 未知错误
4 域名ID错误
5 域名不存在
6 非域名所有者
7 没有记录
 */
/***************************/
TiXmlDocument CDNSpodAPI::RecordList(int DomainID)
{
	if (!bLoginSuccess)
		return *tmp;

	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Record.List");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	strFormData+=_T("&domain_id=");
	CString domainid;
	domainid.Format(_T("%d"),DomainID);
	strFormData+=domainid;
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
		/*
		CFile myfile;
		myfile.Open(_T("record.xml"),CFile::modeCreate|CFile::modeWrite);
		myfile.Write(szServerMsg,dwFileSize);
		myfile.Close();
		*/
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		//return *temp;                                //
	}
	
	//temp->SaveFile("record.xml");
	return *temp;
}

/********修改域名***********/
/*
URL: /API/Record.Modify
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
domain_id - 域名ID, 如 12345
record_id - 记录ID, 如 1234567
sub_domain - 主机记录, 如 www
record_type {'A','CNAME','MX','URL','NS','TXT','AAAA'} - 记录类型
record_line {'default', 'tel', 'cnc', 'edu', 'cmc', 'foreign'} - 记录线路依次为：默认、电信、网通、教育网、移动、国外（cmc/foreign仅对VIP有效）
value - 记录值, 如 IP:200.200.200.200, CNAME: cname.dnspod.com., MX: mail.dnspod.com.
mx {1-20} - MX优先级, 当记录类型是 MX 时有效，范围1-20
ttl {1-604800} - TTL，范围1-604800
响应代码:
-1 登陆失败
1 成功
2 只允许POST方法
3 未知错误
4 域名ID错误
5 域名不存在
6 非域名所有者
7 缺少参数或者参数错误
8 主机记录错误
9 记录值错误
10 不能为@记录设置NS类型
11 TTL 必须大于 1
12 MX 必须大于 1
12 记录重复或者和 CNAME 记录冲突
*/
/***************************/
TiXmlDocument CDNSpodAPI::RecordModify(int DomainID,int RecordID,CString SubDomain,CString record_type,CString record_line,CString RecordValue,int MX, int TTL)
{
	if (!bLoginSuccess)
		return *tmp;

	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Record.Modify");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	strFormData+=_T("&domain_id=");
	CString tmpStr;
	tmpStr.Format(_T("%d"),DomainID);
	strFormData+=tmpStr;
	strFormData+=_T("&record_id=");
	tmpStr.Format(_T("%d"),RecordID);
	strFormData+=tmpStr;
	strFormData+=_T("&sub_domain=");
	strFormData+=SubDomain;
	strFormData+=_T("&record_type=");
	strFormData+=record_type;
	strFormData+=_T("&record_line=");
	strFormData+=record_line;
	strFormData+=_T("&value=");
	strFormData+=RecordValue;
	strFormData+=_T("&mx=");
	tmpStr.Format(_T("%d"),MX);
	strFormData+=tmpStr;
	strFormData+=_T("&ttl=");
	tmpStr.Format(_T("%d"),TTL);
	strFormData+=tmpStr;
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		//return *temp;                                //
	}
	
	return *temp;
}

/********删除记录***********/
/*
URL: /API/Record.Remove
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
domain_id - 域名ID, 如 12345
record_id - 记录ID, 如 1234567
响应代码:
-1 登陆失败
1 成功
2 只允许POST方法
3 未知错误
4 缺少参数或者参数错误
5 域名不存在
6 非域名所有者
7 记录不存在
*/
/***************************/
TiXmlDocument CDNSpodAPI::RecordRemove(int DomainID, int RecordID)
{
	if (!bLoginSuccess)
		return *tmp;

	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Record.Remove");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	strFormData+=_T("&domain_id=");
	CString id;
	id.Format(_T("%d"),DomainID);
	strFormData+=id;
	strFormData+=_T("&record_id=");
	id.Format(_T("%d"),RecordID);
	strFormData+=id;
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;                                  //
	}

	TiXmlHandle hRoot(temp);
	TiXmlElement *pElem;

	pElem=hRoot.FirstChild("dnspod").FirstChild("status").FirstChild("code").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	if (atoi(pElem->GetText())!=1)
	{
		bLoginSuccess=FALSE;                           //删除失败
		return *temp;
	}

	//temp->SaveFile("result.xml");
	return *temp;
}

/********设置记录状态*******/
/*
URL: /API/Record.Status
方法: POST
参数:
login_email - 用户帐号
login_password - 用户密码
format {json,xml} - 返回的数据格式，支持json和xml.
domain_id - 域名ID, 如 12345
record_id - 记录ID, 如 1234567
status {enable,disable} - 状态
响应代码:
-1 登陆失败
1 成功
2 只允许POST方法
3 未知错误
4 缺少参数或者参数错误
5 域名不存在
6 非域名所有者
7 记录不存在
*/
/***************************/
TiXmlDocument CDNSpodAPI::RecordStatus(int DomainID, int RecordID,BOOL RecordEnabled)
{
	if (!bLoginSuccess)
		return *tmp;

	CInternetSession session(_T("MySession"));
	CHttpConnection *pServer=NULL;
	CHttpFile *pFile=NULL;
	DWORD retcode;
	char szServerMsg[102400]; //返回数据缓冲区
	DWORD dwFileSize=0;
	CString paraStr;
	paraStr=_T("/API/Record.Status");
	CString strFormData;
	strFormData=_T("login_email=");
	strFormData+=login_email;
	strFormData+=_T("&login_password=");
	strFormData+=login_password;
	strFormData+=_T("&format=");
	strFormData+=return_format;
	strFormData+=_T("&domain_id=");
	CString id;
	id.Format(_T("%d"),DomainID);
	strFormData+=id;
	strFormData+=_T("&record_id=");
	id.Format(_T("%d"),RecordID);
	strFormData+=id;
	strFormData+=_T("&status=");
	if (RecordEnabled)
		strFormData+=_T("enable");
	else
		strFormData+=_T("disable");
	//unicode -> ansi
	CStringA strFormDataA(strFormData);
	//::AfxMessageBox(strFormData);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	try
	{
		pServer=session.GetHttpConnection(ServerURL,nPort,login_email,login_password);
		pFile=pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,paraStr,NULL,1,NULL,NULL,INTERNET_FLAG_EXISTING_CONNECT);
		pFile->AddRequestHeaders(strHeaders);
		pFile->AddRequestHeaders(_T("User-Agent: DDNSpod 0.4"));
		pFile->SendRequest(strHeaders,(LPVOID)(LPCSTR)strFormDataA,strFormDataA.GetLength()); //发送表单内容
		pFile->QueryInfoStatusCode(retcode);
		
		UINT nSize=pFile->Read(szServerMsg,1024);
		while (nSize>0)
		{
			dwFileSize +=nSize;
			nSize=pFile->Read(szServerMsg+dwFileSize,1024);
		}
		szServerMsg[dwFileSize]='\0';
		//::AfxMessageBox(CString(szServerMsg));
	}
	catch (CInternetException *e)
	{
	}
	pFile->Close();
	pServer->Close();
	delete pFile;
	delete pServer;

	TiXmlDocument *temp = new TiXmlDocument;
	//解析返回结果
	temp->Parse(szServerMsg,NULL,TIXML_ENCODING_UTF8);
	if (temp->Error())
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;                                  //
	}

	TiXmlHandle hRoot(temp);
	TiXmlElement *pElem;

	pElem=hRoot.FirstChild("dnspod").FirstChild("status").FirstChild("code").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return *temp;
	}
	if (atoi(pElem->GetText())!=1)
	{
		bLoginSuccess=FALSE;                           //记录状态设置失败
		return *temp;
	}

	//temp->SaveFile("result.xml");
	return *temp;
}

//返回域名的ID
int CDNSpodAPI::GetDomainID(TiXmlDocument domainlist, CString DomainValue)
{
	if (!bLoginSuccess)
		return 0;
	
	//解析domianlist
	TiXmlHandle hRoot(&domainlist);
	TiXmlElement *pElem;
	pElem=hRoot.FirstChild("dnspod").FirstChild("domains").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return 0;
	}
	
	//存在域名
	TiXmlElement *pSubElem;
	pSubElem=pElem->FirstChildElement("domain");
	if (!pSubElem)
	{
		return 0;
	}
	//查找
	for(const TiXmlElement *sub_tag =pElem->FirstChildElement(); sub_tag; sub_tag = sub_tag->NextSiblingElement())
	{
		CString DomainName(sub_tag->FirstChildElement("name")->GetText());
		if (DomainName==DomainValue)
			return atoi(sub_tag->FirstChildElement("id")->GetText());
	} 

	return 0;
}

//返回记录的ID
int CDNSpodAPI::GetRecordID(TiXmlDocument recordlist, CString RecordValue,CString RecordType)
{
	if (!bLoginSuccess)
		return 0;
	
	//解析recordlist
	TiXmlHandle hRoot(&recordlist);
	TiXmlElement *pElem;
	pElem=hRoot.FirstChild("dnspod").FirstChild("records").ToElement();
	if (!pElem)
	{
		bLoginSuccess=FALSE;                           //login fail
		return 0;
	}
	
	//存在域名
	TiXmlElement *pSubElem;
	pSubElem=pElem->FirstChildElement("record");
	if (!pSubElem)
	{
		return 0;
	}
	//查找
	for(const TiXmlElement *sub_tag =pElem->FirstChildElement(); sub_tag; sub_tag = sub_tag->NextSiblingElement())
	{
		CString RecordName(sub_tag->FirstChildElement("name")->GetText());
		if (RecordName==RecordValue)
		{
			CString recordtype(sub_tag->FirstChildElement("type")->GetText()); //记录类型，但单凭记录类型是不足够筛选出记录ID的
			if (recordtype==RecordType)
			{
				return atoi(sub_tag->FirstChildElement("id")->GetText());
			}
		}
	} 

	return 0;
}

BOOL CDNSpodAPI::bLogin()
{
	return bLoginSuccess;
}

void CDNSpodAPI::SetModeNoChecking(BOOL bNoChecking)
{
	bLoginSuccess=bNoChecking;
}