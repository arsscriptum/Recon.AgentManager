#pragma once
#include "pch.h"
#include "Misc.h"
#include "AsymmetricCrypto.h"

#include "Include/aes.h"  
#include "Include/chacha.h"
#include "Include/hc128.h"
#include "Include/hc256.h"

#include "Include/hex.h"            // StreamTransformationFilter  
#include "Include/modes.h"          // CFB_Mode��
#include "Include/osrng.h"			// AutoSeededRandomPool


using namespace std;
using namespace CryptoPP;


#define MAX_KEY_LEN				512
#define MAX_IV_LEN				512


enum CRYPTO_ALG { 
	// ������
	PLAIN_TEXT, 

	// ChaChaϵ��
	CHACHA_20, 
	CHACHA_12,
	CHACHA_8,
	CHACHA_TLS,

	// HCϵ��
	HC_256,
	HC_128,

	// AESϵ��
	AES_ECB_256, 
	AES_ECB_192,
	AES_ECB_128,
	AES_CBC_256, 
	AES_CBC_192,
	AES_CBC_128,
	AES_CFB_256, 
	AES_CFB_192,
	AES_CFB_128,
	AES_OFB_256, 
	AES_OFB_192,
	AES_OFB_128,
	AES_CTR_256,
	AES_CTR_192,
	AES_CTR_128,
	//AES_CBC_CTS_256,
	//AES_CBC_CTS_192,
	//AES_CBC_CTS_128,
	AES_CFB_FIPS_256,
	AES_CFB_FIPS_192,
	AES_CFB_FIPS_128,

	// 
};


// ************************** ���� **************************

class CEncrypt {
protected:
	PBYTE m_pbKey				= nullptr;
	PBYTE m_pbIv				= nullptr;

	// Encrypt()��ʹ�ô˱�����������ת
	string m_sCipherText;

	// ѡȡ�������㷨
	CRYPTO_ALG m_iCryptoAlg;

	// ������
	StreamTransformationFilter* m_pEncryptor = nullptr;

	// �����㷨�࣬��������Ҫ�õ�
	PVOID m_pEncryption			= nullptr;
	
	// ĳЩ�����㷨���ܻ��õ������������ChaCha_12
	AlgorithmParameters m_AlgParams;

public:
	CEncrypt();
	~CEncrypt();

	BOOL Init(CRYPTO_ALG iCryptoAlg, PBYTE pbKey = nullptr, PBYTE pbIv = nullptr);
	string Encrypt(PBYTE pbPlainText, DWORD dwPlainTextLength);
};


// ************************** ���� **************************

class CDecrypt {
protected:
	PBYTE m_pbKey				= nullptr;
	PBYTE m_pbIv				= nullptr;

	// Decrypt()��ʹ�ô˱�����������ת
	string m_sPlainText;

	// ѡȡ�������㷨
	CRYPTO_ALG m_iCryptoAlg;

	// ������
	StreamTransformationFilter* m_pDecryptor = nullptr;

	// �����㷨�࣬��������Ҫ�õ�
	PVOID m_pDecryption = nullptr;

	// ĳЩ�����㷨���ܻ��õ������������ChaCha_12
	AlgorithmParameters m_AlgParams;

public:
	CDecrypt();
	~CDecrypt();

	BOOL Init(CRYPTO_ALG iCryptoAlg, PBYTE pbKey = nullptr, PBYTE pbIv = nullptr);
	string Decrypt(PBYTE pbCipherText, DWORD dwCipherTextLength);

private:
	// ���"�޷���Ҫ�����չ���ĺ�����ʹ��__try"������
	VOID DecryptBody(PBYTE pbCipherText, DWORD dwCipherTextLength);
};