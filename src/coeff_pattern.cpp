#include "coeff_pattern.h"


// TODO : Rajouter un test sur urgence pour savoir si la fonction est appellee de match OU match_nv
double	coeff_pattern(double	p,
		      double	v,
		      double	urgence,
		      double	k,
		      double	rapport,
		      double 	alpha1,
		      double	alpha2)
{
  double coeff = 0;
//Formule NN :
coeff = rapport * p * (1 + tanh(alpha1 * k + alpha2 * v));
//2ème formule : 
//coeff = (1-alpha1-alpha2) * rapport * p + alpha1 * k + alpha2 * v;

  return coeff;
}

