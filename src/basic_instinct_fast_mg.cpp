//
// C++ Implementation: basic_instinct_fast_mg
//
// Description:
//
//
// Author: Hoock Jean-Baptiste, Olivier Teytaud, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "basic_instinct_fast_mg.h"

#include "Arbre.h"
#include "Forme_kppv.h"
#include "Forme_kppv13.h"
#include "Element_kppv.h"
#include <math.h>

using namespace std;

//Forme_kppv::init_class();

#ifdef AUTOTUNE
int MoGo::BasicInstinctFast::fileNumber=0;
char MoGo::BasicInstinctFast::fileName[500];
FILE *MoGo::BasicInstinctFast::file=NULL;
#endif

double MoGo::BasicInstinctFast::bonus_line_influence = 0; //SMALL >0 ?
double MoGo::BasicInstinctFast::bonus_line_territory = 4./3.; //VALIDATED 4/3
double MoGo::BasicInstinctFast::malus_line_defeat = 0; // TESTED AND REJECTED 
double MoGo::BasicInstinctFast::malus_line_death = -(4)/3; // VALIDATED -1

#ifdef GUILLAUME
double MoGo::BasicInstinctFast::GCoeff= 30.0;
int MoGo::BasicInstinctFast::uninitialized=1;
#endif

double MoGo::BasicInstinctFast::bonus_peep_connect = 1; //HALF-VALIDATED
double MoGo::BasicInstinctFast::bonus_atari_extend = 0; //TESTED AND REJECTED POSITIVE ONLY    double(8)/4.;
double MoGo::BasicInstinctFast::bonus_tsuke_hane = 4/4.;
double MoGo::BasicInstinctFast::bonus_thrust_block = 0;
double MoGo::BasicInstinctFast::bonus_kosumi_tsuke_stretch = 0;
double MoGo::BasicInstinctFast::bonus_kosumi_tsuke_stretch2 = 0; 
//when I need sabaki (cf senseis: basicInstinct)
double MoGo::BasicInstinctFast::bonus_bump_stretch = 0;

double MoGo::BasicInstinctFast::bonus_eat = 0;
double MoGo::BasicInstinctFast::bonus_threat = 0.25; //VALIDATED 0.25
double MoGo::BasicInstinctFast::bonus_eat_or_to_be_eaten = 0;
double MoGo::BasicInstinctFast::bonus_connect = 1;    //VALIDATED 1 
double MoGo::BasicInstinctFast::bonus_connect_if_dead_virtual_connection = 1.; //VALIDATED 1 

double MoGo::BasicInstinctFast::bonus_reduce_liberties = 0; //TESTED AND REJECTED >0
double MoGo::BasicInstinctFast::bonus_increase_liberties = 0.; //TESTED AND REJECTED (positive only)
double MoGo::BasicInstinctFast::bonus_prevent_connection = 0;  //TESTED AND REJECTED POSITIVE ONLY
double MoGo::BasicInstinctFast::bonus_one_point_jump = 0; //TESTED AND REJECTED POSITIVE ONLY (Best result 1/3 1932-1961 [0.48;0.51])
double MoGo::BasicInstinctFast::bonus_kogeima = 0;  //TESTED AND REJECTED 

double MoGo::BasicInstinctFast::bonus_wall = double(2)/3;  //VALIDATED 1  ===> 55%
double MoGo::BasicInstinctFast::bonus_kosumi = 0; //TESTED AND REJECTED
double MoGo::BasicInstinctFast::bonus_kata = 0; //TESTED AND REJECTED

double MoGo::BasicInstinctFast::bonus_extend = 0.8f;
double MoGo::BasicInstinctFast::bonus_cross_cut = 0.5f; //VALIDATED 0.5f

double MoGo::BasicInstinctFast::malus_bad_tobi = 0;   //TESTED AND REJECTED
double MoGo::BasicInstinctFast::malus_bad_kogeima = -(4)/8.;
double MoGo::BasicInstinctFast::malus_bad_elephant = 0; //TESTED AND REJECTED ONLY
double MoGo::BasicInstinctFast::malus_bad_empty_triangle = -1; //VALIDATED -1
double MoGo::BasicInstinctFast::malus_bad_hazama_tobi = -double(1.4)/4.; //-double(8)/4.; //--- : TO BE RETESTED (NOT UNVALIDATED BUT NOT 
//PROMISING)

double MoGo::BasicInstinctFast::malus_alone_border = 0; //TESTED AND REJECTED NEGATIVE ONLY

double MoGo::BasicInstinctFast::malus_too_free = -0.5f;//-0.05f;
int MoGo::BasicInstinctFast::param_too_free = 4;

double MoGo::BasicInstinctFast::malus_border = 0;
double MoGo::BasicInstinctFast::bonus_if_last_border = 0;
double MoGo::BasicInstinctFast::bonus_if_last_corner = 0;

double MoGo::BasicInstinctFast::bonus_demi_noeud_bambou = 0.5f; //0.25f

bool MoGo::BasicInstinctFast::debug_peep_connect = false;
bool MoGo::BasicInstinctFast::debug_atari_extend = false;
bool MoGo::BasicInstinctFast::debug_tsuke_hane = false;
bool MoGo::BasicInstinctFast::debug_thrust_block = false;
bool MoGo::BasicInstinctFast::debug_kosumi_tsuke_stretch = false;
bool MoGo::BasicInstinctFast::debug_bump_stretch = false;

bool MoGo::BasicInstinctFast::debug_eat = false;
bool MoGo::BasicInstinctFast::debug_threat = false;
bool MoGo::BasicInstinctFast::debug_eat_or_to_be_eaten = false;
bool MoGo::BasicInstinctFast::debug_connect = false;
bool MoGo::BasicInstinctFast::debug_connect_if_dead_virtual_connection = false;
bool MoGo::BasicInstinctFast::debug_reduce_liberties = false;
bool MoGo::BasicInstinctFast::debug_increase_liberties = false;
bool MoGo::BasicInstinctFast::debug_prevent_connection = false;
bool MoGo::BasicInstinctFast::debug_one_point_jump = false;
bool MoGo::BasicInstinctFast::debug_kogeima = false;

bool MoGo::BasicInstinctFast::debug_wall = false;
bool MoGo::BasicInstinctFast::debug_kosumi = false;
bool MoGo::BasicInstinctFast::debug_kata = false;

bool MoGo::BasicInstinctFast::debug_extend = false;
bool MoGo::BasicInstinctFast::debug_cross_cut = false;

bool MoGo::BasicInstinctFast::debug_bad_tobi = false;
bool MoGo::BasicInstinctFast::debug_bad_kogeima = false;
bool MoGo::BasicInstinctFast::debug_bad_elephant = false;
bool MoGo::BasicInstinctFast::debug_bad_empty_triangle = false;
bool MoGo::BasicInstinctFast::debug_bad_hazama_tobi = false;

bool MoGo::BasicInstinctFast::debug_avoid_border = false;

bool MoGo::BasicInstinctFast::debug_too_free = false;

bool MoGo::BasicInstinctFast::debug_demi_noeud_bambou = false;


double MoGo::BasicInstinctFast::stats_distances [19][19] =
{
{0,0.21897,0.0673133,0.0247756,0.0132722,0.00902986,0.00610085,0.00395349,0.00289568,0.00201404,0.00173392,0.00169345,0.00312026,0.00243145,0.00105543,0.000440423,0.000305518,0.000179343,0.000113478},
{0,0.126152,0.0956622,0.0417227,0.0218521,0.013838,0.00957661,0.00658888,0.00477006,0.00359798,0.00306867,0.00283537,0.00443676,0.00355909,0.0020069,0.000760225,0.000631669,0.000338848,0.000203943},
{0,0,0.0264452,0.0279307,0.0161417,0.0110185,0.00826646,0.00672061,0.00402332,0.00314962,0.0025854,0.00224576,0.00217434,0.0020196,0.00139745,0.000818155,0.000553901,0.000337261,0.000188866},
{0,0,0,0.0098123,0.0122239,0.00868625,0.00653333,0.00503272,0.0040995,0.00285203,0.00232115,0.00210689,0.00167916,0.00149188,0.00114272,0.000738799,0.000493591,0.000299964,0.000229337},
{0,0,0,0,0.00447961,0.00684837,0.00519699,0.00414791,0.00304407,0.00256001,0.00211324,0.00193786,0.00149347,0.00146728,0.00121493,0.000701502,0.000478513,0.000264254,0.000170614},
{0,0,0,0,0,0.00267031,0.00426218,0.00347339,0.00274649,0.00216958,0.00189739,0.00168234,0.00143395,0.00128318,0.0011332,0.000632463,0.000465816,0.000256318,0.000165853},
{0,0,0,0,0,0,0.00205054,0.00306629,0.00241717,0.00198547,0.00178074,0.0015879,0.00149506,0.00114431,0.000987975,0.000595166,0.000436455,0.000234892,0.000130936},
{0,0,0,0,0,0,0,0.00131492,0.00208546,0.00167519,0.00162599,0.0014157,0.0012435,0.00112129,0.000810219,0.000548346,0.00035948,0.000218227,0},
{0,0,0,0,0,0,0,0,0.000851484,0.00152839,0.00134507,0.0012681,0.00107209,0.000842755,0.000727689,0.000461055,0.00033488,0.000225369,0.000123794},
{0,0,0,0,0,0,0,0,0,0.000682457,0.00114351,0.000981627,0.0008864,0.000702296,0.000591992,0.000427726,0.00031028,0.000192834,0},
{0,0,0,0,0,0,0,0,0,0,0.000504701,0.000925284,0.000819742,0.000773716,0.000674521,0.000365829,0.00023886,0.00014284,0.000111098},
{0,0,0,0,0,0,0,0,0,0,0,0.00050232,0.000864974,0.00094671,0.000717373,0.000294409,0.000222195,0.000135698,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0.00200134,0.002856,0.00124429,0.000259492,0.000206324,0.000109511,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0.000651508,0.000494384,0.000200769,0.000153156,0.000106336,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000138078,0.000169027,0.000152362,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };

double MoGo::BasicInstinctFast::weight_distance = 10.f;



//stats about the distance of the before last move
double MoGo::BasicInstinctFast::stats_distances2 [19][19] =
{
{0,0.122602,0.0578517,0.0259033,0.0160579,0.0108015,0.00823229,0.00555624,0.00422778,0.00334586,0.00292484,0.00274224,0.0042525,0.00332752,0.00210193,0.000711275,0.000484815,0.000280683,0.000152302},
{0,0.0978697,0.0753625,0.0412364,0.026814,0.0184971,0.0130669,0.00961896,0.00728499,0.0057819,0.00516392,0.0045571,0.00480509,0.00472695,0.00294637,0.00133962,0.000855603,0.000610803,0.000340487},
{0,0,0.022701,0.0288864,0.0207187,0.0148116,0.0108756,0.00812305,0.00629064,0.0050794,0.00432267,0.00371346,0.00331476,0.00277652,0.00218805,0.00125908,0.000873146,0.000529469,0.00031497},
{0,0,0,0.0112704,0.0163179,0.0122512,0.00945071,0.00710238,0.00584569,0.00453318,0.00378443,0.00326772,0.00278689,0.00227815,0.0020796,0.00127344,0.000880322,0.000451325,0.000297428},
{0,0,0,0,0.0063951,0.00993154,0.00783838,0.00652348,0.00494862,0.00417435,0.0034575,0.00295753,0.00246474,0.00222553,0.00186909,0.00109961,0.000763106,0.000447338,0.000284669},
{0,0,0,0,0,0.0041297,0.00659763,0.00539596,0.00444866,0.00371984,0.00298066,0.00279885,0.00220958,0.00195441,0.00166416,0.00104618,0.000717654,0.000424213,0.00022646},
{0,0,0,0,0,0,0.00288656,0.00466395,0.00376609,0.00322546,0.00295355,0.00243923,0.00203894,0.00184357,0.0014672,0.000873943,0.000597248,0.000398697,0.000222473},
{0,0,0,0,0,0,0,0.00195042,0.00326852,0.00291368,0.00257,0.00236268,0.00192092,0.00181487,0.00132048,0.000830087,0.00059964,0.00035803,0.000212107},
{0,0,0,0,0,0,0,0,0.00146561,0.00237862,0.00212107,0.00184756,0.00166017,0.00141537,0.00123915,0.000787027,0.000543025,0.000297428,0.000155492},
{0,0,0,0,0,0,0,0,0,0.00106532,0.00190816,0.00165858,0.00143132,0.00124074,0.00108286,0.00069134,0.000445743,0.000301415,0.000177819},
{0,0,0,0,0,0,0,0,0,0,0.000851616,0.00171041,0.00136833,0.00142813,0.00118971,0.000598843,0.000414645,0.0002448,0.000142733},
{0,0,0,0,0,0,0,0,0,0,0,0.000775864,0.00173114,0.00115941,0.00103023,0.000509534,0.000337297,0.000197754,0.000137949},
{0,0,0,0,0,0,0,0,0,0,0,0,0.000982389,0.00160117,0.00104618,0.000436174,0.000291049,0.000169047,0.000107648},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0.000582895,0.000555783,0.000348461,0.00025995,0.000141936,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000185793,0.000279885,0.000173034,0.000122799,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000102864,0.000161073,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

double MoGo::BasicInstinctFast::weight_distance2 = 5.f;



#ifdef TEST_RANDOM_PATTERN
GE_Pattern MoGo::BasicInstinctFast::test_pattern;
#endif

#ifdef RANDOM_PATTERN
vector<list<GE_Pattern> > MoGo::BasicInstinctFast::random_patterns;
#endif

//    12 9  13
// 19 5  1  4 14
// 8  0 [*] 2 10
// 18 6  3  7 15
//    17 11 16

//  directions[0]=-dWE;
//  directions[1]=-dNS;
//  directions[2]=+dWE;
//  directions[3]=+dNS;
//  directions[4]=-dNS+dWE;
//  directions[5]=-dNS-dWE;
//  directions[6]=+dNS-dWE;
//  directions[7]=+dNS+dWE;

/**
 * @brief initialization of the expertise table basic_instinct 
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::init0(double** basic_instinct) const
{

  for(int i = 0; i<FBS_handler::getBigArea(); i++) (*basic_instinct)[i] = 0;
  
  int r1 = 3;
  int c1 = 3;
  
  int r2 =  FBS_handler::board_size-4;
  int c2 =  FBS_handler::board_size-4;
  
  int start1 = FBS_handler::toLocation(r1,c1);
  int start2 = FBS_handler::toLocation(r2,c2);
  
  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[3])
    (*basic_instinct)[i] = bonus_line_influence;
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[1])
    (*basic_instinct)[i] = bonus_line_influence;

  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[2])
    (*basic_instinct)[i] = bonus_line_influence;
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[0])
    (*basic_instinct)[i] = bonus_line_influence;

  start1 += FBS_handler::directions[5];
  start2 += FBS_handler::directions[7];
  
  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[3])
    (*basic_instinct)[i] = bonus_line_territory;
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[1])
    (*basic_instinct)[i] = bonus_line_territory;
  
  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[2])
    (*basic_instinct)[i] = bonus_line_territory;
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[0])
    (*basic_instinct)[i] = bonus_line_territory;

  start1 += FBS_handler::directions[5];
  start2 += FBS_handler::directions[7];
  
  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[3])
    (*basic_instinct)[i] = malus_line_defeat;
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[1])
    (*basic_instinct)[i] = malus_line_defeat;
  
  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[2])
    (*basic_instinct)[i] = malus_line_defeat;
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[0])
    (*basic_instinct)[i] = malus_line_defeat;
  

  start1 += FBS_handler::directions[5];
  start2 += FBS_handler::directions[7];
  
  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[3])
    (*basic_instinct)[i] = malus_line_death;
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[1])
    (*basic_instinct)[i] = malus_line_death;
  
  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[2])
    (*basic_instinct)[i] = malus_line_death;
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[0])
    (*basic_instinct)[i] = malus_line_death;
}



/**
 * @brief local initialization of the expertise table basic_instinct
 *        we use statistical information from professionnal games on the last played move 
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::init_local0(double** basic_instinct) const
{
  int temp_last_move = fastboard->lastMove();
  
  if(temp_last_move==PASSMOVE) return;
  
  int r_lm = FBS_handler::row(temp_last_move);
  int c_lm = FBS_handler::col(temp_last_move);
  
  for(int i = 0; i<FBS_handler::getBigArea(); i++) 
    {
      if(fastboard->position(i)==FB_EDGE) continue;
      
      int r = FBS_handler::row(i);
      int c = FBS_handler::col(i);
      
      int dist_row = (int)fabs(r-r_lm);
      int dist_col = (int)fabs(c-c_lm);
  
      int max_d = 0;
      int min_d = 0;

      if(dist_row>dist_col)
	{
	  max_d = dist_row;
	  min_d = dist_col;
	}
      else
	{
	  max_d = dist_col;
	  min_d = dist_row;
	}
      
    
      if((dist_row==dist_col)||(dist_row==0)||(dist_col==0))
	(*basic_instinct)[i]+=((stats_distances[min_d][max_d]/4.)*weight_distance);
      else 
	(*basic_instinct)[i]+=((stats_distances[min_d][max_d]/8.)*weight_distance);
    }

}


/**
 * @brief local initialization of the expertise table basic_instinct
 *        we use statistical information from professionnal games on the before last played move 
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::init_by_distance_before_last_move0(double** basic_instinct) const
{
  //  Goban goban(fastboard->getGobanKey());
  //   goban.textModeShowPosition(before_last_move);
  //   cerr<<endl;
  
  if(before_last_move==PASSMOVE) return;
  
  int r_blm = FBS_handler::row(before_last_move);
  int c_blm = FBS_handler::col(before_last_move);
  
  for(int i = 0; i<FBS_handler::getBigArea(); i++) 
    {
      if(fastboard->position(i)==FB_EDGE) continue;
      
      int r = FBS_handler::row(i);
      int c = FBS_handler::col(i);
      
      int dist_row = (int)fabs(r-r_blm);
      int dist_col = (int)fabs(c-c_blm);
      
      int max_d = 0;
      int min_d = 0;
      
      if(dist_row>dist_col)
	{
	  max_d = dist_row;
	  min_d = dist_col;
	}
      else
	{
	  max_d = dist_col;
	  min_d = dist_row;
	}
      
      
      if((dist_row==dist_col)||(dist_row==0)||(dist_col==0))
	(*basic_instinct)[i]+=((stats_distances2[min_d][max_d]/4.)*weight_distance2);
      else 
	(*basic_instinct)[i]+=((stats_distances2[min_d][max_d]/8.)*weight_distance2);
    }
}







/**
 * @brief the expertise-go "peep connect"
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm the safeguard of the found move 
 * @return if the expertise has been matched
 */ 

/*********
 * ..... *
 * .@2@. *
 * ..1.. *
 * ..... * 
 ****************************************************
 * if white plays 1 then 2 is a good move for black *
 ****************************************************/

//We don't look if it is a legal move

bool MoGo::BasicInstinctFast::peep_connect0(int dir, double** basic_instinct, 
					    int* gm) const 
{
  Location place1;
  Location place2;
  Location good_move;
  
  switch(dir)
    {
    case NORTH_ : 
      place1 = this->last_move+FBS_handler::directions[4]; 
      place2 = this->last_move+FBS_handler::directions[5];
      good_move = this->last_move+FBS_handler::directions[1];
      break;
      
    case SOUTH_ :
      place1 = this->last_move+FBS_handler::directions[6];
      place2 = this->last_move+FBS_handler::directions[7];
      good_move = this->last_move+FBS_handler::directions[3];
      break;
      
    case EAST_ :
      place1 = this->last_move+FBS_handler::directions[4];
      place2 = this->last_move+FBS_handler::directions[7];
      good_move = this->last_move+FBS_handler::directions[2];
      break;
      
    case WEST_ :
      place1 = this->last_move+FBS_handler::directions[5];
      place2 = this->last_move+FBS_handler::directions[6];
      good_move = this->last_move+FBS_handler::directions[0];
      break;
      
      
    default:
      assert(0);
      
    }
  

  if(not FBS_handler::isOnBoard(place1)||(not FBS_handler::isOnBoard(place2)))
    return false;
  
  if(fastboard->position(good_move)==FB_EMPTY)
    if(fastboard->position(place1)!=FB_EMPTY)
      if(fastboard->position(place1)==fastboard->position(place2))
	{
	  
	  if(fastboard->position(this->last_move)!=fastboard->position(place1))
	    {
	      if(basic_instinct) (*basic_instinct)[good_move] += bonus_peep_connect; 
	      if(gm) *gm = good_move;
	      
	      if(debug_peep_connect)
		{
		  Goban goban(fastboard->getGobanKey());
		  
		  std::cout<<"peep connect"<<std::endl;
		  goban.textModeShowGoban(good_move);
		  std::cout<<std::endl;
		  
		  switch(dir)
		    {
		    case NORTH_ : std::cout<<"north"<<std::endl; break;
		    case SOUTH_ : std::cout<<"south"<<std::endl; break;
		    case EAST_ : std::cout<<"east"<<std::endl; break;
		    case WEST_ : std::cout<<"west"<<std::endl; break;
		    default : assert(0);
		    }
		  
		  goban.textModeShowPosition(place1);
		  goban.textModeShowPosition(good_move);
		  goban.textModeShowPosition(place2);
		}
     
	      return true;
	      
	    }
	}
  
  
  return false;
}


/**
 * @brief the expertise-go "peep connect" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::peep_connect0(double** basic_instinct) const
{
  
  //Location last_move =  goban->lastMove();
  
  this->peep_connect0(NORTH_,basic_instinct);
  this->peep_connect0(SOUTH_,basic_instinct);
  this->peep_connect0(EAST_,basic_instinct);
  this->peep_connect0(WEST_,basic_instinct);
}


/**
 * @brief the expertise-go "atari extend"
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param save_atari the safeguard of the found move 
 * @return if the expertise has been matched
 */ 
bool MoGo::BasicInstinctFast::atari_extend0(int dir, double** basic_instinct, 
					    Location* save_atari) const 
{
  Location neighbour;

  switch(dir)
    {
    case NORTH_ : 
      neighbour = this->last_move+FBS_handler::directions[1];
      break;
      
    case SOUTH_ :
      neighbour = this->last_move+FBS_handler::directions[3];
      break;
      
    case WEST_ :
      neighbour = this->last_move+FBS_handler::directions[0];
      break;

    case EAST_ :
      neighbour = this->last_move+FBS_handler::directions[2];
      break;
      
    default:
      assert(0);
    }
  
  if(not FBS_handler::isOnBoard(neighbour)) return false;
  if(fastboard->position(neighbour)==FB_EMPTY) return false;
  if(fastboard->position(this->last_move)==fastboard->position(neighbour)) return false;

  Location extend;

  if(not fastboard->isAtari(neighbour))
    return false;
  
  extend = fastboard->getOneLibertyOfString(neighbour); 
    
  if(save_atari) *save_atari = extend;
  if(basic_instinct) (*basic_instinct)[extend] += bonus_atari_extend; 
  
  //is already seen ?
  
  if(debug_atari_extend)
    {
      Goban goban(fastboard->getGobanKey());

      std::cout<<"atari extend"<<std::endl;
      goban.textModeShowGoban(extend);
      std::cout<<std::endl;
      
      switch(dir)
	{
	case NORTH_ : std::cout<<"north"<<std::endl; break;
	case SOUTH_ : std::cout<<"south"<<std::endl; break;
	case EAST_ : std::cout<<"east"<<std::endl; break;
	case WEST_ : std::cout<<"west"<<std::endl; break;
	default : assert(0);
	}
    }
      
  return true;
}


/**
 * @brief the expertise-go "atari extend" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::atari_extend0(double** basic_instinct) const
{
  assert(fastboard);
  
  //Location last_move = goban->lastMove();
  
  this->atari_extend0(NORTH_,basic_instinct);
  this->atari_extend0(SOUTH_,basic_instinct);
  this->atari_extend0(EAST_,basic_instinct);
  this->atari_extend0(WEST_,basic_instinct);

}




/**
 * @brief the expertise-go "atari extend" (a new version)
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param save_atari the safeguard of the found move 
 * @return if the expertise has been matched
 */ 
bool MoGo::BasicInstinctFast::atari_extend1(int dir, double** basic_instinct, 
					    Location* save_atari) const 
{
  Location neighbour;

  switch(dir)
    {
    case NORTH_ : 
      neighbour = this->last_move+FBS_handler::directions[1];
      break;
      
    case SOUTH_ :
      neighbour = this->last_move+FBS_handler::directions[3];
      break;
      
    case WEST_ :
      neighbour = this->last_move+FBS_handler::directions[0];
      break;

    case EAST_ :
      neighbour = this->last_move+FBS_handler::directions[2];
      break;
      
    default:
      assert(0);
    }
  
  if(not FBS_handler::isOnBoard(neighbour)) return false;
  if(fastboard->position(neighbour)==FB_EMPTY) return false;
  if(fastboard->position(this->last_move)==fastboard->position(neighbour)) return false;

  Location extend;

  if(not fastboard->isAtari2(neighbour))
    return false;
  
  extend = fastboard->getOneLibertyOfString(neighbour); 
    
  if(save_atari) *save_atari = extend;
  if(basic_instinct) (*basic_instinct)[extend] += bonus_atari_extend; 
  
  //is already seen ?
  
  if(debug_atari_extend)
    {
      Goban goban(fastboard->getGobanKey());

      std::cout<<"atari extend"<<std::endl;
      goban.textModeShowGoban(extend);
      std::cout<<std::endl;
      
      switch(dir)
	{
	case NORTH_ : std::cout<<"north"<<std::endl; break;
	case SOUTH_ : std::cout<<"south"<<std::endl; break;
	case EAST_ : std::cout<<"east"<<std::endl; break;
	case WEST_ : std::cout<<"west"<<std::endl; break;
	default : assert(0);
	}
    }
      
  return true;
}



/**
 * @brief the expertise-go "atari extend" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::atari_extend1(double** basic_instinct) const
{
  assert(fastboard);
  
  //Location last_move = goban->lastMove();
  
  this->atari_extend1(NORTH_,basic_instinct);
  this->atari_extend1(SOUTH_,basic_instinct);
  this->atari_extend1(EAST_,basic_instinct);
  this->atari_extend1(WEST_,basic_instinct);

}


/**
 * @brief the expertise-go "eat": (capture stones) 
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param save_atari the safeguard of the found move 
 * @return if the expertise has been matched
 */ 
bool MoGo::BasicInstinctFast::eat0(double** basic_instinct, 
				   Location* gm) const
{
  if(fastboard->liberty(last_move)==1)
    {
      Location place_eat = fastboard->getOneLibertyOfString(last_move); 
      if(gm) *gm = place_eat;
      if(basic_instinct) (*basic_instinct)[place_eat] += bonus_eat; 
      
      if(debug_eat)
	{
	  Goban goban(fastboard->getGobanKey());
	  
	  std::cout<<"eat"<<std::endl;
	  goban.textModeShowGoban(place_eat);
	  std::cout<<std::endl;
	}
      
      return true;
    }
  
  return false;
}


/**
 * @brief the expertise-go "threat": (a threat of a capture) 
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param th0 the safeguard of the first threat 
 * @param th1 the safeguard of the second threat 
 * @return if the expertise has been matched
 */ 
bool MoGo::BasicInstinctFast::threat0(double** basic_instinct, 
				      Location* th0, Location* th1) const 
{
  if(fastboard->liberty(last_move)==2)
    {
      list<Location> liberties;
      
      int nb_liberties = fastboard->getLibertiesOfString(last_move,liberties,2);
      assert(nb_liberties==2);
      
      const Location& loc1 = liberties.front();
      const Location& loc2 = liberties.back();
      
      if(th0) *th0 = loc1;
      if(th1) *th1 = loc2;
      
      if(basic_instinct) (*basic_instinct)[loc1] += bonus_threat; 
      if(basic_instinct) (*basic_instinct)[loc2] += bonus_threat;
      
      if(debug_threat)
	{
	  Goban goban(fastboard->getGobanKey());
	  
	  std::cout<<"threat"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(loc1);
	  goban.textModeShowPosition(loc2);	
	  std::cout<<std::endl;
	}
      
      return true;
    }
  
  return false;
}


/**
 * @brief the expertise-go "reduce_liberties": (reducing liberties of the enemy group) 
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param max_liberties the maximal number of liberties ot the enemy group (else too free)  
 * @return if the expertise has been matched
 */ 
bool MoGo::BasicInstinctFast::reduce_liberties0(double** basic_instinct, int max_liberties) 
  const 
{
  int nb_liberties = fastboard->liberty(last_move);
  
  if(nb_liberties>max_liberties) return false;
  
  list<int> free_intersections;
  fastboard->getLibertiesOfString(last_move,free_intersections);
  
  assert(nb_liberties==(int)free_intersections.size());
  
  double bonus;
  
  switch(nb_liberties)
    {
    case 1 : bonus = bonus_eat; break; 
    case 2 : bonus = bonus_threat; break;
    default : bonus = bonus_reduce_liberties; 
    }
  
  list<int>::const_iterator i_fi = free_intersections.begin();
  while(i_fi!=free_intersections.end())
    {
      if(basic_instinct) (*basic_instinct)[*i_fi]+=bonus;
      i_fi++;
    }
  
  if(debug_reduce_liberties)
    {
      Goban goban(fastboard->getGobanKey());
      
      std::cout<<"reduce liberties ";
      if(nb_liberties==1)  std::cout<<"(eat)";
      if(nb_liberties==2)  std::cout<<"(threat)";
      std::cout<<std::endl;
      
      goban.textModeShowGoban(last_move);
      std::cout<<std::endl;
      
      i_fi = free_intersections.begin();
      while(i_fi!=free_intersections.end())
	{
	  goban.textModeShowPosition(*i_fi);
	  i_fi++;
	}
     	
      std::cout<<std::endl;
    }

  return true;
}

/**
 * @brief the expertise-go "increase liberties": (increase liberties of a stone by nobi) 
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm the safeguard of the found move 
 * @return if the expertise has been matched
 */ 

//the pattern
/*************
 *           * 
 * . . @ . . *
 * .(@)0 X . *
 * . . . . . *
 *           *
 *************/

bool MoGo::BasicInstinctFast::increase_liberties0(int dir, double** basic_instinct,
						  Location* gm) const
{
  int last_color_stone = fastboard->position(last_move); 
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;
  
  Location place;
  Location place_left;
  Location place_right;
  Location good_move;

  switch(dir)
    {
    case NORTH_ :
      place = last_move + FBS_handler::directions[1];
      place_left = last_move + FBS_handler::directions[5];
      place_right = last_move + FBS_handler::directions[4];
      good_move = last_move + 2*FBS_handler::directions[1];
      break;
      
    case SOUTH_ :
      place = last_move + FBS_handler::directions[3];
      place_left = last_move + FBS_handler::directions[6];
      place_right = last_move + FBS_handler::directions[7];
      good_move = last_move + 2*FBS_handler::directions[3];
      break;

    case EAST_ :
      place = last_move + FBS_handler::directions[2];
      place_left = last_move + FBS_handler::directions[4];
      place_right = last_move + FBS_handler::directions[7];
      good_move = last_move + 2*FBS_handler::directions[2];
      break;
      
    case WEST_ :
      place = last_move + FBS_handler::directions[0];
      place_left = last_move + FBS_handler::directions[5];
      place_right = last_move + FBS_handler::directions[6];
      good_move = last_move + 2*FBS_handler::directions[0];
      break;

    default : assert(0);
    }
  
  if(fastboard->position(place)!=future_color_stone) return false;
  if(fastboard->position(good_move)!=FB_EMPTY) return false;
  
  if(((fastboard->position(place_left)==last_color_stone)
      &&(fastboard->position(place_right)!=future_color_stone))
     ||((fastboard->position(place_right)==last_color_stone)
	&&(fastboard->position(place_left)!=future_color_stone)))
    {

      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move]+=bonus_increase_liberties;
      
      if(debug_increase_liberties)
	{
	  Goban goban(fastboard->getGobanKey());
	  
	  std::cout<<"increase liberties "<<std::endl;
	  
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move);
	  std::cout<<std::endl;
	}

      return true;
    }
  
  return false;
}



