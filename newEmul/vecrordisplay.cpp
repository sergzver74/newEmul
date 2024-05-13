#include "vecrordisplay.h"

void VectorDisplay::SetPalette()
{

	uint8_t RG[8] = { 0, 19, 26, 34, 41, 48, 55, 63 };
	uint8_t BL[4] = { 0, 31, 47, 63 };
	uint8_t B, G, R, k;

	for (B = 0; B < 4; B++)
	{
		for (G = 0; G < 8; G++)
		{
			for (R = 0; R < 8; R++)
			{
				k = B * 64 + G * 8 + R;
				PallTable[k].B = BL[B] * 4;
				PallTable[k].G = RG[G] * 4;
				PallTable[k].R = RG[R] * 4;
			}
		}
	}
}

VectorDisplay::VectorDisplay(SDL_Renderer* rendr, uint8_t *videomem, i8080* c, std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> callback, Keyboard* kbd) {
	
	keyboard = kbd;
	renderer = rendr;
	updateCallback = callback;
	surface = SDL_CreateRGBSurface(0, 640, 640, 32, 0, 0, 0, 0);
	bps = surface->format->BitsPerPixel;

	uint16_t mx = 0;
	uint16_t my = 605;
	uint16_t mx1 = 639;
	uint16_t my1 = 639;
	gContext = new Graph((void*)surface->pixels, 640, 640, bps);
	kbd->setGraphContext(gContext);
	fContext = new Font(gContext);
	gContext->SetFillColor(cWINLIGHTGRAY);
	gContext->bar(mx, my, mx1, my1);
	gContext->SetColor(cWINGRAY);
	gContext->rectangle(mx + 2, my + 2, mx1 - 2, my1 - 2);
	gContext->SetColor(cWHITE);
	gContext->rectangle(mx + 3, my + 3, mx1 - 3, my + 3);
	gContext->rectangle(mx + 3, my + 3, mx + 3, my1 - 3);
	gContext->rectangle(mx + 2, my1 - 1, mx1 - 1, my1 - 1);
	gContext->rectangle(mx1 - 1, my + 1, mx1 - 1, my1 - 1);
	gContext->SetColor(cRED);
	gContext->circlegradient(30, 623, 5, 0, cRED, cRED);
	fContext->setFontParam(0, cBLACK, 1, 0, 0);
	fContext->printxy(40, 617, (char*)"РУС/ЛАТ");
	videoMemory = videomem;
	cpu = c;
	SetPalette();
	for (int i = 0; i < 16; i++) ScreenPalette[i] = 0;
	SGP = 0;
	SVP = 0;
	SADDR = 0xE0FF;
	back = false;
	port00 = 0;
	port02 = 0;
	port03 = 0;
	port0C = 0;
	PRTA = false;
	PRTB = false;
	PRTClo = false;
	PRTChi = false;
	modeBClo = false;
	modeAChi = 0;
}

VectorDisplay::~VectorDisplay() {
	delete fContext;
	delete gContext;
	SGP = 0;
	SVP = 0;
	SADDR = 0xE0FF;
	back = false;
	port00 = 0;
	port02 = 0;
	port03 = 0;
	port0C = 0;
	PRTA = false;
	PRTB = false;
	PRTClo = false;
	PRTChi = false;
	modeBClo = false;
	modeAChi = 0;
}

tPorts VectorDisplay::getPorts() {
	ports.count = 4;
	ports.ports[0] = 0x00;
	ports.ports[1] = 0x02;
	ports.ports[2] = 0x03;
	ports.ports[3] = 0x0C;
	return ports;
}

void VectorDisplay::reset() {
	//for (int i = 0; i < 16; i++) ScreenPalette[i] = 0;
	SGP = 0;
	SVP = 0;
	cpu->setPortData(0x02, 0xFF);
	cpu->setPortData(0x03, 0xFF);
	SADDR = 0xE0FF;
}

