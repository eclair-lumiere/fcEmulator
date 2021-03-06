#pragma once
#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#endif

#include <crtdbg.h>
#include "Famicom/Famicom.h"
#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>
#include "Famicom/ppu/ppu.h"
#include "common/d2d_interface.h"
#include <iostream>


std::shared_ptr<Famicom> temp;
uint32_t palette_data[16];


uint32_t get_pixel(unsigned x, unsigned y, const uint8_t* nt, const uint8_t* bg) {
	// 获取所在名称表
	const unsigned id = (x >> 3) + (y >> 3) * 32;
	const uint32_t name = nt[id];
	// 查找对应图样表
	const uint8_t* nowp0 = bg + name * 16;		//前8字节代表2位中的低位
	const uint8_t* nowp1 = nowp0 + 8;			//后8字节代表2位中的高位
	// Y坐标为平面内偏移						  共8个字节 y为第offset个字节
	const int offset = y & 0x7;
	const uint8_t p0 = nowp0[offset];
	const uint8_t p1 = nowp1[offset];
	/* X坐标为字节内偏移(字节内对应pixel)
	0 - 111		1 - 110		2 - 101		...
	1000 0000	0100 0000	0010 0000   ...
	*/
	const uint8_t shift = (~x) & 0x7;			
	const uint8_t mask = 1 << shift;					 
	/* 计算低二位	
	mask已经算出来是第几位是1了 p0和p1里面数只可能是1或0
	对应像素&运算后也是1或0
	*/
	const uint8_t low = ((p0 & mask) >> shift) | ((p1 & mask) >> shift << 1);
	/* 计算所在属性表
	每个属性表里面有4个tiles
	*/ 
	// 第aid个attr
	const unsigned aid = (x >> 5) + (y >> 5) * 8;
	const uint8_t attr = nt[aid + (32 * 30)];
	// 获取属性表内位偏移
	const uint8_t aoffset = ((x & 0x10) >> 3) | ((y & 0x10) >> 2);
	// 计算高两位
	const uint8_t high = (attr & (3 << aoffset)) >> aoffset << 2;
	// 合并作为颜色
	const uint8_t index = high | low;

	return palette_data[index];
}

// declspec
void expand_line_8(uint8_t p0, uint8_t p1, uint8_t high, uint32_t* output) {
	// 0 - D7
	const uint8_t low0 = ((p0 & (uint8_t)0x80) >> 7) | ((p1 & (uint8_t)0x80) >> 6);
	palette_data[high] = output[0];
	output[0] = palette_data[high | low0];
	// 1 - D6
	const uint8_t low1 = ((p0 & (uint8_t)0x40) >> 6) | ((p1 & (uint8_t)0x40) >> 5);
	palette_data[high] = output[1];
	output[1] = palette_data[high | low1];
	// 2 - D5
	const uint8_t low2 = ((p0 & (uint8_t)0x20) >> 5) | ((p1 & (uint8_t)0x20) >> 4);
	palette_data[high] = output[2];
	output[2] = palette_data[high | low2];
	// 3 - D4
	const uint8_t low3 = ((p0 & (uint8_t)0x10) >> 4) | ((p1 & (uint8_t)0x10) >> 3);
	palette_data[high] = output[3];
	output[3] = palette_data[high | low3];
	// 4 - D3
	const uint8_t low4 = ((p0 & (uint8_t)0x08) >> 3) | ((p1 & (uint8_t)0x08) >> 2);
	palette_data[high] = output[4];
	output[4] = palette_data[high | low4];
	// 5 - D2
	const uint8_t low5 = ((p0 & (uint8_t)0x04) >> 2) | ((p1 & (uint8_t)0x04) >> 1);
	palette_data[high] = output[5];
	output[5] = palette_data[high | low5];
	// 6 - D1
	const uint8_t low6 = ((p0 & (uint8_t)0x02) >> 1) | ((p1 & (uint8_t)0x02) >> 0);
	palette_data[high] = output[6];
	output[6] = palette_data[high | low6];
	// 7 - D0
	const uint8_t low7 = ((p0 & (uint8_t)0x01) >> 0) | ((p1 & (uint8_t)0x01) << 1);
	palette_data[high] = output[7];
	output[7] = palette_data[high | low7];
}

