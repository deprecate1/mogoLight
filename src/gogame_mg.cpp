//
// C++ Implementation: game_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gogame_mg.h"
#include "gotools_mg.h"

using namespace MoGo;

namespace MoGo {

  GoGame::GoGame(int size) : goban(size,size) {/**assert(0);**/
    this->size=size;
    currentMove=0;
    komi=0.;
    moveList.clear();
  }
  MoGo::GoGame::GoGame( const Goban & _goban ): goban(_goban) {/**assert(0);**/
    this->size=_goban.height();
    komi=0.;
    fromGoban(_goban);
  }

  GoGame::~GoGame() {/**assert(0);**/}}




MoGo::GoGame::GoGame( const char * gameName ) {/**assert(0);**///the most ugly part of this project.
  Vector<Location> game;
  FILE *pFile = fopen (gameName , "r");
  readSGFFileHeadInformation(pFile);
  goban.resize(size,size);//size < 10 then suppose it is a 9x9 game.

  moveList.clear();
  if (!readMoveSequence(pFile))
    Cout << "Error while reading SGF files" << gameName << "\n";
  fclose(pFile);
  currentMove=0;

}

void MoGo::GoGame::readSGFFileHeadInformation( FILE *pFile ) {/**assert(0);**/
  char c;
  do {/**assert(0);**/
    c=fgetc(pFile);
  } while (c!=';');
  c=fgetc(pFile);
  size=0;
  while(c!=';') //In the SGF file, the head information ends before the second ';'. I have not found any exception yet...
  {/**assert(0);**/
    c=fgetc(pFile);
    if (c!='S') continue;
    c=fgetc(pFile);
    if (c=='Z') {/**assert(0);**/
      c=fgetc(pFile);
      c=fgetc(pFile);
      if (c=='1') //size >10, we suppose this is then a 19x19 game.
        size=19;
      else size=9;
    }
  }
  if (size==0) size=19;
}

bool MoGo::GoGame::readMoveSequence( FILE *pFile) {/**assert(0);**/
  int state=0;
  int move=1; /*first move*/
  int c=fgetc(pFile),c1,c2;
  do {/**assert(0);**/
    switch(state) {/**assert(0);**/
        case 0:/*looking for commands*/
        if (c>='A' && c<='Z') {/**assert(0);**/
          c1=c;
          c=fgetc(pFile);
          if (c1=='B' || c1=='W' && c=='[') {/**assert(0);**/
            c1=fgetc(pFile);
            c2=fgetc(pFile);
            int x=(int)(c2-'a'),y=(int)(c1-'a');
            if (isNotOnGoban(x,y)) moveList.push_back(PASSMOVE);
            else moveList.push_back(goban.indexOnArray((int)(c2-'a'),(int)(c1-'a')));
            //           Cout << c1 << c2 << moveList[move] << "|";
            state=1;
            move=move+1;
          } else {/**assert(0);**/
            state=2;break;
          }
        }
        break;
        case 1:/*waiting for ';'*/
        if (c==';') state=0;
        break;
        case 2:/*waiting for ']'*/
        if (c==']') state=0;
        break;
    }
    c=fgetc(pFile);
  } while(c!=EOF && !(c==')' && (state==0 || state==1)));
  return true;
}


void MoGo::GoGame::showMoveSequence(int beginMove) const {/**assert(0);**/
  Cout << "In all " << moveList.size() << " moves.";
  for (int i=beginMove;i<(int)moveList.size();i++)
    goban.textModeShowPosition(moveList[i]);
  Cout << "\n";
}

Location MoGo::GoGame::move( const int moveNumber ) const {/**assert(0);**/
  assert(moveNumber<(int)moveList.size());
  return moveList[moveNumber];
}

MoGo::Goban MoGo::GoGame::nextGobanState( ) {/**assert(0);**/
  goban.playMoveOnGoban(moveList[currentMove++]);
  //goban.allLocationInformation();//FIXME
  return goban;

}

MoGo::Goban MoGo::GoGame::goToGobanStateAtMove( const int moveNumber )  {/**assert(0);**/
  if (currentMove==moveNumber) return goban;
  if (currentMove>moveNumber) {/**assert(0);**/
    currentMove=0;
    goban.clearGoban();
  }
  for (;currentMove<moveNumber;currentMove++)
    goban.playMoveOnGoban(moveList[currentMove]);
  return goban;
}

