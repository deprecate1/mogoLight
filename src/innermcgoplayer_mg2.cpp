#include "innermcgoplayer_mg.h"
#include "openingbook_mg.h"

Location MoGo::InnerMCGoPlayer::getOneMoveByFuseki19(const Goban& goban) const
{ 
  static int first = 1;
  static int finish = 0;
  
 // if (lastMoves.size()>10) return PASSMOVE;
  if ((int)lastMoves.size() == 1 || (int)lastMoves.size() == 2)
    finish = 0;
  
  //  std::cerr << "getOneMoveByFuseki19..." << std::endl;
  
 // if(finish == 1) return PASSMOVE;
  
#ifdef LEARNING
miningMove=1;
#endif

#define AUTOSECONDBLACKMOVE
#ifdef AUTOSECONDBLACKMOVE
// on force le second coup noir parce qu'on en a marre de voir E5 G5 F4
if (goban.height()==9)
{
	/*******
	 *
	 * Opening book for black:
         * empty goban ==> black plays E5
         * E5 E7 ==> black plays C6 or F7
         * E5 E7 F7 E3 ==> black plays D3
         * E5 E7 F7 E3 D3 F8 ==> black plays F6
         * E5 E7 F7 E3 D3 F8 F6 D4 ==> black plays E4
         * E5 E7 F7 E3 D3 F8 F6 D4 E4 C3 ==> black plays D2
         *  
         *  
         * Opening book for white:
         * E5 ==> white plays D3 or E7
         * E5 E7 F7 ==> white plays F6 or E3
         * E5 E7 F7 F6 E6 ==> white plays G7
         * E5 E7 F7 F6 E6 G7 F8 ==> white plays D7
         * E5 E7 F7 E3 D3 ==> white plays F8
         * E5 E7 F7 E3 D3 F8 G7 ==> white plays D2
         * E5 E7 F7 E3 D3 F8 G7 D2 D4 ==> white plays G3
      **/
	/*if ((lastMoves.size()==3)&&(lastMoves[1]==60))
	{
		if (lastMoves[2]==62) return 73;
		else if (lastMoves[2]==58) return 47;
		else if (lastMoves[2]==38) return 39;
		else if (lastMoves[2]==82) return 83;
	}*/
/**** BEGIN CODEC.C ****/

#ifdef MOGOTW
fprintf(stderr,"codec.c\n");
#include "treatOB/codec.c"
#endif

/**** END CODEC.C ***************/
}
//fprintf(stderr," ####################"" lastmove.size()=%d %d %d %d %d\n",lastMoves.size(),lastMoves[0],lastMoves[1],lastMoves[2],lastMoves[3]);
#endif

  // Detection du joueur en fct du nombre de coup au démarrage
  if ((lastMoves.size()% 2) == 0)
    OpeningBook::getOpeningBook()->setPlayer(1);
  else
    OpeningBook::getOpeningBook()->setPlayer(0);
  
  
  if (first == 1)  {
    first = 0;
   
    //    if ((loadOB != "")) {
    //OpeningBook::getOpeningBook()->addListDir(&pathsOB);
    //}

    //libfuseki_addListDir(&pathsFuseki);

/*
    OpeningBook::getOpeningBook()->setSize(goban.height());
    OpeningBook::getOpeningBook()->addSGFListDir(&pathsFuseki);
    OpeningBook::getOpeningBook()->undump(&loadOB);
    OpeningBook::getOpeningBook()->dump(&saveOB);*/
    // sauvegarde et fusion de l'arbre d'ouverture de libfuseki

    // if((pathsFuseki!="")&&(saveTreesFuseki!="")) {
// 	string black_tree = "black_";
// 	black_tree+=saveTreesFuseki;
// 	string white_tree = "white_";
// 	white_tree+=saveTreesFuseki;
	
// 	if((saveTreesFuseki.size()<4)
// 	   ||(saveTreesFuseki.substr(saveTreesFuseki.size()-4,4)!=".sgf")) {
// 	    black_tree+=".sgf";
// 	    white_tree+=".sgf";
// 	  }
	
// 	libfuseki_saveTreesSGF(black_tree, white_tree, goban.height());
//       }

  }
  
  // PASSMOVE de debug
  //return PASSMOVE;
 

  Location bestLocation;
  double bestScore;

  bestLocation=PASSMOVE;
  bestScore=-1;
  int location= goban.indexOnArray(0,0);

  for (int i=0;i< goban.height() ;i++,location+=2)
    for (int j=0;j< goban.height(); j++,location++)

      if (goban.isLegal(location))
	{
	  Goban tmpGoban=goban;
          tmpGoban.playMoveOnGoban(location);

	 // goban.textModeShowPosition(location);

#ifdef FUSEKI_RATIO 
	  double nbWins=OpeningBook::getOpeningBook()->getNumberOfWins(tmpGoban,goban.whoToPlay()-1);
	  double nbLosses=OpeningBook::getOpeningBook()->getNumberOfWins(tmpGoban,1-(goban.whoToPlay()-1));
#ifndef NASH_OPENING
          double score = (nbWins+0.5)/(nbLosses+1.);
#else
	/*  if (lastMoves.size()%2==0)
	  { // white
	  }
	  else
	{//black
	}*/
	          double score = (nbWins+10)/(nbWins+nbLosses+20.);

#endif
	  if (nbWins+nbLosses==0) score=-1;
#else
          double score = OpeningBook::getOpeningBook()->getNumberOfWins(tmpGoban,goban.whoToPlay()-1);
	  if (score==0) score=-10;
#endif	  
//  ((lastMoves.size()% 2) == 0)
	  //printf("%g \n",score);
          if (score > bestScore)
	    {bestScore=score;bestLocation=location;}

	}

  return bestLocation;


















 //  libfuseki_print();

  //   list<int> move_x;
  // list<int> move_y;
  
  //  list<int> p_move_x ;
//    list<int> p_move_y ;
//    list<int> count ;
    
//   std::cerr << "nb_last_moves : " << lastMoves.size() << std::endl;

//   for(int i = 1; i < (int)lastMoves.size(); i++) {

//       /*
//       int r = FBS_handler::row(lastMoves[i]);
//       int c = FBS_handler::col(lastMoves[i]);
//       */

//       int r; // = goban.xIndex(lastMoves[i]);
//       int c; // = goban.yIndex(lastMoves[i]);

//       goban.getXYIndice(lastMoves[i],c,r);

//       // std::cerr << lastMoves[i] << std::endl;
      
//       move_x.push_back(r);
//       move_y.push_back(c); 

//       std::cerr<< r << " " << c << std::endl;
//     }
  
  
//   std::cerr << "libfuseki_getmove ..." << std::endl;
//   libfuseki_getMove(&move_x,&move_y,
// 		    &p_move_x,&p_move_y,
// 		    &count, goban.height());
  
//   int max_count;
  
//   list<int> possible_moves;  
//   list<int>::iterator itx_max, ity_max, itcount_max;
//   list<int>::iterator itx, ity, itcount;

//   //if (p_move_x.size() == 0)
//   //	return PASSMOVE;
  
//   //  bool found = false;  
//   //   while (found == false){
  
//   max_count = -1;
  
//   for (itx = p_move_x.begin(), ity =  p_move_y.begin(),
// 	 itcount = count.begin();
//        itx != p_move_x.end(); itx++, ity++, itcount++) {
    
//     std::cerr<< (*itx) << " - " << (*ity) <<" : "<< *itcount <<std::endl;
    
//     if(max_count < (*itcount)) {
//       max_count = (*itcount);
//       itcount_max = itcount;
//       itx_max     = itx;
//       ity_max     = ity; 
//     }
//   }
  
//   if(max_count == -1) {
//     finish = 1;
//     return PASSMOVE;
//   }
  
//   goban.textModeShowGoban();
//   return goban.indexOnArray((*ity_max), (*itx_max));
}