//__declspec(noinline)
//不知道palette_data[high] = output[7];是干嘛的
void expand_line_8_r(uint8_t p0, uint8_t p1, uint8_t high, uint32_t* output) {
	// 7 - D7
	const uint8_t low0 = ((p0 & (uint8_t)0x80) >> 7) | ((p1 & (uint8_t)0x80) >> 6);
	palette_data[high] = output[7];
	output[7] = palette_data[high | low0];
	// 6 - D6
	const uint8_t low1 = ((p0 & (uint8_t)0x40) >> 6) | ((p1 & (uint8_t)0x40) >> 5);
	palette_data[high] = output[6];
	output[6] = palette_data[high | low1];
	// 5 - D5
	const uint8_t low2 = ((p0 & (uint8_t)0x20) >> 5) | ((p1 & (uint8_t)0x20) >> 4);
	palette_data[high] = output[5];
	output[5] = palette_data[high | low2];
	// 4 - D4
	const uint8_t low3 = ((p0 & (uint8_t)0x10) >> 4) | ((p1 & (uint8_t)0x10) >> 3);
	palette_data[high] = output[4];
	output[4] = palette_data[high | low3];
	// 3 - D3
	const uint8_t low4 = ((p0 & (uint8_t)0x08) >> 3) | ((p1 & (uint8_t)0x08) >> 2);
	palette_data[high] = output[3];
	output[3] = palette_data[high | low4];
	// 2 - D2
	const uint8_t low5 = ((p0 & (uint8_t)0x04) >> 2) | ((p1 & (uint8_t)0x04) >> 1);
	palette_data[high] = output[2];
	output[2] = palette_data[high | low5];
	// 1 - D1
	const uint8_t low6 = ((p0 & (uint8_t)0x02) >> 1) | ((p1 & (uint8_t)0x02) >> 0);
	palette_data[high] = output[1];
	output[1] = palette_data[high | low6];
	// 0 - D0
	const uint8_t low7 = ((p0 & (uint8_t)0x01) >> 0) | ((p1 & (uint8_t)0x01) << 1);
	palette_data[high] = output[0];
	output[0] = palette_data[high | low7];
}

/// <summary>
/// 主渲染
/// </summary>
/// <param name="rgba">The RGBA.</param>
void main_render(void* rgba,Famicom& famicom)noexcept {
	uint32_t* data = (uint32_t*)rgba;

	//for (int i = 0; i != 10000; ++i)
	//{
	//	//famicom.BeforeExecute();
	//	famicom.cpu_.ExecuteOnce();
	//}
	//famicom.DoVblank();

	//// 生成调色板颜色
	//{
	//	for (int i = 0; i != 16; ++i) {
	//		palette_data_[i] =stdPalette[famicom.ppu_.spindexes_[i]].data_;
	//	}
	//	palette_data_[4 * 1] = palette_data_[0];
	//	palette_data_[4 * 2] = palette_data_[0];
	//	palette_data_[4 * 3] = palette_data_[0];
	//}
	//// 背景
	//const uint8_t* now = famicom.ppu_.banks_[8];
	//const uint8_t* bgp = famicom.ppu_.banks_[
	//	(famicom.ppu_.ctrl_ & PPU2000_BgTabl ? 4 : 0)];
	//for (unsigned i = 0; i != 256 * 240; ++i) {
	//	data_[i] = get_pixel(i & 0xff, i >> 8, now, bgp);
	//}
	//==========================================================
	//step7
	uint8_t buffer[256 * 256];
	famicom.RenderFrameEasy(buffer);
	//temp->sfc_render_frame(buffer);

	//生成调色板数据
	uint32_t palette[32];

	for (int i = 0; i != 32; ++i) {
		palette[i] = stdPalette[famicom.ppu_.spindexes_[i]].data;
	}
	//镜像数据
	palette[4 * 1] = palette[0];
	palette[4 * 2] = palette[0];
	palette[4 * 3] = palette[0];
	palette[4 * 4] = palette[0];
	palette[4 * 5] = palette[0];
	palette[4 * 6] = palette[0];
	palette[4 * 7] = palette[0];

	for (int i = 0; i != 256 * 240; ++i) {
		data[i] = palette[buffer[i] >> 1];
	}
}

