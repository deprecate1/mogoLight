#ifndef ARBRE_H
#define ARBRE_H

class Forme_kppv;

class Arbre{
public:

// Constructeur :
	Arbre(Arbre *,Arbre *,Arbre *,Arbre *,Forme_kppv *);
// Méthodes :
	static void remplir_arbre(Forme_kppv *);

// Variables :
	Arbre * blanc;
	Arbre * noir;
	Arbre * vide;
	Arbre * bord;
	Forme_kppv * pattern_associe;
// Liens
/*	Arbre * lien[5];
	double lien_urgence[5];
	int lien_hauteur_faite[5];*/
	~Arbre();
};
#endif
