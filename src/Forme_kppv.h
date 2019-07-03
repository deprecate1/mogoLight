#ifndef FORME_KPPV_H
#define FORME_KPPV_H

//#include "Parameters.h" 

// TODO: remove this useless dependancy
#include <fstream>
#include <iostream>
#include "goexperiments_mg.h"

using namespace std;

#define NB_CERCLES 165 //   Le nombre de cercles est 164 en fait (+cercle du milieu=165)

#define GOBAN_SIZE_2 361
#define GOBAN_SIZE 19

#define KPP_NB_VOISINES 36 
#define KPP_URGENCE_3EME_LIGNE 100
#define T_URG int
#define T_IDX int

#define TAILLE_TABLE_POS_IDX_VERS_IDX 500000


#define PRINTLOG(x) cerr<<x<<endl;

#define COEF_APPRENTISSAGE 1
#define GBLACK 1
#define GWHITE -1 

//#include "apprentissage_kppv.h"


class apprentissage_kppv;
class Element_kppv;
class Liste;
class Arbre;
class Forme_kppv{
friend class MoGo::GoExperiments;
public:
  static void init_class();
  static void delete_all();
  static void init_urgences(Arbre *,double,double,double,double,double,double);
  static T_URG de_proba_a_urgence_noir(double,double,double);
  static T_URG de_proba_a_urgence_blanc(double,double,double);
  
	static void construire_liens(Arbre *,Arbre *,int,int);
	static void lier_pattern(Arbre *, Arbre *,int,int,double);
	static void verification(void);
	static void effacer_listes(Arbre *);
	
	// patterns 3x3 int�r�
	static void tests(void);
	static Forme_kppv * generer_le_pattern_3x3(long long int);
	static void generer_patterns_3x3(Arbre *,int,long long int);
	static Arbre * grimper(Arbre *,int);

	// intialisation des tables
	static int calculer_coordonees_construction(int,int,int);
	static int calculer_coordonees(int,int,int);
	static void calculer_table_pos_idx_vers_pos(void);
	static void calculer_table_pos_idx_vers_pos_construction(void);

	static int cercle_no_vers_x [NB_CERCLES][24];
	static int cercle_no_vers_y [NB_CERCLES][24];
	static int contenances_triees[NB_CERCLES+1];
	static int contenances_c[NB_CERCLES+1];
	static int x_y_vers_anneau[37][37];
	static int x_y_vers_no[37][37];
	static double urgences_noir[GOBAN_SIZE_2+1];
	static double urgences_blanc[GOBAN_SIZE_2+1];

	static int the_goban [GOBAN_SIZE_2+1];	
	static int goban_recure [GOBAN_SIZE_2+1];	

//	static int tailles_urgences[GOBAN_SIZE_2];
	static int table_pos_idx_vers_idx[TAILLE_TABLE_POS_IDX_VERS_IDX];
	static Arbre * racine_arbre;
	static Arbre * racine_arbre_nv;
	static Arbre * racine_arbre_nv_bord;
	static Arbre * racine_arbre_blanc;
	static Arbre * racine_arbre_noir;
	static Liste * liste_des_patterns_blanc;
	static Liste * liste_des_patterns_noir;
	static ifstream * ifile;
	static ofstream * ofile;
	static Arbre * arbre_retour;
//	static int quarantaines[362];
	static int meilleur;
	static int moins_bon;
	static double valeur_du_meilleur;
	static double valeur_du_moins_bon;
	static double valeur_de_la_moyenne;

// liens
	static int goban_construction[1521];
	static int table_pos_idx_vers_idx_construction[6845];
// Probas
	static long long int nb_boucles;	//
	static int n_joue;	//
	static int n_test;	//
//	static double p_joue;	
	static int nb_de_patterns_total;
	static int nb_de_patterns_sauves[30];
	static int nb_de_patterns_affiches;
	static int nb_de_patterns_lus;
	static double somme_GRV_noir;
	//static double somme_GRV_blanc;
	static double somme_V_noir;
	//static double somme_V_blanc;
	static double nb_coups;
//////////////////////////////////////// Variables non statiques /////////////////////////////////
	double GRV_noir;
	double proba_pres_noir;
	T_URG urgence_noir;
	T_URG urgence_noir_init;
	T_URG urgence_blanc;
	T_URG urgence_blanc_init;
	double urgence_noir_app;  
	double urgence_blanc_app;
	unsigned char K;
	unsigned char nfs;
	Element_kppv * liste_pierres;
	//Forme_kppv * symetrie_suivante;
// Probas :
	double bonte_noir;
	double bonte_blanc;
	double n_match_noir;
	double n_match_blanc;
	unsigned int n_joue_sachant_match;
	double p_joue_sachant_match;
	double p_joue_sachant_match_noir;
	double p_joue_sachant_match_blanc;
	

	
// Constructeur
	Forme_kppv(unsigned char,unsigned short int,unsigned char, long long int);
	~Forme_kppv();
// Initialisation
	static void trouver_cercles(int);
	static void creer_les_listes(void);
	void mettre_pattern_dans_arbre(Arbre *);
//	static void marquer_les_bords(int[GOBAN_SIZE_2]);

// Matching
	static void match_proba_noir(int);
	static void match_proba_blanc(int);

//	static void match_77(int);

	static void match(double *,T_IDX *,double,int);
	static void match_nv(double *,T_IDX *,double,int);
	static void match_blanc(void);
	static void match_reduit_lineaire(int);
	static void match_recurrent(int);
	static void match_recurrent_noir(void);

	static void match_intersection_recurrent(int,Arbre *,int );
	static void match_intersection(int &,Arbre *,int);


	static int trouver_urgence_max(double *);
// Génération de patterns
	static void generer_un_pattern(int[GOBAN_SIZE_2],int,unsigned char,int);

// Construction de l'arbre
	static void detruire_feuilles(Arbre *);
	static void detruire_arbre(Arbre *);

// Procédures de lecture écriture écran
	static void ecrire_nos_2d(char *);
	static void ecrire_urgences(void);
	static void ecrire_urgences_blanc(void);
	static void ecrire_urgences_noir(void);
	static void ecrire_damier(int[GOBAN_SIZE_2]);
	static void afficher_arbre(Arbre *,int,double,double,unsigned char);
	static void ecrire_arbre(Arbre *,int,double,double,double,int);

// Procédures de lecture écriture fichiers
	static int lire_pattern_nv(int,int&);
	static int lire_pattern_nv_bord(int,int&);
	static int lire_tous_les_patterns_nv(int);
	static int lire_tous_les_patterns_nv_bord(int);
	static Forme_kppv * lire_pattern(int);
	static int lire_tous_les_patterns_noir(void);
	static int lire_tous_les_patterns_blanc(void);
	static void ecrire_tous_les_patterns_nv(double,double,double);
	int ecrire_pattern_2d(ostream *); 	// a modifier pour fichier
	void fimprimer(ostream &, int, int);
	void ecrire_le_pattern(void);

	static double alpha1;
	static double alpha2;
};
#endif