void VectorDisplay::syncDisplay() {
	
	//bool result = false;
	uint16_t i;
	uint8_t p0, p1, p2, p3, b0, b1, b2, b3, col;

	if (((port02 >> 4) & 0x01) == 0)
	{
		bcol1 = ScreenPalette[port02 & 0x0F];
		bcol2 = ScreenPalette[port02 & 0x0F];
	}
	else
	{
		bcol1 = ScreenPalette[(port02 & 0x0F) & 0x03];
		bcol2 = ScreenPalette[(port02 & 0x0F) >> 2];
	}

	if (!SVP) back = false;

	if (SVP < 16)
	{
		if (SGP < 40)
		{
			for (i = 0; i < 8; i++)
			{
				//SDL_SetRenderDrawColor(renderer, PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - ((SVP * 2) + 1)));
				//SDL_SetRenderDrawColor(renderer, PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - ((SVP * 2) + 1)));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 26, gContext->RGBtoColor(PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 27, gContext->RGBtoColor(PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 26, gContext->RGBtoColor(PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 27, gContext->RGBtoColor(PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B));
			}
		}
	}

	if ((SVP >= 16) && (SVP < 272))
	{
		if (SGP < 4)
		{

			for (i = 0; i < 8; i++)
			{
				//SDL_SetRenderDrawColor(renderer, PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2 + 1));
				//SDL_SetRenderDrawColor(renderer, PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2 + 1));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 26, gContext->RGBtoColor(PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 27, gContext->RGBtoColor(PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 26, gContext->RGBtoColor(PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 27, gContext->RGBtoColor(PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B));
			}
		}

		if ((SGP >= 4) && (SGP < 36))
		{
			fline = 255 - port03;
			if (SVP + fline >= 272)
			{
				p0 = videoMemory[SADDR - fline + 0x100];
				p1 = videoMemory[SADDR - 0x2000 - fline + 0x100];
				p2 = videoMemory[SADDR - 0x4000 - fline + 0x100];
				p3 = videoMemory[SADDR - 0x6000 - fline + 0x100];
			}
			else
			{
				p0 = videoMemory[SADDR - fline];
				p1 = videoMemory[SADDR - 0x2000 - fline];
				p2 = videoMemory[SADDR - 0x4000 - fline];
				p3 = videoMemory[SADDR - 0x6000 - fline];
			}
			for (int i = 7; i >= 0; i--)
			{
				b0 = p0 & 0x01;
				p0 = p0 >> 1;
				b1 = p1 & 0x01;
				p1 = p1 >> 1;
				b2 = p2 & 0x01;
				p2 = p2 >> 1;
				b3 = p3 & 0x01;
				p3 = p3 >> 1;
				col = (b3 << 3) + (b2 << 2) + (b1 << 1) + b0;
				if (((port02 >> 4) & 0x01) == 0)
				{
					scol1 = ScreenPalette[col];
					scol2 = ScreenPalette[col];
				}
				else
				{
					scol1 = ScreenPalette[col & 0x03];
					scol2 = ScreenPalette[(col >> 2) << 2];
				}
				//SDL_SetRenderDrawColor(renderer, PallTable[scol1].R, PallTable[scol1].G, PallTable[scol1].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2 + 1));
				//SDL_SetRenderDrawColor(renderer, PallTable[scol2].R, PallTable[scol2].G, PallTable[scol2].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2 + 1));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 26, gContext->RGBtoColor(PallTable[scol1].R, PallTable[scol1].G, PallTable[scol1].B));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 27, gContext->RGBtoColor(PallTable[scol1].R, PallTable[scol1].G, PallTable[scol1].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 26, gContext->RGBtoColor(PallTable[scol2].R, PallTable[scol2].G, PallTable[scol2].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 27, gContext->RGBtoColor(PallTable[scol2].R, PallTable[scol2].G, PallTable[scol2].B));
			}
			SADDR += 0x0100;
			if (SADDR < 0x0100)
			{
				SADDR -= 0x2001;
				if (SADDR == 0xDFFF) SADDR = 0xE0FF;
			}
		}

		if ((SGP >= 36) && (SGP < 40))
		{
			for (i = 0; i < 8; i++)
			{
				//SDL_SetRenderDrawColor(renderer, PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2 + 1));
				//SDL_SetRenderDrawColor(renderer, PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2 + 1));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 26, gContext->RGBtoColor(PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 27, gContext->RGBtoColor(PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 26, gContext->RGBtoColor(PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 27, gContext->RGBtoColor(PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B));
			}
		}
	}
	if ((SVP >= 272) && (SVP < 288))
	{
		if (SGP < 40)
		{
			for (i = 0; i < 8; i++)
			{
				//SDL_SetRenderDrawColor(renderer, PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2)), (616 - SVP * 2 + 1));
				//SDL_SetRenderDrawColor(renderer, PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B, 0xFF);
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2));
				//SDL_RenderDrawPoint(renderer, (SGP * 16 + (i * 2 + 1)), (616 - SVP * 2 + 1));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 26, gContext->RGBtoColor(PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B));
				gContext->putpixel(SGP * 16 + i * 2, SVP * 2 + 27, gContext->RGBtoColor(PallTable[bcol1].R, PallTable[bcol1].G, PallTable[bcol1].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 26, gContext->RGBtoColor(PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B));
				gContext->putpixel(SGP * 16 + i * 2 + 1, SVP * 2 + 27, gContext->RGBtoColor(PallTable[bcol2].R, PallTable[bcol2].G, PallTable[bcol2].B));
			}
		}
	}

	if ((SVP == 288) && (SGP == 0))
	{
		back = true;
		//SwapBuffers(st->dcWindow);
		//SDL_RenderPresent(renderer);
		updateCallback(renderer, surface);
		cpu->interruptRequest();
		//fprintf(st->dbgfile, "TickCount, %d", st->TickCount * st->TickTime / 3200000000);
		//result = true;
		/*
		if (st->INTE) //{ && !stopcpu }
		{
			st->HLT = false;
			st->INTE = false;
			st->sp--;
			setToStack(st, st->sp, st->pc >> 8);
			st->sp--;
			setToStack(st, st->sp, st->pc & 0xFF);
			st->pc = 56;
			st->NumTicks += 36;
		}
		*/
	}

	SGP++;
	if (SGP == 48)
	{
		SGP = 0;
		SVP++;
	}
	if (SVP == 312) SVP = 0;

	//return result;
}

