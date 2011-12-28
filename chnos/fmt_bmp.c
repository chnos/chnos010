
#include "core.h"

//パディングの関係でこんなことになってる
typedef struct DATA_FORMAT_BMP_FILE_HEADER {
	uchar sign[2];
	uchar filesize[4];
	uchar reserved0[2];
	uchar reserved1[2];
	uchar offset_to_data[4];
} DATA_Format_BMP_FileHeader;

//OS/2 Type
typedef struct DATA_FORMAT_BMP_CORE_HEADER {
	uint headersize;
	short xsize;
	short ysize;
	ushort planes;
	ushort bpp;
} DATA_Format_BMP_CoreHeader;

//Windows Type
typedef struct DATA_FORMAT_BMP_INFO_HEADER {
	uint headersize;
	int xsize;
	int ysize;
	ushort planes;
	ushort bpp;
	uint compression_type;	//0:no compression, 1:RLE8 2:RLE4 3:bit field
	uint image_data_size;
	uint pixel_per_meter_x;
	uint pixel_per_meter_y;
	uint colors_in_palette;
	uint index_of_important_color;
} DATA_Format_BMP_InformationHeader;

typedef struct DATA_FORMAT_BMP_RGB24 {
	uchar b;
	uchar g;
	uchar r;
} DATA_Format_BMP_RGB24;

uint Format_BMP_DrawPicture(void *vram, uint xsize, uint x, uint y, uint pxsize, uint pysize, void *bmp)
{
	uchar s[128];
	DATA_Format_BMP_FileHeader *fheader;
	DATA_Format_BMP_InformationHeader *infoheader;
	uint *read32;
	DATA_Format_BMP_RGB24 *rgb24;
	uint ix, iy;

	uint offset_to_data, filesize;

	fheader = (DATA_Format_BMP_FileHeader *)bmp;

	if(fheader->sign[0] != 'B' || fheader->sign[1] != 'M'){
		Drawing_Put_String(vram, xsize, x, y, 0x000000, "BMP:Unknown File Format.");
		return 1;
	}

	read32 = (uint *)(&fheader->filesize[0]);
	filesize = *read32;
	read32 = (uint *)(&fheader->offset_to_data[0]);
	offset_to_data = *read32;

	//snprintf(s, "size:%d offset:%d", sizeof(s), filesize, offset_to_data);
	//Drawing_Put_String(vram, xsize, x, y, 0x000000, s);

	read32 = (uint *)(bmp + 0x0e);

	if(*read32 == 12){
		Drawing_Put_String(vram, xsize, x, y + 16 * 1, 0x000000, "BMP Core Header.(not implemented.)");
		return 2;
	} else if(*read32 == 40){
		//Drawing_Put_String(vram, xsize, x, y + 16 * 1, 0x000000, "BMP INFO Header.");
		infoheader = (DATA_Format_BMP_InformationHeader *)(bmp + 0x0e);
		if(infoheader->ysize > 0){
			//Drawing_Put_String(vram, xsize, x, y + 16 * 2, 0x000000, "Bottom Up.");
		} else{
			Drawing_Put_String(vram, xsize, x, y + 16 * 2, 0x000000, "Top Down (not implemented).");
			return 5;
		}

		if(infoheader->compression_type == 0){
			//Drawing_Put_String(vram, xsize, x, y + 16 * 3, 0x000000, "No compression.");
		} else{
			Drawing_Put_String(vram, xsize, x, y + 16 * 3, 0x000000, "Compressed (not implemented).");
			return 4;
		}

		if(infoheader->bpp == 24){
			//Drawing_Put_String(vram, xsize, x, y + 16 * 4, 0x000000, "24 bpp.");
		} else{
			snprintf(s, "%d bpp(not implemented).", sizeof(s), infoheader->bpp);
			Drawing_Put_String(vram, xsize, x, y + 16 * 4, 0x000000, s);
			return 6;
		}

		//snprintf(s, "xsize:%d ysize:%d", sizeof(s), infoheader->xsize, infoheader->ysize);
		//Drawing_Put_String(vram, xsize, x, y + 16 * 5, 0x000000, s);

		rgb24 = (DATA_Format_BMP_RGB24 *)(bmp + offset_to_data);

		for(iy = infoheader->ysize; iy > 0; iy--){
			for(ix = 0; ix < infoheader->xsize; ix++){
				Drawing_Draw_Point(vram, xsize, x + ix, y + iy - 1, rgb24->r << 16 | rgb24->g << 8 | rgb24->b);
				rgb24 = (DATA_Format_BMP_RGB24 *)((uint)rgb24 + 3);
			}
			rgb24 = (DATA_Format_BMP_RGB24 *)((uint)rgb24 + ((infoheader->xsize + 3) & ~3) - infoheader->xsize);
		}
	} else{
		Drawing_Put_String(vram, xsize, x, y + 16 * 1, 0x000000, "Unknown BMP Header.");
		return 3;
	}

	return 0;
}
