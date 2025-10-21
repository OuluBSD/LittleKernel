#ifndef _nImage__bmp_h_
#define _nImage__bmp_h_

#if defined(COMPILER_MSC) || defined(COMPILER_GCC)
#pragma pack(push, 1)
#endif

struct BMP_FILEHEADER {
	word    bfType;
	dword   bfSize;
	word    bfReserved1;
	word    bfReserved2;
	dword   bfOffBits;

	void    SwapEndian()
	{
#ifdef CPU_BIG_ENDIAN
		UPP::EndianSwap(bfType);
		UPP::EndianSwap(bfSize);
		UPP::EndianSwap(bfOffBits);
#endif
	}
}
#if defined COMPILER_GCC && !defined CPU_BIG_ENDIAN
__attribute__((packed))
#endif
;

struct BMP_INFOHEADER
{
	dword      biSize;
	int        biWidth;
	int        biHeight;
	word       biPlanes;
	word       biBitCount;
	dword      biCompression;
	dword      biSizeImage;
	int        biXPelsPerMeter;
	int        biYPelsPerMeter;
	dword      biClrUsed;
	dword      biClrImportant;

	void    SwapEndian()
	{
#ifdef CPU_BIG_ENDIAN
		UPP::EndianSwap(biSize);
		UPP::EndianSwap(biWidth);
		UPP::EndianSwap(biHeight);
		UPP::EndianSwap(biPlanes);
		UPP::EndianSwap(biBitCount);
		UPP::EndianSwap(biCompression);
		UPP::EndianSwap(biSizeImage);
		UPP::EndianSwap(biXPelsPerMeter);
		UPP::EndianSwap(biYPelsPerMeter);
		UPP::EndianSwap(biClrUsed);
		UPP::EndianSwap(biClrImportant);
#endif
	}
}
#if defined COMPILER_GCC && !defined CPU_BIG_ENDIAN
__attribute__((packed))
#endif
;

struct BMP_RGB
{
    byte    rgbBlue;
    byte    rgbGreen;
    byte    rgbRed;
    byte    rgbReserved;
};

struct BMP_ICONDIR {
	word           idReserved;   // Reserved (must be 0)
	word           idType;       // Resource Type (1 for icons)
	word           idCount;      // How many images?
}
#ifdef COMPILER_GCC
__attribute__((packed))
#endif
;

struct BMP_ICONDIRENTRY {
	byte        bWidth;          // Width, in pixels, of the image
	byte        bHeight;         // Height, in pixels, of the image
	byte        bColorCount;
	byte        bReserved;
	short       wHotSpotX;
	short       wHotSpotY;
	dword       dwBytesInRes;    // How many bytes in this resource?
	dword       dwImageOffset;   // Where in the file is this image?
}
#ifdef COMPILER_GCC
__attribute__((packed))
#endif
;

#if defined(COMPILER_MSC) || defined(COMPILER_GCC)
#pragma pack(pop)
#endif

struct BMPHeader : public BMP_INFOHEADER
{
	BMP_RGB palette[256];
};

#endif
