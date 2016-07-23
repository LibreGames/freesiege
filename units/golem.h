//	This file is part of freesiege program <http://freesiege.sourceforge.net>.
//	Copyright (C) 2007 Pierre Gueth <pierregueth@users.sourceforge.net>
//	                   Joel Schaerer <joelschaerer@users.sourceforge.net>
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef __GOLEM_H
#define __GOLEM_H

#include "unit.h"

class Golem: public Unit {
public:
	Golem(const SpriteCollection *spr_coll,PLAYER player);
	virtual ~Golem();
	virtual void post_message(MessageQueue *mess_queue);
	virtual void handle_message(const Message &mess,MessageQueue *mess_queue);
	virtual void draw();

    static constexpr int Life=300;
    static constexpr int Damage=7;
    static constexpr int PlayerDamage=5;
    static constexpr float W=74;
    static constexpr float H=70;
    static constexpr float DX=1;
private:
	typedef enum {STOP,WALK,FIGHT,CADAVER} State;

	State state;
	int life;
	Anim::CycleIterator anim_golem_walk;
	Anim::ForwardBackwardIterator anim_golem_fight;
	const Sprite *bit_golem_dead;
	float dx;
};

#endif