int MoGo::GoGame::gameSize( ) const {/**assert(0);**/
  return size;
}

bool MoGo::GoGame::isNotOnGoban( int x, int y ) const {/**assert(0);**/
  if (x<0 || x>=size || y<0 || y>=size) return true;
  return false;
}

int MoGo::GoGame::gameMoveNumber( ) const {/**assert(0);**/
  return moveList.size();
}

void MoGo::GoGame::addNextMove( const Location location ) {/**assert(0);**/
  moveList.push_back(location);
}
void MoGo::GoGame::undoMove( ) {/**assert(0);**/
  if (moveList.size()==0)
    return;
  moveList.pop_back();
}


bool MoGo::GoGame::saveGame( const std::string &gameName, const std::string &gameDiscription ) {/**assert(0);**/
  GoTools::saveOutputMode();
  GoTools::setOutputMode(FILE_MODE);
  std::string name=gameName+std::string(".sgf");
  GoTools::changeFileName(name,false);


  GoTools::print("(;FF[4]GM[1]SZ[%d]KM[7.5]GN[%s]RE[%5.1f];",size,gameDiscription.c_str(),score);
  //gamefile << "(;FF[4]GM[1]SZ[" << size << "]PB[" << *PlayerBlack << "]PW[" << *PlayerWhite << "]GN[" << *GameTitle << "]RE[" << score << "];";
  for (int i=0;i<(int)moveList.size();i++) {/**assert(0);**/
  
    if (i%2==0) GoTools::print("B");
    else GoTools::print("W");
    if (moveList[i]==PASSMOVE)
      GoTools::print("[];");
    else
      GoTools::print("[%c%c];",char('a'+goban.yIndex(moveList[i])),char('a'+goban.xIndex(moveList[i])));
    //               gamefile << "[" << char('a'+POSI(game[i]-1)) << char('a'+POSJ(game[i])-1) << "];";
  }
  GoTools::print(")");
  GoTools::restoreOutputMode();


  return true;
}
double MoGo::GoGame::getScore( ) const {/**assert(0);**/
  return score;
}
void MoGo::GoGame::setKomi( double komi ) {/**assert(0);**/
  this->komi = komi;
}

int MoGo::GoGame::nextMoveNumber( ) const {/**assert(0);**/
  return moveList.size()+1;
}

bool MoGo::GoGame::saveGobanPosition( const Goban & goban, const std::string & gameName, const std::string & gameDiscription ) {/**assert(0);**/
  GoGame game(goban.height());
  game.fromGoban(goban);

  return game.saveGame(gameName, gameDiscription);
}

void MoGo::GoGame::fromGoban( const Goban & goban ) {/**assert(0);**/
  clear();
  for (int i = 0 ; i < goban.height() ; i++)
    for (int j = 0 ; j < goban.width() ; j++) {/**assert(0);**/
      if (goban.indexOnArray(i,j)==goban.lastMove()) continue;
      PositionState s=goban.position(i,j);
      if (s == EMPTY)
        continue;
      else if (s == BLACK) {/**assert(0);**/
        addNextMove(goban.indexOnArray(i,j));
        addNextMove(PASSMOVE);
      }

    }
  addNextMove(PASSMOVE);
  for (int i = 0 ; i < goban.height() ; i++)
    for (int j = 0 ; j < goban.width() ; j++) {/**assert(0);**/
      if (goban.indexOnArray(i,j)==goban.lastMove()) continue;
      PositionState s=goban.position(i,j);
      if (s == EMPTY)
        continue;
      else if (s == WHITE) {/**assert(0);**/
        addNextMove(goban.indexOnArray(i,j));
        addNextMove(PASSMOVE);
      }
    }

  if (!goban.isBlacksTurn()) addNextMove(PASSMOVE);
  addNextMove(goban.indexOnArray(goban.xIndex(goban.lastMove()),goban.yIndex(goban.lastMove())));
  // goban.textModeShowPosition(goban.lastMove());
}



void MoGo::GoGame::clear( ) {/**assert(0);**/
  currentMove=0;
  komi=0.;
  moveList.clear();
}

