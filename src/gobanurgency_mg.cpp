//
// C++ Implementation: gobanurgency_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gobanurgency_mg.h"
#include "goban_mg.h"
#include "gotools_mg.h"
#include "gobannode_mg.h"
#include <math.h>
#include "cfg_mg.h"

namespace MoGo {
  UrgencyType GobanUrgency::neverPlayUrgency;
  UrgencyType GobanUrgency::firstPlayUrgency;
  UrgencyType GobanUrgency::secondPlayUrgency;

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  GobanUrgency::GobanUrgency( int h, int w, double firstPlayUrgency ) : urgency((h+2)*(w+2)) {
    this->neverPlayUrgency = 0.;
    this->firstPlayUrgency = firstPlayUrgency;
    if (urgency.size()>10) {
      urgency[1]=0.;
      urgency[3]=0.;
      urgency[4]=0.;
    }
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
    urgencyByLine.resize(h);
    //     resetUrgency();
    totalUrgency = 0.;
#endif

  }


 // TEMPORARY COMMENT: THIS FUNCTION IS USED
  GobanUrgency::~GobanUrgency() {}}




void MoGo::GobanUrgency::resetUrgency(double resetValue ) {/**assert(0);**/
  for (int i=0;i<GobanNode::height;i++) {/**assert(0);**/
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
    urgencyByLine[i]=resetValue*GobanNode::width;
#endif
    for (int j=0;j<GobanNode::width;j++)
      urgency[indexOnArray(i,j)]=resetValue;
  }
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
  totalUrgency=resetValue*GobanNode::height*GobanNode::width;
#endif
}



