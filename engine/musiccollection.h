//	This file is part of freesiege program <http://freesiege.sourceforge.net>.
//	Copyright (C) 2007 Pierre Gueth <pierregueth@users.sourceforge.net>
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
#ifndef __MUSICCOLLECTION_H
#define __MUSICCOLLECTION_H

class MusicCollection {
friend std::ostream &operator<<(std::ostream &os,const MusicCollection &collection);
public:
    typedef std::map<std::string,Mix_Music*> Coll;
    MusicCollection(const std::string &path,const std::string &base_dir);
    ~MusicCollection();
    void play_music(const std::string &name);
    void play_random_music();
private:
    Coll coll;
};


#endif