/**
 * @brief the expertise-go "increase liberties" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::increase_liberties0(double** basic_instinct) const
{
  assert(fastboard);
  
  //Location last_move = goban->lastMove();
  
  this->increase_liberties0(NORTH_,basic_instinct);
  this->increase_liberties0(SOUTH_,basic_instinct);
  this->increase_liberties0(EAST_,basic_instinct);
  this->increase_liberties0(WEST_,basic_instinct);
}



/**
 * @brief the expertise-go "eat or to be eaten": (eat a group of enemy stones before the opponent)i) 
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm the safeguard of the found move 
 * @return if the expertise has been matched
 */ 
bool MoGo::BasicInstinctFast::eat_or_to_be_eaten0(int dir, double** basic_instinct, 
						  Location* gm) const 
{
  Location neighbour;


  switch(dir)
    {
    case NORTH_ : 
      neighbour = this->last_move+FBS_handler::directions[1];
      break;
      
    case SOUTH_ :
      neighbour = this->last_move+FBS_handler::directions[3];
      break;
      
    case WEST_ :
      neighbour = this->last_move+FBS_handler::directions[0];
      break;
      
    case EAST_ :
      neighbour = this->last_move+FBS_handler::directions[2];
      break;
      
    default:
      assert(0);
    }
  

  //IMPROVEMENT: avoid to capture a stone in the corner
  //TO PREFER: flee
  
  if(not FBS_handler::isOnBoard(neighbour)) return false;
  if(fastboard->position(neighbour)==FB_EMPTY) return false;
  if(fastboard->position(this->last_move)==fastboard->position(neighbour)) return false;


  Location savingMove;
  if(fastboard->isUnderAtariStringSavableByEatingMove(neighbour,savingMove))
    {
      if(gm) *gm = savingMove;
      if(basic_instinct) (*basic_instinct)[savingMove] += bonus_eat_or_to_be_eaten; 
      
      if(debug_eat_or_to_be_eaten)
	{
	  Goban goban(fastboard->getGobanKey());
	  
	  std::cout<<"eat or to be eaten"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(savingMove);
	  std::cout<<std::endl;
	}
      
      
      return true;
    }
  
  //if(fastboard->liberty(neighbour)==1)
  
  return false;
}


/**
 * @brief the expertise-go "eat or to be eaten" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::eat_or_to_be_eaten0(double** basic_instinct) const
{
  assert(fastboard);
  
  //Location last_move = goban->lastMove();
  
  this->eat_or_to_be_eaten0(NORTH_,basic_instinct);
  this->eat_or_to_be_eaten0(SOUTH_,basic_instinct);
  this->eat_or_to_be_eaten0(EAST_,basic_instinct);
  this->eat_or_to_be_eaten0(WEST_,basic_instinct);
}


/**
 * @brief the expertise-go "connect": (connect 2 groups) 
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm the safeguard of the found move 
 * @return if the expertise has been matched
 */  

//the pattern
/********
 * O @  *
 * @ X  *
 * .(O) * 
 ********/

bool MoGo::BasicInstinctFast::connect0(int dir, double** basic_instinct, 
				       Location* gm) const 
{

  int last_color_stone = fastboard->position(last_move); 
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;

  Location place;
  Location place_left;
  Location place_right;
  Location disconnection_left;
  Location disconnection_right;
  Location good_move;



  switch(dir)
    {
    case NORTH_ :
      good_move = last_move+FBS_handler::directions[1];
      place_left = last_move+FBS_handler::directions[5];
      place_right = last_move+FBS_handler::directions[4];
      place = good_move+FBS_handler::directions[1];
      disconnection_left = good_move+FBS_handler::directions[5];
      disconnection_right = good_move+FBS_handler::directions[4];
      break;

    case SOUTH_ :
      good_move = last_move+FBS_handler::directions[3];
      place_left = last_move+FBS_handler::directions[6];
      place_right = last_move+FBS_handler::directions[7];
      place = good_move+FBS_handler::directions[3];
      disconnection_left = good_move+FBS_handler::directions[6];
      disconnection_right = good_move+FBS_handler::directions[7];
      break;

    case EAST_ :
      good_move = last_move+FBS_handler::directions[2];
      place_left = last_move+FBS_handler::directions[4];
      place_right = last_move+FBS_handler::directions[7];
      place = good_move+FBS_handler::directions[2];
      disconnection_left = good_move+FBS_handler::directions[4];
      disconnection_right = good_move+FBS_handler::directions[7];
      break;

    case WEST_ :
      good_move = last_move+FBS_handler::directions[0];
      place_left = last_move+FBS_handler::directions[6];
      place_right = last_move+FBS_handler::directions[5];
      place = good_move+FBS_handler::directions[0];
      disconnection_left = good_move+FBS_handler::directions[6];
      disconnection_right = good_move+FBS_handler::directions[5];
      break;

    default:
      assert(0);
    }

  if(FBS_handler::isOnBoard(disconnection_left))
    {
      if(fastboard->position(place)==future_color_stone)
	if(fastboard->position(place_left)==future_color_stone)
	  if(fastboard->position(disconnection_left)==last_color_stone)
	    if(fastboard->position(good_move)==FB_EMPTY)
	      {
		if(gm) *gm = good_move;
		if(basic_instinct) (*basic_instinct)[good_move] += bonus_connect; 
		
		if(debug_connect)
		  {
		    Goban goban(fastboard->getGobanKey());
		    
		    std::cout<<"connect"<<std::endl;
		    goban.textModeShowGoban(last_move);
		    std::cout<<std::endl;
		    
		    goban.textModeShowPosition(good_move);
		    std::cout<<std::endl;
		  }

		return true;
	      }
    }
  
  if(FBS_handler::isOnBoard(disconnection_right))
    {
      if(fastboard->position(place)==future_color_stone)
	if(fastboard->position(place_right)==future_color_stone)
	  if(fastboard->position(disconnection_right)==last_color_stone)
	    if(fastboard->position(good_move)==FB_EMPTY)
	      {
		if(gm) *gm = good_move;
		if(basic_instinct) (*basic_instinct)[good_move] += bonus_connect; 
		
		if(debug_connect)
		  {
		    Goban goban(fastboard->getGobanKey());
		    
		    std::cout<<"connect"<<std::endl;
		    goban.textModeShowGoban(last_move);
		    std::cout<<std::endl;
		    
		    goban.textModeShowPosition(good_move);
		    std::cout<<std::endl;
		  }
	      }
      
      return true;
    }
  
  return false;
}


/**
 * @brief the expertise-go "connect" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::connect0(double** basic_instinct) const
{
  assert(fastboard);
  
  //Location last_move = goban->lastMove();
  
  this->connect0(NORTH_,basic_instinct);
  this->connect0(SOUTH_,basic_instinct);
  this->connect0(EAST_,basic_instinct);
  this->connect0(WEST_,basic_instinct);
}




/**
 * @brief the expertise-go "eat or to be eaten" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
bool MoGo::BasicInstinctFast::connect_if_dead_virtual_connection0(list<Location>& liberties1,
								  list<Location>& liberties2,
								  double** basic_instinct,
								  Location* gm) 
  const 
{
  if(liberties1.empty()||liberties2.empty())
    return false;
  
  list<Location> result;
  ListOperators::intersection(liberties1,liberties2,result);
  if(result.size()==1)
    {
      Location& good_move = result.front();
      
      if(gm) *gm = good_move;
      if(basic_instinct) 
	(*basic_instinct)[good_move]+=bonus_connect_if_dead_virtual_connection;
      
      if(debug_connect_if_dead_virtual_connection)
	{
	  Goban goban(fastboard->getGobanKey());
	  
	  std::cout<<"connect_if_dead_virtual_connection"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move);
	  std::cout<<std::endl;
	}
      
      return true;
    }
  
  
  return false;
}


/**
 * @brief the expertise-go "connect if dead virtual connection" 
 *        (connect 2 groups which risk to be separated) 
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @return if the expertise has been matched
 */  
void MoGo::BasicInstinctFast::connect_if_dead_virtual_connection0(double** basic_instinct) 
  const 
{
  int future_color_stone = (fastboard->position(last_move)==FB_BLACK) ? FB_WHITE : FB_BLACK;  


  Location locN = this->last_move+FBS_handler::directions[1];
  Location locS = this->last_move+FBS_handler::directions[3];
  Location locW = this->last_move+FBS_handler::directions[0];
  Location locE = this->last_move+FBS_handler::directions[2];
  
  list<Location> libertiesN;
  list<Location> libertiesS;
  list<Location> libertiesW;
  list<Location> libertiesE;
  
  
  if((FBS_handler::isOnBoard(locN)) 
     &&(fastboard->position(locN)==future_color_stone))
    fastboard->getLibertiesOfString(locN,libertiesN);
  
  if((FBS_handler::isOnBoard(locS))
     &&(fastboard->position(locS)==future_color_stone)
     &&((libertiesN.empty())||(fastboard->chainId[locN]!=fastboard->chainId[locS])))
    fastboard->getLibertiesOfString(locS,libertiesS);
  
  if((FBS_handler::isOnBoard(locW)) 
     &&(fastboard->position(locW)==future_color_stone)
     &&((libertiesN.empty())||(fastboard->chainId[locN]!=fastboard->chainId[locW]))
     &&((libertiesS.empty())||(fastboard->chainId[locS]!=fastboard->chainId[locW])))
    fastboard->getLibertiesOfString(locW,libertiesW);
  
  if((FBS_handler::isOnBoard(locE))
     &&(fastboard->position(locE)==future_color_stone)
     &&((libertiesN.empty())||(fastboard->chainId[locN]!=fastboard->chainId[locE]))
     &&((libertiesS.empty())||(fastboard->chainId[locS]!=fastboard->chainId[locE]))
     &&((libertiesW.empty())||(fastboard->chainId[locW]!=fastboard->chainId[locE])))
    fastboard->getLibertiesOfString(locE,libertiesE);
  
  
  connect_if_dead_virtual_connection0(libertiesN, libertiesS, basic_instinct);
  connect_if_dead_virtual_connection0(libertiesN, libertiesW, basic_instinct);
  connect_if_dead_virtual_connection0(libertiesN, libertiesE, basic_instinct);

  connect_if_dead_virtual_connection0(libertiesS, libertiesW, basic_instinct);
  connect_if_dead_virtual_connection0(libertiesS, libertiesE, basic_instinct);

  connect_if_dead_virtual_connection0(libertiesW, libertiesE, basic_instinct);
}