void VectorDisplay::setPortData(uint16_t portNum, uint16_t data) {

	if (portNum == 0) {
		if (data != 0x88 && data != 0x8A && data != 0x06 && data != 0x07 && data != 0x03 && data != 0x00 && data != 0x01) {
			printf("Port0=%02X", data);
			//exit(0);
		}
		port00 = data;
		if (data < 0x80) {
			uint8_t bit = data & 0x01;
			uint8_t prt = (data >> 1) & 0x07;

			if (prt == 0) {
				// TO DO вывод на магнитофон
				keyboard->setTapeOut(bit);
			}
			if (prt == 1) {
				// управление двигателем магнитофона.
			}
			if (prt == 2) {
				// CO клавиатуры (не тспользуется)
			}
			if (prt == 3) {
				// РУС/ЛАТ
				if (!bit) {
					gContext->SetColor(cRED);
					gContext->circlegradient(30, 623, 5, 0, cRED, cRED);
					return;
				} else {
					gContext->SetColor(cLIGHTRED);
					gContext->circlegradient(30, 623, 5, 0, cLIGHTRED, cLIGHTRED);
					return;
				}
			}
			// осталыные линии (4-7) на вывод не работают
		}
		else {
			PRTClo = data & 0x01;
			data >>= 1;
			PRTB = data & 0x01;
			data >>= 1;
			modeBClo = data & 0x01;
			if (modeBClo) {
				printf("Settings mode=1 to PORT B\n");
				exit(1);
			}
			data >>= 1;
			PRTChi = data & 0x01;
			data >>= 1;
			PRTA = data & 0x01;
			data >>= 1;
			modeAChi = data & 0x03;
			if (modeAChi) {
				printf("Settings mode=%d to PORT A\n", modeAChi);
				exit(1);
			}
		}
	}
	if (portNum == 2) {
		//if (port00 == 0x88) port02 = data;
		if (!PRTB) port02 = data;
	}
	if (portNum == 3) {
		if (!PRTA) {
			if (PRTB) keyboard->setPortData(portNum, data); else port03 = data;
		}
		//if (port00 == 0x88) port03 = data;
		//if (port00 == 0x8A) keyboard->setPortData(portNum, data);
	}
	if (portNum == 0x0C) {
		port0C = data;
		ScreenPalette[port02 & 0x0F] = data;
	}
}

uint16_t VectorDisplay::getPortData(uint16_t portNum) {
	if (portNum == 0) return 0;
	if (portNum == 2) {
		if (PRTB) {
			//if (back)
			return keyboard->getPortData(portNum);// else return port02;
		} else return port02;
		//if (port00 == 0x88) return port02; 
		//if (port00 == 0x8A) return keyboard->getPortData(portNum);
	}
	if (portNum == 3) {
		if (PRTA) return port03;
	}
	if (portNum == 0x0C) return port0C;
	return 0;
}