#include <stdlib.h>
#include "Arbre.h"
#include "Forme_kppv.h"
#include <iostream>

using namespace std;

class Forme_kppv;

Arbre::Arbre(Arbre * vi, Arbre * no, Arbre * bl, Arbre * bo, Forme_kppv * pat) {
noir=no;
blanc=bl;
vide=vi;
bord=bo;
pattern_associe=pat;
/*lien[1]=NULL;
lien[2]=NULL;
lien[3]=NULL;
lien[4]=NULL;
lien_urgence[1]=0;
lien_urgence[2]=0;
lien_urgence[3]=0;
lien_urgence[4]=0;
lien_hauteur_faite[1]=0;
lien_hauteur_faite[2]=0;
lien_hauteur_faite[3]=0;
lien_hauteur_faite[4]=0;
*/
}


Arbre::~Arbre()
{
	if (noir) {delete noir;}

	if (blanc) {delete blanc;}
	if (vide) {delete vide;}
	if (bord) {delete bord;}
	if (pattern_associe) {delete pattern_associe;}
}