/**
 * @brief the expertise-go "prevent connection" 
 *        (prevent a connexion of 2 groups in playing at the location before) 
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @return if the expertise has been matched
 */ 
bool MoGo::BasicInstinctFast::prevent_connection0(int dir, double** basic_instinct,
						  Location* gm) 
  const 
{
  Location neighbour;
  
  switch(dir)
    {
    case NORTH_ : neighbour = this->last_move+FBS_handler::directions[1]; break;
    case SOUTH_ : neighbour = this->last_move+FBS_handler::directions[3]; break;
    case EAST_ : neighbour = this->last_move+FBS_handler::directions[2]; break;
    case WEST_ : neighbour = this->last_move+FBS_handler::directions[0]; break;
    default : assert(0);
    }
  
  if(not (FBS_handler::isOnBoard(neighbour)))
    return false;
  
  if(fastboard->position(neighbour)!=FB_EMPTY)
    return false;
  
  
  int last_color_stone = fastboard->position(last_move);
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;
  
  int nb_enemy = fastboard->getNbNeighbours(neighbour,last_color_stone);
  int nb_friends = fastboard->getNbNeighbours(neighbour,future_color_stone);
  
  if((nb_friends>=1)&&(nb_enemy>=2))  //nb_friends>1 BUG ????? >= seems more logical
    {
      if(gm) *gm = neighbour;
      if(basic_instinct) 
	(*basic_instinct)[neighbour]+=bonus_prevent_connection;
      
      if(debug_prevent_connection)
	{
	  Goban goban(fastboard->getGobanKey());
	  
	  std::cout<<"prevent connection"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(neighbour);
	  std::cout<<std::endl;
	}

      return true;
    }

  return false;
  
}


/**
 * @brief the expertise-go "prevent connection" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::prevent_connection0(double** basic_instinct) const
{
  
  //Location last_move = goban->lastMove();
  
  this->prevent_connection0(NORTH_,basic_instinct);
  this->prevent_connection0(SOUTH_,basic_instinct);
  this->prevent_connection0(EAST_,basic_instinct);
  this->prevent_connection0(WEST_,basic_instinct);
}



/**
 * @brief the expertise-go "one point jump" (or extension "tobi" 
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm the safeguard of the found move 
 * @return if the expertise has been matched
 */  
bool MoGo::BasicInstinctFast::one_point_jump0(int dir, double** basic_instinct, Location* gm)
  const
{
  /*******
   * .X. *  @ is the before last move.
   * EEE *  E means empty
   * .@. *  X is the "good move"
   *******/
  
  Location place_center;
  Location place_right;
  Location place_left;
  Location good_move;
  
  switch(dir)
    {
    case NORTH_ :
      place_center = this->before_last_move+FBS_handler::directions[1]; 
      place_left = this->before_last_move+FBS_handler::directions[5]; 
      place_right = this->before_last_move+FBS_handler::directions[4]; 
      good_move = place_center+FBS_handler::directions[1];   
      break;

    case SOUTH_ :
      place_center = this->before_last_move+FBS_handler::directions[3]; 
      place_left = this->before_last_move+FBS_handler::directions[6]; 
      place_right = this->before_last_move+FBS_handler::directions[7]; 
      good_move = place_center+FBS_handler::directions[3];   
      break;

    case EAST_ :
      place_center = this->before_last_move+FBS_handler::directions[2]; 
      place_left = this->before_last_move+FBS_handler::directions[4]; 
      place_right = this->before_last_move+FBS_handler::directions[7]; 
      good_move = place_center+FBS_handler::directions[2];   
      break;
      
    case WEST_ :
      place_center = this->before_last_move+FBS_handler::directions[0]; 
      place_left = this->before_last_move+FBS_handler::directions[5]; 
      place_right = this->before_last_move+FBS_handler::directions[6]; 
      good_move = place_center+FBS_handler::directions[0];   
      break;

    default : assert(0);
    }
  
  if(not FBS_handler::isOnBoard(good_move)) return false;
  if(FBS_handler::isOnBorder(good_move)) return false;
  if(fastboard->position(place_center)!=FB_EMPTY) return false;
  if(fastboard->position(place_right)!=FB_EMPTY) return false;
  if(fastboard->position(place_left)!=FB_EMPTY) return false;
  if(fastboard->position(good_move)!=FB_EMPTY) return false;
  
  if(gm) *gm = good_move;
  if(basic_instinct) (*basic_instinct)[good_move] += bonus_one_point_jump;
  
  if(debug_one_point_jump)
    {
      Goban goban(fastboard->getGobanKey());
      
      std::cout<<"one-point jump"<<std::endl;
      goban.textModeShowGoban(last_move);
      std::cout<<std::endl;
      
      std::cout<<"before last move: ";
      goban.textModeShowPosition(before_last_move);
      std::cout<<std::endl;
      goban.textModeShowPosition(good_move);
      std::cout<<std::endl;
    }


  return true;
}


/**
 * @brief the expertise-go "one point jump" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void  MoGo::BasicInstinctFast::one_point_jump0(double** basic_instinct) const
{
  if(FBS_handler::isOnBorder(this->before_last_move))
    return;

  this->one_point_jump0(NORTH_,basic_instinct);
  this->one_point_jump0(SOUTH_,basic_instinct);
  this->one_point_jump0(EAST_,basic_instinct);
  this->one_point_jump0(WEST_,basic_instinct);
}


/**
 * @brief the expertise-go "kogeima" or extension "keima" (jump of knight in chess) 
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm1 the safeguard of the first found move 
 * @param gm2 the safeguard of the second found move 
 * @return if the expertise has been matched
 */  
bool MoGo::BasicInstinctFast::kogeima0(int dir, double** basic_instinct, Location* gm1,
				       Location* gm2) 
  const
{
  /*******
   * X.. *  @ is the before last move.
   * EEE *  E means empty
   * .@. *  X is the "good move"
   *******/
  
  Location place_center;
  Location place_right;
  Location place_left;
  Location good_move_left;
  Location good_move_right;

  bool is_pattern = false;
  
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16

  switch(dir)
    {
    case NORTH_ :
      place_center = this->before_last_move+FBS_handler::directions[1]; 
      place_left = this->before_last_move+FBS_handler::directions[5]; 
      place_right = this->before_last_move+FBS_handler::directions[4]; 
      good_move_left = place_center+FBS_handler::directions[5];
      good_move_right = place_center+FBS_handler::directions[4];   
      break;

    case SOUTH_ :
      place_center = this->before_last_move+FBS_handler::directions[3]; 
      place_left = this->before_last_move+FBS_handler::directions[6]; 
      place_right = this->before_last_move+FBS_handler::directions[7]; 
      good_move_left = place_center+FBS_handler::directions[6];   
      good_move_right = place_center+FBS_handler::directions[7];   
      break;

    case EAST_ :
      place_center = this->before_last_move+FBS_handler::directions[2]; 
      place_left = this->before_last_move+FBS_handler::directions[4]; 
      place_right = this->before_last_move+FBS_handler::directions[7]; 
      good_move_left = place_center+FBS_handler::directions[4];   
      good_move_right = place_center+FBS_handler::directions[7];   
      break;
      
    case WEST_ :
      place_center = this->before_last_move+FBS_handler::directions[0]; 
      place_left = this->before_last_move+FBS_handler::directions[5]; 
      place_right = this->before_last_move+FBS_handler::directions[6]; 
      good_move_left = place_center+FBS_handler::directions[5];   
      good_move_right = place_center+FBS_handler::directions[6];   
      break;

    default : assert(0);
    }
  


  if(FBS_handler::isOnBoard(good_move_left))
    if(not FBS_handler::isOnBorder(good_move_left)) 
      if(fastboard->position(place_center)==FB_EMPTY)
	if(fastboard->position(place_left)==FB_EMPTY) 
	  {
	    if(gm1) *gm1 = good_move_left;
	    if(basic_instinct) (*basic_instinct)[good_move_left] += bonus_kogeima;
	    
	    if(debug_kogeima)
	      {
		Goban goban(fastboard->getGobanKey());
		
		std::cout<<"kogeima"<<std::endl;
		goban.textModeShowGoban(last_move);
		std::cout<<std::endl;
		
		std::cout<<"before last move: ";
		goban.textModeShowPosition(before_last_move);
		std::cout<<std::endl;
		goban.textModeShowPosition(good_move_left);
		std::cout<<std::endl;
	      }

	    is_pattern = true;
	  }
  
  
  if(FBS_handler::isOnBoard(good_move_right))
    if(not FBS_handler::isOnBorder(good_move_right)) 
      if(fastboard->position(place_center)==FB_EMPTY)
	if(fastboard->position(place_right)==FB_EMPTY) 
	  {
	    if(gm2) *gm2 = good_move_right;
	    if(basic_instinct) (*basic_instinct)[good_move_right] += bonus_kogeima;
	    
	    if(debug_kogeima)
	      {
		Goban goban(fastboard->getGobanKey());
		
		std::cout<<"kogeima"<<std::endl;
		goban.textModeShowGoban(last_move);
		std::cout<<std::endl;
		
		std::cout<<"before last move: ";
		goban.textModeShowPosition(before_last_move);
		std::cout<<std::endl;
		goban.textModeShowPosition(good_move_right);
		std::cout<<std::endl;
	      }

	    is_pattern = true;
	  }
  
  return is_pattern;

}


/**
 * @brief the expertise-go "kogeima" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void  MoGo::BasicInstinctFast::kogeima0(double** basic_instinct) const
{
  if(FBS_handler::isOnBorder(this->before_last_move))
    return;

  
  this->kogeima0(NORTH_,basic_instinct);
  this->kogeima0(SOUTH_,basic_instinct);
  this->kogeima0(EAST_,basic_instinct);
  this->kogeima0(WEST_,basic_instinct);
}


/**
 * @brief the expertise-gos "tsuke hane" AND "thrust block"
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm1 the safeguard of the first found move 
 * @param gm2 the safeguard of the second found move 
 * @return if the expertise has been matched
 */  
//tsuke hane AND thrust block
bool MoGo::BasicInstinctFast::tsuke_hane0(int dir, double** basic_instinct, 
					  int* gm1, int* gm2) const 
{
  Location place_left;
  bool is_left = false;
  Location place_right;
  bool is_right = false;
  Location good_move1;
  Location good_move2;
  bool is_thrust_block = true;
  
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  
  
  switch(dir)
    {
    case SOUTH_ :
    case NORTH_ :
      place_left = this->last_move+FBS_handler::directions[0];
      place_right = this->last_move+FBS_handler::directions[2];
      good_move1 = this->last_move+FBS_handler::directions[1];
      good_move2 = this->last_move+FBS_handler::directions[3];
      break;
      
    case WEST_ :
    case EAST_ :
      place_left = this->last_move+FBS_handler::directions[1];
      place_right = this->last_move+FBS_handler::directions[3];
      good_move1 = this->last_move+FBS_handler::directions[0];
      good_move2 = this->last_move+FBS_handler::directions[2];
      break;
      
    default:
      assert(0);
      
    }
  
  if(FBS_handler::isOnBoard(place_left))
    if(fastboard->position(place_left)!=fastboard->position(this->last_move))
      if(fastboard->position(place_left)!=EMPTY)
	is_left = true;
  
  if(FBS_handler::isOnBoard(place_right))
    if(fastboard->position(place_right)!=fastboard->position(this->last_move))
      if(fastboard->position(place_right)!=EMPTY)
	is_right = true;
  
  is_thrust_block = is_right && is_left;

  if(is_thrust_block)
    {
      if(FBS_handler::isOnBoard(good_move1))
	{
	  if(gm1) *gm1 = good_move1;
	  if(basic_instinct) (*basic_instinct)[good_move1] += bonus_thrust_block;
	}
      
      if(FBS_handler::isOnBoard(good_move2))
	{
	  if(gm2) *gm2 = good_move2;
	  if(basic_instinct) (*basic_instinct)[good_move2] += bonus_thrust_block;
	}
      
      if(debug_thrust_block)
	{
	  Goban goban(fastboard->getGobanKey());
	  
	  std::cout<<"thrust block"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  switch(dir)
	    {
	    case NORTH_ : std::cout<<"north"<<std::endl; break;
	    case SOUTH_ : std::cout<<"south"<<std::endl; break;
	    case EAST_ : std::cout<<"east"<<std::endl; break;
	    case WEST_ : std::cout<<"west"<<std::endl; break;
	    default : assert(0);
	    }
	  
	  if(not goban.isOutsideOfGoban(place_left)) goban.textModeShowPosition(place_left);
	  if(not goban.isOutsideOfGoban(place_right)) goban.textModeShowPosition(place_right);
	  if(not goban.isOutsideOfGoban(good_move1)) goban.textModeShowPosition(good_move1);
	  if(not goban.isOutsideOfGoban(good_move2)) goban.textModeShowPosition(good_move2);
	}
      
      return true;
    }
  
  if(is_right || is_left)
    {
      
      //correctif  //CAREFUL perhaps bug in the old version
      //the good test is if(NOT goban->isWithoutFriend4(LAST_MOVE))
      //and NOT  if(goban->isWithoutFriend4(place_right))

      int last_color_stone = fastboard->position(last_move);
      if(fastboard->getNbNeighbours(this->last_move,last_color_stone)>0) //not alone
	return false;
      
      //end correctif

      
      if(FBS_handler::isOnBoard(good_move1))
	{
	  if(gm1) *gm1 = good_move1;
	  if(basic_instinct) (*basic_instinct)[good_move1] += bonus_tsuke_hane;
	}
      
      if(FBS_handler::isOnBoard(good_move2))
	{
	  if(gm2) *gm2 = good_move2;
	  if(basic_instinct) (*basic_instinct)[good_move2] += bonus_tsuke_hane;
	}
      
      if(debug_tsuke_hane)
	{
	  Goban goban(fastboard->getGobanKey());
	  
	  std::cout<<"tsuke hane"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  switch(dir)
	    {
	    case NORTH_ : std::cout<<"north"<<std::endl; break;
	    case SOUTH_ : std::cout<<"south"<<std::endl; break;
	    case EAST_ : std::cout<<"east"<<std::endl; break;
	    case WEST_ : std::cout<<"west"<<std::endl; break;
	    default : assert(0);
	    }
	  
	  if(not goban.isOutsideOfGoban(place_left)) goban.textModeShowPosition(place_left);
	  if(not goban.isOutsideOfGoban(place_right)) goban.textModeShowPosition(place_right);
	  if(not goban.isOutsideOfGoban(good_move1)) goban.textModeShowPosition(good_move1);
	  if(not goban.isOutsideOfGoban(good_move2)) goban.textModeShowPosition(good_move2);
	}
      
      return true;
    }
  
  
  return false;
}

/**
 * @brief the expertise-go "tsuke hane" AND "thrust block" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::tsuke_hane0(double** basic_instinct) const
{ 
  //Location last_move = goban->lastMove();
  
  this->tsuke_hane0(NORTH_,basic_instinct);
  this->tsuke_hane0(EAST_,basic_instinct);
}

/**
 * @brief the expertise-go "kosumi tsuke stretch" 
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm11 the safeguard of the first found move 
 * @param gm12 the safeguard of the second found move 
 * @param gm21 the safeguard of the third found move 
 * @param gm22 the safeguard of the fourth found move 
 * @return if the expertise has been matched
 */  
bool MoGo::BasicInstinctFast::kosumi_tsuke_stretch0(int dir, double** basic_instinct, 
						    int* gm11, int* gm12, int* gm21, int* gm22) const 
{
  Location place_left_up;
  Location place_left_down;
  Location place_right;
  Location good_move11;
  Location good_move12;
  Location good_move21;
  Location good_move22;
  bool is_pattern = false;
  
  
  
  switch(dir)
    {
    case EAST_ :
      place_left_up = last_move+FBS_handler::directions[5];
      place_left_down = last_move+FBS_handler::directions[6];
      place_right = last_move+FBS_handler::directions[2];
      good_move11 = last_move+FBS_handler::directions[4];
      good_move12 = last_move+FBS_handler::directions[4]+FBS_handler::directions[1];//+FBS_handler::directions[13];
      good_move21 = last_move+FBS_handler::directions[7];
      good_move22 = last_move+FBS_handler::directions[7]+FBS_handler::directions[3];//FBS_handler::directions[16];
      break;        

    case WEST_ :
      place_left_up = last_move+FBS_handler::directions[4];
      place_left_down = last_move+FBS_handler::directions[7];
      place_right = last_move+FBS_handler::directions[0];
      good_move11 = last_move+FBS_handler::directions[5];
      good_move12 = last_move+FBS_handler::directions[5]+FBS_handler::directions[1];//FBS_handler::directions[12];
      good_move21 = last_move+FBS_handler::directions[6];
      good_move22 = last_move+FBS_handler::directions[6]+FBS_handler::directions[3];//FBS_handler::directions[17];
      break;
      
      //  12 9  13
      //19 5  1  4 14
      //8  0 [*] 2 10
      //18 6  3  7 15
      //  17 11 16


    case NORTH_ :
      place_left_up = last_move+FBS_handler::directions[6];
      place_left_down = last_move+FBS_handler::directions[7];
      place_right = last_move+FBS_handler::directions[1];
      good_move11 = last_move+FBS_handler::directions[5];
      good_move12 = last_move+FBS_handler::directions[5]+FBS_handler::directions[0];//FBS_handler::directions[19];
      good_move21 = last_move+FBS_handler::directions[4];
      good_move22 = last_move+FBS_handler::directions[4]+FBS_handler::directions[2];//FBS_handler::directions[14];
      break;
      
    case SOUTH_ :
      place_left_up = last_move+FBS_handler::directions[5];
      place_left_down = last_move+FBS_handler::directions[4];
      place_right = last_move+FBS_handler::directions[3];
      good_move11 = last_move+FBS_handler::directions[6];
      good_move12 = last_move+FBS_handler::directions[6]+FBS_handler::directions[0];//FBS_handler::directions[18];
      good_move21 = last_move+FBS_handler::directions[7];
      good_move22 = last_move+FBS_handler::directions[7]+FBS_handler::directions[2];//FBS_handler::directions[15];
      break;
      
    default:
      assert(0);
    }
  
  
  
  if((not FBS_handler::isOnBoard(place_right))
     ||(fastboard->position(place_right)==EMPTY)
     ||(fastboard->position(place_right)==fastboard->position(last_move)))
    return false;
  
  if(FBS_handler::isOnBoard(place_left_up))
    if(fastboard->position(place_left_up)==fastboard->position(last_move))
      {
	if(gm11) *gm11 = good_move11;
	
	if(basic_instinct) (*basic_instinct)[good_move11] += bonus_kosumi_tsuke_stretch;
	
	if(FBS_handler::isOnBoard(good_move12))
	  {
	    if(gm12) *gm12 = good_move12;
	    if(basic_instinct) (*basic_instinct)[good_move12] += bonus_kosumi_tsuke_stretch2;
	  }
	
	if(debug_kosumi_tsuke_stretch)
	  {
	    Goban goban(fastboard->getGobanKey());
	    
	    std::cout<<"kosumi-tsuke stretch"<<std::endl;
	    goban.textModeShowGoban(last_move);
	    std::cout<<std::endl;
	    
	    switch(dir)
	      {
	      case NORTH_ : std::cout<<"north"<<std::endl; break;
	      case SOUTH_ : std::cout<<"south"<<std::endl; break;
	      case EAST_ : std::cout<<"east"<<std::endl; break;
	      case WEST_ : std::cout<<"west"<<std::endl; break;
	      default : assert(0);
	      }
	    
	    std::cout<<"left up"<<std::endl;
	    
	    goban.textModeShowPosition(place_left_up);
	    goban.textModeShowPosition(place_right);	   
	    goban.textModeShowPosition(good_move11); 
	    if(not goban.isOutsideOfGoban(good_move12)) goban.textModeShowPosition(good_move12);
	  }
	
	is_pattern = true;
      }
  
  
  if(FBS_handler::isOnBoard(place_left_down))
    if(fastboard->position(place_left_down)==fastboard->position(last_move))
      {

	if(gm21) *gm21 = good_move21;
	
	if(basic_instinct) (*basic_instinct)[good_move21] += bonus_kosumi_tsuke_stretch;
	
	if(FBS_handler::isOnBoard(good_move22))
	  {
	    if(gm22) *gm22 = good_move22;
	    if(basic_instinct) (*basic_instinct)[good_move22] += bonus_kosumi_tsuke_stretch2;
	  }
	
	if(debug_kosumi_tsuke_stretch)
	  {
	    Goban goban(fastboard->getGobanKey()); 
	    
	    std::cout<<"kosumi-tsuke stretch"<<std::endl;
	    goban.textModeShowGoban(last_move);
	    std::cout<<std::endl;
	    
	    switch(dir)
	      {
	      case NORTH_ : std::cout<<"north"<<std::endl; break;
	      case SOUTH_ : std::cout<<"south"<<std::endl; break;
	      case EAST_ : std::cout<<"east"<<std::endl; break;
	      case WEST_ : std::cout<<"west"<<std::endl; break;
	      default : assert(0);
	      }
	    
	    std::cout<<"left down"<<std::endl;
	    
	    goban.textModeShowPosition(place_left_down);
	    goban.textModeShowPosition(place_right);	   
	    goban.textModeShowPosition(good_move21); 
	    if(not goban.isOutsideOfGoban(good_move22)) goban.textModeShowPosition(good_move22);
	  }
	
	return true;
      }
  
  return is_pattern;
}



