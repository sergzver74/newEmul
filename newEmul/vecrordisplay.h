#ifndef __VECTORDISPLAY_H__
#define __VECTORDISPLAY_H__

#include <SDL.h>
#include <functional>
#include "graph.h"
#include "fonts.h"
#include "i8080.h"
#include "ports.h"
#include "kbdv6c.h"

typedef struct
{
	unsigned char R, G, B;
} TPalette;

class VectorDisplay : public Ports {

public:
	VectorDisplay(SDL_Renderer* rendr, uint8_t* videomem, i8080 *c, std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> callback, Keyboard* kbd);
	~VectorDisplay();
	void syncDisplay();
	void reset();
	void SetPalette();
	void setPortData(uint16_t portNum, uint16_t data);
	uint16_t getPortData(uint16_t portNum);
	tPorts getPorts();

private:
	Keyboard* keyboard;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	uint8_t* videoMemory;
	Graph* gContext;
	Font* fContext;
	uint16_t bps;
	i8080* cpu;
	std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> updateCallback;

	uint8_t fline;
	uint8_t bcol1, bcol2, scol1, scol2;
	uint8_t ScreenPalette[16];
	TPalette PallTable[256];
	uint16_t SVP;
	uint8_t SGP;
	uint16_t SADDR;
	uint8_t port00;
	uint8_t port02;
	uint8_t port03;
	uint8_t port0C;
	bool back;
	bool PRTA;
	bool PRTB;
	bool PRTClo;
	bool PRTChi;
	bool modeBClo;
	uint8_t modeAChi;

};

#endif // !__VECTORDISPLAY_H__