void run()
{
	//Famicom* famicom = new Famicom(nullptr);
	std::shared_ptr<Famicom> famicom = Famicom::getInstance(nullptr);
	temp = famicom;

	auto test = famicom->GetRomInfo();
	printf("ROM:PRG-ROM: %d x 16kb	CHR-ROM %d x 8kb	Mapper: %03d\n",
		(int)test.count_prgrom16kb,
		(int)test.count_chrrom_8kb,
		(int)test.mapper_number);

	printf("\n");

	main_cpp(*famicom);
	//delete famicom;

	std::cout << famicom.use_count() << std::endl;
	
	std::cout << famicom.use_count() << std::endl;
}


int main() {
	//_CrtSetBreakAlloc(156);
	run();
	_CrtDumpMemoryLeaks();
	return 0;

}

//=====================================================
// 接收到键盘上的消息就把对应的信息传给states
SFC_EXTERN_C void user_input(int index, unsigned char data) SFC_NOEXCEPT {
	assert(index >= 0 && index < 16);
	temp->cpu_.button_states_[index] = data;
}
//=====================================================
SFC_EXTERN_C int sub_render(void* rgba) SFC_NOEXCEPT {
	return 0;
	auto data = (uint32_t*)rgba;
	//生成调色板颜色
	{
		for (int i = 0; i != 16; ++i) {
			palette_data[i] = stdPalette[temp->ppu_.spindexes_[i + 16]].data;
		}
		palette_data[4 * 1] = palette_data[0];
		palette_data[4 * 2] = palette_data[0];
		palette_data[4 * 3] = palette_data[0];
	}
	//设置为背景色
	for (int i = 0; i != 256 * 240; ++i) {
		data[i] = palette_data[0];
	}
	//精灵图样地址
	//4是从$1000开始
	//0是从$0000开始
	const uint8_t* spp = temp->ppu_.banks_[temp->ppu_.ctrl_ & PPU2000_SpTabl ? 4 : 0];
	

	for (int i = 63; i != -1; --i) {
		const uint8_t* ptr = temp->ppu_.sprites_ + i * 4;
		const uint8_t yy = ptr[0];
		const uint8_t ii = ptr[1];	//Tile索引号(类似于名称表)
		const uint8_t aa = ptr[2];
		const uint8_t xx = ptr[3];
		//要点7：由于+1s, 所以(yy)$EF-$FF的显示不了, 设置为这个值就相当于隐藏
		//256*240的屏幕 0xef=239 就是说y>=239的地方不需要显示 
		//这样的话实际上只有256*239吧
		if (yy >= 0xef)continue;
		//查找对应图样表
		const uint8_t* nowp0 = spp + ii * 16;
		const uint8_t* nowp1 = nowp0 + 8;
		const uint8_t high = (aa & 3) << 2;
		//水平翻转
		//0x40 - 0100 0000
		if (aa & 0x40) {
			for (uint8_t i = 0; i != 8; ++i) {
				expand_line_8_r(nowp0[i], nowp1[i], high, data + xx + (yy + i + 1) * 256);	//（yy+i+1）是因为不能显示在第一行
			}
		}
		else{
			for (uint8_t i = 0; i != 8; ++i) {
				expand_line_8(nowp0[i], nowp1[i], high, data + xx + (yy + i + 1) * 256);
			}
		}
	}

	return 1;
}
SFC_EXTERN_C void qsave() SFC_NOEXCEPT {

}
SFC_EXTERN_C void qload() SFC_NOEXCEPT {

}
