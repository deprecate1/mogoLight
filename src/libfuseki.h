/*
** libfuseki.h
** Login : <julien@HEAVENSDOORS>
** Started on  Thu Jan 17 11:16:04 2008 
** $Id: libfuseki.h,v 1.3 2008-06-16 17:23:22 jbhoock Exp $
** 
** Copyright (C) 2008 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef   	LIBFUSEKI_H_
# define   	LIBFUSEKI_H_

#include <list>
using namespace std;

#include <fstream>
#include <assert.h>

struct libfuseki_node{
  
  int nb_pass;
  unsigned char move[2];
  list<struct libfuseki_node*> children;  
};
typedef struct libfuseki_node libfuseki_node;

void libfuseki_setPlayer(int _player);
int  libfuseki_getPlayer();
void libfuseki_addDir(const char *dir);
void libfuseki_addListDir(string *lst_dir);
void libfuseki_getMove(list<int> *move_x, 
		       list<int> *move_y, 
		       list<int> *p_move_x, 
		       list<int> *p_move_y, 
		       list<int> *count,
		       int size);

// debug
void libfuseki_print();


static char moveToCharSGF(int move);
void libfuseki_saveTreeSGF(const libfuseki_node*, ofstream&, int = -1);
void libfuseki_saveTreesSGF(const string&, const string&, int);

#endif 	    /* !LIBFUSEKI_H_ */
