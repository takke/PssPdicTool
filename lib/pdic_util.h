#ifndef ___PDIC_UTIL_H_INCLUdED_
#define ___PDIC_UTIL_H_INCLUdED_

#include "hdde.h"
//#include "hreg.h"
#include <atlstr.h>
typedef std::wstring std_xstring;

/// PDIC �̌����x���N���X
class PdicSearchManager
{
	HDdeManager& hdm_;
	std_xstring strErrorMsg_;

public:
	bool bPdicUnicode_;

	PdicSearchManager( HDdeManager& a_hdm ) : hdm_(a_hdm), bPdicUnicode_(false) {}

	/// ���o����̎擾
	bool get_wordw(std_xstring& strAnswer) {
		return hdm_.requestw(L"GetWord", strAnswer);
	}

	/// ���̎擾
	bool get_japaw(std_xstring& strQuestion) {
		return hdm_.requestw(L"GetJapa", strQuestion);
	}

	/// �p��̎擾
	bool get_expw(std_xstring& strDesc) {
		return hdm_.requestw(L"GetExp", strDesc);
	}


	/// ����
	bool findw(LPCTSTR szFind) {
		return hdm_.pokew(L"Find", szFind);
	}
	/// �Q�ڈȍ~�̌���
	bool findw_next() {
		return hdm_.pokew(L"Find", L"");
	}

	/// init_pdic �Ȃǂ̎��s���̌����̎擾
	LPCTSTR get_error_reason() { return strErrorMsg_.c_str(); }

	/**
	 * PDIC �̏���
	 *
	 * @return pdic ���N�����Ă��Ȃ���� false, �N�����Ă��ăI�[�v���ł���� true ��Ԃ��D
	 * @param mode		"Dictionary", "Simulate" �Ȃ�
	 */
	bool init_pdicw(LPCTSTR mode = L"Dictionary")
	{
		if (hdm_.initw(L"PDICU", mode)) {
			bPdicUnicode_ = true;
		}
		else {
			strErrorMsg_ = L"";
			return false;
		}

		// �����I�[�v��
		if (hdm_.pokew(L"Open", L"") == false) {
			strErrorMsg_ = L"PDIC �̎������J���܂���ł���\n";
			return false;
		}

		return true;
	}
};

#endif