void MoGo::GobanUrgency::textModeShowUrgency() const {/**assert(0);**/
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
  GoTools::print("Urgency show. Total urgency: %f",totalUrgency);
#endif
  int HorSideWidth=5;
  //textModeShowPosition(location);
  GoTools::print("\n");

  for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");GoTools::print(" ");
  for(int i=0;i<GobanNode::width;i++)
    if (i>=8)
      GoTools::print("    %c    ",(char)('A'+i+1));
    else
      GoTools::print("    %c    ",(char)('A'+i));

  GoTools::print("\n");
  for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
  for(int i=0;i<GobanNode::width;i++) GoTools::print("---------"); GoTools::print("+");

  for(int i=0;i<GobanNode::height;i++) {/**assert(0);**/
    GoTools::print("\n%2i",GobanNode::height-i);
    //if (i<9) GoTools::print(" ");
    for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
    GoTools::print("|");
    for(int j=0;j<GobanNode::width;j++)
      GoTools::print(" %7.4f ", urgency[indexOnArray(i,j)]);
    //GoTools::print("|");
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
    GoTools::print("| %f",urgencyByLine[i]);
#endif
    if (i==GobanNode::height-1)
      GoTools::print("   PASSMOVE urgency: %7.4f",urgency[PASSMOVE]);

  }
  GoTools::print("\n");
  for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" ");GoTools::print("+");
  for(int i=0;i<GobanNode::width;i++) GoTools::print("---------");
  GoTools::print("+\n");

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanUrgency::textModeShowUrgencyRange( const Goban & goban ) const {
  int HorSideWidth=6;
  //textModeShowPosition(location);
  GoTools::print("\n");
  if (goban.height()<=9) {
    for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
    for(int i=0;i<goban.width();i++)
      if (i>=8)
        GoTools::print(" %c ",(char)('A'+i+1));
      else
        GoTools::print(" %c ",(char)('A'+i));
    {
      GoTools::print(" ");
      for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
      for(int i=0;i<goban.width();i++)
        if (i>=8)
          GoTools::print("    %c    ",(char)('A'+i+1));
        else
          GoTools::print("    %c    ",(char)('A'+i));
    }
    GoTools::print("\n");
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<goban.width();i++) GoTools::print("---"); GoTools::print("+");
    {
      for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
      for(int i=0;i<goban.width();i++) GoTools::print("---------"); GoTools::print("+");
    }
    double mean=0.;double std=0.; int nb=0;
    for(int i=0;i<goban.height();i++)
      for(int j=0;j<goban.width();j++) {
        if (urgency[indexOnArray(i,j)] == 0.)
          continue;
        nb ++;
        mean += urgency[indexOnArray(i,j)];
      }
    mean /= nb;
    for(int i=0;i<goban.height();i++)
      for(int j=0;j<goban.width();j++) {
        if (urgency[indexOnArray(i,j)] == 0.)
          continue;
        std += (urgency[indexOnArray(i,j)]-mean)*(urgency[indexOnArray(i,j)]-mean);
      }
    std=sqrt(std/nb);

    for(int i=0;i<goban.height();i++) {
      GoTools::print("\n%2d",goban.height()-i);
      //if (i<9) GoTools::print(" ");
      for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
      GoTools::print("|");
      for(int j=0;j<goban.width();j++) {
        int llocation=indexOnArray(i,j);
        if (goban.gobanState[llocation]==EMPTY)
          if (urgency[llocation] == 0.)
            GoTools::print("   ");
          else
            GoTools::print(" . ");
        else {
          GoTools::print(" ");
          if (goban.gobanState[llocation]==WHITE) GoTools::print("O");
          else if (goban.gobanState[llocation]==BLACK) GoTools::print("@");
          else if (goban.gobanState[llocation]==OUTSIDE) GoTools::print("#");

          GoTools::print(" ");
        }

      }
      GoTools::print("| ");

      {
        if (i<9) GoTools::print(" ");
        for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
        GoTools::print("|");
        for(int j=0;j<goban.width();j++) {
          double v=urgency[indexOnArray(i,j)]; double nv=(v-mean)/(std+1e-50);
          if (v == 0.) nv = -1000.;
          GoTools::printWithColorScaled(nv," %7.4f ",v);
          /*       std::string color;
                 if (nv < -0.8)
                   color="34";
                 else if (nv < -0.4)
                   color = "36";
                 else if (nv < 0.)
                   color = "35";
                 else if (nv < 0.4)
                   color = "32";
                 else if (nv < .8)
                   color = "33";
                 else
                   color = "31";
          //         printf("\033[34mThis is blue.\033[0m\n");
                 std::string message="\033["+color+"m %7.4f \033[0m";
                   GoTools::print(message.c_str(), v);*/
        }

        GoTools::print("|");
      }
      /*    30    black foreground
          31    red foreground
          32    green foreground
          33    brown foreground
          34    blue foreground
          35    magenta (purple) foreground
          36    cyan (light blue) foreground
          37    gray foreground
      */

      if (i==goban.height()/2) {
        if (goban.isBlacksTurn()) GoTools::print("  Black");
        else GoTools::print("  White");
        GoTools::print(" to play");
      } else
        if (i==goban.height()/2+1)
          GoTools::print("  Black eaten stone(s): %d",goban.getBlackEatenStoneNumber());
        else
          if (i==goban.height()/2+2)
            GoTools::print("  White eaten stone(s): %d",goban.getWhiteEatenStoneNumber());
    }
    GoTools::print("\n");
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<goban.width();i++) GoTools::print("---");
    GoTools::print("+");

    {
      for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
      for(int i=0;i<goban.width();i++) GoTools::print("---------");
      GoTools::print("+");
    }
    GoTools::print("\n");
  } else {/**assert(0);**/
    for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
    for(int i=0;i<goban.width();i++)
      if (i>=8)
        GoTools::print(" %c ",(char)('A'+i+1));
      else
        GoTools::print(" %c ",(char)('A'+i));
    GoTools::print("\n");
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<goban.width();i++) GoTools::print("---"); GoTools::print("+");
    double mean=0.;double std=0.; int nb=0;
    for(int i=0;i<goban.height();i++)
      for(int j=0;j<goban.width();j++) {/**assert(0);**/
        if (urgency[indexOnArray(i,j)] == 0.)
          continue;
        nb ++;
        mean += urgency[indexOnArray(i,j)];
      }
    mean /= nb;
    for(int i=0;i<goban.height();i++)
      for(int j=0;j<goban.width();j++) {/**assert(0);**/
        if (urgency[indexOnArray(i,j)] == 0.)
          continue;
        std += (urgency[indexOnArray(i,j)]-mean)*(urgency[indexOnArray(i,j)]-mean);
      }
    std=sqrt(std/nb);

    for(int i=0;i<goban.height();i++) {/**assert(0);**/
      GoTools::print("\n%2d",goban.height()-i);
      //if (i<9) GoTools::print(" ");
      for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
      GoTools::print("|");
      for(int j=0;j<goban.width();j++) {/**assert(0);**/
        int llocation=indexOnArray(i,j);
        if (goban.gobanState[llocation]==EMPTY)
          if (urgency[llocation] == 0.)
            GoTools::print("   ");
          else
            GoTools::print(" . ");
        else {/**assert(0);**/
          GoTools::print(" ");
          if (goban.gobanState[llocation]==WHITE) GoTools::print("O");
          else if (goban.gobanState[llocation] == BLACK) GoTools::print("@");
          else if (goban.gobanState[llocation] == OUTSIDE) GoTools::print("#");
          GoTools::print(" ");
        }

      }
      GoTools::print("| ");

      /*    30    black foreground
          31    red foreground
          32    green foreground
          33    brown foreground
          34    blue foreground
          35    magenta (purple) foreground
          36    cyan (light blue) foreground
          37    gray foreground
      */

      if (i==goban.height()/2) {/**assert(0);**/
        if (goban.isBlacksTurn()) GoTools::print("  Black");
        else GoTools::print("  White");
        GoTools::print(" to play");
      } else
        if (i==goban.height()/2+1)
          GoTools::print("  Black eaten stone(s): %d",goban.getBlackEatenStoneNumber());
        else
          if (i==goban.height()/2+2)
            GoTools::print("  White eaten stone(s): %d",goban.getWhiteEatenStoneNumber());
    }
    GoTools::print("\n");
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<goban.width();i++) GoTools::print("---");
    GoTools::print("+\n");



    {/**assert(0);**/
      for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" ");
      for(int i=0;i<goban.width();i++)
        if (i>=8)
          GoTools::print("   %c   ",(char)('A'+i+1));
        else
          GoTools::print("   %c   ",(char)('A'+i));

      GoTools::print("\n");
    }
    {/**assert(0);**/
      for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
      for(int i=0;i<goban.width();i++) GoTools::print("-------"); GoTools::print("+\n");
    }
    for(int i=0;i<goban.height();i++) {/**assert(0);**/
      for(int j=0;j<HorSideWidth-1;j++) GoTools::print(" ");
      GoTools::print("|");
      for(int j=0;j<goban.width();j++) {/**assert(0);**/
        double v=urgency[indexOnArray(i,j)]; double nv=(v-mean)/(std+1e-50);
        if (v == 0.) nv = -1000.;
        GoTools::printWithColorScaled(nv," %5.3f ",v);
        //GoTools::printWithColorScaled(0.5," %7.4f ",v);
        /*       std::string color;
               if (nv < -0.8)
                 color="34";
               else if (nv < -0.4)
                 color = "36";
               else if (nv < 0.)
                 color = "35";
               else if (nv < 0.4)
                 color = "32";
               else if (nv < .8)
                 color = "33";
               else
                 color = "31";
        //         printf("\033[34mThis is blue.\033[0m\n");
               std::string message="\033["+color+"m %7.4f \033[0m";
                 GoTools::print(message.c_str(), v);*/
      }

      GoTools::print("|\n");
    }
    {/**assert(0);**/
      for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
      for(int i=0;i<goban.width();i++) GoTools::print("-------");
      GoTools::print("+");
    }
    GoTools::print("\n");


  }


}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanUrgency::resetNeverPlayUrgency( Location location ) {
  resetUrgency(location,0);
}

