#ifndef ARBRE_H
#define ARBRE_H

class Forme_kppv13;

class Arbre13{
public:

// Constructeur :
	Arbre13(Arbre13 *,Arbre13 *,Arbre13 *,Arbre13 *,Forme_kppv13 *);
// Méthodes :
	static void remplir_arbre(Forme_kppv13 *);

// Variables :
	Arbre13 * blanc;
	Arbre13 * noir;
	Arbre13 * vide;
	Arbre13 * bord;
	Forme_kppv13 * pattern_associe;
// Liens
/*	Arbre13 * lien[5];
	double lien_urgence[5];
	int lien_hauteur_faite[5];*/
	~Arbre13();
};
#endif