/**
 * @brief the expertise-go "kosumi tsuke stretch" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::kosumi_tsuke_stretch0(double** basic_instinct) const
{ 
  //Location last_move = goban->lastMove();
  
  this->kosumi_tsuke_stretch0(NORTH_,basic_instinct);
  this->kosumi_tsuke_stretch0(SOUTH_,basic_instinct);
  this->kosumi_tsuke_stretch0(EAST_,basic_instinct);
  this->kosumi_tsuke_stretch0(WEST_,basic_instinct);
}

/**
 * @brief the expertise-go "bump stretch"
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm1 the safeguard of the first found move 
 * @param gm2 the safeguard of the second found move 
 * @return if the expertise has been matched
 */  
bool MoGo::BasicInstinctFast::bump_stretch0(int dir, double** basic_instinct, 
					    int* gm1, int* gm2) const 
{
  Location place_left;
  Location place_right;
  Location good_move1;
  Location good_move2;
  bool is_pattern = false;
  
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  
  
  switch(dir)
    {
    case EAST_ :
      place_left = last_move+FBS_handler::directions[0];
      place_right = last_move+FBS_handler::directions[2];
      good_move1 = last_move+FBS_handler::directions[4];
      good_move2 = last_move+FBS_handler::directions[7];
      break;     
      
    case WEST_ :
      place_left = last_move+FBS_handler::directions[2];
      place_right = last_move+FBS_handler::directions[0];
      good_move1 = last_move+FBS_handler::directions[5];
      good_move2 = last_move+FBS_handler::directions[6];
      break;     
      
    case NORTH_ :
      place_left = last_move+FBS_handler::directions[1];
      place_right = last_move+FBS_handler::directions[3];
      good_move1 = last_move+FBS_handler::directions[6];
      good_move2 = last_move+FBS_handler::directions[7];
      break;     
      
    case SOUTH_ :
      place_left = last_move+FBS_handler::directions[3];
      place_right = last_move+FBS_handler::directions[1];
      good_move1 = last_move+FBS_handler::directions[5];
      good_move2 = last_move+FBS_handler::directions[4];
      break;     
     
    default:
      assert(0);
    }

  if(fastboard->position(place_left)!=fastboard->position(last_move))
    return false;
  
  if((fastboard->position(place_right)==FB_EMPTY)
     ||(fastboard->position(place_right)==fastboard->position(last_move)))
    return false;
    
  if(fastboard->position(good_move1)==FB_EMPTY)
    {
      if(gm1) *gm1 = good_move1;
      if(basic_instinct) (*basic_instinct)[good_move1] += bonus_bump_stretch;
      is_pattern = true;
    }
  
  if(fastboard->position(good_move2)==FB_EMPTY)
    {
      if(gm2) *gm2 = good_move2;
      if(basic_instinct) (*basic_instinct)[good_move2] += bonus_bump_stretch;
      is_pattern = true;
    }
  
  
  if(debug_bump_stretch)
    {
      Goban goban(fastboard->getGobanKey()); 

      std::cout<<"bump stretch"<<std::endl;
      goban.textModeShowGoban(last_move);
      std::cout<<std::endl;
      
      switch(dir)
	{
	case NORTH_ : std::cout<<"north"<<std::endl; break;
	case SOUTH_ : std::cout<<"south"<<std::endl; break;
	case EAST_ : std::cout<<"east"<<std::endl; break;
	case WEST_ : std::cout<<"west"<<std::endl; break;
	default : assert(0);
	}
      
      goban.textModeShowPosition(place_left);
      goban.textModeShowPosition(place_right);	   
      if(not goban.isOutsideOfGoban(good_move1)) goban.textModeShowPosition(good_move1); 
      if(not goban.isOutsideOfGoban(good_move2)) goban.textModeShowPosition(good_move2);
    }
  
  return is_pattern;
}



/**
 * @brief the expertise-go "bump stretch" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::bump_stretch0(double** basic_instinct) const
{ 
  //Location last_move = goban->lastMove();
  
  this->bump_stretch0(NORTH_,basic_instinct);
  this->bump_stretch0(SOUTH_,basic_instinct);
  this->bump_stretch0(EAST_,basic_instinct);
  this->bump_stretch0(WEST_,basic_instinct);
}


/**
 * @brief the expertise-go "wall"
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm1 the safeguard of the first found move 
 * @param gm2 the safeguard of the second found move 
 * @return if the expertise has been matched
 */  

//the pattern
/******
 * @X *
 * XO *
 ******/

bool MoGo::BasicInstinctFast::wall0(double** basic_instinct, 
				    int* gm1, int* gm2) const
{
  bool is_pattern = false;
  
  if(last_move<=0) return false;
  if(before_last_move<=0) return false;
  
  int rlmv = FBS_handler::row(last_move);
  int clmv = FBS_handler::col(last_move);
  int rblmv = FBS_handler::row(before_last_move);
  int cblmv = FBS_handler::col(before_last_move);
  
  int diff_col = abs(clmv-cblmv);
  int diff_row = abs(rlmv-rblmv);
  
  if((diff_col==1)&&(diff_row==1))
    {
      Location good_move1 = FBS_handler::toLocation(rlmv,cblmv);
      Location good_move2 = FBS_handler::toLocation(rblmv,clmv);

      if(fastboard->position(good_move1)==FB_EMPTY)
	{
	  if(gm1) *gm1 = good_move1;
	  if(basic_instinct) (*basic_instinct)[good_move1] += bonus_wall;

	  if(debug_wall)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"wall 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1);   
	    }
	  
	  is_pattern = true;
	}
      
      if(fastboard->position(good_move2)==FB_EMPTY)
	{
	  if(gm2) *gm2 = good_move2;
	  if(basic_instinct) (*basic_instinct)[good_move2] += bonus_wall;
	  
	  if(debug_wall)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"wall 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move2);   
	    }
	  
	  is_pattern = true;
	}
    }

  return is_pattern;
}


/**
 * @brief the expertise-go "wall" (new version) 
 * @param dir the direction
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 * @param gm1 the safeguard of the first found move 
 * @param gm2 the safeguard of the second found move 
 * @return if the expertise has been matched
 */  

//the pattern
/******
 * @X *
 * XO *
 ******/
bool MoGo::BasicInstinctFast::wall1(int dir, double** basic_instinct, 
				    int* gm1, int* gm2) const
{
  int last_color_stone = fastboard->position(last_move); 
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;
  
  bool is_pattern = false;
  
  Location neighbour;
  Location good_move1;
  Location good_move2;
  
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  
  switch(dir)
    {
    case NORTHEAST_ : 
      neighbour = last_move+FBS_handler::directions[4];
      good_move1 = last_move+FBS_handler::directions[1];
      good_move2 = last_move+FBS_handler::directions[2];
      break;
      
    case SOUTHEAST_ : 
      neighbour = last_move+FBS_handler::directions[7];
      good_move1 = last_move+FBS_handler::directions[3];
      good_move2 = last_move+FBS_handler::directions[2];
      break;
      
    case NORTHWEST_ : 
      neighbour = last_move+FBS_handler::directions[5];
      good_move1 = last_move+FBS_handler::directions[0];
      good_move2 = last_move+FBS_handler::directions[1];
      break;
      
    case SOUTHWEST_ : 
      neighbour = last_move+FBS_handler::directions[6];
      good_move1 = last_move+FBS_handler::directions[0];
      good_move2 = last_move+FBS_handler::directions[3];
      break;
      
    default : assert(0);
    }
  
  if(fastboard->position(neighbour)!=future_color_stone)
    return false;
  
  if(fastboard->position(good_move1)==FB_EMPTY)
    {
      if(gm1) *gm1 = good_move1;
      if(basic_instinct) (*basic_instinct)[good_move1] += bonus_wall;
      
      if(debug_wall)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"wall 1"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move1);   
	}
      
      is_pattern = true;
    }
  
  if(fastboard->position(good_move2)==FB_EMPTY)
    {
      if(gm2) *gm2 = good_move2;
      if(basic_instinct) (*basic_instinct)[good_move2] += bonus_wall;
      
      if(debug_wall)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"wall 1"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move2);   
	}
      
      is_pattern = true;
    }

  return is_pattern;
}


/**
 * @brief the expertise-go "wall" (in all directions)
 * @param basic_instinct the expertise table (containing the cumulation of different bonus and malus)
 */ 
void MoGo::BasicInstinctFast::wall1(double** basic_instinct) const
{
  
  //Location last_move =  goban->lastMove();
  
  this->wall1(NORTHWEST_,basic_instinct);
  this->wall1(NORTHEAST_,basic_instinct);
  this->wall1(SOUTHEAST_,basic_instinct);
  this->wall1(SOUTHWEST_,basic_instinct);
}

bool MoGo::BasicInstinctFast::kosumi0(int dir, double** basic_instinct, 
				      int* gm) const
{
  
  Location good_move;
  Location neighbour1;
  Location neighbour2;
  
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  
  switch(dir)
    {
    case NORTHEAST_ : 
      good_move = last_move+FBS_handler::directions[4];
      neighbour1 = last_move+FBS_handler::directions[1];
      neighbour2 = last_move+FBS_handler::directions[2];
      break;
      
    case SOUTHEAST_ : 
      good_move = last_move+FBS_handler::directions[7];
      neighbour1 = last_move+FBS_handler::directions[3];
      neighbour2 = last_move+FBS_handler::directions[2];
      break;
      
    case NORTHWEST_ : 
      good_move = last_move+FBS_handler::directions[5];
      neighbour1 = last_move+FBS_handler::directions[0];
      neighbour2 = last_move+FBS_handler::directions[1];
      break;
      
    case SOUTHWEST_ : 
      good_move = last_move+FBS_handler::directions[6];
      neighbour1 = last_move+FBS_handler::directions[0];
      neighbour2 = last_move+FBS_handler::directions[3];
      break;
      
    default : assert(0);
    }
  
  if((fastboard->position(neighbour1)!=FB_EMPTY)&&(fastboard->position(neighbour1)!=FB_EDGE))
    return false;
  
  if((fastboard->position(neighbour2)!=FB_EMPTY)&&(fastboard->position(neighbour2)!=FB_EDGE))
    return false;
  
  if(fastboard->position(good_move)!=FB_EMPTY)
    return false;
  
  if(gm) *gm = good_move;
  if(basic_instinct) (*basic_instinct)[good_move] += bonus_kosumi;
  
  if(debug_kosumi)
    {
      Goban goban(fastboard->getGobanKey()); 
      
      std::cout<<"kosumi 0"<<std::endl;
      goban.textModeShowGoban(last_move);
      std::cout<<std::endl;
      
      goban.textModeShowPosition(good_move);   
    }
  
  return true;
}



void MoGo::BasicInstinctFast::kosumi0(double** basic_instinct) const
{ 
  //Location last_move =  goban->lastMove();
  
  this->kosumi0(NORTHWEST_,basic_instinct);
  this->kosumi0(NORTHEAST_,basic_instinct);
  this->kosumi0(SOUTHEAST_,basic_instinct);
  this->kosumi0(SOUTHWEST_,basic_instinct);
}


bool MoGo::BasicInstinctFast::kata0(int dir, double** basic_instinct, 
				    int* gm) const
{
  
  Location good_move;
  Location neighbour1;
  Location neighbour2;
  
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  
  switch(dir)
    {
    case NORTHEAST_ : 
      good_move = before_last_move+FBS_handler::directions[4];
      neighbour1 = before_last_move+FBS_handler::directions[1];
      neighbour2 = before_last_move+FBS_handler::directions[2];
      break;
      
    case SOUTHEAST_ : 
      good_move = before_last_move+FBS_handler::directions[7];
      neighbour1 = before_last_move+FBS_handler::directions[3];
      neighbour2 = before_last_move+FBS_handler::directions[2];
      break;
      
    case NORTHWEST_ : 
      good_move = before_last_move+FBS_handler::directions[5];
      neighbour1 = before_last_move+FBS_handler::directions[0];
      neighbour2 = before_last_move+FBS_handler::directions[1];
      break;
      
    case SOUTHWEST_ : 
      good_move = before_last_move+FBS_handler::directions[6];
      neighbour1 = before_last_move+FBS_handler::directions[0];
      neighbour2 = before_last_move+FBS_handler::directions[3];
      break;
      
    default : assert(0);
    }
  
  if((fastboard->position(neighbour1)!=FB_EMPTY)&&(fastboard->position(neighbour1)!=FB_EDGE))
    return false;
  
  if((fastboard->position(neighbour2)!=FB_EMPTY)&&(fastboard->position(neighbour2)!=FB_EDGE))
    return false;
  
  if(fastboard->position(good_move)!=FB_EMPTY)
    return false;
  
  if(gm) *gm = good_move;
  if(basic_instinct) (*basic_instinct)[good_move] += bonus_kata;
  
  if(debug_kata)
    {
      Goban goban(fastboard->getGobanKey()); 
      
      std::cout<<"kata 0"<<std::endl;
      goban.textModeShowGoban(before_last_move);
      std::cout<<std::endl;
      
      goban.textModeShowPosition(good_move);   
    }
  
  return true;
}

void MoGo::BasicInstinctFast::kata0(double** basic_instinct) const
{ 
  if(before_last_move==PASSMOVE) return;

  //Location last_move =  goban->lastMove();
  
  this->kata0(NORTHWEST_,basic_instinct);
  this->kata0(NORTHEAST_,basic_instinct);
  this->kata0(SOUTHEAST_,basic_instinct);
  this->kata0(SOUTHWEST_,basic_instinct);
}


/********
 * (0)@ *
 *  . X *
 ********/

bool MoGo::BasicInstinctFast::extend0(int dir, double** basic_instinct, int* gm1, int* gm2) const
{
  bool is_pattern = false;
  
  int last_color_stone = fastboard->position(last_move); 
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;
  
  Location neighbour_up;
  Location neighbour_left;
  Location neighbour_right;
  Location neighbour_left_up;
  Location neighbour_right_up;
  
  switch(dir)
    {
    case NORTH_ :
      neighbour_up = last_move + FBS_handler::directions[1];
      neighbour_left = last_move + FBS_handler::directions[0];
      neighbour_right = last_move + FBS_handler::directions[2];
      neighbour_left_up = neighbour_up  + FBS_handler::directions[0];
      neighbour_right_up = neighbour_up + FBS_handler::directions[2];
      break;
      
      
    case SOUTH_ :
      neighbour_up = last_move + FBS_handler::directions[3];
      neighbour_left = last_move + FBS_handler::directions[2];
      neighbour_right = last_move + FBS_handler::directions[0];
      neighbour_left_up = neighbour_up  + FBS_handler::directions[2];
      neighbour_right_up = neighbour_up + FBS_handler::directions[0];
      break;
      
    case WEST_ :
      neighbour_up = last_move + FBS_handler::directions[0];
      neighbour_left = last_move + FBS_handler::directions[3];
      neighbour_right = last_move + FBS_handler::directions[1];
      neighbour_left_up = neighbour_up  + FBS_handler::directions[3];
      neighbour_right_up = neighbour_up + FBS_handler::directions[1];
      break;
      
    case EAST_ :
      neighbour_up = last_move + FBS_handler::directions[2];
      neighbour_left = last_move + FBS_handler::directions[1];
      neighbour_right = last_move + FBS_handler::directions[3];
      neighbour_left_up = neighbour_up  + FBS_handler::directions[1];
      neighbour_right_up = neighbour_up + FBS_handler::directions[3];
      break;
      
    default:
      assert(0);
      
    }
  
  if(fastboard->position(neighbour_up)!=FB_EMPTY)
    return false;
  
  if((fastboard->position(neighbour_left_up)==FB_EMPTY)
     &&(fastboard->position(neighbour_left)==future_color_stone))
    {
      
      if(gm1) *gm1 = neighbour_left_up;
      if(basic_instinct) (*basic_instinct)[neighbour_left_up] += bonus_extend;
      
      if(debug_extend)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"extend 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(neighbour_left_up);   
	}
      
      is_pattern = true;
    }


  if((fastboard->position(neighbour_right_up)==FB_EMPTY)
     &&(fastboard->position(neighbour_right)==future_color_stone))
    {
      
      if(gm2) *gm2 = neighbour_right_up;
      if(basic_instinct) (*basic_instinct)[neighbour_right_up] += bonus_extend;
      
      if(debug_extend)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"extend 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(neighbour_right_up);   
	}
      
      is_pattern = true;
    }
  
  return is_pattern;
}


void MoGo::BasicInstinctFast::extend0(double** basic_instinct) const
{
  
  //Location last_move =  goban->lastMove();
  
  this->extend0(NORTH_,basic_instinct);
  this->extend0(EAST_,basic_instinct);
  this->extend0(SOUTH_,basic_instinct);
  this->extend0(WEST_,basic_instinct);
}

/*********
 * ? 1 ? *
 * @(0)2 *
 * 0 X ? *
 *********/

bool MoGo::BasicInstinctFast::cross_cut0(int dir, double** basic_instinct, int* gm1) const
{
  int last_color_stone = fastboard->position(last_move); 
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;
  
  Location good_move;
  Location neighbour_left;
  Location neighbour_right;
  Location neighbour_left_up;
  Location neighbour_right_up;
  Location neighbour_up_up;

  int nb_total_liberties= 0;
  //int nb_liberties1 = 0;
  //int nb_liberties2 = 0;
  
  switch(dir)
    {
    case NORTH_ :
      good_move = last_move + FBS_handler::directions[1];
      neighbour_left = last_move + FBS_handler::directions[0];
      neighbour_right = last_move + FBS_handler::directions[2];
      neighbour_left_up = good_move  + FBS_handler::directions[0];
      neighbour_right_up = good_move + FBS_handler::directions[2];
      neighbour_up_up = good_move + FBS_handler::directions[1];
      break;
      
      
    case SOUTH_ :
      good_move = last_move + FBS_handler::directions[3];
      neighbour_left = last_move + FBS_handler::directions[2];
      neighbour_right = last_move + FBS_handler::directions[0];
      neighbour_left_up = good_move  + FBS_handler::directions[2];
      neighbour_right_up = good_move + FBS_handler::directions[0];
      neighbour_up_up = good_move + FBS_handler::directions[3];
      break;
      
    case WEST_ :
      good_move = last_move + FBS_handler::directions[0];
      neighbour_left = last_move + FBS_handler::directions[3];
      neighbour_right = last_move + FBS_handler::directions[1];
      neighbour_left_up = good_move  + FBS_handler::directions[3];
      neighbour_right_up = good_move + FBS_handler::directions[1];
      neighbour_up_up = good_move + FBS_handler::directions[0];
      break;
      
    case EAST_ :
      good_move = last_move + FBS_handler::directions[2];
      neighbour_left = last_move + FBS_handler::directions[1];
      neighbour_right = last_move + FBS_handler::directions[3];
      neighbour_left_up = good_move  + FBS_handler::directions[1];
      neighbour_right_up = good_move + FBS_handler::directions[3];
      neighbour_up_up = good_move + FBS_handler::directions[2];
      break;
      
    default:
      assert(0);
      
    }
  
  if(fastboard->position(good_move)!=FB_EMPTY)
    return false;
  
  if((fastboard->position(neighbour_left_up)==last_color_stone)
     &&(fastboard->position(neighbour_left)==future_color_stone))
    {
      if(fastboard->position(neighbour_up_up)==future_color_stone)
	nb_total_liberties = fastboard->liberty(neighbour_up_up)-1; //we assume we have played the move
      else
	{
	  if(fastboard->position(neighbour_up_up)==FB_EMPTY)
	    nb_total_liberties++; 
	}
      
      if(fastboard->position(neighbour_right_up)==future_color_stone)
	nb_total_liberties += fastboard->liberty(neighbour_right_up)-1; //we assume we have played the move
      else
	{
	  if(fastboard->position(neighbour_right_up)==FB_EMPTY)
	    nb_total_liberties++; 
	}
      
      if(nb_total_liberties<=1)
	return false;
      
      if(gm1) *gm1 = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_cross_cut;
      
      if(debug_cross_cut)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"cross cut 0"<<std::endl;
	  std::cerr<<"left"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move);   
	}
      
      return true;
    }
  
  //nb_total_liberties equals 0 because we don't go to here if 
  //the last test has been checked: 
  //if((fastboard->position(neighbour_left_up)==last_color_stone)
  // &&(fastboard->position(neighbour_left)==future_color_stone))
  
  if((fastboard->position(neighbour_right_up)==last_color_stone)
     &&(fastboard->position(neighbour_right)==future_color_stone))
    {
      if(fastboard->position(neighbour_up_up)==future_color_stone)
	nb_total_liberties = fastboard->liberty(neighbour_up_up)-1; //we assume we have played the move
      else
	{
	  if(fastboard->position(neighbour_up_up)==FB_EMPTY)
	    nb_total_liberties++; 
	}
      
      if(fastboard->position(neighbour_left_up)==future_color_stone)
	nb_total_liberties += fastboard->liberty(neighbour_left_up)-1; //we assume we have played the move
      else
	{
	  if(fastboard->position(neighbour_left_up)==FB_EMPTY)
	    nb_total_liberties++; 
	}
      
      if(nb_total_liberties<=1)
	return false;
      
      if(gm1) *gm1 = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_cross_cut;
      
      if(debug_cross_cut)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"cross cut 0"<<std::endl;
	  std::cerr<<"right"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move);   
	}
      
      return true;
    }
 
  return false;
}


