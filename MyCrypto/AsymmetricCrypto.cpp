#include "pch.h"
#include "Misc.h"

#include "Include/rsa.h"
#include "Include/base64.h"
#include "Include/osrng.h"				// AutoSeededRandomPool

using namespace std;
using namespace CryptoPP;


// ����RSA��Կ
void RsaGenerateKey(int iKeyLen, string& sPublicKey, string& sPrivateKey) {
	// α�����������
	AutoSeededRandomPool rng;

	// ����˽Կ
	RSA::PrivateKey rsaPrivateKey;
	rsaPrivateKey.GenerateRandomWithKeySize(rng, iKeyLen);

	// ����˽Կ���ɹ�Կ
	RSA::PublicKey rsaPublicKey(rsaPrivateKey);

	// ��Կ��string��ʽ�洢
	string sPublicKeyTemp, sPrivateKeyTemp;
	rsaPublicKey.Save(StringSink(sPublicKeyTemp).Ref());
	rsaPrivateKey.Save(StringSink(sPrivateKeyTemp).Ref());

	// Base64������Կ
	StringSource ss1(sPublicKeyTemp, true, new Base64Encoder(new StringSink(sPublicKey)));
	StringSource ss2(sPrivateKeyTemp, true, new Base64Encoder(new StringSink(sPrivateKey)));
}


// ��Base64�ַ������ع�Կ��˽Կ
template <typename Key>
const Key LoadKeyFromBase64Str(const std::string& str)
{
	Key key;
	StringSource source(str.c_str(), true, new Base64Decoder);
	key.Load(source);
	return key;
}


string RsaEncryptUsePublicKey(string sPublicKey, string sPlainText) {
	RSA::PublicKey rsaPublicKey = LoadKeyFromBase64Str<RSA::PublicKey>(sPublicKey);
	AutoSeededRandomPool prng;

	// ����
	string sCipherText;
	RSAES_OAEP_SHA_Encryptor Enc(rsaPublicKey);
	StringSource ss1(sPlainText, true,
		new PK_EncryptorFilter(prng, Enc,
			new StringSink(sCipherText)
		) // PK_EncryptorFilter
	); // StringSource

	return sCipherText;
}


string RsaDecryptUsePrivateKey(string sPrivateKey, string sCipherText) {
	RSA::PrivateKey rsaPrivateKey = LoadKeyFromBase64Str<RSA::PrivateKey>(sPrivateKey);
	AutoSeededRandomPool prng;

	// ����
	string sPlainText;
	RSAES_OAEP_SHA_Decryptor Dec(rsaPrivateKey);
	StringSource ss2(sCipherText, true,
		new PK_DecryptorFilter(prng, Dec,
			new StringSink(sPlainText)
		) // PK_DecryptorFilter
	); // StringSource

	return sPlainText;
}


// Crypto++��֧��˽Կ���ܣ���Կ����, �ɼ� https://www.cryptopp.com/wiki/Raw_RSA
// ������ֻ���ֶ�ģ����㣬����˺����İ�ȫ��δ֪��
string RsaEncryptUsePrivateKey(string sPrivateKey, string sPlainText) {


	//RSA::PrivateKey rsaPrivateKey = LoadKeyFromBase64Str<RSA::PrivateKey>(sPrivateKey);
	//AutoSeededRandomPool prng;

	//string sCipherText;
	//RSAES_OAEP_SHA_Encryptor Enc(rsaPrivateKey);
	//StringSource ss1(sPlainText, true,
	//	new PK_EncryptorFilter(prng, Enc,
	//		new StringSink(sCipherText)
	//	) // PK_EncryptorFilter
	//); // StringSource

	//return sCipherText;

	RSA::PrivateKey rsaPrivateKey = LoadKeyFromBase64Str<RSA::PrivateKey>(sPrivateKey);

	const Integer& n = rsaPrivateKey.GetModulus();
	const Integer& d = rsaPrivateKey.GetPrivateExponent();

	Integer m((const byte*)sPlainText.data(), sPlainText.size());
	Integer c = a_exp_b_mod_c(m, d, n);

	size_t req = c.MinEncodedSize();
	string sCipherText;
	sCipherText.resize(req);
	c.Encode((byte*)&sCipherText[0], sCipherText.size());

	return sCipherText;
}


// Crypto++��֧��˽Կ���ܣ���Կ����, �ɼ� https://www.cryptopp.com/wiki/Raw_RSA
// ������ֻ���ֶ�ģ����㣬����˺����İ�ȫ��δ֪��
string RsaDecryptUsePublicKey(string sPublicKey, string sCipherText) {
	//RSA::PublicKey rsaPublicKey = LoadKeyFromBase64Str<RSA::PublicKey>(sPublicKey);
	//AutoSeededRandomPool prng;

	//string sPlainText;
	//RSAES_OAEP_SHA_Decryptor Dec(rsaPublicKey);
	//StringSource ss2(sCipherText, true,
	//	new PK_DecryptorFilter(prng, Dec,
	//		new StringSink(sPlainText)
	//	) // PK_DecryptorFilter
	//); // StringSource

	//return sPlainText;

	RSA::PublicKey rsaPublicKey = LoadKeyFromBase64Str<RSA::PublicKey>(sPublicKey);

	const Integer& n = rsaPublicKey.GetModulus();
	const Integer& e = rsaPublicKey.GetPublicExponent();

	Integer c((const byte*)sCipherText.data(), sCipherText.size());
	Integer m = a_exp_b_mod_c(c, e, n);

	size_t req = m.MinEncodedSize();
	string sPlainText;
	sPlainText.resize(req);
	m.Encode((byte*)&sPlainText[0], sPlainText.size());

	return sPlainText;
}


// ����
VOID RsaTest() {
	// ������Կ
	string sPublicKey, sPrivateKey;
	RsaGenerateKey(3072, sPublicKey, sPrivateKey);

	printf("��Կ��\n");
	cout << sPublicKey;

	printf("˽Կ��\n");
	cout << sPrivateKey;


	// ��Կ���ܣ�˽Կ����
	string plain = "RSA Encryption Use Public Key!", cipher, recovered;

	cipher = RsaEncryptUsePublicKey(sPublicKey, plain);
	printf("���ģ�\n");
	PrintData((PBYTE)cipher.c_str(), cipher.length());

	recovered = RsaDecryptUsePrivateKey(sPrivateKey, cipher);
	printf("�ָ������ģ�\n");
	PrintData((PBYTE)recovered.c_str(), recovered.length());


	// ˽Կ���ܣ���Կ����
	string plain2 = "RSA Encryption Use Private Key!", cipher2, recovered2;

	cipher2 = RsaEncryptUsePrivateKey(sPrivateKey, plain2);
	printf("���ģ�\n");
	PrintData((PBYTE)cipher2.c_str(), cipher2.length());

	recovered2 = RsaDecryptUsePublicKey(sPublicKey, cipher2);
	printf("�ָ������ģ�\n");
	PrintData((PBYTE)recovered2.c_str(), recovered2.length());
}