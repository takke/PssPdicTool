// PssPdicTool.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "../lib/hdde.h"
#include "../lib/pdic_util.h"

static HDDEDATA CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2,
	HDDEDATA hdata, DWORD dwData1, DWORD dwData2)
{
	wprintf(L"DdeCallback[%u]\n", uType);

	return (HDDEDATA)NULL;
}

int main()
{
	HDdeManager hdm(DdeCallback);
	PdicSearchManager psm(hdm);
	if (!psm.init_pdicw()) {
		wprintf(L"error[%s]\n", psm.get_error_reason());
		return -1;
	}

	LPCTSTR s = L"être";
	wprintf(L"psm.findw[%s]", s);
	if (!psm.findw(s)) {
		wprintf(L" -> not found\n");
	}
	else {
		std_xstring strAnswer, strQuestion, strDesc;
		psm.get_wordw(strAnswer);
		psm.get_japaw(strQuestion);
		psm.get_expw(strDesc);

		wprintf(L" -> found [%s][%s][%s]\n", strAnswer.c_str(), strQuestion.c_str(), strDesc.c_str());
	}


    return 0;
}

