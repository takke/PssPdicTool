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

void doQuery(PdicSearchManager& psm, LPCTSTR s) {

	wprintf(L"psm.findw[%s]", s);
	if (!psm.findw(s)) {
		wprintf(L" -> not found\n\n");
	}
	else {
		std_xstring strAnswer, strQuestion, strDesc;
		if (!psm.get_wordw(strAnswer))		{ wprintf(L"error:get_wordw\n"); }
		if (!psm.get_japaw(strQuestion))	{ wprintf(L"error:get_japaw\n"); }
		if (!psm.get_expw(strDesc))			{ wprintf(L"error:get_expw\n"); }

		wprintf(L" -> found [%s][%s][%s]\n\n", strAnswer.c_str(), strQuestion.c_str(), strDesc.c_str());
	}
}

int main()
{
	setlocale(LC_ALL, "Japanese");

	HDdeManager hdm(DdeCallback);
	PdicSearchManager psm(hdm);
	if (!psm.init_pdicw()) {
		wprintf(L"error[%s]\n", psm.get_error_reason());
		return -1;
	}

	doQuery(psm, L"être");

	doQuery(psm, L"achète");

	doQuery(psm, L"acheter");

    return 0;
}