void MoGo::BasicInstinctFast::cross_cut0(double** basic_instinct) const
{
  
  //Location last_move =  goban->lastMove();
  
  this->cross_cut0(NORTH_,basic_instinct);
  this->cross_cut0(EAST_,basic_instinct);
  this->cross_cut0(SOUTH_,basic_instinct);
  this->cross_cut0(WEST_,basic_instinct);
}


bool MoGo::BasicInstinctFast::bad_tobi0(int dir, double** basic_instinct, 
					   int* bm1, int* bm2) const 
{
  int last_color_stone = fastboard->position(last_move); 
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;
  
  Location place_left_up;
  Location neighbour1;
  Location neighbour2;
  //Location tobi1_last_mv; //replaced by isAlone
  //Location tobi2_last_mv; //replaced by isAlone
  Location bad_move1;
  Location bad_move2;
  
  bool is_pattern = false;


  switch(dir)
    {
    case NORTHWEST_ :
      place_left_up = this->last_move + FBS_handler::directions[5];
      neighbour1 = this->last_move + FBS_handler::directions[0];
      neighbour2 = this->last_move + FBS_handler::directions[1];
      bad_move1 = this->last_move + FBS_handler::directions[6];
      bad_move2 = this->last_move + FBS_handler::directions[4];
      //tobi1_last_mv = neighbour1 + FBS_handler::directions[0];
      //tobi2_last_mv = neighbour2 + FBS_handler::directions[1];
      break;

    case NORTHEAST_ :
      place_left_up = this->last_move + FBS_handler::directions[4];
      neighbour1 = this->last_move + FBS_handler::directions[2];
      neighbour2 = this->last_move + FBS_handler::directions[1];
      bad_move1 = this->last_move + FBS_handler::directions[7];
      bad_move2 = this->last_move + FBS_handler::directions[5];
      //tobi1_last_mv = neighbour1 + FBS_handler::directions[2];
      //tobi2_last_mv = neighbour2 + FBS_handler::directions[1];
      break;

    case SOUTHWEST_ :
      place_left_up = this->last_move + FBS_handler::directions[6];
      neighbour1 = this->last_move + FBS_handler::directions[0];
      neighbour2 = this->last_move + FBS_handler::directions[3];
      bad_move1 = this->last_move + FBS_handler::directions[5];
      bad_move2 = this->last_move + FBS_handler::directions[7];
      //tobi1_last_mv = neighbour1 + FBS_handler::directions[0];
      //tobi2_last_mv = neighbour2 + FBS_handler::directions[3];
      break;

    case SOUTHEAST_ :
      place_left_up = this->last_move + FBS_handler::directions[7];
      neighbour1 = this->last_move + FBS_handler::directions[2];
      neighbour2 = this->last_move + FBS_handler::directions[3];
      bad_move1 = this->last_move + FBS_handler::directions[4];
      bad_move2 = this->last_move + FBS_handler::directions[6];
      //tobi1_last_mv = neighbour1 + FBS_handler::directions[2];
      //tobi2_last_mv = neighbour2 + FBS_handler::directions[3];
      break;
     
    default: assert(0);
    }
  
  //if(not FBS_handler::isOnBoard(place_left_up)) return false;  //because big area
  if(fastboard->position(place_left_up)!=future_color_stone) return false;
  
  if(fastboard->position(bad_move1)==FB_EMPTY)
    if(fastboard->position(neighbour1)==FB_EMPTY)
      if(fastboard->isAlone8(bad_move1,future_color_stone))
	{
	  if(bm1) *bm1 = bad_move1;
	  if(basic_instinct) (*basic_instinct)[bad_move1] += malus_bad_tobi;
	  
	  if(debug_bad_tobi)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"bad tobi 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      switch(dir)
		{
		case NORTHWEST_ : std::cout<<"northwest"<<std::endl; break;
		case SOUTHWEST_ : std::cout<<"southwest"<<std::endl; break;
		case SOUTHEAST_ : std::cout<<"southeast"<<std::endl; break;
		case NORTHEAST_ : std::cout<<"northeast"<<std::endl; break;
		default : assert(0);
		}
	      
	      goban.textModeShowPosition(place_left_up);   
	      goban.textModeShowPosition(bad_move1); 
	    }
	  
	  is_pattern = true;
	}
  
   if(fastboard->position(bad_move2)==FB_EMPTY)
    if(fastboard->position(neighbour2)==FB_EMPTY)
      if(fastboard->isAlone8(bad_move2,future_color_stone))
	{
	  if(bm2) *bm2 = bad_move2;
	  if(basic_instinct) (*basic_instinct)[bad_move2] += malus_bad_tobi;
	  
	  if(debug_bad_tobi)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"bad tobi 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      switch(dir)
		{
		case NORTHWEST_ : std::cout<<"northwest"<<std::endl; break;
		case SOUTHWEST_ : std::cout<<"southwest"<<std::endl; break;
		case SOUTHEAST_ : std::cout<<"southeast"<<std::endl; break;
		case NORTHEAST_ : std::cout<<"northeast"<<std::endl; break;
		default : assert(0);
		}
	      
	      goban.textModeShowPosition(place_left_up);   
	      goban.textModeShowPosition(bad_move2); 
	    }
	  
	  is_pattern = true;
	}


  return is_pattern;
}

void MoGo::BasicInstinctFast::bad_tobi0(double** basic_instinct) const
{
  
  //Location last_move =  goban->lastMove();
  
  this->bad_tobi0(NORTHWEST_,basic_instinct);
  this->bad_tobi0(NORTHEAST_,basic_instinct);
  this->bad_tobi0(SOUTHEAST_,basic_instinct);
  this->bad_tobi0(SOUTHWEST_,basic_instinct);
}


bool MoGo::BasicInstinctFast::bad_kogeima0(int dir, double** basic_instinct, 
					   int* bm1, int* bm2) const 
{
  int last_color_stone = fastboard->position(last_move); 
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;
  
  Location place_tobi_left;
  Location place_tobi_right;
  Location bad_move1;
  Location bad_move2;
  
  bool is_pattern = false;
  //    12 9  13
// 19 5  1  4 14
// 8  0 [*] 2 10
// 18 6  3  7 15
//    17 11 16
  switch(dir)
    {
    case NORTH_ :
      place_tobi_left = this->last_move + 2*FBS_handler::directions[0];
      place_tobi_right = this->last_move + 2*FBS_handler::directions[2]; 
      bad_move1 = this->last_move + FBS_handler::directions[1];
      bad_move2 = this->last_move + FBS_handler::directions[3];
      break;
      
      
    case EAST_ :
      place_tobi_left = this->last_move + 2*FBS_handler::directions[1];
      place_tobi_right = this->last_move + 2*FBS_handler::directions[3]; 
      bad_move1 = this->last_move + FBS_handler::directions[0];
      bad_move2 = this->last_move + FBS_handler::directions[2];
      break;
      
    default: 
      assert(0);
    }
  
  if(((FBS_handler::isOnBoard(place_tobi_left))
      &&(fastboard->position(place_tobi_left)==future_color_stone))
     ||((FBS_handler::isOnBoard(place_tobi_right))
	&&(fastboard->position(place_tobi_right)==future_color_stone)))
    {
      if((fastboard->position(bad_move1)==FB_EMPTY)
	 &&(fastboard->isAlone8(bad_move1,future_color_stone)))
	{
	  if(bm1) *bm1 = bad_move1;
	  if(basic_instinct) (*basic_instinct)[bad_move1] += malus_bad_kogeima;
	  
	  if(debug_bad_kogeima)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"bad kogeima 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(bad_move1); 
	    }
	 
	  is_pattern = true;
	}

      if((fastboard->position(bad_move2)==FB_EMPTY)
	 &&(fastboard->isAlone8(bad_move2,future_color_stone)))
	{
	  if(bm2) *bm2 = bad_move2;
	  if(basic_instinct) (*basic_instinct)[bad_move2] += malus_bad_kogeima;
	  
	  if(debug_bad_kogeima)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"bad kogeima 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(bad_move2); 
	    }
	 
	  is_pattern = true;
	}
      
    }

  return is_pattern;
}


void MoGo::BasicInstinctFast::bad_kogeima0(double** basic_instinct) const
{
  
  //Location last_move =  goban->lastMove();
  
  this->bad_kogeima0(NORTH_,basic_instinct);
  this->bad_kogeima0(EAST_,basic_instinct);
}

bool MoGo::BasicInstinctFast::bad_elephant0(int dir, double** basic_instinct, 
					    int* bm) const 
{
  int last_color_stone = fastboard->position(last_move); 
  int future_color_stone = (last_color_stone==FB_BLACK) ? FB_WHITE : FB_BLACK;
  
  Location tobi1_last_mv;
  Location neighbour1;
  Location tobi2_last_mv;
  Location neighbour2;
  Location bad_move;  
  
  switch(dir)
    {
    case NORTH_ :
      tobi1_last_mv = this->last_move + 2*FBS_handler::directions[0];
      tobi2_last_mv = this->last_move + 2*FBS_handler::directions[2];
      neighbour1 = this->last_move + FBS_handler::directions[5];
      neighbour2 = this->last_move + FBS_handler::directions[4];
      bad_move = this->last_move + 2*FBS_handler::directions[1];
      break;
      
    case SOUTH_ :
      tobi1_last_mv = this->last_move + 2*FBS_handler::directions[0];
      tobi2_last_mv = this->last_move + 2*FBS_handler::directions[2];
      neighbour1 = this->last_move + FBS_handler::directions[6];
      neighbour2 = this->last_move + FBS_handler::directions[7];
      bad_move = this->last_move + 2*FBS_handler::directions[3];
      break;
      
    case EAST_ :
      tobi1_last_mv = this->last_move + 2*FBS_handler::directions[1];
      tobi2_last_mv = this->last_move + 2*FBS_handler::directions[3];
      neighbour1 = this->last_move + FBS_handler::directions[4];
      neighbour2 = this->last_move + FBS_handler::directions[7];
      bad_move = this->last_move + 2*FBS_handler::directions[2];
      break;
      
    case WEST_ :
      tobi1_last_mv = this->last_move + 2*FBS_handler::directions[1];
      tobi2_last_mv = this->last_move + 2*FBS_handler::directions[3];
      neighbour1 = this->last_move + FBS_handler::directions[5];
      neighbour2 = this->last_move + FBS_handler::directions[6];
      bad_move = this->last_move + 2*FBS_handler::directions[0];
      break;
      
    default :
      assert(0);
    }
  
  if(((FBS_handler::isOnBoard(tobi1_last_mv))
      &&(fastboard->position(tobi1_last_mv)==future_color_stone)
      &&(fastboard->isWithoutStone4(neighbour1)))
     ||((FBS_handler::isOnBoard(tobi2_last_mv))
	&&(fastboard->position(tobi2_last_mv)==future_color_stone)
	&&(fastboard->isWithoutStone4(neighbour2))))
    {
      if((FBS_handler::isOnBoard(bad_move))
	 &&(fastboard->position(bad_move)==FB_EMPTY)
	 &&(fastboard->isAlone8(bad_move,future_color_stone)))
	{
	  if(bm) *bm = bad_move;
	  if(basic_instinct) (*basic_instinct)[bad_move] += malus_bad_elephant;
	  
	  if(debug_bad_elephant)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"bad elephant 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(bad_move); 
	    }
	  
	  return true;
	}
      
    }
  
  return false;
}

void MoGo::BasicInstinctFast::bad_elephant0(double** basic_instinct) const
{
  
  //Location last_move =  goban->lastMove();
  
  this->bad_elephant0(NORTH_,basic_instinct);
  this->bad_elephant0(SOUTH_,basic_instinct);
  this->bad_elephant0(EAST_,basic_instinct);
  this->bad_elephant0(WEST_,basic_instinct);
}



bool MoGo::BasicInstinctFast::bad_empty_triangle0(int dir, double** basic_instinct, 
						  list<int>* lbm) const 
{
  int future_color_stone = fastboard->position(before_last_move);
  
  bool is_pattern = false;
  
  Location empty_square;
  Location place_left_up;
  Location place_right_up;
  Location place_left;
  Location place_right;
  

  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  
  switch(dir)
    {
    case NORTH_ :
      empty_square = before_last_move+FBS_handler::directions[1];
      place_left_up = before_last_move+FBS_handler::directions[5];
      place_right_up = before_last_move+FBS_handler::directions[4]; 
      place_left = before_last_move+FBS_handler::directions[0];
      place_right = before_last_move+FBS_handler::directions[2]; 
      break;
      
    case SOUTH_ :
      empty_square = before_last_move+FBS_handler::directions[3];
      place_left_up = before_last_move+FBS_handler::directions[7];
      place_right_up = before_last_move+FBS_handler::directions[6]; 
      place_left = before_last_move+FBS_handler::directions[2];
      place_right = before_last_move+FBS_handler::directions[0]; 
      break;
      
    case WEST_ :
      empty_square = before_last_move+FBS_handler::directions[0];
      place_left_up = before_last_move+FBS_handler::directions[5];
      place_right_up = before_last_move+FBS_handler::directions[6]; 
      place_left = before_last_move+FBS_handler::directions[1];
      place_right = before_last_move+FBS_handler::directions[3]; 
      break;
      
    case EAST_ :
      empty_square = before_last_move+FBS_handler::directions[2];
      place_left_up = before_last_move+FBS_handler::directions[4];
      place_right_up = before_last_move+FBS_handler::directions[7]; 
      place_left = before_last_move+FBS_handler::directions[1];
      place_right = before_last_move+FBS_handler::directions[3]; 
      break;
      
    default:
      assert(0);
    }
  
  
  if(fastboard->position(empty_square)!=FB_EMPTY) return false;
  
  if(fastboard->position(place_left_up)==future_color_stone)
    if(fastboard->position(place_left)==FB_EMPTY)
      {
	if(lbm) lbm->push_back(place_left);
	if(basic_instinct) (*basic_instinct)[place_left] += malus_bad_empty_triangle;
	is_pattern = true;
	
	if(debug_bad_empty_triangle)
	  {
	    Goban goban(fastboard->getGobanKey()); 
	    
	    std::cout<<"bad empty triangle 0"<<std::endl;
	    goban.textModeShowGoban(before_last_move);
	    std::cout<<std::endl;
	    
	    goban.textModeShowPosition(place_left); 
	  }
      }
  
  if(fastboard->position(place_left)==future_color_stone)
    if(fastboard->position(place_left_up)==FB_EMPTY)
      {
	if(lbm) lbm->push_back(place_left_up);
	if(basic_instinct) (*basic_instinct)[place_left_up] += malus_bad_empty_triangle;
	is_pattern = true;
	
	if(debug_bad_empty_triangle)
	  {
	    Goban goban(fastboard->getGobanKey()); 
	    
	    std::cout<<"bad empty triangle 0"<<std::endl;
	    goban.textModeShowGoban(before_last_move);
	    std::cout<<std::endl;
	    
	    goban.textModeShowPosition(place_left_up); 
	  }
      }
  
  
  if(fastboard->position(place_right_up)==future_color_stone)
    if(fastboard->position(place_right)==FB_EMPTY)
      {
	if(lbm) lbm->push_back(place_right);
	if(basic_instinct) (*basic_instinct)[place_right] += malus_bad_empty_triangle;
	is_pattern = true;
	
	if(debug_bad_empty_triangle)
	  {
	    Goban goban(fastboard->getGobanKey()); 
	    
	    std::cout<<"bad empty triangle 0"<<std::endl;
	    goban.textModeShowGoban(before_last_move);
	    std::cout<<std::endl;
	    
	    goban.textModeShowPosition(place_right); 
	  }
      }
  
  if(fastboard->position(place_right)==future_color_stone)
    if(fastboard->position(place_right_up)==FB_EMPTY)
      {
	if(lbm) lbm->push_back(place_right_up);
	if(basic_instinct) (*basic_instinct)[place_right_up] += malus_bad_empty_triangle;
	is_pattern = true;
	
	if(debug_bad_empty_triangle)
	  {
	    Goban goban(fastboard->getGobanKey()); 
	    
	    std::cout<<"bad empty triangle 0"<<std::endl;
	    goban.textModeShowGoban(before_last_move);
	    std::cout<<std::endl;
	    
	    goban.textModeShowPosition(place_right_up); 
	  }
      }
  
  
  

  return is_pattern;
}



void MoGo::BasicInstinctFast::bad_empty_triangle0(double** basic_instinct) const
{  
  if(before_last_move<=0) return;

 
  this->bad_empty_triangle0(NORTH_,basic_instinct);
  this->bad_empty_triangle0(SOUTH_,basic_instinct);
  this->bad_empty_triangle0(EAST_,basic_instinct);
  this->bad_empty_triangle0(WEST_,basic_instinct);
  //this->bad_empty_triangle0(NORTHWEST_,basic_instinct);
  //this->bad_empty_triangle0(SOUTHWEST_,basic_instinct);
  //this->bad_empty_triangle0(SOUTHEAST_,basic_instinct);
  //this->bad_empty_triangle0(NORTHEAST_,basic_instinct);
}


bool MoGo::BasicInstinctFast::bad_hazama_tobi0(int dir, double** basic_instinct, int* bm) const
{
  Location bad_move;
  Location place_between;
  
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  
  
  switch(dir)
    {
    case NORTHWEST_ : 
      place_between = before_last_move+FBS_handler::directions[4];
      bad_move = place_between+FBS_handler::directions[4];
      break;
    
    case SOUTHWEST_ : 
      place_between = before_last_move+FBS_handler::directions[7];
      bad_move = place_between+FBS_handler::directions[7];
      break;
    
    case NORTHEAST_ : 
      place_between = before_last_move+FBS_handler::directions[5];
      bad_move = place_between+FBS_handler::directions[5];
      break;

    case SOUTHEAST_ : 
      place_between = before_last_move+FBS_handler::directions[6];
      bad_move = place_between+FBS_handler::directions[6];
      break;

    default: assert(0);
    }


  //int future_color_stone = fastboard->position(before_last_move); 
  //  if(fastboard->position(place_between)!=future_color_stone)
  // if(fastboard->position(place_between)!=FB_EDGE)

  if(fastboard->position(place_between)==FB_EMPTY)
    if(fastboard->position(bad_move)==FB_EMPTY)
      {
	if(bm) *bm = bad_move;
	if(basic_instinct) (*basic_instinct)[bad_move] += malus_bad_hazama_tobi;
	
	if(debug_bad_hazama_tobi)
	  {
	    Goban goban(fastboard->getGobanKey()); 
	    
	    std::cout<<"bad hazama tobi 0"<<std::endl;
	    goban.textModeShowGoban(before_last_move);
	    std::cout<<std::endl;
	    
	    goban.textModeShowPosition(bad_move); 
	  }
	
	return true;
      }
  
  return false;
}



void MoGo::BasicInstinctFast::bad_hazama_tobi0(double** basic_instinct) const
{  
  if(before_last_move<=0) return;

  this->bad_hazama_tobi0(NORTHWEST_,basic_instinct);
  this->bad_hazama_tobi0(SOUTHWEST_,basic_instinct);
  this->bad_hazama_tobi0(SOUTHEAST_,basic_instinct);
  this->bad_hazama_tobi0(NORTHEAST_,basic_instinct);
}


