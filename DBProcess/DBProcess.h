// DBProcess.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDBProcessApp:
// �йش����ʵ�֣������ DBProcess.cpp
//

class CDBProcessApp : public CWinApp
{
public:
	CDBProcessApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDBProcessApp theApp;