void MoGo::GobanUrgency::resetFirstPlayUrgency( Location location ) {/**assert(0);**/
  resetUrgency(location,firstPlayUrgency);
}

void MoGo::GobanUrgency::resetSecondPlayUrgency( Location location ) {/**assert(0);**/
  resetUrgency(location,firstPlayUrgency);
}



int MoGo::GobanUrgency::possibleMoveNumber( ) const {/**assert(0);**/
  return possibleMoveNumber_;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanUrgency::setFirstPlayUrgency( const double firstPlayUrgency ) {
  /* //FIXME I believe these lines are now useless...
    for (int i=0;i<height;i++)
      for (int j=0;j<width;j++)
        if (urgency[indexOnArray(i,j)]==this->firstPlayUrgency)
          resetUrgency(indexOnArray(i,j),firstPlayUrgency);
  */
  this->firstPlayUrgency=firstPlayUrgency;
}

void MoGo::GobanUrgency::setSecondPlayUrgency( const double secondPlayUrgency ) {/**assert(0);**/
  this->secondPlayUrgency = secondPlayUrgency;
}


double MoGo::GobanUrgency::highestUrgency( ) const {/**assert(0);**/
  double hUrgency=0;
  for (int i=0;i<GobanNode::height;i++)
    for (int j=0;j<GobanNode::width;j++)
      if (urgency[indexOnArray(i,j)]>hUrgency) hUrgency=urgency[indexOnArray(i,j)];
  return hUrgency;
}

bool MoGo::GobanUrgency::isIntestingToContinue( ) const {/**assert(0);**/
  double hUrgency=highestUrgency();
  for (int i=0;i<GobanNode::height;i++)
    for (int j=0;j<GobanNode::width;j++)
      if (urgency[indexOnArray(i,j)]!=0 && hUrgency-urgency[indexOnArray(i,j)]>0.05) return false;
  return true;
}

void MoGo::GobanUrgency::changeToBitmap( Bitmap & bitmap ) const {/**assert(0);**/
  GobanBitmap::clear(bitmap);
  for (int i=0;i<GobanNode::height;i++)
    for (int j=0;j<GobanNode::width;j++)
      if (urgency[GobanLocation::locationXY(i,j)] > 0.)
        GobanBitmap::setBitValue(bitmap,i,j);
}





// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanUrgency::clear( ) {
  urgency.clear();
  possibleMoveNumber_=0;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::GobanUrgency::indexOnArray( const int x, const int y ) const {
  return (x+1)*(GobanNode::width+2)+(y+1);
}//FIXME be CAREFUL here if the definition of location changed.