void MoGo::BasicInstinctFast::alone_border0(Location location,
					    double** basic_instinct) const
{
  //TO DO isOnBorder
 
  if(fastboard->isWithoutStone8(location))
    {
      if(basic_instinct)
	(*basic_instinct)[location]+=malus_alone_border;
    }
}

void MoGo::BasicInstinctFast::alone_border0(double** basic_instinct) const
{
  int r1 = 0;
  int c1 = 0;
  
  int r2 =  FBS_handler::board_size-1;
  int c2 =  FBS_handler::board_size-1;
  
  int start1 = FBS_handler::toLocation(r1,c1);
  int start2 = FBS_handler::toLocation(r2,c2);
  
  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[3])
    alone_border0(i,basic_instinct);
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[1])
    alone_border0(i,basic_instinct);

  for(int i = start1; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[2])
    alone_border0(i,basic_instinct);
  
  for(int i = start2; fastboard->position(i)!=FB_EDGE; i+=FBS_handler::directions[0])
    alone_border0(i,basic_instinct);
}




void MoGo::BasicInstinctFast::avoid_border0(double** basic_instinct, int* gm, int* bm) const
{
  int r = FBS_handler::row(last_move);
  int c = FBS_handler::col(last_move);
  
  if(r==0)
    {
      if(c==0)
	{
	  Location good_move1 = last_move+FBS_handler::directions[7];
	  Location good_move2 = last_move+FBS_handler::directions[2];
	  Location good_move3 = last_move+FBS_handler::directions[3];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
      
      //    12 9  13
      // 19 5  1  4 14
      // 8  0 [*] 2 10
      // 18 6  3  7 15
      //    17 11 16
      
      
      if(c==FBS_handler::board_size-1)
	{
	  Location good_move1 = last_move+FBS_handler::directions[6];
	  Location good_move2 = last_move+FBS_handler::directions[0];
	  Location good_move3 = last_move+FBS_handler::directions[3];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
     
      
      Location good_move = last_move+FBS_handler::directions[3];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }

  
  if(r==FBS_handler::board_size-1)
    {
      if(c==0)
	{
	  Location good_move1 = last_move+FBS_handler::directions[4];
	  Location good_move2 = last_move+FBS_handler::directions[1];
	  Location good_move3 = last_move+FBS_handler::directions[2];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
      
      //    12 9  13
      // 19 5  1  4 14
      // 8  0 [*] 2 10
      // 18 6  3  7 15
      //    17 11 16
      
      
      if(c==FBS_handler::board_size-1)
	{
	  Location good_move1 = last_move+FBS_handler::directions[5];
	  Location good_move2 = last_move+FBS_handler::directions[0];
	  Location good_move3 = last_move+FBS_handler::directions[1];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
     
      
      Location good_move = last_move+FBS_handler::directions[1];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }

     
  if(c==FBS_handler::board_size-1)
    {
      Location good_move = last_move+FBS_handler::directions[0];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }
  
  if(c==0)
    {
      Location good_move = last_move+FBS_handler::directions[2];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }
}


void MoGo::BasicInstinctFast::avoid_border1(double** basic_instinct, int* gm, int* bm) const
{
  int r = FBS_handler::row(last_move);
  int c = FBS_handler::col(last_move);
  
  if(r==0)
    {
      if(c==0)
	{
	  Location good_move1 = last_move+FBS_handler::directions[7];
	  Location good_move2 = last_move+FBS_handler::directions[2];
	  Location good_move3 = last_move+FBS_handler::directions[3];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
      
      //    12 9  13
      // 19 5  1  4 14
      // 8  0 [*] 2 10
      // 18 6  3  7 15
      //    17 11 16
      
      
      if(c==FBS_handler::board_size-1)
	{
	  Location good_move1 = last_move+FBS_handler::directions[6];
	  Location good_move2 = last_move+FBS_handler::directions[0];
	  Location good_move3 = last_move+FBS_handler::directions[3];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
     
      
      Location good_move = last_move+FBS_handler::directions[3];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }

  
  if(r==FBS_handler::board_size-1)
    {
      if(c==0)
	{
	  Location good_move1 = last_move+FBS_handler::directions[4];
	  Location good_move2 = last_move+FBS_handler::directions[1];
	  Location good_move3 = last_move+FBS_handler::directions[2];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
      
      //    12 9  13
      // 19 5  1  4 14
      // 8  0 [*] 2 10
      // 18 6  3  7 15
      //    17 11 16
      
      
      if(c==FBS_handler::board_size-1)
	{
	  Location good_move1 = last_move+FBS_handler::directions[5];
	  Location good_move2 = last_move+FBS_handler::directions[0];
	  Location good_move3 = last_move+FBS_handler::directions[1];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
     
      
      Location good_move = last_move+FBS_handler::directions[1];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }

     
  if(c==FBS_handler::board_size-1)
    {
      Location good_move = last_move+FBS_handler::directions[0];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }
  
  if(c==0)
    {
      Location good_move = last_move+FBS_handler::directions[2];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }


  //the line of the defeat

  //TODO bad_move on the border  
  if(r==1)
    {
      if(c==1)
	{
	  Location good_move1 = last_move+FBS_handler::directions[7];
	  Location good_move2 = last_move+FBS_handler::directions[2];
	  Location good_move3 = last_move+FBS_handler::directions[3];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
      
      //    12 9  13
      // 19 5  1  4 14
      // 8  0 [*] 2 10
      // 18 6  3  7 15
      //    17 11 16
      
      
      if(c==FBS_handler::board_size-2)
	{
	  Location good_move1 = last_move+FBS_handler::directions[6];
	  Location good_move2 = last_move+FBS_handler::directions[0];
	  Location good_move3 = last_move+FBS_handler::directions[3];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
     
      
      Location good_move = last_move+FBS_handler::directions[3];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }

  
  if(r==FBS_handler::board_size-2)
    {
      if(c==1)
	{
	  Location good_move1 = last_move+FBS_handler::directions[4];
	  Location good_move2 = last_move+FBS_handler::directions[1];
	  Location good_move3 = last_move+FBS_handler::directions[2];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
     
      
      if(c==FBS_handler::board_size-2)
	{
	  Location good_move1 = last_move+FBS_handler::directions[5];
	  Location good_move2 = last_move+FBS_handler::directions[0];
	  Location good_move3 = last_move+FBS_handler::directions[1];
	  
	  if(gm) *gm = good_move1;
	  if(basic_instinct) 
	    {
	      (*basic_instinct)[good_move1] += bonus_if_last_corner;
	      (*basic_instinct)[good_move2] += bonus_if_last_border;
	      (*basic_instinct)[good_move3] += bonus_if_last_border;
	    }
	  
	  if(debug_avoid_border)
	    {
	      Goban goban(fastboard->getGobanKey()); 
	      
	      std::cout<<"avoir border 0"<<std::endl;
	      goban.textModeShowGoban(last_move);
	      std::cout<<std::endl;
	      
	      goban.textModeShowPosition(good_move1); 
	    }
	  
	  return;
	}
     
      
      Location good_move = last_move+FBS_handler::directions[1];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }

     
  if(c==FBS_handler::board_size-2)
    {
      Location good_move = last_move+FBS_handler::directions[0];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }
  
  if(c==1)
    {
      Location good_move = last_move+FBS_handler::directions[2];
      
      if(gm) *gm = good_move;
      if(basic_instinct) (*basic_instinct)[good_move] += bonus_if_last_border;
      
      
      if(debug_avoid_border)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"avoir border 0"<<std::endl;
	  goban.textModeShowGoban(last_move);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(good_move); 
	}
      
      return;
    }
}


#ifdef GUILLAUME
void MoGo::BasicInstinctFast::computeGuillaume(double** basic_instinct, int color) const
{

   int board[GOBAN_SIZE_2];
   double value[GOBAN_SIZE_2];
   Location location = 22;//node->indexOnArray(0,0); //DIRTY!
	for (int i=0;i<19;i++, location+=2)
	for (int j=0;j<19;j++, location++) 
	{
		board[i*19+j]= (int(round(fastboard->position(location)))*2-1)%3;
		/*
		 * mogo        mango
			0 --> -1
			1 --> 1
			2 --> 0*/
	}
	/*for (int i=0;i<GOBAN_SIZE;i++)
	{
		for (int j=0;j<GOBAN_SIZE;j++)
		{
			if (board[i*19+j]==1)
				printf("B");
			else if (board[i*19+j]==-1)
				printf("W");
			else printf(" ");

		}
		printf("\n");
	}*/
	//assert(0);
	Forme_kppv::match(value,board,30.0/WEIGHT_EXPERTISE,(color%2)*2-1);      // That's the function!
	//Forme_kppv::match(value,board,500.0/WEIGHT_EXPERTISE,(color%2)*2-1);      // That's the function!
	
        location = 22;//node->indexOnArray(0,0); //DIRTY!
	for (int i=0;i<19;i++, location+=2)
	for (int j=0;j<19;j++, location++) 
		(*basic_instinct)[location]+=value[i*19+j];
	/*for (int i=0;i<19;i++)
		for (int j=0;j<19;j++)
		{
			if (board[i][j]==0)
			printf("%g --> ",basic_instinct[i][j]);
			basic_instinct[i][j]+=value[i*19+j];
			printf(" %g\n",basic_instinct[i][j]);
		}*/
}
void MoGo::BasicInstinctFast::computeGuillaume13(double** basic_instinct, int color) const
{

   int board[169];
   double value[169];
   Location location = 16;//node->indexOnArray(0,0); //DIRTY!
	for (int i=0;i<13;i++, location+=2)
	for (int j=0;j<13;j++, location++) 
	{
		board[i*13+j]= (int(round(fastboard->position(location)))*2-1)%3;
		/*
		 * mogo        mango
			0 --> -1
			1 --> 1
			2 --> 0*/
	}
	/*for (int i=0;i<GOBAN_SIZE;i++)
	{
		for (int j=0;j<GOBAN_SIZE;j++)
		{
			if (board[i*19+j]==1)
				printf("B");
			else if (board[i*19+j]==-1)
				printf("W");
			else printf(" ");

		}
		printf("\n");
	}*/
	//assert(0);
	
	Forme_kppv13::match(value,board,30.0/WEIGHT_EXPERTISE,(color%2)*2-1);      // That's the function!
	//Forme_kppv::match(value,board,500.0/WEIGHT_EXPERTISE,(color%2)*2-1);      // That's the function!
	
        location = 16;//node->indexOnArray(0,0); //DIRTY!
	for (int i=0;i<13;i++, location+=2)
	for (int j=0;j<13;j++, location++) 
		(*basic_instinct)[location]+=value[i*13+j];
	/*for (int i=0;i<19;i++)
		for (int j=0;j<19;j++)
		{
			if (board[i][j]==0)
			printf("%g --> ",basic_instinct[i][j]);
			basic_instinct[i][j]+=value[i*19+j];
			printf(" %g\n",basic_instinct[i][j]);
		}*/
}
#endif


void MoGo::BasicInstinctFast::too_free0(double** basic_instinct, int max_free) const
{
#ifdef TOO_FREE
  
  if(fastboard->liberty(last_move)>max_free)
    {
      list<int> liberties;
      fastboard->getLibertiesOfString(last_move,liberties);
      while(not liberties.empty())
	{
	  (*basic_instinct)[liberties.front()]+=malus_too_free;
	  liberties.pop_front();
	}
      
      if(debug_too_free)
	{
	  //TO IMPLEMENT
	}
      
    }
  
#endif
}


void MoGo::BasicInstinctFast::too_free1(int dir, int location, double** basic_instinct, 
					int delta) const
{
#ifdef TOO_FREE
  
  int last_color_stone;
  int future_color_stone;
  
  if(fastboard->isBlacksTurn())
    {
      last_color_stone = FB_WHITE;
      future_color_stone = FB_BLACK;
    }
  else
    {
      last_color_stone = FB_BLACK;
      future_color_stone = FB_WHITE;
    }
  
  
  Location neighbour = PASSMOVE;
  
  switch(dir)
    {
    case NORTH_ :
      neighbour = location + FBS_handler::directions[1];
      break;
      
    case EAST_ :
      neighbour = location + FBS_handler::directions[0];
      break;
      
    case SOUTH_ :
      neighbour = location + FBS_handler::directions[3];
      break;
      
    case WEST_ :
      neighbour = location + FBS_handler::directions[2];
      break;
      
    default:
      assert(0);
    }
  
  if(fastboard->position(neighbour)!=last_color_stone) return;
  
  //if(fastboard->liberty(neighbour)>=fastboard->getChainLength(neighbour)+delta)
  if(fastboard->liberty(neighbour)==fastboard->getChainLength(neighbour)+delta)
    {
      if(basic_instinct) (*basic_instinct)[neighbour] += malus_too_free;
      
      if(debug_too_free)
	{
	  //TODO implementation of the printing of this go-expertise
	}
    }
  
#endif
}

void MoGo::BasicInstinctFast::too_free1(int location, double** basic_instinct, int delta) const
{
#ifdef TOO_FREE
  
  too_free1(NORTH_,location,basic_instinct,delta);
  too_free1(SOUTH_,location,basic_instinct,delta);
  too_free1(EAST_,location,basic_instinct,delta);
  too_free1(WEST_,location,basic_instinct,delta);

#endif
}

void MoGo::BasicInstinctFast::too_free2(int dir, int location, double** basic_instinct, 
					int max_free) const
{
  int last_color_stone;
  int future_color_stone;
  
  if(fastboard->isBlacksTurn())
    {
      last_color_stone = FB_WHITE;
      future_color_stone = FB_BLACK;
    }
  else
    {
      last_color_stone = FB_BLACK;
      future_color_stone = FB_WHITE;
    }
  
  
  Location neighbour = PASSMOVE;
  
  switch(dir)
    {
    case NORTH_ :
      neighbour = location + FBS_handler::directions[1];
      break;
      
    case EAST_ :
      neighbour = location + FBS_handler::directions[0];
      break;
      
    case SOUTH_ :
      neighbour = location + FBS_handler::directions[3];
      break;
      
    case WEST_ :
      neighbour = location + FBS_handler::directions[2];
      break;
      
    default:
      assert(0);
    }
  
  if(fastboard->position(neighbour)!=last_color_stone) return;
  
  if(fastboard->liberty(neighbour)>max_free)
    {
      if(basic_instinct) (*basic_instinct)[location] += malus_too_free;
      
      if(debug_too_free)
	{
	  //TODO implementation of the printing of this go-expertise
	}
    }
}


void MoGo::BasicInstinctFast::too_free2(int location, double** basic_instinct, int delta) const
{
  too_free2(NORTH_,location,basic_instinct,delta);
  too_free2(SOUTH_,location,basic_instinct,delta);
  too_free2(EAST_,location,basic_instinct,delta);
  too_free2(WEST_,location,basic_instinct,delta);
}


void MoGo::BasicInstinctFast::demi_noeud_bambou0(int dir, int location, 
						 double** basic_instinct) const
{
  int last_color_stone;
  int future_color_stone;
  
  if(fastboard->isBlacksTurn())
    {
      last_color_stone = FB_WHITE;
      future_color_stone = FB_BLACK;
    }
  else
    {
      last_color_stone = FB_BLACK;
      future_color_stone = FB_WHITE;
    }
  
  Location neighbour_up;
  Location neighbour_tobi_up;
  Location neighbour_left_up;
  Location neighbour_right_up;
  Location neighbour_keima_left_up;
  Location neighbour_keima_right_up;
  Location neighbour_down;

  switch(dir)
    {
    case NORTH_ :
      neighbour_up  = location + FBS_handler::directions[1];
      neighbour_tobi_up = neighbour_up + FBS_handler::directions[1];
      neighbour_left_up  = location + FBS_handler::directions[5];
      neighbour_right_up  = location + FBS_handler::directions[4];
      neighbour_keima_left_up = neighbour_up + FBS_handler::directions[5];
      neighbour_keima_right_up = neighbour_up + FBS_handler::directions[4];
      neighbour_down = location + FBS_handler::directions[3];
      break;
      
    case EAST_ :
      neighbour_up  = location + FBS_handler::directions[2];
      neighbour_tobi_up = neighbour_up + FBS_handler::directions[2];
      neighbour_left_up  = location + FBS_handler::directions[4];
      neighbour_right_up  = location + FBS_handler::directions[7];
      neighbour_keima_left_up = neighbour_up + FBS_handler::directions[4];
      neighbour_keima_right_up = neighbour_up + FBS_handler::directions[7];
      neighbour_down = location + FBS_handler::directions[0];
      break;
      
    case SOUTH_ :
      neighbour_up  = location + FBS_handler::directions[3];
      neighbour_tobi_up = neighbour_up + FBS_handler::directions[3];
      neighbour_left_up  = location + FBS_handler::directions[7];
      neighbour_right_up  = location + FBS_handler::directions[6];
      neighbour_keima_left_up = neighbour_up + FBS_handler::directions[7];
      neighbour_keima_right_up = neighbour_up + FBS_handler::directions[6];
      neighbour_down = location + FBS_handler::directions[1];
      break;
      
    case WEST_ :
      neighbour_up  = location + FBS_handler::directions[0];
      neighbour_tobi_up = neighbour_up + FBS_handler::directions[0];
      neighbour_left_up  = location + FBS_handler::directions[6];
      neighbour_right_up  = location + FBS_handler::directions[5];
      neighbour_keima_left_up = neighbour_up + FBS_handler::directions[6];
      neighbour_keima_right_up = neighbour_up + FBS_handler::directions[5];
      neighbour_down = location + FBS_handler::directions[2];
      break;
      
    default:
      assert(0);
    }
  
  if(fastboard->position(neighbour_up)!=FB_EMPTY) return;
  if(fastboard->position(neighbour_down)==FB_EDGE) return;
  
  if(fastboard->position(neighbour_tobi_up)!=future_color_stone) return;
  
  if(fastboard->position(neighbour_keima_left_up)==future_color_stone)
    {
      if(fastboard->position(neighbour_keima_right_up)==future_color_stone)
	return;

      if(fastboard->position(neighbour_left_up)!=FB_EMPTY)
	return;

      if(basic_instinct) (*basic_instinct)[location] += bonus_demi_noeud_bambou;
      
      if(debug_demi_noeud_bambou)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"demi noeud bambou 0"<<std::endl;
	  goban.textModeShowGoban(neighbour_tobi_up);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(location); 
	}
      
      return;
    }

  if(fastboard->position(neighbour_keima_right_up)==future_color_stone)
    {
      if(fastboard->position(neighbour_keima_left_up)==future_color_stone)
	return;

      if(fastboard->position(neighbour_right_up)!=FB_EMPTY)
	return;

      if(basic_instinct) (*basic_instinct)[location] += bonus_demi_noeud_bambou;
      
      if(debug_demi_noeud_bambou)
	{
	  Goban goban(fastboard->getGobanKey()); 
	  
	  std::cout<<"demi noeud bambou 0"<<std::endl;
	  goban.textModeShowGoban(neighbour_tobi_up);
	  std::cout<<std::endl;
	  
	  goban.textModeShowPosition(location); 
	}
      
      return;
    }
}


void MoGo::BasicInstinctFast::demi_noeud_bambou0(int location, double** basic_instinct) const
{
  demi_noeud_bambou0(NORTH_,location,basic_instinct);
  demi_noeud_bambou0(SOUTH_,location,basic_instinct);
  demi_noeud_bambou0(EAST_,location,basic_instinct);
  demi_noeud_bambou0(WEST_,location,basic_instinct);
}




void MoGo::BasicInstinctFast::compute(double** basic_instinct) const
{
  assert(0);
  assert(fastboard);
  
  if(last_move<=0) return;
  
  //this->init0(basic_instinct);
  //this->avoid_border0(basic_instinct);
  this->avoid_border1(basic_instinct);
  
  //this->atari_extend0(basic_instinct);
  this->atari_extend1(basic_instinct);
  this->peep_connect0(basic_instinct);
  this->tsuke_hane0(basic_instinct);
  this->kosumi_tsuke_stretch0(basic_instinct);
  this->bump_stretch0(basic_instinct);
  
  this->eat0(basic_instinct); //don't put if reduce_liberties0

  this->threat0(basic_instinct); //don't put if reduce_liberties0
  
  //this->reduce_liberties0(basic_instinct,3); //(eat0 + threat0 + ...)
  this->increase_liberties0(basic_instinct);
  this->eat_or_to_be_eaten0(basic_instinct);  
  this->connect0(basic_instinct);
  
  this->connect_if_dead_virtual_connection0(basic_instinct);
  
  this->prevent_connection0(basic_instinct);
  
  
  this->one_point_jump0(basic_instinct);
  
  this->kogeima0(basic_instinct);
  
  this->wall0(basic_instinct);   
  // this->wall1(basic_instinct);
  this->kosumi0(basic_instinct);
  this->kata0(basic_instinct);
  
  this->extend0(basic_instinct);
  this->cross_cut0(basic_instinct);

  this->bad_tobi0(basic_instinct);   
  
  this->bad_kogeima0(basic_instinct);
  
  this->bad_elephant0(basic_instinct);
  this->bad_empty_triangle0(basic_instinct);
  this->bad_hazama_tobi0(basic_instinct);
  
  //this->too_free0(basic_instinct,param_too_free);
  
}

#ifdef AUTOTUNE
double MoGo::BasicInstinctFast::configValue(char *s)
{
  float v;
  if (fileNumber>0)
    {
      sprintf(fileName,"mogoConfig%d",fileNumber);
      file=fopen(fileName,"r");
      fileNumber=-1;
    }
  else
    assert(fileNumber!=0);
  fprintf(stderr,"reading parameter %s in file %s...\n",s,fileName);fflush(stdout);
  fscanf(file,"%f",&v);
  fprintf(stderr,"reading coef %g\n",v);fflush(stderr);
  return ((double)v);
}
#endif

#ifdef RANDOM_PATTERN
bool MoGo::BasicInstinctFast::load_random_patterns(const char* filename,
						   int nbThreads)
{
  ifstream f_pattern(filename);
  
  if(not f_pattern.is_open())
    {
      cerr<<filename<<" not found"<<endl;
      assert(0);
      return false;
    }
  
  random_patterns.clear();
  random_patterns.resize(nbThreads);
  
  while(not f_pattern.eof())
    {
      string s_pattern;
      f_pattern>>s_pattern;
      
      if((int)s_pattern.size()==0)
	{
	  //assert(0);
	  continue;
	}
      
      for(int i = 0; i<(int)s_pattern.size(); i++)
	{
	  if(s_pattern[i]==GE_Pattern::nothing)
	    s_pattern[i] = '\n';
	}
      
      double temp_value = 0;
      assert(not f_pattern.eof());
      
      f_pattern>>temp_value;
      
      for(int i = 0; i<(int)random_patterns.size(); i++)
	{
	  (random_patterns[i]).push_back(GE_Pattern());
	  ((random_patterns[i]).back()).init(s_pattern);
	  ((random_patterns[i]).back()).val_pattern = temp_value;
	}

      //list<GE_Pattern>::const_iterator i_patt = (random_patterns[0]).begin();
      //while(i_patt!=(random_patterns[0]).end())
      //{
      //i_patt->print_error();
      //cerr<<"value: "<<i_patt->val_pattern<<endl;
      //i_patt++;
      //}
    }
  
  cerr<<(int)(random_patterns[0]).size()<<" random patterns"<<endl;
  
  f_pattern.close();
}

#endif //RANDOM_PATTERN

void MoGo::BasicInstinctFast::compute(double** basic_instinct, int color)
{
  //compute(basic_instinct); //decommenter ?
  init0(basic_instinct);
  init_local0(basic_instinct);
  init_by_distance_before_last_move0(basic_instinct);
  
 // cerr << " getBigArea =" << FBS_handler::getBigArea() << endl;
 // exit(0);
  for(int location = 0; location<FBS_handler::getBigArea(); location++)
    {
      if(fastboard->position(location)==FB_EDGE) continue;
      if(fastboard->position(location)==FB_EMPTY) 
	{
	  //too_free1(location,basic_instinct,3);
	  too_free2(location,basic_instinct,param_too_free);
	  demi_noeud_bambou0(location,basic_instinct);
	  continue;
	}
      compute(basic_instinct,color,location);
    }
  

  int future_color_stone;
  
  if(fastboard->isBlacksTurn())
    future_color_stone = FB_BLACK;
  else
    future_color_stone = FB_WHITE;
  
#ifdef MODULE_RANDOM_PATTERN
  v_goban.resize(FBS_handler::getBigArea(),FB_EDGE);
  
  for(int location = 0; location<FBS_handler::getBigArea(); location++)
    {
      v_goban[location] = fastboard->position(location);
    }
#endif //MODULE_RANDOM_PATTERN
  
  
#ifdef TEST_RANDOM_PATTERN
  //TEST RANDOM PATTERN
  
  if(test_pattern.length<0)
    {
      string s_pattern;
      GE_Pattern::load_pattern("/users/dsa/hoock/Stats/RandomPattern/my_pattern.txt",s_pattern);
      test_pattern.init(s_pattern);
    }
  
  test_pattern.init_board(FB_BLACK,FB_WHITE,FB_EMPTY,v_goban);
  
  
  list<int> all_matchings;
  
  test_pattern.get_matching_out(future_color_stone,all_matchings);
  
  //cerr<<"pattern :"<<endl;
  //cerr<<test_pattern.pattern<<endl;
  //GE_ListOperators::print_error(all_matchings);
  
  if(basic_instinct)
    {
      while(not all_matchings.empty())
	{
	  int pattern_move = all_matchings.front();
	  all_matchings.pop_front();
	  
	  (*basic_instinct)[pattern_move] += 0.75;
	  
	  /*
	    Goban goban(fastboard->getGobanKey());
	    
	    goban.textModeShowGoban();  
	    goban.textModeShowPosition(pattern_move);
	  */
	  
	}
    }
#endif //END TEST_RANDOM_PATTERN
  
  
#ifdef RANDOM_PATTERN
  
  if(basic_instinct)
    {
      int numThread=0;
#ifdef PARALLELIZED
      numThread=ThreadsManagement::getNumThread(); //from 1 to nbThreads (not from 0 to nbThreads-1) 
#endif
      
      if(numThread!=0)
	numThread = numThread - 1;
      assert(numThread>=0);
      assert(numThread<(int)random_patterns.size());
      
      
      list<GE_Pattern>::iterator i_patt = (random_patterns[numThread]).begin();
      while(i_patt!=(random_patterns[numThread]).end())
	{
	  i_patt->init_board(v_goban,FBS_handler::board_size,FBS_handler::board_size);
	  list<int> temp_all_matchings; 
	  i_patt->get_matching_out(future_color_stone,temp_all_matchings);
	  
	  while(not temp_all_matchings.empty())
	    {
	      int pattern_move = temp_all_matchings.front();
	      temp_all_matchings.pop_front();
	      
	      (*basic_instinct)[pattern_move] += i_patt->val_pattern;
	      
	      
	      //Goban goban(fastboard->getGobanKey());
	      
	      //goban.textModeShowGoban();  
	      //goban.textModeShowPosition(pattern_move);
	      //cerr<<endl;
	      //cerr<<i_patt->pattern<<endl<<endl;
	      
	    }

	  i_patt++;
	}
    }
  
#endif // END RANDOM_PATTERN


	// cerr << "####################################" << FBS_handler::board_size << endl;
#ifdef GUILLAUME
  if (uninitialized)
	{
	 //      printf("%d\n",FBS_handler::board_size);
	// cerr << "####################################" << FBS_handler::board_size << endl;
	// fflush(stderr);
	if (FBS_handler::board_size==19)       
		Forme_kppv::init_class();
	else if (FBS_handler::board_size==13)       
		Forme_kppv13::init_class();
		
	  
	//     if (FBS_handler::board_size==19)
// 	    {
	    
// 	    malus_line_defeat +=(0.29);
// 	    bonus_atari_extend +=(-0.29);
// 	    bonus_thrust_block +=(-0.05);
// 	    bonus_kosumi_tsuke_stretch +=(-0.53);
// 	    bonus_kosumi_tsuke_stretch2 +=(0.09);
// 	    bonus_bump_stretch +=(0.07);
// 	    bonus_eat +=(-0.12);
// 	    bonus_eat_or_to_be_eaten +=(-0.16);
// 	    bonus_reduce_liberties +=(-0.05);
// 	    bonus_increase_liberties +=(0.1);
// 	    bonus_prevent_connection +=(0.09);
// 	    bonus_one_point_jump +=(-0.08);
// 	    bonus_kogeima +=(0.01);
// 	    bonus_kosumi +=(-0.27);
// 	    bonus_kata +=(-0.37);
// 	    malus_bad_tobi +=(-0.14);
// 	    malus_bad_elephant +=(-0.13);
// 	    malus_alone_border +=(-0.27);
// 	    malus_border +=(0.026);
// 	    bonus_if_last_border +=(0.03);
// 	    bonus_if_last_corner +=(-0.16);
	    
	    
// 	    GCoeff*=exp((-0.33)/4.);
// 	    //bonus_line_influence*=exp(0.76 /*configValue("bonus_line_influence")*/ /4.0);
// 	    //bonus_tsuke_hane*=exp(configValue("bonus_tsuke_hane")/4.0);
// 	    bonus_line_territory*=exp((-2.02+0.07)/*configValue("bonus_line_territory")*/ /4.0);
// 	    malus_line_death*=exp((-1.47-0.37)/*configValue("malus_line_death")*/ /4.0);
// 	    bonus_wall*=exp((-0.19)/*configValue("bonus_wall")*/ /4.0);
// 	    malus_bad_empty_triangle*=exp((-1.50+0.21)/*configValue("malus_bad_empty_triangle")*/ /4.0);
// 	    bonus_connect*=exp((-0.67+0.31)/*configValue("bonus_connect")*/ /4.0);
// 	    //bonus_tsuke_hane*=exp(configValue("bonus_tsuke_hane")/4.0);
// 	    bonus_threat*=exp((-0.71+0.04)/*configValue("bonus_threat")*/ /4.0);
// 	    bonus_extend*=exp((-1.2-0.01)/*configValue("bonus_extend")*/ /4.0);

// 	    bonus_peep_connect *=exp((-0.28)/4.0);
// 	    bonus_tsuke_hane *=exp((-0.25)/4.0);
// 	    bonus_connect_if_dead_virtual_connection *=exp((-0.17)/4.0);
// 	    bonus_cross_cut *=exp((-0.11)/4.0);
// 	    malus_bad_kogeima *=exp((-0.06)/4.0);
// 	    malus_bad_hazama_tobi *=exp((-0.27)/4.0);
// 	    malus_too_free *=exp((-0.19)/4.0);
// 	    weight_distance *=exp((-0.15)/4.0);
	    
	    
// 	    param_too_free +=(-0.54);
	    
//}

#ifdef AUTOTUNE

// bonus_line_influence +=(configValue("bonus_line_influence"));
// malus_line_defeat +=(configValue("malus_line_defeat"));
// bonus_atari_extend +=(configValue("bonus_atari_extend"));
// bonus_thrust_block +=(configValue("bonus_thrust_block"));
// bonus_kosumi_tsuke_stretch +=(configValue("bonus_kosumi_tsuke_stretch"));
// bonus_kosumi_tsuke_stretch2 +=(configValue("bonus_kosumi_tsuke_stretch2"));
// bonus_bump_stretch +=(configValue("bonus_bump_stretch"));
// bonus_eat +=(configValue("bonus_eat"));
// bonus_eat_or_to_be_eaten +=(configValue("bonus_eat_or_to_be_eaten"));
// bonus_reduce_liberties +=(configValue("bonus_reduce_liberties"));
// bonus_increase_liberties +=(configValue("bonus_increase_liberties"));
// bonus_prevent_connection +=(configValue("bonus_prevent_connection"));
// bonus_one_point_jump +=(configValue("bonus_one_point_jump"));
// bonus_kogeima +=(configValue("bonus_kogeima"));
// bonus_kosumi +=(configValue("bonus_kosumi"));
// bonus_kata +=(configValue("bonus_kata"));
// malus_bad_tobi +=(configValue("malus_bad_tobi"));
// malus_bad_elephant +=(configValue("malus_bad_elephant"));
// malus_alone_border +=(configValue("malus_alone_border"));
// malus_border +=(configValue("malus_border"));
// bonus_if_last_border +=(configValue("bonus_if_last_border"));
// bonus_if_last_corner +=(configValue("bonus_if_last_corner"));

// bonus_line_territory *=exp(configValue("bonus_line_territory")/4.0);
// malus_line_death *=exp(configValue("malus_line_death")/4.0);
// GCoeff *=exp(configValue("GCoeff")/4.0);
// bonus_peep_connect *=exp(configValue("bonus_peep_connect")/4.0);
// bonus_tsuke_hane *=exp(configValue("bonus_tsuke_hane")/4.0);
// bonus_threat *=exp(configValue("bonus_threat")/4.0);
// bonus_connect *=exp(configValue("bonus_connect")/4.0);
// bonus_connect_if_dead_virtual_connection *=exp(configValue("bonus_connect_if_dead_virtual_connection")/4.0);
// bonus_wall *=exp(configValue("bonus_wall")/4.0);
// bonus_extend *=exp(configValue("bonus_extend")/4.0);
// bonus_cross_cut *=exp(configValue("bonus_cross_cut")/4.0);
// malus_bad_kogeima *=exp(configValue("malus_bad_kogeima")/4.0);
// malus_bad_empty_triangle *=exp(configValue("malus_bad_empty_triangle")/4.0);
// malus_bad_hazama_tobi *=exp(configValue("malus_bad_hazama_tobi")/4.0);
// malus_too_free *=exp(configValue("malus_too_free")/4.0);
// weight_distance *=exp(configValue("weight_distance")/4.0);
// param_too_free +=(configValue("param_too_free"));


#endif


		uninitialized=0;}
// cerr << "####################################" << FBS_handler::board_size << endl;
	// fflush(stderr);
  if (FBS_handler::board_size==19)
  {
	  computeGuillaume(basic_instinct,color);
  }
  else 
  if (FBS_handler::board_size==13)
	  computeGuillaume13(basic_instinct,color);

#endif
  //computeSlow(fastboard,color);
//#define TRISTAN
#define TRISTAN_GOBAN_SIZE (FBS_handler::board_size) 
#define TRISTAN_GOBAN_SIZE_2 ((TRISTAN_GOBAN_SIZE*TRISTAN_GOBAN_SIZE))
#ifdef TRISTAN
{
   int board[TRISTAN_GOBAN_SIZE_2];
   double value[TRISTAN_GOBAN_SIZE_2];
   Location location = TRISTAN_GOBAN_SIZE+3;//node->indexOnArray(0,0); //DIRTY!
	for (int i=0;i<TRISTAN_GOBAN_SIZE;i++, location+=2)
	for (int j=0;j<TRISTAN_GOBAN_SIZE;j++, location++) 
	{
		board[i*TRISTAN_GOBAN_SIZE+j]= (int(round(fastboard->position(location)))*2-1)%3;
		value[i*TRISTAN_GOBAN_SIZE+j]=0;
		/*
		 * mogo        mango
			0 --> -1
			1 --> 1
			2 --> 0*/
	}
// TRISTAN CA SE PASSE ICI --- tu decommentes ci-dessous si tu veux afficher
// mais a priori tu n'en as pas besoin
	/*for (int i=0;i<TRISTAN_GOBAN_SIZE;i++)
	{
		for (int j=0;j<TRISTAN_GOBAN_SIZE;j++)
		{
			if (board[i*TRISTAN_GOBAN_SIZE+j]==1)
				printf("B");
			else if (board[i*TRISTAN_GOBAN_SIZE+j]==-1)
				printf("W");
			else printf(" ");

		}
		printf("\n");
	}*/
	//assert(0);
	//Forme_kppv::match(value,board,30.0/WEIGHT_EXPERTISE,(color%2)*2-1);      // That's the function!

       // C'EST LA QUE TU INTERVIENS TRISTAN:
        // board contient un goban sous la forme qui suit:
        //type:tableau de "int" de taille 81 en 9x9, 361 en 19x19
        //valeurs: 0 pour case (i,j) vide, 
        //         1 pour pierre (i,j) du joueur 1,
        //         2 pour pierre (i,j) du joueur 2
        //a faire:
        //mettre dans value[i*TRISTAN_GOBAN_SIZE+j] 
        //      un nombre positif grand si point d'interet (ne pas avoir peur
        //                            de mettre 100 ou plus)
        //      eventuelleemnt, un nombre negatif si mauvais (pas oblige)
        // 
        // TRISTAN TES MODIFS DOIVENT NORMALEMENT ETRE AU DESSOUS DE CETTE LIGNE

	// TRISTAN ICI !!! ==== une boucle pour remplir value[i*TRISTAN_GOBAN_SIZE+j]
	// ATTENTION ICI C'EST FORCEMENT UNE MODIF QUI VA VITE (AU PLUS 1/1000s)
        // JE METTRAI UN DEUXIEME PATCH POUR UNE MODIF "LENTE"

        // TRISTAN TES MODIFS DOIVENT NORMALEMENT ETRE AU DESSUS DE CETTE LIGNE
        location = TRISTAN_GOBAN_SIZE+3;
	for (int i=0;i<TRISTAN_GOBAN_SIZE;i++, location+=2)
	for (int j=0;j<TRISTAN_GOBAN_SIZE;j++, location++) 
		(*basic_instinct)[location]+=value[i*TRISTAN_GOBAN_SIZE+j];
}
#endif
 
//   int chainNext[MAX_FAST_BOARD_TAB_SIZE];
//   int chainId[MAX_FAST_BOARD_TAB_SIZE];
  
  
//   int current = location;
//   do 
//     {
//       compute(basic_instinct,color,current);
//       current = chainNext[current];
//     } 
//   while (int(current) != location);
}



#ifdef GOLDENEYE
pthread_mutex_t MoGo::BasicInstinctFast::mutexComputeSlow;

void MoGo::BasicInstinctFast::computeSlow(FastBoard * fastboard,int color,vector<int>*guessLocation,vector<int>*guessColor,vector<double>*guessWeight,GobanNode*node)
{
	pthread_mutex_init(&mutexComputeSlow,0);
	guessLocation->resize(0);
  //compute(basic_instinct); //decommenter ?
 // cerr << " getBigArea =" << FBS_handler::getBigArea() << endl;
 // exit(0);
  
	// cerr << "####################################" << FBS_handler::board_size << endl;
  pthread_mutex_lock(&mutexComputeSlow);
  if (node->golden)
  {
	  pthread_mutex_unlock(&mutexComputeSlow);
	  return;
  }
  GE_Bibliotheque::init(FBS_handler::board_size,FBS_handler::board_size); //initialise aussi GE_Directions
  
  GE_Goban ge_gob(FBS_handler::board_size,FBS_handler::board_size);
  for(int big_location = 0; big_location<FBS_handler::getBigArea(); big_location++)
    {
      int location = GE_Bibliotheque::to_goban[big_location];  

      if(location<0)
	continue;
      
      int color_stone = fastboard->position(big_location);
      int ge_color_stone = GE_WITHOUT_STONE;
      switch(color_stone)
	{
	case FB_BLACK : ge_color_stone = GE_BLACK_STONE; break;
	case FB_WHITE : ge_color_stone = GE_WHITE_STONE; break;
	case FB_EMPTY : ge_color_stone = GE_WITHOUT_STONE; break;
	default : ;
	}
      
      ge_gob.set_stone(location,ge_color_stone);
    }
  
  team_color ge_color_player = GE_BLACK;
  if(color==BLACK)  //OLIVIERFIXME plus fixme
    ge_color_player = GE_BLACK;
  else ge_color_player = GE_WHITE;
  
  GE_Game ge_game(ge_gob,ge_color_player);
  
  GE_FastGoban ge_fg;
  GE_FastGoban ge_fg_for_simu;
  
  //GE_Directions::init(gob.height, gob.width);
  ge_fg.init();
  ge_fg_for_simu.init();
  ge_fg.fromGame(ge_game);
  

  //construction de la liste des pierres blanches et de la liste des pierres noires

  vector<int> big_black_stones;
  vector<int> big_white_stones;

  for(int big_loc = 0; big_loc<(int)ge_fg.goban.size(); big_loc++)  
    {
      if(ge_fg.goban[big_loc]==GE_BLACK_STONE)
	{
	  big_black_stones.push_back(big_loc);//GE_Bibliotheque::to_goban[big_loc]);
	  continue;
	}

      if(ge_fg.goban[big_loc]==GE_WHITE_STONE)
	{
	  big_white_stones.push_back(big_loc);//GE_Bibliotheque::to_goban[big_loc]);
	  continue;
	}
    }
  

  //int ge_friend_color_stone = GE_BLACK_STONE;
  //if(ge_color_player==GE_WHITE)
  //ge_friend_color_stone = GE_WHITE_STONE;
  //GE_FastMCLocationsInformations ge_fmcli(ge_friend_color_stone);
  
  clock_t t_start;
  clock_t t_end;
  float timing = 0.f;
  
  vector<vector<int> > v_semeai;
  v_semeai.resize(GE_Directions::board_area);
  for(int i = 0; i<(int)v_semeai.size(); i++)
    {
      v_semeai[i].resize(GE_Directions::board_area, 0);
    }


  vector<int> v_group;
  v_group.resize(GE_Directions::board_area, 0);
  
  // ge_fg.print_error_goban();
  
  int num_sim = 0; 
  int max_sim = 100;
  int min_capture = 4; //le MC ne reprend pas des groupes de plus de 4 pierres
  //double my_komi = 7.5f; //Comment recuperer l'information du komi
  
  
  
  while(true)
    {      
      t_start = clock();
      //fg.fromGame(*game);
      ge_fg_for_simu.copy(ge_fg);
      ge_fg_for_simu.launch_simulation(min_capture);
      //ge_fmcli.update(ge_fg_for_simu, my_komi);
      
      GE_FastMCLocationsInformations::update_stats_semeai(ge_fg_for_simu, big_black_stones, 
							  big_white_stones, v_semeai);
      
      GE_FastMCLocationsInformations::update_stats_group(ge_fg, ge_fg_for_simu, 
							 v_group);
      
      t_end = clock();
      
      timing+=((t_end-t_start)/(float)CLOCKS_PER_SEC);
      
      num_sim++;
      
      
      if(num_sim>=max_sim)
	break;
    }
  
  GE_Chains chs;
  GE_AnalyzeSemeai a_sem;
  a_sem.setSemeais(ge_game, v_semeai, max_sim, chs);
  /* setSemeais recupere les semeai a partir des si;ulations
   * a_sem contient les semeai
   * v_semeai contient les statistiques des simulations
   */
  
  //a_sem.print_error_moves_for_semeai(ge_game);
  /**
   * affiche les positions concernees par le semeai
   *
   */
  
  GE_Informations infos(ge_game);
  infos.init_stats_group(v_group, max_sim, ge_game.tc);
  infos.init_neighbour_groups(ge_game);
  infos.init_group_mobility(ge_game);
  infos.init_virtual_connections();
  infos.init_eyes();
  infos.init_false_eyes();
  infos.init_type_holes();
  infos.find_unkillable();
  infos.init_alive_with_unkillable_group();
  infos.init_alive_with_possible_connexion();
  infos.init_alive_by_direct_connexion_two_eyes();
  infos.init_alive_by_connexion_different_one_eyes();
  infos.init_alive_by_connexion_two_eyes();
  infos.init_alive_with_alive_group();
  
  infos.study_alive(ge_game);
  infos.find_sekis();
  
  
  //infos.init_false_wall_connections();
  
  infos.init_groups_networks(GE_POSSIBLE_CONNEXION);
  
  infos.study_alive_by_stats();
  
  infos.refine_possible_eyes();
  infos.init_possible_eyes();
  
  
  
  //sleep(20);
  list<int> bl_moves;
  list<int> wl_moves;
  //a_sem.moves_for_semeais(ge_game,bl_moves,wl_moves);

  a_sem.initVitalStones(ge_game,infos);
  a_sem.initFrontiers(ge_game,infos);a_sem.print_error_semeais();
  ge_game.print_error_goban();
  fprintf(stderr,"basicinstinct et les libertes\n");fflush(stderr);
  a_sem.toolsLiberties(ge_game);
  fprintf(stderr,"basicinstinct et les libertes   ---   over\n");fflush(stderr);
  
  GE_Semeai* temp_best_semeai = 0;
  
  int nbSimusForSemeai = 100;
  int res = a_sem.study_best_semeais(ge_game,nbSimusForSemeai,&infos,
				     &(temp_best_semeai));
  
  Location locationAliveAtTheEnd = -1;
  int colorShouldBe = BLACK;
double borne_min_for_win = 0.9;

 int temp_res= GE_NO_RESULT;
  double stats_life = 0;
  if(temp_best_semeai)
    {
      (temp_best_semeai->solveur).get_best_result(temp_res,stats_life);
     
      assert(temp_res==res);
	  Goban goban(fastboard->getGobanKey()); 
	  
	  goban.textModeShowGoban();
cerr<<"I think my chance to win the semeai is "<<stats_life<<endl;
  }

  //if((res!=GE_NO_RESULT)||(stats_life>=borne_min_for_win))
      if (temp_best_semeai)
  if (stats_life<1-borne_min_for_win) //OLIVIERFIXME
    {
	    fprintf(stderr,"solved\n");fflush(stderr);
      //if(temp_best_semeai->main_tree_semeai==&((temp_best_semeai->solveur).only_win))
	{
		fprintf(stderr,"action\n");
	  
	  if(color==BLACK) //OLIVIERFIXME plus fixme
	    {
	//	fprintf(stderr,"### %d ###\n",temp_best_semeai);fflush(stderr);
	//	    fprintf(stderr,"<< %d >>\n",temp_best_semeai->main_black_stone);fflush(stderr);
	      locationAliveAtTheEnd = 
		GE_Bibliotheque::to_big_goban[temp_best_semeai->main_black_stone];   // avec la location   (ou -1 si on ne détecte pas de semeai bien tranché)
	      colorShouldBe = FB_WHITE; 
	    }
	  else
	    {
	      locationAliveAtTheEnd = 
		GE_Bibliotheque::to_big_goban[temp_best_semeai->main_white_stone];   // avec la location   (ou -1 si on ne détecte pas de semeai bien tranché)
	      colorShouldBe = FB_BLACK; 
	    }
	  
#define PRINT_SEMEAI_STONE
#ifdef PRINT_SEMEAI_STONE
	  Goban goban(fastboard->getGobanKey()); 
	  
	  goban.textModeShowGoban();
	  cerr<<endl;
	  cerr<<"the stone ";
	  goban.textModeShowPosition(locationAliveAtTheEnd);   
	  cerr<<"should be ";
	
	  guessLocation->push_back(locationAliveAtTheEnd);
	  guessColor->push_back(colorShouldBe);
	  guessWeight->push_back(0.001);

	  node->goldenbis=1;
	  node->golden=1;
	  if(colorShouldBe==BLACK) cerr<<"black";
	  else cerr<<"white";
	  
	  cerr<<" at the end of the simulation"<<endl<<endl;
#endif
	  
	}
      //else fprintf(stderr,"inaction\n");
    }
  else
  {    fprintf(stderr,"unsolved\n");fflush(stderr);}
  
//OLIVIERFIXME debut

#define K1 500.
#define K2 500.
  if((res!=GE_NO_RESULT)||(stats_life>=borne_min_for_win))
  {
  a_sem.moves_for_semeais(ge_game,bl_moves,wl_moves);
  if (color==BLACK)
  {
  while (not bl_moves.empty())
  {
	int bad_location=bl_moves.front();
      int location = GE_Bibliotheque::to_big_goban[bad_location];
	if (node->nodeUrgency()[location]>0)
	{
		node->nodeUrgency()[location]+=K2;
		node->nodeValue()[location+3]+=K2;
		GobanNode * child =node->getChild(location);
		if (child>0)
		{
			child->nodeValue()[1]+=K1;
			child->resetNumberOfSimulation(child->numberOfSimulation()+K1);
		}
	}
	bl_moves.pop_front();
  }
  }
  else
  {
  while (not wl_moves.empty())
  {
	int bad_location=wl_moves.front();
      int location = GE_Bibliotheque::to_big_goban[bad_location];
	if (node->nodeUrgency()[location]>0)
	{
		node->nodeUrgency()[location]+=K2;
		node->nodeValue()[location+3]+=K2;
		GobanNode * child =node->getChild(location);
		if (child>0)
		{
			child->nodeValue()[1]+=K1;
			child->resetNumberOfSimulation(child->numberOfSimulation()+K1);
		}
	}
	wl_moves.pop_front();
  }
  }
  }
//OLIVIERFIXME fin

#ifdef GOLDENEYE_MC
  fastboard->semeaiB.resize(0);
  fastboard->semeaiW.resize(0);
  
  Goban goban(fastboard->getGobanKey());
  while(not bl_moves.empty())
    {
      int location = bl_moves.front(); 
      
      int big_location = GE_Bibliotheque::to_big_goban[location];
      fastboard->semeaiB.push_back(big_location);
      //   goban.textModeShowPosition(big_location); //sleep(1);
      
      bl_moves.pop_front();
    }
  
  
  while(not wl_moves.empty())
    {
      int location = wl_moves.front(); 
      
      int big_location = GE_Bibliotheque::to_big_goban[location];
      fastboard->semeaiW.push_back(big_location);
      //     goban.textModeShowPosition(big_location); //sleep(1);
      
      wl_moves.pop_front();
    }
  
#endif
  
  
  //fprintf(stderr," black: %d white: %d \n",fastboard->semeaiB.size(),fastboard->semeaiW.size());
  //sleep(1);
  //cerr<<endl;
  //cerr<<"end semeai"<<endl;*/
  pthread_mutex_unlock(&mutexComputeSlow);

}
#endif




void MoGo::BasicInstinctFast::compute(double** basic_instinct, int color, int stone)
{
  assert(fastboard);
  
  if(last_move<=0) return;

  if(fastboard->position(stone)==FB_EMPTY) return;
  
  if(((color==WHITE)&&(fastboard->position(stone)==FB_WHITE))
     ||((color==BLACK)&&(fastboard->position(stone)==FB_BLACK)))
    {
      before_last_move = stone;
      
      this->one_point_jump0(basic_instinct);
      
      this->kogeima0(basic_instinct);  
      
      this->kata0(basic_instinct);
 
      this->bad_empty_triangle0(basic_instinct);
      this->bad_hazama_tobi0(basic_instinct);
	
      this->bad_tobi0(basic_instinct);  
      
      this->bad_kogeima0(basic_instinct);
      
      this->bad_elephant0(basic_instinct);
      return;
      
    }
  
  
  last_move = stone;

  
  //this->avoid_border0(basic_instinct);
  this->avoid_border1(basic_instinct);
  
  //this->atari_extend0(basic_instinct);
  this->atari_extend1(basic_instinct);

  this->peep_connect0(basic_instinct);

  this->tsuke_hane0(basic_instinct);
  this->kosumi_tsuke_stretch0(basic_instinct);
  this->bump_stretch0(basic_instinct);
  

  this->threat0(basic_instinct); //don't put if reduce_liberties0
  
  this->reduce_liberties0(basic_instinct,3); //(eat0 + threat0 + ...)

  this->increase_liberties0(basic_instinct);

  //this->eat_or_to_be_eaten0(basic_instinct);
   this->connect0(basic_instinct);

  this->connect_if_dead_virtual_connection0(basic_instinct);

    this->prevent_connection0(basic_instinct);

  
  //this->wall0(basic_instinct);
  this->wall1(basic_instinct);
  
  this->kosumi0(basic_instinct);

  this->extend0(basic_instinct);
  this->cross_cut0(basic_instinct);

  //this->too_free0(basic_instinct,param_too_free); //replaced by too_free2

  //with local, it's made two times but i think it is not a real problem but ...
}






/*



//A RETESTER


double MoGo::BasicInstinctFast::bonus_line_influence = 0.1f;
double MoGo::BasicInstinctFast::bonus_line_territory = 0.1f;
double MoGo::BasicInstinctFast::malus_line_defeat = -0.05f;
double MoGo::BasicInstinctFast::malus_line_death = -0.1f;



double MoGo::BasicInstinctFast::bonus_peep_connect = 0.9f;
double MoGo::BasicInstinctFast::bonus_atari_extend = 0.99f;
double MoGo::BasicInstinctFast::bonus_tsuke_hane = 0.28f;
double MoGo::BasicInstinctFast::bonus_thrust_block = 0.99f;
double MoGo::BasicInstinctFast::bonus_kosumi_tsuke_stretch = 0.25f;
double MoGo::BasicInstinctFast::bonus_kosumi_tsuke_stretch2 = 0.08f; 
//when I need sabaki (cf senseis: basicInstinct)
double MoGo::BasicInstinctFast::bonus_bump_stretch = 0.24f;

double MoGo::BasicInstinctFast::bonus_eat = 0.99f;
double MoGo::BasicInstinctFast::bonus_threat = 0.8f;
double MoGo::BasicInstinctFast::bonus_eat_or_to_be_eaten = 1.5f;
double MoGo::BasicInstinctFast::bonus_connect = 0.3f;
double MoGo::BasicInstinctFast::bonus_connect_if_dead_virtual_connection = 0.3f;
double MoGo::BasicInstinctFast::bonus_reduce_liberties = 0.2f;
double MoGo::BasicInstinctFast::bonus_increase_liberties = 0.13f;
double MoGo::BasicInstinctFast::bonus_prevent_connection = 0.15f;
double MoGo::BasicInstinctFast::bonus_one_point_jump = 0.1f;
double MoGo::BasicInstinctFast::bonus_kogeima = 0.15f; //0.4f

double MoGo::BasicInstinctFast::bonus_wall = 0.17f;

double MoGo::BasicInstinctFast::malus_bad_tobi = -0.15f;
double MoGo::BasicInstinctFast::malus_bad_kogeima = -0.15f;
double MoGo::BasicInstinctFast::malus_bad_elephant = -0.15f;

double MoGo::BasicInstinctFast::malus_border = -0.7f;
double MoGo::BasicInstinctFast::bonus_if_last_border = 0.5f;
double MoGo::BasicInstinctFast::bonus_if_last_corner = 0.7f;


void MoGo::BasicInstinctFast::compute(double** basic_instinct) const
{
  assert(fastboard);
  
  if(last_move<=0) return;
  
  this->init0(basic_instinct);

  //this->avoid_border0(basic_instinct);
  this->avoid_border1(basic_instinct);
  
  //this->atari_extend0(basic_instinct);
  //this->atari_extend1(basic_instinct);
  //this->peep_connect0(basic_instinct);
  //this->tsuke_hane0(basic_instinct);
  //this->kosumi_tsuke_stretch0(basic_instinct);
  //this->bump_stretch0(basic_instinct);
  
  //this->eat0(basic_instinct); //don't put if reduce_liberties0
  //this->threat0(basic_instinct); //don't put if reduce_liberties0
  //this->reduce_liberties0(basic_instinct,3); //(eat0 + threat0 + ...)
  //this->increase_liberties0(basic_instinct);
  //this->eat_or_to_be_eaten0(basic_instinct);
  //this->connect0(basic_instinct);
  //this->connect_if_dead_virtual_connection0(basic_instinct);
  this->prevent_connection0(basic_instinct); //
  this->one_point_jump0(basic_instinct);
  this->kogeima0(basic_instinct);
  
  //this->wall0(basic_instinct);
  
  this->bad_tobi0(basic_instinct);
  this->bad_kogeima0(basic_instinct);
  this->bad_elephant0(basic_instinct);
}
*/


/*
G1
void MoGo::BasicInstinctFast::compute(double** basic_instinct) const
{
  assert(fastboard);
  
  if(last_move<=0) return;
  
  this->init0(basic_instinct);

  //this->avoid_border0(basic_instinct);
  this->avoid_border1(basic_instinct);
  
  //this->atari_extend0(basic_instinct);
  this->atari_extend1(basic_instinct);
  this->peep_connect0(basic_instinct);
  this->tsuke_hane0(basic_instinct);
  this->kosumi_tsuke_stretch0(basic_instinct);
  this->bump_stretch0(basic_instinct);
  
  this->eat0(basic_instinct); //don't put if reduce_liberties0
  this->threat0(basic_instinct); //don't put if reduce_liberties0
  //this->reduce_liberties0(basic_instinct,3); //(eat0 + threat0 + ...)
  //this->increase_liberties0(basic_instinct);
  this->eat_or_to_be_eaten0(basic_instinct);
  this->connect0(basic_instinct);
  //this->connect_if_dead_virtual_connection0(basic_instinct);
  //this->prevent_connection0(basic_instinct);
  this->one_point_jump0(basic_instinct);
  this->kogeima0(basic_instinct);
  
  //this->wall0(basic_instinct);
  
  this->bad_tobi0(basic_instinct);
  this->bad_kogeima0(basic_instinct);
  this->bad_elephant0(basic_instinct);
}
*/





/*
double MoGo::BasicInstinctFast::bonus_line_influence = 0.1f;
double MoGo::BasicInstinctFast::bonus_line_territory = 0.1f;
double MoGo::BasicInstinctFast::malus_line_defeat = -0.05f;
double MoGo::BasicInstinctFast::malus_line_death = -0.1f;



double MoGo::BasicInstinctFast::bonus_peep_connect = 0.25f;
double MoGo::BasicInstinctFast::bonus_atari_extend = 0.3f;
double MoGo::BasicInstinctFast::bonus_tsuke_hane = 0.23f;
double MoGo::BasicInstinctFast::bonus_thrust_block = 0.28f;
double MoGo::BasicInstinctFast::bonus_kosumi_tsuke_stretch = 0.22f;
double MoGo::BasicInstinctFast::bonus_kosumi_tsuke_stretch2 = 0.08f; 
//when I need sabaki (cf senseis: basicInstinct)
double MoGo::BasicInstinctFast::bonus_bump_stretch = 0.22f;

double MoGo::BasicInstinctFast::bonus_eat = 0.3f;
double MoGo::BasicInstinctFast::bonus_threat = 0.15f;
double MoGo::BasicInstinctFast::bonus_eat_or_to_be_eaten = 0.2f;
double MoGo::BasicInstinctFast::bonus_connect = 0.2f;
double MoGo::BasicInstinctFast::bonus_connect_if_dead_virtual_connection = 0.25f;
double MoGo::BasicInstinctFast::bonus_reduce_liberties = 0.1f;
double MoGo::BasicInstinctFast::bonus_increase_liberties = 0.13f;
double MoGo::BasicInstinctFast::bonus_prevent_connection = 0.2f;
double MoGo::BasicInstinctFast::bonus_one_point_jump = 0.2f;
double MoGo::BasicInstinctFast::bonus_kogeima = 0.25f; //0.4f

double MoGo::BasicInstinctFast::bonus_wall = 0.17f;

double MoGo::BasicInstinctFast::malus_bad_tobi = -0.15f; 
double MoGo::BasicInstinctFast::malus_bad_kogeima = -0.15f;
double MoGo::BasicInstinctFast::malus_bad_elephant = -0.15f;
double MoGo::BasicInstinctFast::malus_bad_empty_triangle = -0.15f;

double MoGo::BasicInstinctFast::malus_border = -0.2f;
double MoGo::BasicInstinctFast::bonus_if_last_border = 0.1f;
double MoGo::BasicInstinctFast::bonus_if_last_corner = 0.2f;
*/












/*
  double MoGo::BasicInstinctFast::bonus_line_influence = 0.1f;
  double MoGo::BasicInstinctFast::bonus_line_territory = 0.1f;
  double MoGo::BasicInstinctFast::malus_line_defeat = -0.05f;
  double MoGo::BasicInstinctFast::malus_line_death = -0.1f;
  
  
  
  double MoGo::BasicInstinctFast::bonus_peep_connect = 0.25f;
  double MoGo::BasicInstinctFast::bonus_atari_extend = 0.25f; //0.25f;
  double MoGo::BasicInstinctFast::bonus_tsuke_hane = 0.4f;
  double MoGo::BasicInstinctFast::bonus_thrust_block = 0.1f;
  double MoGo::BasicInstinctFast::bonus_kosumi_tsuke_stretch = 0.05f; //0.18f;
  double MoGo::BasicInstinctFast::bonus_kosumi_tsuke_stretch2 = 0.02f; 
  //when I need sabaki (cf senseis: basicInstinct)
  double MoGo::BasicInstinctFast::bonus_bump_stretch = 0.08f;//0.16f;
  
  double MoGo::BasicInstinctFast::bonus_eat = 0.7f;
  double MoGo::BasicInstinctFast::bonus_threat = 0.45f;//0.095f;
  double MoGo::BasicInstinctFast::bonus_eat_or_to_be_eaten = 0.6f;
  double MoGo::BasicInstinctFast::bonus_connect = 0.4f;//0.2f;  
  double MoGo::BasicInstinctFast::bonus_connect_if_dead_virtual_connection = 0.15f;
  
  double MoGo::BasicInstinctFast::bonus_reduce_liberties = 0.05f;
  double MoGo::BasicInstinctFast::bonus_increase_liberties = 0.05f;
  double MoGo::BasicInstinctFast::bonus_prevent_connection = 0.5f;
  double MoGo::BasicInstinctFast::bonus_one_point_jump = 0.35f;
  double MoGo::BasicInstinctFast::bonus_kogeima = 0.2f; //0.4f
  
  double MoGo::BasicInstinctFast::bonus_wall = 0.35f;
  
  double MoGo::BasicInstinctFast::malus_bad_tobi = -0.25f;
  double MoGo::BasicInstinctFast::malus_bad_kogeima = -0.15f;
  double MoGo::BasicInstinctFast::malus_bad_elephant = -0.25f;
  double MoGo::BasicInstinctFast::malus_bad_empty_triangle = -0.25f;
  double MoGo::BasicInstinctFast::malus_bad_hazama_tobi = -0.25f;
  
  double MoGo::BasicInstinctFast::malus_border = -0.2f;
  double MoGo::BasicInstinctFast::bonus_if_last_border = 0.1f;
  double MoGo::BasicInstinctFast::bonus_if_last_corner = 0.2f;
*/
