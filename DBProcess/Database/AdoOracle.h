#pragma  once

#include "Ado.h"
#include "AdoCommand.h"
#include "AdoRecordSet.h"
#include "windows.h"
#include "RWLockFavorWriters.h"

#define  NULL_THROW(p)  {if(!(p))throw (0);}
#define  OUT 
/*
  ���ݿ��࣬ʹ�õ�ʱ���ж��·���ֵ�ɣ����к���ʧ�ܶ��������쳣
*/
class CAdoOracle
{
private:
	CString m_strIP;
	CString m_strDBName;
	CString m_strDBUser;
	CString m_strDBPwd;
	CString m_strCon;
	HANDLE m_hCheck;
	HANDLE m_hEvent;
	int    m_nCheckTime;
	RWLockFavorWriters m_RWLock;
	CAdoConnection m_con;

	bool checkDB();
	bool reConDB();
	static DWORD WINAPI checkThread(LPVOID lp);
public:
	CAdoOracle();
	~CAdoOracle();
	CString GetLastErrorText();
    
    CAdoConnection* GetAdoConnection(){return &m_con;};
    
	void KeepAlive(int nCheckTime = 3000);

	bool Open(const CString& strDBServerIP, const CString& strDBName, 
		      const CString& strDBUser, const CString& strDBPwd);

	void GetDBInfo(CString& OUT strDBServerIP, CString& OUT strDBName, CString& OUT strDBUser,CString& OUT strDBPwd);

	void Close();

	bool ExcuteNoResult(const CString& strSql);//��ͨ����ɾ����

	bool ExcuteMutiSql(const vector<CString> strSqlVec);

	bool ExcuteQuery(const CString& strSql , OUT CAdoRecordSet& rsOut);//��ѯ����

	bool ExcuteSalary(const CString& strSql, OUT int& nCount);//��ȡ���� select count() ...

	bool CheckInfoExsist(const CString& strSql, OUT bool& bExistOut);

	bool GetTableRowNum(const CString& tableName, OUT int& nCount);

	bool GetQueryCount(const CString& strSql, const CString& rowName, OUT int& nCount);

	bool GetSequenceNextValue(const CString& strSequenceName, OUT int& nSeqVal);//��������������ȡ��һ������
};



