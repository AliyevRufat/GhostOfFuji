#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	//...
	if (pBinReader->Read<char>() != 'B' || pBinReader->Read<char>() != 'M' || pBinReader->Read<char>() != 'F')
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Not a valid .fnt font!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}
	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	//...
	int versionNumber = (int)pBinReader->Read<char>();
	const int currentVersion = 3;
	if (versionNumber < currentVersion)
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}
	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	//Retrieve the FontName [SpriteFont::m_FontName]
	//...

	//read block id and size
	int blockId = (int)pBinReader->Read<char>();
	int blockSize = pBinReader->Read<int>();
	//if wrong block return nullptr
	if (blockId != 1)
	{
		Logger::LogFormat(LogLevel::Error, L"BlockId mismatach!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}
	//set font size and move binreader pos
	pSpriteFont->m_FontSize = (int)pBinReader->Read<char>();
	pBinReader->MoveBufferPosition(13);
	//get and set font name
	std::wstring fontName = {};
	const int blockOffset = 15;
	for (int i = 0; i < blockSize - blockOffset; i++)
	{
		fontName += pBinReader->Read<char>();
	}
	pSpriteFont->m_FontName = fontName;
	//terminating 0 char
	pBinReader->Read<char>();

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	//Retrieve PageCount
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	//Advance to Block2 (Move Reader)
	//...

	//read block id and size
	blockId = (int)pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	//if wrong block return nullptr
	if (blockId != 2)
	{
		Logger::LogFormat(LogLevel::Error, L"BlockId mismatach!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}
	//get texture width ,height and pagecount
	pBinReader->MoveBufferPosition(4);
	int width = pBinReader->Read<UINT16>();
	int height = pBinReader->Read<UINT16>();
	pSpriteFont->m_TextureWidth = width;
	pSpriteFont->m_TextureHeight = height;
	int pageCount = pBinReader->Read<UINT16>();

	if (pageCount > 1)
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}
	//moving to the end of the block
	pBinReader->MoveBufferPosition(5);

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the PageName (store Local)
	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	//...

	//read block id and size
	blockId = (int)pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	//if wrong block return nullptr
	if (blockId != 3)
	{
		Logger::LogFormat(LogLevel::Error, L"BlockId mismatach!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}
	//get the pagename
	std::wstring pageName = {};
	for (int i = 0; i < blockSize; i++)
	{
		pageName += pBinReader->Read<char>();
	}
	//if pagename empty
	if (pageName == L"")
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}
	//texture filepath
	const int offset = 3;//the . and / at the start of the assetFile
	std::wstring newFilePath;
	newFilePath = assetFile.c_str();
	newFilePath = newFilePath.substr(0, newFilePath.length() - pageName.length() + offset);
	//
	newFilePath += pageName;
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(newFilePath);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Character Count (see documentation)
	//Retrieve Every Character, For every Character:
	//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
	//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
	//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
	//> Set IsValid to true [FontMetric::IsValid]
	//> Set Character (CharacterId) [FontMetric::Character]
	//> Retrieve Xposition (store Local)
	//> Retrieve Yposition (store Local)
	//> Retrieve & Set Width [FontMetric::Width]
	//> Retrieve & Set Height [FontMetric::Height]
	//> Retrieve & Set OffsetX [FontMetric::OffsetX]
	//> Retrieve & Set OffsetY [FontMetric::OffsetY]
	//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
	//> Retrieve & Set Page [FontMetric::Page]
	//> Retrieve Channel (BITFIELD!!!)
	//	> See documentation for BitField meaning [FontMetrix::Channel]
	//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
	//...

	//read block id and size
	blockId = (int)pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	//if wrong block return nullptr
	if (blockId != 4)
	{
		Logger::LogFormat(LogLevel::Error, L"BlockId mismatach!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//retrieve stuff
	const int charInfoCount = 20;
	int charCount = blockSize / charInfoCount;
	wchar_t  characterId = {};

	for (int i = 0; i < charCount; i++)
	{
		characterId = wchar_t(pBinReader->Read<UINT32>());
		//pBinReader->MoveBufferPosition(2);//in case of UINT16

		if (!pSpriteFont->IsCharValid(characterId))
		{
			Logger::LogFormat(LogLevel::Info, i + L" character ID is not valid!\nPath: \'%s\'", assetFile.c_str());
			continue;
		}

		FontMetric& metric = pSpriteFont->GetMetric(characterId);
		metric.IsValid = true;
		metric.Character = characterId;

		UINT16 posX = pBinReader->Read<UINT16>();
		UINT16 posY = pBinReader->Read<UINT16>();
		metric.Width = pBinReader->Read<UINT16>();
		metric.Height = pBinReader->Read<UINT16>();
		metric.OffsetX = pBinReader->Read<UINT16>();
		metric.OffsetY = pBinReader->Read<UINT16>();
		metric.AdvanceX = pBinReader->Read<UINT16>();
		metric.Page = pBinReader->Read<unsigned char>();
		UINT8 channel = pBinReader->Read<UINT8>();
		//metric.Channel = channel;

		switch (channel)
		{
		case 1: metric.Channel = 2; break;
		case 2: metric.Channel = 1; break;
		case 3: metric.Channel = 0; break;
		case 4: metric.Channel = 0; break;
		}

		metric.TexCoord = DirectX::XMFLOAT2(float(posX) / float(pSpriteFont->GetTextureWidth()), float(posY) / float(pSpriteFont->GetTextureHeight()));
	}

	//DONE :)

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}