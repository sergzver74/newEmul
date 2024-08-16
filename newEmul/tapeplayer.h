#ifndef __TAPEPLAYER_H__
#define __TAPEPLAYER_H__

#include <stdint.h>
#include <SDL.h>
#include "window.h"
#include "graph.h"
#include "fonts.h"
#include "gui.h"
#include "bmp.h"

class TapePlayer : public Window {
private:
	tBitmap* playbmp;
public:
	TapePlayer(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev);
	virtual ~TapePlayer();
	bool eventManager(SDL_Event event);

};




#endif // !__TAPEPLAYER_H__

