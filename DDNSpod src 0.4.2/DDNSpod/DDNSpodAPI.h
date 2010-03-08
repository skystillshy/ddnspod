class CDNSpodAPI
{
private:
	CString ServerURL;
	INTERNET_PORT nPort;
	CString login_email;
	CString login_password;
	CString return_format;
	BOOL bLoginSuccess;
	CString APIVersion;
	TiXmlDocument *tmp;

public:
	CDNSpodAPI();
	CDNSpodAPI(CString URL,WORD port,CString account,CString password,CString format);
	~CDNSpodAPI();
	TiXmlDocument GetAPIVersion();
	BOOL APIVerMatch(CString version);
	TiXmlDocument DomainCreate(CString Domain);
	TiXmlDocument DomainList();
	TiXmlDocument DomainRemove(int DomainID);
	TiXmlDocument DomainStatus(int DomainID,BOOL DomainEnabled);
	TiXmlDocument RecordCreate(int DomainID,CString SubDomain,CString record_type,CString record_line,CString RecordValue,int MX,int TTL);
	TiXmlDocument RecordList(int DomainID);
	TiXmlDocument RecordModify(int DomainID,int RecordID,CString SubDomain,CString record_type,CString record_line,CString RecordValue,int MX, int TTL);
	TiXmlDocument RecordRemove(int DomainID, int RecordID);
	TiXmlDocument RecordStatus(int DomainID, int RecordID,BOOL RecordEnabled);
	int GetDomainID(TiXmlDocument domainlist,CString DomainValue);
	int GetRecordID(TiXmlDocument recordlist,CString RecordValue,CString RecordType);
	BOOL bLogin(void);
	void SetModeNoChecking(BOOL bNoChecking=FALSE);
};