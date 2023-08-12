#include <Misc/HappyMode.h>


bool HappyMode::Check()
{
	BYTE* x = CCINIClass::INI_Rules_FileName.get();
	wchar_t* wx = new wchar_t[255];
	MultiByteToWideChar(CP_ACP, 0, (char*)x, -1, wx, 255);
	MessageBoxW(NULL,
		wx,
		L"调试信息", MB_OK);
	//AES中使用的固定参数是以类AES中定义的enum数据类型出现的，而不是成员函数或变量
	//因此需要用::符号来索引
	std::cout << "AES Parameters: " << std::endl;
	std::cout << "Algorithm name : " << AES::StaticAlgorithmName() << std::endl;

	//Crypto++库中一般用字节数来表示长度，而不是常用的字节数
	std::cout << "Block size : " << AES::BLOCKSIZE * 8 << std::endl;
	std::cout << "Min key length : " << AES::MIN_KEYLENGTH * 8 << std::endl;
	std::cout << "Max key length : " << AES::MAX_KEYLENGTH * 8 << std::endl;

	//AES中只包含一些固定的数据，而加密解密的功能由AESEncryption和AESDecryption来完成
	//加密过程
	AESEncryption aesEncryptor; //加密器 

	unsigned char aesKey[AES::DEFAULT_KEYLENGTH]; //密钥
	unsigned char inBlock[AES::BLOCKSIZE] = "123456789"; //要加密的数据块
	unsigned char outBlock[AES::BLOCKSIZE]; //加密后的密文块
	unsigned char xorBlock[AES::BLOCKSIZE]; //必须设定为全零

	memset(xorBlock, 0, AES::BLOCKSIZE); //置零

	aesEncryptor.SetKey(aesKey, AES::DEFAULT_KEYLENGTH); //设定加密密钥
	aesEncryptor.ProcessAndXorBlock(inBlock, xorBlock, outBlock); //加密

	//以16进制显示加密后的数据
	for (int i = 0; i < 16; i++) {
		std::cout << std::hex << (int)outBlock[i] << " ";
	}
	std::cout << std::endl;

	//解密
	AESDecryption aesDecryptor;
	unsigned char plainText[AES::BLOCKSIZE];

	aesDecryptor.SetKey(aesKey, AES::DEFAULT_KEYLENGTH);
	//细心的朋友注意到这里的函数不是之前在DES中出现过的：ProcessBlock，
	//而是多了一个Xor。其实，ProcessAndXorBlock也有DES版本。用法跟AES版本差不多。
	//笔者分别在两份代码中列出这两个函数，有兴趣的朋友可以自己研究一下有何差异。
	aesDecryptor.ProcessAndXorBlock(outBlock, xorBlock, plainText);


	for (int i = 0; i < 16; i++)
	{
		std::cout << plainText[i];
	}
	std::cout << std::endl;

	return true;
}
