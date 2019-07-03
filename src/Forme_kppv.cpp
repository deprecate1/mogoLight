#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

#include "Arbre.h"
#include "Forme_kppv.h"
#include "Element_kppv.h"
#include "coeff_pattern.h"
//#include "Fichier_kppv.h"
//#include "apprentissage_kppv.h"

using namespace std;

#define PASNEG(X) (X<0)?0:X
#define GOOD(X) ((X>=0)&&(X<19))?X:400
#define GOOD39(X) ((X>=0)&&(X<39))?X:400
#define K_MAX 255
#define BEAUCOUP 2000						//   Pour le tri


// modification guidee des fonction importantes : rechercher les "modification" !

int Forme_kppv::cercle_no_vers_x [NB_CERCLES][24];		// Les no commencent à 0
int Forme_kppv::cercle_no_vers_y [NB_CERCLES][24];
int Forme_kppv::x_y_vers_anneau[37][37];		// arguments : 18+i
int Forme_kppv::x_y_vers_no[37][37];
int Forme_kppv::contenances_triees[NB_CERCLES+1];
int Forme_kppv::contenances_c[NB_CERCLES+1];              	// Contenances cumulées
double Forme_kppv::urgences_noir[GOBAN_SIZE_2+1];
double Forme_kppv::urgences_blanc[GOBAN_SIZE_2+1];
int Forme_kppv::table_pos_idx_vers_idx[TAILLE_TABLE_POS_IDX_VERS_IDX];
Liste * Forme_kppv::liste_des_patterns_blanc;
Liste * Forme_kppv::liste_des_patterns_noir;
Arbre * Forme_kppv::racine_arbre_blanc;
Arbre * Forme_kppv::racine_arbre_noir;
Arbre * Forme_kppv::racine_arbre_nv;
Arbre * Forme_kppv::racine_arbre_nv_bord;
Arbre * Forme_kppv::racine_arbre;
Arbre * Forme_kppv::arbre_retour;
ifstream * Forme_kppv::ifile;
ofstream * Forme_kppv::ofile;
double Forme_kppv::nb_coups;
double Forme_kppv::alpha1;
double Forme_kppv::alpha2;

//int Forme_kppv::tailles_urgences[GOBAN_SIZE_2+1];
//int Forme_kppv::quarantaines[362];
int Forme_kppv::the_goban[GOBAN_SIZE_2+1];
int Forme_kppv::goban_recure[GOBAN_SIZE_2+1];

// Liens
int Forme_kppv::goban_construction[1521];
int Forme_kppv::table_pos_idx_vers_idx_construction[6845];

// Probas
	int Forme_kppv::n_joue;	
	int Forme_kppv::n_test;
	int Forme_kppv::nb_de_patterns_total;
	int Forme_kppv::nb_de_patterns_sauves[30];
	int Forme_kppv::nb_de_patterns_affiches;
	int Forme_kppv::nb_de_patterns_lus;
	long long int Forme_kppv::nb_boucles;

//	int Forme_kppv::p_joue;

void Forme_kppv::ecrire_damier(int damier [GOBAN_SIZE_2+1]){
int i;
int j;
int Cho=0;
char * c;
for (i=0;i<19;i++){
	cout << endl;
	for (j=0;j<19;j++){
	c=". ";
	if ((i==3)||(i==9)||(i==15)){
		if ((j==3)||(j==9)||(j==15)) {	c=", ";}
	}
	if (damier[Cho]==1){c="@ ";}
	if (damier[Cho]==-1){c="0 ";}
	if (damier[Cho]==2){c="- ";}
	cout << c;
	Cho++;
	}
	cout <<"  "<<19-i;
}
cout << endl;
cout<<"a b c d e f g h j k l m n o p q r s t"<<endl;
cout << endl;
}


//**********************************************************************************************
//****************************** Initialisation ************************************************
//**********************************************************************************************

// INITIALISATION DE LA LISTE DES PATERNS, RACINE DE L'ARBRE
void Forme_kppv::creer_les_listes(void){
//racine_arbre_noir=new Arbre(NULL,NULL,NULL,NULL,NULL);
//racine_arbre_blanc=new Arbre(NULL,NULL,NULL,NULL,NULL);
racine_arbre_nv=new Arbre(NULL,NULL,NULL,NULL,NULL);
racine_arbre_nv_bord=new Arbre(NULL,NULL,NULL,NULL,NULL);
racine_arbre=new Arbre(NULL,NULL,NULL,NULL,NULL);
}

void Forme_kppv::delete_all()
{
	PRINTLOG(" ===> Delete all trees... ")

	delete racine_arbre_nv;
	delete racine_arbre_nv_bord;
	delete racine_arbre;
}

void Forme_kppv::trouver_cercles(int n){

int i;
int indice;
int j;
bool l;
int k;
int d;
int occupation[NB_CERCLES+1];
int distances[NB_CERCLES+1];
int distances_triees[NB_CERCLES+1];
int distance_compt=0;
int contenances[NB_CERCLES+1];

// MISE A 0 DES URGENCES

//for (k=0;k<1521;k++) {goban_construction[k]=3;}
//for (k=0;k<GOBAN_SIZE_2+1;k++) {goban_recure[k]=0;}

// CALCUL DES DISTANCES

for (k=0;k<=NB_CERCLES;k++) {contenances[k]=1;}
for (k=0;k<NB_CERCLES;k++) {distances[k]=0;}
for (i=-n;i<=n;i++){
	for (j=-n;j<=n;j++){
	d=(j*j+i*i);
	l=0;
	for (k=0;k<NB_CERCLES;k++){if (distances[k]==d) {l=1;contenances[k]+=1;}}
	if (l==0) {distance_compt++;distances[distance_compt]=d;}
	}
}
contenances[0]=0;
//  TRI
i=0;
k=0;
d=0;
indice=0;
for (i=0;i<=NB_CERCLES;i++){
	d=BEAUCOUP;
	for (j=0;j<NB_CERCLES;j++) {
		if (distances[j]<d) {d=distances[j];indice=j;}
	}
	distances_triees[i]=distances[indice];
	contenances_triees[i]=contenances[indice];
	distances[indice]=BEAUCOUP;
}
//  ATTRIBUTION DES ANNEAUX

for (k=0;k<=NB_CERCLES;k++) {occupation[k]=0;}
for (i=-n;i<=n;i++){
	for (j=-n;j<=n;j++){
	d=(j*j+i*i);
	for (k=0;k<=NB_CERCLES;k++){
		if (d==distances_triees[k]) {
		cercle_no_vers_x[k][occupation[k]]=i;
		cercle_no_vers_y[k][occupation[k]]=j;
		x_y_vers_anneau[i+18][j+18]=k;
		x_y_vers_no[i+18][j+18]=occupation[k];
		occupation[k]=occupation[k]+1;
		}}
	}
}
/*for (i=-n;i<=n;i++){
	for (j=-n;j<=n;j++){
cout << " "<<x_y_vers_no[i][j];}}*/

//  Calcul des contenances cumulées
contenances_c[0]=0;
for (i=1;i<=NB_CERCLES;i++) {contenances_c[i]=contenances_triees[i]+contenances_c[i-1];}

//for (k=0;k<NB_CERCLES;k++){cout << "k= " << k << "  distance :  "<<distances_triees[k]<< " Contenances triées  "<< contenances[k] << endl;}
//for (k=0;k<NB_CERCLES;k++){cout << "k= " << k << "  distance :  "<<distances_triees[k]<< "   "<< contenances_triees[k] << endl;}

}


Forme_kppv::Forme_kppv(unsigned char kk,unsigned short int p1, unsigned char p2, long long int ur){
K=kk;
urgence_noir=ur;
urgence_blanc=ur;
urgence_noir_init=ur;
urgence_blanc_init=ur;
urgence_noir_app=(double)ur;
urgence_blanc_app=(double)ur;
GRV_noir=0;
n_match_noir=1;
n_match_blanc=1;
n_joue_sachant_match=0;
bonte_noir=0;
bonte_blanc=0;
p_joue_sachant_match=0;
p_joue_sachant_match_noir=0;
p_joue_sachant_match_blanc=0;
nfs=0;
liste_pierres =NULL;
if (p1>0) {liste_pierres = new Element_kppv(p1,p2,NULL);} else {liste_pierres=NULL;}
}


Forme_kppv::~Forme_kppv()
{

if (liste_pierres) delete liste_pierres; 
/*
if (liste_pierres) {
	while (liste_pierres->suivant) {
	// detruire le dernier element de la liste
	Element_kppv * e =liste_pierres;
	Element_kppv * a =liste_pierres;
	while(e) {
		if (e->suivant) {
			a = e;
			e = e -> suivant;
		}
		else {
			delete e;
			e=NULL;
			a->suivant = NULL;
		}
	}
	}
	delete liste_pierres;
	liste_pierres = NULL;
}*/
}

void Forme_kppv::fimprimer(ostream & o, int d, int n){//
 //ecrire_pattern_2d(&cout);
}
//**********************************************************************************************
//******************************   CALCUL DE LA TABLE POSITION-COORDONNEE CONSTRUCTION *********
//**********************************************************************************************

int Forme_kppv::calculer_coordonees_construction(int i,int no_courant,int anneau_courant){
int abs_courante;
int ord_courante;
int idx_courant;
		abs_courante=GOOD39(i-(i/39)*39+cercle_no_vers_x[anneau_courant][no_courant]);
 		ord_courante=GOOD39((i/39)+cercle_no_vers_y[anneau_courant][no_courant]);
		idx_courant=39*ord_courante+abs_courante;
		if (idx_courant<1521) {return idx_courant;} else {return 1521;}
}

void Forme_kppv::calculer_table_pos_idx_vers_pos_construction(){
int i;
int j;
int no_courant=0;
int anneau_courant=1;
table_pos_idx_vers_idx_construction[0]=760;
for (j=1;j<1369;j++){
		table_pos_idx_vers_idx_construction[j+1369*0]=calculer_coordonees_construction(760,no_courant,anneau_courant);
		table_pos_idx_vers_idx_construction[j+1369*1]=calculer_coordonees_construction(759,no_courant,anneau_courant);
		table_pos_idx_vers_idx_construction[j+1369*2]=calculer_coordonees_construction(721,no_courant,anneau_courant);
		table_pos_idx_vers_idx_construction[j+1369*3]=calculer_coordonees_construction(799,no_courant,anneau_courant);
		table_pos_idx_vers_idx_construction[j+1369*4]=calculer_coordonees_construction(761,no_courant,anneau_courant);
		no_courant++;
 		if (contenances_triees[anneau_courant]==no_courant){
			anneau_courant++;
			no_courant=0;
		}
}
//for (j=1;j<1369;j++){cout<<"Table "<<j<< "   " << table_pos_idx_vers_idx[j]<<endl;}
}



void Forme_kppv::effacer_listes(Arbre * mon_arbre){
int i;
if (mon_arbre) {
	if (mon_arbre->pattern_associe){
		if (mon_arbre->pattern_associe->liste_pierres) {
			while (mon_arbre->pattern_associe->liste_pierres->suivant) {
			// detruire le dernier element de la liste
			Element_kppv * e =mon_arbre->pattern_associe->liste_pierres;
			Element_kppv * a =mon_arbre->pattern_associe->liste_pierres;
			while(e) {
				if (e->suivant) {
					a = e;
					e = e -> suivant;
				}
				else {
					delete e;
					e=NULL;
					a->suivant = NULL;
				}
			}
			}
			delete mon_arbre->pattern_associe->liste_pierres;
			mon_arbre->pattern_associe->liste_pierres = NULL;
		}
	}
	effacer_listes(mon_arbre->noir);
	effacer_listes(mon_arbre->blanc);
	effacer_listes(mon_arbre->vide);
	effacer_listes(mon_arbre->bord);
}else return;
}


//**********************************************************************************************
//****************************** ECRIRE 2D ******************************************************
//**********************************************************************************************

int Forme_kppv::ecrire_pattern_2d(ostream * out){
Element_kppv * element_courant = liste_pierres;
int nbLignes=0;
int Cho;
int Chi;
int i;
int j;
int i_max;
unsigned short int n_max;

// Calcul de l'indice de l'anneau externe

while (element_courant->suivant!=NULL) { // détermination du dernier élément
	element_courant=element_courant->suivant;
	}
n_max=element_courant->position;
//cout << " On est rentré dans la procédure d'écriture 2d, on lit une position max de :" << n_max;

//for (i=0;i<NB_CERCLES;i++) {cout << "contenance = "<< contenances_c[i]<<endl;}
//cout << "n_max = "<< n_max;
Chi=0;
while (((int)n_max)>contenances_c[Chi]) {Chi++;}  // Chi est maintenant l'indice de l'anneau externe
// On le parcours pour savoir les i et j extrêmes
Cho=0;
i_max=0;
//cout << " Chi = "<<Chi << endl;
while (Cho<contenances_triees[Chi]) {
//cout << "Chi " << Chi << " Cho "<< Cho << " abs ="<< abs(cercle_no_vers_x[Chi][Cho])<< endl;
	if (abs(cercle_no_vers_x[Chi][Cho])>i_max) {i_max=abs(cercle_no_vers_x [Chi][Cho]);};
	if (abs(cercle_no_vers_y[Chi][Cho])>i_max) {i_max=abs(cercle_no_vers_y [Chi][Cho]);};
	Cho++;
	}
// Définition d'un tableau avec toutes les valeurs et toutes les positions
int pos[K_MAX];
int cou[K_MAX];
element_courant = liste_pierres;
i=0;
int pos_max;
while ((element_courant)!=NULL) { // détermination du dernier élément
	pos[i]=element_courant->position;
	cou[i]=element_courant->couleur;
	element_courant=(element_courant->suivant);
	pos_max=pos[i];
	i++;
	}
//for (i=0;i<K;i++) {cout <<" Couleurs : "<< cou[i]<< endl;}
// Ecriture
int Kun;
int pos_courante;
char c;
//cout  << " i_max = " << i_max << endl;
for (i=0;i<2*i_max+1;i++) {
	for (j=0;j<2*i_max+1;j++) {
		c='.';
		pos_courante=contenances_c[PASNEG(x_y_vers_anneau[j+18-i_max][i+18-i_max]-1)]+x_y_vers_no[j+18-i_max][i+18-i_max]+1;
//		cout <<"pos_courante : "<<pos_courante <<endl;
		for (Kun=0;Kun<K;Kun++) {
			if (pos_courante==pos[Kun]) {
				if (cou[Kun]==0) {c='@';} 
				if (cou[Kun]==1) {c='O';} 
				if (cou[Kun]==2) {c='-';} 
				}
			}
		if (pos_courante>pos_max) {c=' ';}
		if ((i==i_max)&&(j==i_max)) {c='X';}
		*out << c;
		}
	*out << endl;
	nbLignes++;
	}
return nbLignes;
}

//**********************************************************************************************
//****************************** ECRIRE NOS ******************************************************
//**********************************************************************************************

void Forme_kppv::ecrire_nos_2d(char * nom){
ofstream fichierSortie(nom, ios::out) ;
if ( !fichierSortie) {
	cerr << "erreur : ouverture fichier impossible..." << endl ;
	exit(1) ;
	}
int Cho;
int Chi;
int i;
int j;
int i_max;
unsigned short int n_max;

n_max=1368;  // position max

//for (i=0;i<NB_CERCLES;i++) {cout << "contenance = "<< contenances_c[i]<<endl;}
//cout << "n_max = "<< n_max;
Chi=0;
while (((int)n_max)>contenances_c[Chi]) {Chi++;}  // Chi est maintenant l'indice de l'anneau externe
// On le parcours pour savoir les i et j extrêmes
Cho=0;
i_max=0;

//cout << " Chi = "<<Chi << endl;
while (Cho<contenances_triees[Chi]) {
//cout << "Chi " << Chi << " Cho "<< Cho << " abs ="<< abs(cercle_no_vers_x[Chi][Cho])<< endl;
	if (abs(cercle_no_vers_x[Chi][Cho])>i_max) {i_max=abs(cercle_no_vers_x [Chi][Cho]);};
	if (abs(cercle_no_vers_y[Chi][Cho])>i_max) {i_max=abs(cercle_no_vers_y [Chi][Cho]);};
	Cho++;
	}
// Ecriture
int Kun;
int pos_courante;
char c[4];
//cout  << " i_max = " << i_max << endl;
for (i=0;i<2*i_max+1;i++) {
	for (j=0;j<2*i_max+1;j++) {
		sprintf(c,"   ");
		pos_courante=contenances_c[PASNEG(x_y_vers_anneau[j+18-i_max][i+18-i_max]-1)]+x_y_vers_no[j+18-i_max][i+18-i_max]+1;
//		cout <<"pos_courante : "<<pos_courante <<endl;
		sprintf(c," %d",pos_courante);
		if ((i==i_max)&&(j==i_max)) {sprintf(c,"   ");}
		cout << c;
	}
	cout << endl;
}
fichierSortie.close() ;
}



//**********************************************************************************************
//****************************** ECRIRE ********************************************************
//**********************************************************************************************

void Forme_kppv::ecrire_tous_les_patterns_nv(double proba, double bonte,double urgence){
unsigned short int tmp=0;
Forme_kppv::ofile = new ofstream("pattern_noir_nv.dat", ios::out);
if (!(*ofile)) {
	cerr << "erreur : ouverture fichier impossible..." << endl ;
	return;
	}
ecrire_arbre(racine_arbre_nv,0,proba,bonte,urgence,GBLACK);
(*ofile).close() ;
delete ofile;

Forme_kppv::ofile = new ofstream("pattern_blanc_nv.dat", ios::out);
if (!(*ofile)) {
	cerr << "erreur : ouverture fichier impossible..." << endl ;
	return;
	}
ecrire_arbre(racine_arbre_nv,0,proba,bonte,urgence,GWHITE);
(*ofile).close();
delete ofile;

Forme_kppv::ofile = new ofstream("pattern_bord_noir_nv.dat", ios::out);
if (!(*ofile)) {
	cerr << "erreur : ouverture fichier impossible..." << endl ;
	return;
	}
ecrire_arbre(racine_arbre_nv_bord,0,proba,bonte,urgence,GBLACK);
(*ofile).close() ;
delete ofile;

Forme_kppv::ofile = new ofstream("pattern_bord_blanc_nv.dat", ios::out);
if (!(*ofile)) {
	cerr << "erreur : ouverture fichier impossible..." << endl ;
	return;
	}
ecrire_arbre(racine_arbre_nv_bord,0,proba,bonte,urgence,GWHITE);
(*ofile).close();
delete ofile;

}

void Forme_kppv::ecrire_le_pattern(void){
Element_kppv * element_courant = liste_pierres;
int Cho=1;int Chi=1;
while (element_courant!=NULL) { //(Cho<(K+1))
	while (Chi<(element_courant->position)){
		(*ofile) << ".";
		Chi++;
		}
	if ((element_courant->couleur)==1) {(*ofile) << 'O';}
	if ((element_courant->couleur)==0) {(*ofile) << '@';}
	if ((element_courant->couleur)==2) {(*ofile) << '-';}
	if ((element_courant->couleur)==3) {(*ofile) << '!';}	
	element_courant=element_courant->suivant;
	Cho++;
	Chi++;
	}
}

void Forme_kppv::ecrire_arbre(Arbre * mon_arbre,int mon_no,double proba, double bonte,double urgence, int couleur){ // attention !  plus a jour du tout ! apprentissage a chang�tout
double pn,pb;
double bn,bb;
double un,ub;
double epsilon=0.0000001;

if (mon_arbre) {
//	cout<<" Hauteur dans l'arbre : "<<mon_no<<" Forme_kppv :"<<endl;
	if (mon_arbre->pattern_associe) {
		pn=(double)mon_arbre->pattern_associe->p_joue_sachant_match_noir-epsilon;
		bn=(double)mon_arbre->pattern_associe->bonte_noir-epsilon;
		pb=(double)mon_arbre->pattern_associe->p_joue_sachant_match_blanc-epsilon;
		bb=(double)mon_arbre->pattern_associe->bonte_blanc-epsilon;
		un=(double)mon_arbre->pattern_associe->urgence_noir-epsilon;
		ub=(double)mon_arbre->pattern_associe->urgence_blanc-epsilon;
//		if ((mon_arbre->pattern_associe->n_match_noir>0)&&(n_joue_sachant_match>0)) {
//			p=(double)mon_arbre->pattern_associe->n_joue_sachant_match/(double)mon_arbre->pattern_associe->n_match_noir-0.000001;
//			b=(double)mon_arbre->pattern_associe->n_joue_sachant_match/sqrt((double)mon_arbre->pattern_associe->n_match_noir)-0.000001;
//		}
		
		if ((pn>(proba-100*epsilon))&&(bn>(bonte-100*epsilon))&&(un>urgence)&&(couleur==GBLACK)){
			nb_de_patterns_sauves[mon_arbre->pattern_associe->nfs]++;
			mon_arbre->pattern_associe->ecrire_le_pattern();
			(*ofile) << ':';
			(*ofile) << endl;
			(*ofile) << pn;
			(*ofile) << endl;
			(*ofile) << bn;
			(*ofile) << endl;
			(*ofile) << (int)mon_arbre->pattern_associe->urgence_noir;
			(*ofile) << endl;			
			(*ofile) << (int)mon_arbre->pattern_associe->nfs;			
			(*ofile) << ';';
			(*ofile) << endl;
		}
		if ((pb>(proba-100*epsilon))&&(bb>(bonte-100*epsilon))&&(ub>urgence)&&(couleur==GWHITE)){
			nb_de_patterns_sauves[mon_arbre->pattern_associe->nfs]++;
			mon_arbre->pattern_associe->ecrire_le_pattern();
			(*ofile) << ':';
			(*ofile) << endl;
			(*ofile) << pb;
			(*ofile) << endl;
			(*ofile) << bb;
			(*ofile) << endl;
			(*ofile) << (int)mon_arbre->pattern_associe->urgence_blanc;
			(*ofile) << endl;			
			(*ofile) << (int)mon_arbre->pattern_associe->nfs;
			(*ofile) << ';';
			(*ofile) << endl;
		}

	} //else {cout<< " Aucun "<<endl;}
	ecrire_arbre(mon_arbre->noir,mon_no+1,proba,bonte,urgence,couleur);
	ecrire_arbre(mon_arbre->blanc,mon_no+1,proba,bonte,urgence,couleur);
	ecrire_arbre(mon_arbre->vide,mon_no+1,proba,bonte,urgence,couleur);
	ecrire_arbre(mon_arbre->bord,mon_no+1,proba,bonte,urgence,couleur);
}
}



//**********************************************************************************************
//****************************** GENERATION 3x3  *****************************************************
//**********************************************************************************************

Arbre * Forme_kppv::grimper(Arbre * liane,int i){
Arbre * q;

switch (i){
 case (0):
	if (liane->vide==NULL){
		q=new Arbre(NULL,NULL,NULL,NULL,NULL);
		liane->vide=q;
	}
	q=liane->vide;
 break;
 case (1):
	if (liane->noir==NULL){
		q=new Arbre(NULL,NULL,NULL,NULL,NULL);
		liane->noir=q;
	}
	q=liane->noir;
 break;
 case (2):
	if (liane->blanc==NULL){
		q=new Arbre(NULL,NULL,NULL,NULL,NULL);
		liane->blanc=q;
	}
	q=liane->blanc;
 break;
 case (3):
	if (liane->bord==NULL){
		q=new Arbre(NULL,NULL,NULL,NULL,NULL);
		liane->bord=q;
	}
	q=liane->bord;
 break;
}
return q;
}

Forme_kppv * Forme_kppv::generer_le_pattern_3x3(long long int no){
int hmax=8; //!!!!!!!!!!!!!!!!!!!!!!!!! attention, double d�laration de hmax !     
Element_kppv * ep=NULL;
     
     int h_courant=0;
     int K_courant=0;
     int maCouleur=-1;
     if (no==0) cout << "bon, tu te fout de ma gueule ?"<<endl;
     
     while((maCouleur==-1)&&(h_courant<hmax)){// !!!! c'est la qu'on ne peut pas g�er un pattern purement vide !
     	maCouleur= (no % 10)-1;
	if (maCouleur==3) cout << "MaCouleur= 3 alors que bon !" <<endl;
	no=no/10;
	h_courant++;
     }
     Forme_kppv * P = new Forme_kppv(1,h_courant,maCouleur,4);
     K_courant++;
     
     ep=P->liste_pierres;
     while(h_courant<hmax-1){
     	maCouleur=(no % 10)-1;
	if (maCouleur==3) cout << "Macouleur= 3 alors que bon !" <<endl;
	h_courant++;
	if (maCouleur>-1) {ep = new Element_kppv(h_courant,maCouleur,ep);K_courant++;}
	no=no/10;
     }
     if (h_courant<hmax){
        if (h_courant<(hmax-1)) cout << "Gros bug" <<endl;
        maCouleur=(no % 10)-1;
        if (maCouleur==-1) maCouleur=3;
        h_courant++;
        ep = new Element_kppv(h_courant,maCouleur,ep);
	K_courant++;
     }
P->K=K_courant;
return P;
}

void Forme_kppv::generer_patterns_3x3(Arbre * arbre_init,int hauteur,long long int no){ // !!!!!!!!!!!!!!!!!!!!!!!! Attention, il n'y a pas de gestion du pattern vide possible avec cette repr�entation !!!
int hmax=8;

if(hauteur==hmax) {
  if (no>0){
	arbre_init->pattern_associe=generer_le_pattern_3x3(no);
	if (arbre_init->pattern_associe==NULL) cout << "ah ben oui mais bon, on met un pattern null dans l'arbre !"<<endl;
	//if (arbre_init->pattern_associe->urgence_noir==314) cout <<" jusque la, � va"<<endl;
  }
}else{
	generer_patterns_3x3(grimper(arbre_init,0),hauteur+1,no);
	generer_patterns_3x3(grimper(arbre_init,1),hauteur+1,no+(long long int)pow((double)10,(double)hauteur));
	generer_patterns_3x3(grimper(arbre_init,2),hauteur+1,no+(long long int)(2*pow((double)10,(double)hauteur)));
	generer_patterns_3x3(grimper(arbre_init,3),hauteur+1,no+(long long int)(3*pow((double)10,(double)hauteur)));
}

}

/*void Forme_kppv::generer_3x3(Arbre * arbre_init){ // pourquoi j'ai pas fait une proc�ure r�urente ? Parceque (2 variables globales, lourd...)!

Arbre * arbre_courant[8];
int  i[8];

long long int mon_urgence;

int h=0;
for (int i[h]=0;i[h]<4;i[h]++){
 h++;
 if h=
 arbre_courant1=grimper(arbre_init,i1);
 for (int i2=0;i2<4;i2++){
  arbre_courant2=grimper(arbre_courant1,i2); 
  for (int i3=0;i3<4;i3++){
   arbre_courant3=grimper(arbre_courant2,i3);    
   for (int i4=0;i4<4;i4++){
    arbre_courant4=grimper(arbre_courant3,i4); 
    for (int i5=0;i5<4;i5++){
     arbre_courant5=grimper(arbre_courant4,i5); 
     for (int i6=0;i6<4;i6++){
      arbre_courant6=grimper(arbre_courant5,i6); 
      for (int i7=0;i7<4;i7++){
       arbre_courant7=grimper(arbre_courant6,i7); 
       for (int i8=0;i8<4;i8++){
        arbre_courant8=grimper(arbre_courant7,i8); 
        if (i1+i2+i3+i4+i5+i6+i7+i8>0)
        {
        Forme_kppv * P = new Forme_kppv(1,0,0,mon_urgence); 
	arbre_courant8->pattern_associe=P;
        }
       }
      }
     }
    }
   }
  }
 }
}
}*/
//**********************************************************************************************
//****************************** LIRE  *****************************************************
//**********************************************************************************************



int Forme_kppv::lire_tous_les_patterns_nv(int longueur){
int i=1;
int s=1;
nb_de_patterns_lus=0;

Forme_kppv::ifile = new ifstream("pattern_noir_nv.dat", ios::in);
if (!(*ifile)) {return 0;}
while (s==1){
	s=lire_pattern_nv(1,longueur);
}
(*ifile ).close();
delete ifile;

Forme_kppv::ifile = new ifstream("pattern_blanc_nv.dat", ios::in);
if (!(*ifile)) {return 0;}
s=1;
while (s==1){
	s=lire_pattern_nv(2,longueur);
}
(*ifile ).close();
delete ifile;
return nb_de_patterns_lus;
}

int Forme_kppv::lire_pattern_nv(int couleur,int & longueur){
char c;
int Lee=1;
int Chang=0;
int pomme;
int indic=0;
Forme_kppv * P = new Forme_kppv(1,1,0,0);

Element_kppv * pointeur_fou=P->liste_pierres;
while (Chang==0) {
	if (!((*ifile) >> c)) {delete P;return(0);}
	switch (c){
		case ('@'): (P->liste_pierres)->couleur=0;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('O'): (P->liste_pierres)->couleur=1;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('-'): (P->liste_pierres)->couleur=2;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('!'): (P->liste_pierres)->couleur=3;(P->liste_pierres)->position=Lee;Chang++;break;
		case (';'): P->K=Chang; cout << " Catastrophe, la base de pattern est mal formee, il y a un pattern comprenant 0 fait saillant !!!!";break;
		default:break;
	}
	Lee++;
	}
indic=0;
while (((*ifile) >> c)&&(indic==0)) {
	switch (c){
		case ('@') : pointeur_fou=new Element_kppv(Lee,0,pointeur_fou);Chang++;break;
		case ('O') : pointeur_fou=new Element_kppv(Lee,1,pointeur_fou);Chang++;break;
		case ('-') : pointeur_fou=new Element_kppv(Lee,2,pointeur_fou);Chang++;break;
		case ('!') : pointeur_fou=new Element_kppv(Lee,3,pointeur_fou);Chang++;break;		
		case (':') :indic=1;break;
		case (';') : P->K=Chang; cout<< "erreur, base mal formee !";break;
		default : break;
	}
	Lee++;
	}
double mon_p;
double ma_bonte;
int mon_nfs;
int urg;
(*ifile) >> mon_p;
(*ifile) >> ma_bonte;
(*ifile) >> urg;
(*ifile) >> mon_nfs;
if (!((*ifile) >> c)) {delete P ;return(0);}
if (c!=';') {cout<< " Il manque un point virgule �a fin "<<endl;delete P;return (0);}
if (mon_p==0) cout << "erreur dans la lecture de la proba"<<endl;
if (ma_bonte==0) cout << "erreur dans la lecture de la bonte"<<endl;
if (couleur==1){P->p_joue_sachant_match_noir=(double)mon_p;P->bonte_noir=(double)ma_bonte;P->urgence_noir=(long long int)urg;P->urgence_noir_app=(double)urg;}
if (couleur==2){P->p_joue_sachant_match_blanc=(double)mon_p;P->bonte_blanc=(double)ma_bonte;P->urgence_blanc=(long long int)urg;P->urgence_blanc_app=(double)urg;}
P->K=Chang;
P->nfs=(unsigned char)mon_nfs;
P->n_match_noir=0;
P->n_joue_sachant_match=0;
if (P==NULL) return 0;
if (Lee<(longueur+1)){P->mettre_pattern_dans_arbre(Forme_kppv::racine_arbre_nv);nb_de_patterns_lus++;return 1;}else {delete P;return 1;}
// un jour il faudra qu'on détruise P s'il est trop gros ;)
}

int Forme_kppv::lire_tous_les_patterns_nv_bord(int longueur){
int i=1;
int s=1;
nb_de_patterns_lus=0;

Forme_kppv::ifile = new ifstream("pattern_bord_noir_nv.dat", ios::in);
if (!(*ifile)) {return 0;}
while (s==1){
	s=lire_pattern_nv_bord(1,longueur);
}
(*ifile ).close();
delete ifile;

Forme_kppv::ifile = new ifstream("pattern_bord_blanc_nv.dat", ios::in);
if (!(*ifile)) {return 0;}
s=1;
while (s==1){
	s=lire_pattern_nv_bord(2,longueur);
}
(*ifile ).close();
delete ifile;

return nb_de_patterns_lus;
}

int Forme_kppv::lire_pattern_nv_bord(int couleur,int & longueur){
char c;
int urg;
int Lee=1;
int Chang=0;
int pomme;
int indic=0;
Forme_kppv * P = new Forme_kppv(1,1,0,0);

Element_kppv * pointeur_fou=P->liste_pierres;
while (Chang==0) {
	if (!((*ifile) >> c)) {delete P;return(0);}
	switch (c){
		case ('@'): (P->liste_pierres)->couleur=0;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('O'): (P->liste_pierres)->couleur=1;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('-'): (P->liste_pierres)->couleur=2;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('!'): (P->liste_pierres)->couleur=3;(P->liste_pierres)->position=Lee;Chang++;break;
		case (';'): P->K=Chang; cout << " Catastrophe, la base de pattern est mal formee, il y a un pattern comprenant 0 fait saillant !!!!";break;
		default:break;
	}
	Lee++;
	}
indic=0;
while (((*ifile) >> c)&&(indic==0)) {
	switch (c){
		case ('@') : pointeur_fou=new Element_kppv(Lee,0,pointeur_fou);Chang++;break;
		case ('O') : pointeur_fou=new Element_kppv(Lee,1,pointeur_fou);Chang++;break;
		case ('-') : pointeur_fou=new Element_kppv(Lee,2,pointeur_fou);Chang++;break;
		case ('!') : pointeur_fou=new Element_kppv(Lee,3,pointeur_fou);Chang++;break;
		case (':') : indic=1;break;
		case (';') : P->K=Chang; cout<< "erreur, base mal formée !";break;
		default : break;
	}
	Lee++;
	}
double mon_p;
double ma_bonte;
int mon_nfs;
(*ifile) >> mon_p;
(*ifile) >> ma_bonte;
(*ifile) >> urg;
(*ifile) >> mon_nfs;
if (!((*ifile) >> c)) {delete P;return(0);}
if (c!=';') {cout<< " Il manque un point virgule à la fin "<<endl;delete P;return (0);}
if (mon_p==0) cout << "erreur dans la lecture de la proba"<<endl;
if (ma_bonte==0) cout << "erreur dans la lecture de la bonte"<<endl;

if (couleur==1){P->p_joue_sachant_match_noir=(double)mon_p;P->bonte_noir=(double)ma_bonte;P->urgence_noir=(long long int)urg;P->urgence_noir_app=(double)urg;}
if (couleur==2){P->p_joue_sachant_match_blanc=(double)mon_p;P->bonte_blanc=(double)ma_bonte;P->urgence_blanc=(long long int)urg;P->urgence_blanc_app=(double)urg;}

P->K=Chang;
P->nfs=(unsigned char)mon_nfs;
P->n_match_noir=0;
P->n_joue_sachant_match=0;
if (P==NULL) return 0;
if (Lee<(longueur+1)){P->mettre_pattern_dans_arbre(Forme_kppv::racine_arbre_nv_bord);nb_de_patterns_lus++;return 1;}else {delete P;return 1;}
}

int Forme_kppv::lire_tous_les_patterns_noir(void){
int i=1;
int r=0;
int nb_pats_lus=0;
Forme_kppv * P=NULL;
Forme_kppv::ifile = new ifstream("pattern_noir.dat", ios::in);
if (!(*ifile)) {return 0;}
while ((r==0)||(P)){
	r=1;
	P=lire_pattern(1);
	if (P) {
		nb_pats_lus++;
	}
}
(*ifile ).close();
delete ifile;
return nb_pats_lus;
}

int Forme_kppv::lire_tous_les_patterns_blanc(void){
int i=1;
int r=0;
int nb_pats_lus=0;
Forme_kppv * P=NULL;
Forme_kppv::ifile = new ifstream("pattern_blanc.dat", ios::in);
if (!(*ifile)) {return 0;}
while ((r==0)||(P)){
	r=1;
	P=lire_pattern(2);
	if (P) {
		nb_pats_lus++;
	}
}
(*ifile ).close();
delete ifile;
return nb_pats_lus;
}


Forme_kppv * Forme_kppv::lire_pattern(int couleur){
char c;
int Lee=1;
int Chang=0;
int pomme;
int indic=0;
Forme_kppv * P = new Forme_kppv(1,1,0,0);

Element_kppv * pointeur_fou=P->liste_pierres;
while (Chang==0) {
	if (!((*ifile) >> c)) { delete P;return(NULL);}
	switch (c){
		case ('@'): (P->liste_pierres)->couleur=0;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('O'): (P->liste_pierres)->couleur=1;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('-'): (P->liste_pierres)->couleur=2;(P->liste_pierres)->position=Lee;Chang++;break;
		case ('!'): (P->liste_pierres)->couleur=3;(P->liste_pierres)->position=Lee;Chang++;break;
		case (';'): P->K=Chang; cout << " Catastrophe, la base de pattern est mal formee, un pattern comprenant 0 fait saillant !!!!";break;
		default:break;
	}
	Lee++;
	}
indic=0;
while (((*ifile) >> c)&&(indic==0)) {
	switch (c){
		case ('@') : pointeur_fou=new Element_kppv(Lee,0,pointeur_fou);Chang++;break;
		case ('O') : pointeur_fou=new Element_kppv(Lee,1,pointeur_fou);Chang++;break;
		case ('-') : pointeur_fou=new Element_kppv(Lee,2,pointeur_fou);Chang++;break;
		case ('!') : pointeur_fou=new Element_kppv(Lee,3,pointeur_fou);Chang++;break;
		case (':') :indic=1;break;
		case (';') : P->K=Chang; cout<< "erreur, base mal forme !";break;
		default : break;
	}
	Lee++;
	}
double mon_p;
double ma_bonte;
int urg;
int mon_nfs;
(*ifile) >> mon_p;
(*ifile) >> ma_bonte;
(*ifile) >> mon_nfs;
if (!((*ifile) >> c)) {delete P;return(NULL);}
if (c!=';') {cout<< " Il manque un point virgule a la fin "<<endl;delete P;return (NULL);}
if (mon_p==0) cout << "erreur dans la lecture de la proba"<<endl;
if (ma_bonte==0) cout << "erreur dans la lecture de la bonte"<<endl;
if (couleur==1){P->p_joue_sachant_match_noir=(double)mon_p;P->bonte_noir=(double)ma_bonte;if (P->bonte_noir==0) cout << "GROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";}
if (couleur==2){P->p_joue_sachant_match_blanc=(double)mon_p;P->bonte_blanc=(double)ma_bonte;}
P->K=Chang;
P->nfs=(unsigned char)mon_nfs;
P->n_match_noir=0;
P->n_joue_sachant_match=0;
P->mettre_pattern_dans_arbre(Forme_kppv::racine_arbre);
return(P);// un jour il faudra qu'on détruise P s'il est trop gros ;)
}

//**********************************************************************************************
//****************************** MATCHING NOIR *****************************************************
//**********************************************************************************************


void Forme_kppv::match_proba_noir(int prochain_coup){
int i=0;
int j=0;
int k=0;

int temp;
Arbre * arbre_courant;

//cout << endl << endl << "Forme_kppvs qui matchent"<< endl <<endl;

for (i=0;i<GOBAN_SIZE_2;i++) {if (the_goban[i]==0){ // pour chaque intersection vide
	n_test++;
	arbre_courant=racine_arbre_noir;
	for (j=1;;j++){
		nb_boucles++;
		temp=the_goban[table_pos_idx_vers_idx[j*361+i]];
			switch (temp){
				case (1) :arbre_courant=arbre_courant->noir;break;
				case (-1):arbre_courant=arbre_courant->blanc;break;
				case (2) :arbre_courant=arbre_courant->bord;break;
				case (0) :arbre_courant=arbre_courant->vide;break;
				default: cout<<"Erreur : the_goban mal formé ! ";break;
			}
		if (!(arbre_courant)) break;
		if (arbre_courant->pattern_associe) {
//			urgences[i]=arbre_courant->pattern_associe->urgence;/*arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);*/ // NB: j'utilise pas encore l'urgence du pattern, mais juste p_joue_sachant_match
			arbre_courant->pattern_associe->n_match_noir++;
			if (i==prochain_coup) {arbre_courant->pattern_associe->n_joue_sachant_match++;}
		}
	}
}}
}

void Forme_kppv::match_nv(double * tab_urgences,T_IDX * goban,double rapport,int couleur)
{
int i;
int j;
double temp;
double temp2;
double temp3;
int goban2[GOBAN_SIZE_2+1];
//int tmp;
Arbre * arbre_courant;
  for (i=0;i<GOBAN_SIZE_2;i++) tab_urgences[i]=0;

  for (int idx=0; idx<GOBAN_SIZE_2; idx++)
    goban2[idx] = goban[idx];
  for (int i=0;i<GOBAN_SIZE;i++)
    if (goban2[i]==0) goban2[i]=2;
  for (int i=0;i<GOBAN_SIZE;i++)
    if (goban2[i*GOBAN_SIZE]==0) goban2[i*GOBAN_SIZE]=2;
  for (int i=1;i<GOBAN_SIZE;i++)
    if (goban2[i*GOBAN_SIZE-1]==0) goban2[i*GOBAN_SIZE-1]=2;
  for (int i=0;i<GOBAN_SIZE;i++)
    if (goban2[i+(GOBAN_SIZE-1)*GOBAN_SIZE]==0) goban2[i+(GOBAN_SIZE-1)*GOBAN_SIZE]=2;
  goban2[GOBAN_SIZE_2]=0;


for (i=0;i<GOBAN_SIZE_2;i++) {
	if (goban2[i]==0)
	{ // pour chaque intersection vide
	arbre_courant=racine_arbre_nv; // racine_arbre_nv for limited patterns to 7x7
	n_test++;
	temp=0;
	temp2 = 0;
	temp3 = 0;
	for (j=1;;j++){
		nb_boucles++; 
			switch (goban2[table_pos_idx_vers_idx[j*GOBAN_SIZE_2+i]]){
				case (1) :arbre_courant=arbre_courant->noir;break;
				case (-1):arbre_courant=arbre_courant->blanc;break;
				case (2) :arbre_courant=arbre_courant->bord;break;
				case (0) :arbre_courant=arbre_courant->vide;break;
				default: cout<<"Erreur : the_goban mal forme ! ";break;
			}
		if (!(arbre_courant)) break;
		if (arbre_courant->pattern_associe) {
			if (couleur==1) {
				temp=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
				temp2=arbre_courant->pattern_associe->bonte_noir;
				temp3=arbre_courant->pattern_associe->nfs;
//				cout <<temp<<endl;
				//if (temp>0) {
				if (temp>0) {tab_urgences[i]=coeff_pattern(temp, temp2, 0, temp3, rapport, alpha1, alpha2);}
			}
			if (couleur==-1) {
				temp=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
				temp2=arbre_courant->pattern_associe->bonte_blanc;
				temp3=arbre_courant->pattern_associe->nfs;
//				cout <<temp<<endl;
				//if (temp>0) {
				if (temp>0) {tab_urgences[i]=coeff_pattern(temp, temp2, 0, temp3, rapport, alpha1, alpha2);}
			}
//		arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);
		}
	}}

        if (goban2[i]==2)
        { // for each empty border intersection
        arbre_courant=racine_arbre_nv_bord;
        n_test++;
	temp=0;
        for (j=1;;j++){
                nb_boucles++;
                        switch (goban2[table_pos_idx_vers_idx[j*GOBAN_SIZE_2+i]]){
                                case (1) :arbre_courant=arbre_courant->noir;break;
                                case (-1):arbre_courant=arbre_courant->blanc;break;
                                case (2) :arbre_courant=arbre_courant->bord;break;
                                case (0) :arbre_courant=arbre_courant->vide;break;
                                default: cout<<"Erreur : the_goban mal formé ! ";break;
                        }
                if (!(arbre_courant)) break;
                if (arbre_courant->pattern_associe) {
			if (couleur==1) {
				temp=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
				temp2=arbre_courant->pattern_associe->bonte_noir;
				temp3=arbre_courant->pattern_associe->nfs;
//				cout <<temp<<endl;
				//if (temp>0) {
				if (temp>0) {tab_urgences[i]=coeff_pattern(temp, temp2, 0, temp3, rapport, alpha1, alpha2);}
			}
			if (couleur==-1) {
				temp=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
				temp2=arbre_courant->pattern_associe->bonte_blanc;
				temp3=arbre_courant->pattern_associe->nfs;
//				cout <<temp<<endl;
				//if (temp>0) {
				if (temp>0) {tab_urgences[i]=coeff_pattern(temp, temp2, 0, temp3, rapport, alpha1, alpha2);}
			}
//              arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);
                }
        }}
}
}

void Forme_kppv::match(double * tab_urgences,T_IDX * goban,double rapport,int couleur)
{
int i;
int j;
double temp;
double temp2;
double temp3;
int goban2[GOBAN_SIZE_2+1];
//int tmp;
Arbre * arbre_courant;
  for (i=0;i<GOBAN_SIZE_2;i++) tab_urgences[i]=0;

  for (int idx=0; idx<GOBAN_SIZE_2; idx++)
    goban2[idx] = goban[idx];
  for (int i=0;i<GOBAN_SIZE;i++)
    if (goban2[i]==0) goban2[i]=2;
  for (int i=0;i<GOBAN_SIZE;i++)
    if (goban2[i*GOBAN_SIZE]==0) goban2[i*GOBAN_SIZE]=2;
  for (int i=1;i<GOBAN_SIZE;i++)
    if (goban2[i*GOBAN_SIZE-1]==0) goban2[i*GOBAN_SIZE-1]=2;
  for (int i=0;i<GOBAN_SIZE;i++)
    if (goban2[i+(GOBAN_SIZE-1)*GOBAN_SIZE]==0) goban2[i+(GOBAN_SIZE-1)*GOBAN_SIZE]=2;
  goban2[GOBAN_SIZE_2]=0;


for (i=0;i<GOBAN_SIZE_2;i++) {
	if (goban2[i]==0)
	{ // pour chaque intersection vide
	arbre_courant=racine_arbre; // racine_arbre_nv for limited patterns to 7x7
	n_test++;
	temp=0;
	temp2 = 0;
	temp3 = 0;
	for (j=1;;j++){
		nb_boucles++; 
			switch (goban2[table_pos_idx_vers_idx[j*GOBAN_SIZE_2+i]]){
				case (1) :arbre_courant=arbre_courant->noir;break;
				case (-1):arbre_courant=arbre_courant->blanc;break;
				case (2) :arbre_courant=arbre_courant->bord;break;
				case (0) :arbre_courant=arbre_courant->vide;break;
				default: cout<<"Erreur : the_goban mal forme ! ";break;
			}
		if (!(arbre_courant)) break;
		if (arbre_courant->pattern_associe) {
			if (couleur==1) {
				temp=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
				temp2=arbre_courant->pattern_associe->bonte_noir;
				temp3=arbre_courant->pattern_associe->nfs;
//				cout <<temp<<endl;
				//if (temp>0) {
				if (temp>0) {tab_urgences[i]=coeff_pattern(temp, temp2, 0, temp3, rapport, alpha1, alpha2);}
			}
			if (couleur==-1) {
				temp=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
				temp2=arbre_courant->pattern_associe->bonte_blanc;
				temp3=arbre_courant->pattern_associe->nfs;
//				cout <<temp<<endl;
				//if (temp>0) {
				if (temp>0) {tab_urgences[i]=coeff_pattern(temp, temp2, 0, temp3, rapport, alpha1, alpha2);}
			}
//		arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);
		}
	}}

        if (goban2[i]==2)
        { // for each empty border intersection
        arbre_courant=racine_arbre_nv_bord;
        n_test++;
	temp=0;
        for (j=1;;j++){
                nb_boucles++;
                        switch (goban2[table_pos_idx_vers_idx[j*GOBAN_SIZE_2+i]]){
                                case (1) :arbre_courant=arbre_courant->noir;break;
                                case (-1):arbre_courant=arbre_courant->blanc;break;
                                case (2) :arbre_courant=arbre_courant->bord;break;
                                case (0) :arbre_courant=arbre_courant->vide;break;
                                default: cout<<"Erreur : the_goban mal formé ! ";break;
                        }
                if (!(arbre_courant)) break;
                if (arbre_courant->pattern_associe) {
			if (couleur==1) {
				temp=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
				temp2=arbre_courant->pattern_associe->bonte_noir;
				temp3=arbre_courant->pattern_associe->nfs;
//				cout <<temp<<endl;
				//if (temp>0) {
				if (temp>0) {tab_urgences[i]=coeff_pattern(temp, temp2, 0, temp3, rapport, alpha1, alpha2);}
			}
			if (couleur==-1) {
				temp=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
				temp2=arbre_courant->pattern_associe->bonte_blanc;
				temp3=arbre_courant->pattern_associe->nfs;
//				cout <<temp<<endl;
				//if (temp>0) {
				if (temp>0) {tab_urgences[i]=coeff_pattern(temp, temp2, 0, temp3, rapport, alpha1, alpha2);}
			}
//              arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);
                }
        }}
}
}

//**********************************************************************************************
//****************************** MATCHING BLANC ************************************************
//**********************************************************************************************


void Forme_kppv::match_proba_blanc(int prochain_coup){ // !! n_match noir !!
int i=0;
int j=0;
int k=0;
int temp;
Arbre * arbre_courant;

//cout << endl << endl << "Forme_kppvs qui matchent"<< endl <<endl;

for (i=0;i<GOBAN_SIZE_2;i++) {if (the_goban[i]==0){ // pour chaque intersection vide
//	cout <<"i=  "<<i<<endl;
	n_test++;
	arbre_courant=racine_arbre_blanc;
	for (j=1;;j++){
		Forme_kppv::nb_boucles++; 
		temp=the_goban[table_pos_idx_vers_idx[j*GOBAN_SIZE_2+i]];
			switch (temp){
				case (1) :arbre_courant=arbre_courant->noir;break;
				case (-1):arbre_courant=arbre_courant->blanc;break;
				case (2) :arbre_courant=arbre_courant->bord;break;
				case (0) :arbre_courant=arbre_courant->vide;break;
				default: cout<<"Erreur : the_goban mal formé ! ";break;
			}
		if (!(arbre_courant)) break;
		if(arbre_courant->pattern_associe) {
//			urgences[i]=arbre_courant->pattern_associe->urgence;/*arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);*/
			arbre_courant->pattern_associe->n_match_noir++;
			if (i==prochain_coup) {arbre_courant->pattern_associe->n_joue_sachant_match++;}
		}
	}
}}
}

void Forme_kppv::afficher_arbre(Arbre * mon_arbre,int mon_no,double proba,double bonte,unsigned char nfs_demande){ // bonf faut tout revoir ici
double p;
double b;
if (mon_arbre) {
//	cout<<" Hauteur dans l'arbre : "<<mon_no<<" Forme_kppv :"<<endl;
	if (mon_arbre->pattern_associe) {
		p=(double)mon_arbre->pattern_associe->p_joue_sachant_match;
		b=(double)mon_arbre->pattern_associe->bonte_noir;
		if (mon_arbre->pattern_associe->n_match_noir>0) {
			p=(double)mon_arbre->pattern_associe->n_joue_sachant_match/(double)mon_arbre->pattern_associe->n_match_noir;
			b=(double)mon_arbre->pattern_associe->n_joue_sachant_match/sqrt((double)mon_arbre->pattern_associe->n_match_noir);
		}
		if ((p>proba)&&(b>bonte)&&(mon_arbre->pattern_associe->nfs==nfs_demande)){
			nb_de_patterns_affiches++;
			mon_arbre->pattern_associe->ecrire_pattern_2d(&cout);
			cout << " Matché : "<< mon_arbre->pattern_associe->n_match_noir<<endl;
			cout << " Joué sachant matché : "<< mon_arbre->pattern_associe->n_joue_sachant_match<<endl;
			cout << " La probabilité de jouer sachant match est : "<< p <<endl;
			cout << " R bonté : "<< b;
			cout << "     R utilité : "<<(double)mon_arbre->pattern_associe->n_joue_sachant_match/sqrt((double)n_joue);
			cout << " NFS : "<< (int)nfs_demande;
			cout << endl<<endl;
		}
	} //else {cout<< " Aucun "<<endl;}
	afficher_arbre(mon_arbre->noir,mon_no+1,proba,bonte,nfs_demande);
	afficher_arbre(mon_arbre->blanc,mon_no+1,proba,bonte,nfs_demande);
	afficher_arbre(mon_arbre->vide,mon_no+1,proba,bonte,nfs_demande);
	afficher_arbre(mon_arbre->bord,mon_no+1,proba,bonte,nfs_demande);
}
}

//**********************************************************************************************
//******************************  DETRUIRE ARBRE **************************************
//**********************************************************************************************

void Forme_kppv::detruire_arbre(Arbre * arbre_mort){
while ((arbre_mort->noir!=NULL)||(arbre_mort->blanc!=NULL)||(arbre_mort->vide!=NULL)||(arbre_mort->bord!=NULL)){
//	cout << "On va écréter "<<endl;
	detruire_feuilles(arbre_mort);
//cout<<"On a écrété "<<endl;
}
}


void Forme_kppv::detruire_feuilles(Arbre * arbre_pere){
Arbre * arbre_fils;
if (arbre_pere) {
	if (arbre_pere->noir) {
		arbre_fils=arbre_pere->noir;
		if ((arbre_fils->noir==NULL)&&(arbre_fils->blanc==NULL)&&(arbre_fils->vide==NULL)&&(arbre_fils->bord==NULL)){
//			cout << "on va avoir un noir "<<endl;
			delete arbre_fils->pattern_associe;
//			cout << "on a eu le pattern "<<endl;
			delete arbre_fils;
//			cout << "on a eu un noir "<<endl;
			arbre_pere->noir=NULL;
		} else {
//		cout << "on peut pas tuer le noir tout de suite "<<endl;
		detruire_feuilles(arbre_fils);
		}
	}
	if (arbre_pere->blanc) {
		arbre_fils=arbre_pere->blanc;
		if ((arbre_fils->noir==NULL)&&(arbre_fils->blanc==NULL)&&(arbre_fils->vide==NULL)&&(arbre_fils->bord==NULL)){
			delete arbre_fils->pattern_associe;
			delete arbre_fils;
//			cout << "on a eu un blanc "<<endl;
			arbre_pere->blanc=NULL;
		} else {
//		cout << "on peut pas tuer le blanc tout de suite "<<endl;
		detruire_feuilles(arbre_fils);
		}
	}
	if (arbre_pere->vide) {
		arbre_fils=arbre_pere->vide;
		if ((arbre_fils->noir==NULL)&&(arbre_fils->blanc==NULL)&&(arbre_fils->vide==NULL)&&(arbre_fils->bord==NULL)){
			delete arbre_fils->pattern_associe;
			delete arbre_fils;
//			cout << "on a eu un vide "<<endl;
			arbre_pere->vide=NULL;
		} else {
//		cout << "on peut pas tuer le vide tout de suite "<<endl;
		detruire_feuilles(arbre_fils);
		}
	}
	if (arbre_pere->bord) {	
		arbre_fils=arbre_pere->bord;
		if ((arbre_fils->noir==NULL)&&(arbre_fils->blanc==NULL)&&(arbre_fils->vide==NULL)&&(arbre_fils->bord==NULL)){
			delete arbre_fils->pattern_associe;
			delete arbre_fils;
//			cout << "on a eu un bord "<<endl;
			arbre_pere->bord=NULL;
		} else {
//		cout << "on peut pas tuer le bord tout de suite "<<endl;
		detruire_feuilles(arbre_fils);
		}
	}
}	
}	
//**********************************************************************************************
//******************************  METTRE PATERN DANS ARBRE **************************************
//**********************************************************************************************

void Forme_kppv::mettre_pattern_dans_arbre(Arbre * mon_arbre){
int i=1;
unsigned short int pos;
Arbre * arbre_courant=mon_arbre;
Element_kppv * pierre_courante=this->liste_pierres;
Arbre * q;
while (pierre_courante){
	pos=pierre_courante->position;
	while (i < (int) pos) {
		if (arbre_courant->vide==NULL){
			q=new Arbre(NULL,NULL,NULL,NULL,NULL);
			arbre_courant->vide=q;
		}
		arbre_courant=arbre_courant->vide;
		i++;
	} // on arrive sur la pierre
	if (pierre_courante->couleur==3){
		if (arbre_courant->vide==NULL){
			q=new Arbre(NULL,NULL,NULL,NULL,NULL);
			arbre_courant->vide=q;
		}
		arbre_courant=arbre_courant->vide;
		i++;
	}
	if (pierre_courante->couleur==2){
		if (arbre_courant->bord==NULL){
			q=new Arbre(NULL,NULL,NULL,NULL,NULL);
			arbre_courant->bord=q;
		}
		arbre_courant=arbre_courant->bord;
		i++;
	}
	if (pierre_courante->couleur==1){
		if (arbre_courant->blanc==NULL){
			q=new Arbre(NULL,NULL,NULL,NULL,NULL);
			arbre_courant->blanc=q;
		}
		arbre_courant=arbre_courant->blanc;
		i++;
	}
	if (pierre_courante->couleur==0){
		if (arbre_courant->noir==NULL){
			q=new Arbre(NULL,NULL,NULL,NULL,NULL);
			arbre_courant->noir=q;
		}
		arbre_courant=arbre_courant->noir;
		i++;
	}
	pierre_courante=pierre_courante->suivant;
}
// modification : J'ai chang�aussi cette procedure. S'il y a d��un pattern dans l'arbre, on le d�ruit, en gardant les valeurs de p_n_joue_sachant_match existante.
// S'il y a deux paterns qui ont ont une urgence diff�ente, on prends l'urgence du plus grand patern, si elle est non nulle. En effet, lorsque l'urgence est nulle, c'est que le patern n'as pas match�assez souvant...Donc on ne peut pas en tirer de conclusions. 
//Il y aura un interet a sauvegarder un grand nombre de patern a urgence faible : ainsi ils "effacerons" des urgences de patterns trop locaux.

if (arbre_courant->pattern_associe) 
{
	if ((arbre_courant->pattern_associe->p_joue_sachant_match_noir!=0)&&(this->p_joue_sachant_match_noir==0)){
		this->p_joue_sachant_match_noir=arbre_courant->pattern_associe->p_joue_sachant_match_noir;
		this->bonte_noir=arbre_courant->pattern_associe->bonte_noir;
		}
	if ((arbre_courant->pattern_associe->p_joue_sachant_match_blanc!=0)&&(this->p_joue_sachant_match_blanc==0)) {
		this->p_joue_sachant_match_blanc=arbre_courant->pattern_associe->p_joue_sachant_match_blanc;
		this->bonte_blanc=arbre_courant->pattern_associe->bonte_blanc;
		}
	delete arbre_courant->pattern_associe;
}
arbre_courant->pattern_associe=this;
/*cout << " On a mis un dans l'arbre le pattern suivant"<<endl;
this->ecrire_pattern_2d(&cout);
cout << "---------------------------------------------"<<endl;*/
}

//**********************************************************************************************
//****************************** CALCUL DE LA TABLE POSITION-COORDONNEE ************************
//**********************************************************************************************

int Forme_kppv::calculer_coordonees(int i,int no_courant,int anneau_courant){ // apprentissage : pas mal de trucs modifi� ici
int abs_courante;
int ord_courante;
int idx_courant;
int X;
int Y;   
		X= i % GOBAN_SIZE+cercle_no_vers_x[anneau_courant][no_courant];// apprentissage X=i-(i/GOBAN_SIZE)*GOBAN_SIZE+cercle_no_vers_x[anneau_courant][no_courant];
		abs_courante=((X>=0)&&(X<GOBAN_SIZE))?X:400; // apprentissage
 		Y=(i/GOBAN_SIZE)+cercle_no_vers_y[anneau_courant][no_courant]; //apprentissage
		ord_courante=((Y>=0)&&(Y<GOBAN_SIZE))?Y:400;
		idx_courant=GOBAN_SIZE*ord_courante+abs_courante;
		if (idx_courant<GOBAN_SIZE_2) {return idx_courant;} else {return GOBAN_SIZE_2;}
}

void Forme_kppv::calculer_table_pos_idx_vers_pos(){
//cout << GOBAN_SIZE_2<<endl;
//cout << GOBAN_SIZE<<endl;
int i;
int j;
int no_courant=0;
int anneau_courant=1;
for (j=1;j<1369;j++){
		for (i=0;i<GOBAN_SIZE_2;i++){
			table_pos_idx_vers_idx[j*GOBAN_SIZE_2+i]=calculer_coordonees(i,no_courant,anneau_courant);
		}
		no_courant++;
		if (contenances_triees[anneau_courant]==no_courant){
			anneau_courant++;
			no_courant=0;
		}
}
//for (j=1;j<1369;j++){cout<<"Table "<<j<< "   " << table_pos_idx_vers_idx[j]<<endl;}
}

//**********************************************************************************************
//******************************  GENERATION DE PATERNS ****************************************
//**********************************************************************************************


void Forme_kppv::generer_un_pattern(int damier[GOBAN_SIZE_2],int mon_idx,unsigned char nfs,int c){
int Lee; 	// compteur de K
int Chang=0; 	// compteur de nfs
int Ho;
int vertical=0;
int horizontal=0;
int j;

//	cout << "bon, on est rentré ds le générateur de patterns "<<endl;

Forme_kppv * P = new Forme_kppv(1,1,0,0);
Element_kppv * pointeur_fou=P->liste_pierres;
	Chang=0;
	Lee=0;
	j=1;
	while(Chang==0){
//	cout << "On va essayer de lire ds la table à la valeur "<<(j*361+mon_idx)<<endl;
		Ho=Forme_kppv::table_pos_idx_vers_idx[j*361+mon_idx];
//	cout << "bon, Ho est bien défini, pas de pb et = "<<Ho<<endl;
//	cout << "bon, damier[Ho] est bien défini, pas de pb et = "<<damier[Ho]<<endl;
		switch (damier[Ho]){
				case (1) :(P->liste_pierres)->couleur=1;(P->liste_pierres)->position=j;Chang++;Lee++;break;
				case (-1):(P->liste_pierres)->couleur=0;(P->liste_pierres)->position=j;Chang++;Lee++;break;
				case (2) :(P->liste_pierres)->couleur=2;(P->liste_pierres)->position=j;Lee++;
						if ((Ho%19==0)||(Ho%19==18)) vertical++;
						if ((Ho/19==0)||(Ho/19==18)) horizontal++;
						if (horizontal+vertical<3) Chang++;
						break;
				case (0) :break;
				default: cout<<" Erreur : damier mal formé ! "<<damier[Ho];break;
		}
		j++;
	}
//	cout << "bon, ben la première pierre, pas de pb"<<endl;
	while ((Chang<(int)nfs)&&(j<1368)){
//	cout << "On va essayer de lire ds la table à la valeur "<<(j*361+mon_idx)<<endl;
		Ho=Forme_kppv::table_pos_idx_vers_idx[j*361+mon_idx];
//	cout << "bon, Ho est bien défini, pas de pb et = "<<Ho<<endl;
//	cout << "bon, damier[Ho] est bien défini, pas de pb et = "<<damier[Ho]<<endl;
		switch (damier[Ho]){
				case (1) :pointeur_fou=new Element_kppv(j,1,pointeur_fou);Chang++;Lee++;break;
				case (-1):pointeur_fou=new Element_kppv(j,0,pointeur_fou);Chang++;Lee++;break;
				case (2) :pointeur_fou=new Element_kppv(j,2,pointeur_fou);Lee++;
						if ((Ho%19==0)||(Ho%19==18)) vertical++;
						if ((Ho/19==0)||(Ho/19==18)) horizontal++;
						if (horizontal+vertical<3) Chang++;
						break;
				case (0) :break;
				default: cout<<" Erreur : damier mal formé ! "<<damier[Ho];break;
		}
		j++;
	}
//if (j>1367) {delete P;cout<<"Oh, on essaie des K un peu trop grand. C'est pas grave ;) "<<endl;return;}
P->K=(unsigned char)Lee;
P->nfs=(unsigned char)Chang;
//cout<<"Un pattern vient d'être généré P->K = "<<(int)P->K<<endl;;
//P->ecrire_pattern_2d(&cout);
P->n_match_noir=0;
P->urgence_noir=0;
P->urgence_noir_init=0;
P->urgence_blanc=0;
P->urgence_blanc_init=0;
P->n_joue_sachant_match=0;
P->p_joue_sachant_match=0;
//P->bonte_noir=0;
P->bonte_blanc=0;

Forme_kppv::nb_de_patterns_total++;

if (c==1) {P->mettre_pattern_dans_arbre(racine_arbre_noir);}else{P->mettre_pattern_dans_arbre(racine_arbre_blanc);}
}



//		TROUVER URGENCE MAX

int Forme_kppv::trouver_urgence_max(double * mon_damier_des_urgences){
int i;
int mon_idx=0;
double max=0;
for (i=0;i<GOBAN_SIZE_2;i++){
	if (mon_damier_des_urgences[i]>=max) {
		max=mon_damier_des_urgences[i];
		mon_idx=i;
	}
}
return mon_idx;
}


// attention !!!! Init urgences ne met plus les urgences !!! 
void Forme_kppv::init_urgences(Arbre * a,double coef_noir,double exp_noir,double coef_blanc,double exp_blanc,double Rinit,double probaMin)// modification : procedure amelioree pour pouvoir calculer le coeficient optimal.
{
//  cout <<"+"<<endl;
//  cout <<"*";
  if (a==NULL) return;
  if (a->pattern_associe){
    
    if (a->pattern_associe->p_joue_sachant_match_noir>probaMin){
    	//a->pattern_associe->urgence_noir = de_proba_a_urgence_noir(a->pattern_associe->p_joue_sachant_match_noir,coef_noir,exp_noir);
    	a->pattern_associe->urgence_noir_init = a->pattern_associe->urgence_noir;
	//a->pattern_associe->urgence_noir_app = de_proba_a_urgence_noir(a->pattern_associe->p_joue_sachant_match_noir,coef_noir,exp_noir);
    }else{
    	a->pattern_associe->urgence_noir_app=0;
    }
    
    //  cout << a->pattern_associe->urgence_noir<<endl;    
    if (a->pattern_associe->p_joue_sachant_match_blanc>probaMin){
    	//a->pattern_associe->urgence_blanc = de_proba_a_urgence_blanc(a->pattern_associe->p_joue_sachant_match_blanc,coef_blanc,exp_blanc);
 	a->pattern_associe->urgence_blanc_init = a->pattern_associe->urgence_blanc;
    	//a->pattern_associe->urgence_blanc_app = de_proba_a_urgence_blanc(a->pattern_associe->p_joue_sachant_match_blanc,coef_blanc,exp_blanc);
    }else{
    	//a->pattern_associe->urgence_blanc = 0;
    	a->pattern_associe->urgence_blanc_app = 0;    
    }	
  
    // Calcul de la proba de pr�ence du pattern
    double Pn = a->pattern_associe->p_joue_sachant_match_noir;
    double Pb = a->pattern_associe->p_joue_sachant_match_blanc;    
    double Bn = a->pattern_associe->bonte_noir;
    double Bb = a->pattern_associe->bonte_blanc;
    double Ppn=0;
    double Ppb=0;
    if (Pn>0){
	Ppn = ((double)GOBAN_SIZE_2/381)*(Bn*Bn/(Pn*Pn))/3000000;
	//if (a->pattern_associe->bonte_noir==0) cout << " Nom d'un ptit schtroumf, la bont�est nulle " <<Bn<< "! et pn = "<< Pn<<endl;
	a->pattern_associe->GRV_noir=Ppn*(double)a->pattern_associe->urgence_noir*Rinit/Ppn;
//        Apprentissage_kppv::somme_GRV_noir+=a->pattern_associe->GRV_noir;           //
//        Apprentissage_kppv::somme_V_noir+=(double)a->pattern_associe->urgence_noir;      
    }else {
    	Ppn=0;
    } 				// Pn=0 Pb>0
    a->pattern_associe->proba_pres_noir=Ppn;
       
    }
    //a->pattern_associe->proba_pres_noir=Ppb;
    
    //if (a->pattern_associe->p_joue_sachant_match_noir>0) {Nmatch_noir=(a->pattern_associe->bonte_noir*a->pattern_associe->bonte_noir/(a->pattern_associe->p_joue_sachant_match_noir*a->pattern_associe->p_joue_sachant_match_noir);} else {}
    //a->pattern_associe->proba_pres_noir =  + a->pattern_associe->bonte_blanc*a->pattern_associe->bonte_blanc/(a->pattern_associe->p_joue_sachant_match_blanc*a->pattern_associe->p_joue_sachant_match_blanc))/5000000;
    //a->pattern_associe->proba_pres_noir= (a->pattern_associe->bonte_noir^2/(a->pattern_associe->p_joue_sachant_match_noir^2) + a->pattern_associe->bonte_blanc^2/(a->pattern_associe->p_joue_sachant_match_blanc^2))/5000000;
  //  cout << a->pattern_associe->urgence_noir<<endl;
  //  a->pattern_associe->urgence = de_proba_a_urgence_noir(a->pattern_associe->p_joue_sachant_match,coef_noir);}
  init_urgences(a->noir,coef_noir,exp_noir,coef_blanc,exp_blanc,Rinit,probaMin);
  init_urgences(a->blanc,coef_noir,exp_noir,coef_blanc,exp_blanc,Rinit,probaMin);
  init_urgences(a->bord,coef_noir,exp_noir,coef_blanc,exp_blanc,Rinit,probaMin);
  init_urgences(a->vide,coef_noir,exp_noir,coef_blanc,exp_blanc,Rinit,probaMin);
}

T_URG Forme_kppv::de_proba_a_urgence_blanc(double p, double coef_blanc, double exp_blanc)
{
  return ((long long int)(coef_blanc*pow(p,exp_blanc))); // modification : ici on regle l'exposant de p. on regle le coeficient dans la procedure d'en des}
}

T_URG Forme_kppv::de_proba_a_urgence_noir(double p, double coef_noir,double exp_noir)
{
  return ((long long int)(coef_noir*pow(p,exp_noir)));
}

void Forme_kppv::tests(){

cout << "on va tester que les patterns 3x3 existent bien, ont une urgence coh�ente etc..."<<endl;

cout << "Alors le super pattern existe "<<racine_arbre_nv->noir->blanc->blanc->noir->vide->vide->vide->vide<<" OK "<<endl;
cout << "Alors le super pattern a un pattern "<<racine_arbre_nv->noir->blanc->blanc->noir->vide->vide->vide->vide->pattern_associe<<" OK "<<endl;
cout << "Alors le super pattern a pour urgence "<<racine_arbre_nv->noir->blanc->blanc->noir->vide->vide->vide->vide->pattern_associe->urgence_noir<<endl;
cout << "Alors le super pattern 2 existe "<<racine_arbre_nv->noir->blanc->blanc->noir->vide->vide->noir->vide<<" OK "<<endl;
cout << "Alors le super pattern 2 a un pattern "<<racine_arbre_nv->noir->blanc->blanc->noir->vide->vide->noir->vide->pattern_associe<<" OK "<<endl;
cout << "Alors le super pattern 2 a pour urgence "<<racine_arbre_nv->noir->blanc->blanc->noir->vide->vide->noir->vide->pattern_associe->urgence_noir<<endl;
cout << "les 7 vides :  "<<racine_arbre_nv->vide->vide->vide->vide->vide->vide->vide->pattern_associe<<endl;



}

void Forme_kppv::init_class()
{
  //cout << "Init kppv: debut." << endl;
  int tmp;

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // en conclusion, a terme il faudrait deux fonctions:
  // une appelee au debut du programme qui lit les patterns
  // une appelee apres chaque changement de taille de damier
  
  Forme_kppv::creer_les_listes();
  Forme_kppv::trouver_cercles(18);// Determiner la correspondance anneau-goban. Il faut laisser 18 quelque soit la taille du goban (en fait 18*18 est la taille maximale du plus gros patern qui peut �re stoqu�en m�oire...)
  Forme_kppv::calculer_table_pos_idx_vers_pos();

  tmp=Forme_kppv::lire_tous_les_patterns_noir(); // Charger tous les patterns en mémoire vive
  PRINTLOG( tmp << " patterns NOIRS dans racine_arbre.");
  if (tmp == 0) { 
	  assert(0);
  }
  tmp=Forme_kppv::lire_tous_les_patterns_blanc();   // Charger tous les patterns en mémoire vive
  PRINTLOG(tmp << " patterns BLANCS dans racine_arbre.");
  if (tmp == 0) { 
	  assert(0);
  }

  tmp=Forme_kppv::lire_tous_les_patterns_nv(KPP_NB_VOISINES); // Charger tous les patterns en mémoire vive
  PRINTLOG( tmp << " patterns reduits dans racine_arbre_nv."); 
  if (tmp == 0) { 
	  assert(0);
  }

  tmp=Forme_kppv::lire_tous_les_patterns_nv_bord(KPP_NB_VOISINES); // Charger tous les patterns en mémoire vive
  PRINTLOG( tmp << " patterns reduits dans racine_arbre_nv_bord.");
  if (tmp == 0) { 
	  assert(0);
  }
//  Apprentissage_kppv::somme_GRV_noir=0;
//  Apprentissage_kppv::somme_V_noir=0;
  
  init_urgences(racine_arbre_nv,100000,2,100000,2,COEF_APPRENTISSAGE*0.0002,0);
  init_urgences(racine_arbre_nv_bord,100000,2,100000,2,COEF_APPRENTISSAGE*0.0002,0);
  // modification : on declare ici le coefficient pour noir et le coefficient pour blanc dans le pattern matching 36v. Si .00002 � marche bien !
  
  //cout << " On va g��er les patterns 3x3  :" << endl;
  //Forme_kppv::generer_patterns_3x3(racine_arbre_nv,0,0);
  //Forme_kppv::generer_patterns_3x3(racine_arbre_nv_bord,0,0); // !!!!! attention : on ne peut plus appeller init_urgences !!!!!
  //cout << " Patterns 3x3 g���." << endl;
  //tests();
  
  effacer_listes(racine_arbre);
  //effacer_listes(racine_arbre_nv); apprentissage

  
  //Forme_kppv::calculer_table_pos_idx_vers_pos_construction();
  //cout << " La table a ete realisee pour les liens dans l'arbre "<<endl;
  
  //Forme_kppv::construire_liens(Forme_kppv::racine_arbre_77,Forme_kppv::racine_arbre_77,0,0);
  //cout << "Les liens ont été construit pour l'arbre 77"<<endl;

  //Damier::FKPPV_matcher_tout();
  //for (int i=0;i<362;i++) Forme_kppv::quarantaines[i]=0;

  //cout << "Init kppv: fin." << endl;
}

//**********************************************************************************************
//****************************** MATCHING RECURRENT NOIR************************************************
//**********************************************************************************************
/*
void Forme_kppv::match_recurrent_noir(){
int i;
for (i=0;i<GOBAN_SIZE_2;i++){
	switch (the_goban[i]){
		case (0):goban_recure[i]=1;break;
		case (2):goban_recure[i]=1;
	}
}
for (i=180;i>0;i--) {
	if (goban_recure[i]==1){
		match_intersection_recurrent(i,racine_arbre_noir,0);
	}
}
for (i=181;i<GOBAN_SIZE_2;i++) {
	if (goban_recure[i]==1){
		match_intersection_recurrent(i,racine_arbre_noir,0);
	}
}

}


void Forme_kppv::match_77(int idx_centrale){
int i;
int j;
int temp;
//int tmp;
Arbre * arbre_courant;

//cout << endl << endl << "Forme_kppvs qui matchent"<< endl <<endl;
urgences[idx_centrale]=0;
for (i=1;i<37;i++) {
	temp=table_pos_idx_vers_idx[i*361+idx_centrale];// a propos de la table, un jour il faudra que ça retourne idx pour i=0
	if ((the_goban[temp]==0)||(the_goban[temp]==2)){ 
	urgences[temp]=0;
	n_test++;
	arbre_courant=racine_arbre_77;
	for (j=1;;j++){
		nb_boucles++; 
			switch (the_goban[table_pos_idx_vers_idx[j*361+temp]]){
				case (1) :arbre_courant=arbre_courant->noir;break;
				case (-1):arbre_courant=arbre_courant->blanc;break;
				case (2) :arbre_courant=arbre_courant->bord;break;
				case (0) :arbre_courant=arbre_courant->vide;break;
				default: cout<<"Erreur : the_goban mal formé ! ";break;
			}
		if (!(arbre_courant)) {break;}
		if (arbre_courant->pattern_associe) {
			urgences[temp]=arbre_courant->pattern_associe->p_joue_sachant_match;//arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);
		}
	}
}}
}
//**********************************************************************************************
//****************************** MATCHING RECURRENT ************************************************
//**********************************************************************************************

void Forme_kppv::match_recurrent(int idx_centrale){
int i;
int temp;
urgences[idx_centrale]=0;
for (i=1;i<37;i++) {
	temp=table_pos_idx_vers_idx[i*361+idx_centrale];
	switch (the_goban[temp]){
		case (0):goban_recure[temp]=1;break;
		case (2):goban_recure[temp]=1;
	}
}
for (i=KPP_NB_VOISINES;i>0;i--) {
	if (goban_recure[table_pos_idx_vers_idx[i*361+idx_centrale]]==1){
	match_intersection_recurrent(table_pos_idx_vers_idx[i*361+idx_centrale],racine_arbre_77,0);
	}
}
}
/*
void Forme_kppv::match_intersection_recurrent(int idx_testee,Arbre * arbre_depart,int hauteur_de_depart){
Arbre * arbre_courant=arbre_depart;
goban_recure[idx_testee]=0;
n_test++;
int j;
	for (j=hauteur_de_depart+1;;j++){
		nb_boucles++;
		arbre_retour=arbre_courant; // arbre ancien !!!
		switch (the_goban[table_pos_idx_vers_idx[j*361+idx_testee]]){
			case (0) :arbre_courant=arbre_courant->vide;break;
			case (1) :arbre_courant=arbre_courant->noir;break;
			case (-1):arbre_courant=arbre_courant->blanc;break;
			case (2) :arbre_courant=arbre_courant->bord;break;
			default: cout<<"Erreur : the_goban mal formé ! ";
		}
		if (!(arbre_courant)) {break;}
		if (arbre_courant->pattern_associe) {
			urgences[idx_testee]=arbre_courant->pattern_associe->p_joue_sachant_match;//arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);
		}
	}
if (goban_recure[table_pos_idx_vers_idx[1*361+idx_testee]]==1){urgences[table_pos_idx_vers_idx[1*361+idx_testee]]=arbre_retour->lien_urgence[1];match_intersection_recurrent(table_pos_idx_vers_idx[1*361+idx_testee],arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);}
if (goban_recure[table_pos_idx_vers_idx[2*361+idx_testee]]==1){urgences[table_pos_idx_vers_idx[2*361+idx_testee]]=arbre_retour->lien_urgence[2];match_intersection_recurrent(table_pos_idx_vers_idx[2*361+idx_testee],arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);}
if (goban_recure[table_pos_idx_vers_idx[3*361+idx_testee]]==1){urgences[table_pos_idx_vers_idx[3*361+idx_testee]]=arbre_retour->lien_urgence[3];match_intersection_recurrent(table_pos_idx_vers_idx[3*361+idx_testee],arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);}
if (goban_recure[table_pos_idx_vers_idx[4*361+idx_testee]]==1){urgences[table_pos_idx_vers_idx[4*361+idx_testee]]=arbre_retour->lien_urgence[4];match_intersection_recurrent(table_pos_idx_vers_idx[4*361+idx_testee],arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);}
}*/

//**********************************************************************************************
//****************************** MATCHING LINEAIRE ************************************************
//**********************************************************************************************
/*
void Forme_kppv::match_reduit_lineaire(int idx_centrale){
int i;
int j;
int temp;
//int tmp;

//cout << endl << endl << "Forme_kppvs qui matchent"<< endl <<endl;
urgences[idx_centrale]=0;
//i=KPP_NB_VOISINES
temp=table_pos_idx_vers_idx[9*361+idx_centrale];
switch (the_goban[temp]){case 0:urgences[temp]=0;match_intersection(temp,racine_arbre_77,0);}

temp=table_pos_idx_vers_idx[10*361+idx_centrale];
switch (the_goban[temp]){case 0:urgences[temp]=0;match_intersection(temp,racine_arbre_77,0);break;case 2:urgences[temp]=0;match_intersection(temp,racine_arbre_77,0);break;default:break;}

temp=table_pos_idx_vers_idx[11*361+idx_centrale];
switch (the_goban[temp]){case 0:urgences[temp]=0;match_intersection(temp,racine_arbre_77,0);break;case 2:urgences[temp]=0;match_intersection(temp,racine_arbre_77,0);break;default:break;}

temp=table_pos_idx_vers_idx[36*361+idx_centrale];
switch (the_goban[temp]){case 0:urgences[temp]=0;match_intersection(temp,racine_arbre_77,0);break;case 2:urgences[temp]=0;match_intersection(temp,racine_arbre_77,0);break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[28*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[35*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[19*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[23*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[17*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}


temp=table_pos_idx_vers_idx[33*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;default:arbre_retour=racine_arbre_77;break;}


temp=table_pos_idx_vers_idx[26*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[31*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[15*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[21*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[13*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[29*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[25*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[30*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[14*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[22*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[16*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[32*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[27*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[34*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[18*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[24*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[20*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[12*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[4*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[7*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[2];
	match_intersection(temp,arbre_retour->lien[2],arbre_retour->lien_hauteur_faite[2]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[2*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[5*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[1];
	match_intersection(temp,arbre_retour->lien[1],arbre_retour->lien_hauteur_faite[1]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[1*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[6*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[3];
	match_intersection(temp,arbre_retour->lien[3],arbre_retour->lien_hauteur_faite[3]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[3*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;default:arbre_retour=racine_arbre_77;break;}

temp=table_pos_idx_vers_idx[8*361+idx_centrale];
switch (the_goban[temp]){
	case 0:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;
	case 2:	urgences[temp]=arbre_retour->lien_urgence[4];
	match_intersection(temp,arbre_retour->lien[4],arbre_retour->lien_hauteur_faite[4]);
	break;default:arbre_retour=racine_arbre_77;break;}
}


void Forme_kppv::match_intersection(int & intersection, Arbre * arbre_courant,int hauteur_faite){
int j;
// initialiser l'urgence avant l'appel de cette fonction, à l'urgence ou à 0 selon le cas
// remplit urgences ? ok!
	for (j=hauteur_faite+1;;j++){
		nb_boucles++; 
		if (!(arbre_courant)) {arbre_retour=racine_arbre_77;break;}
		arbre_retour=arbre_courant; // arbre ancien !!!
			switch (the_goban[table_pos_idx_vers_idx[j*361+intersection]]){
				case (0) :arbre_courant=arbre_courant->vide;break;
				case (1) :arbre_courant=arbre_courant->noir;break;
				case (-1):arbre_courant=arbre_courant->blanc;break;
				case (2) :arbre_courant=arbre_courant->bord;break;
				default: cout<<"Erreur : the_goban mal formé ! ";break;
			}
		if (!(arbre_courant)) {break;}
		if (arbre_courant->pattern_associe) {
			urgences[intersection]=arbre_courant->pattern_associe->p_joue_sachant_match;//arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);
		}
	}
}


*/


/*
void Forme_kppv::match_blanc(){
int i;
int j;
//int tmp;
Arbre * arbre_courant;

//cout << endl << endl << "Forme_kppvs qui matchent"<< endl <<endl;

for (i=0;i<GOBAN_SIZE_2;i++) {if (the_goban[i]==0){ // pour chaque intersection vide
//	cout <<"i=  "<<i<<endl;
	arbre_courant=racine_arbre;
	for (j=1;;j++){
//		Forme_kppv::n_boucles++; 
		switch (the_goban[table_pos_idx_vers_idx[j*361+i]]){
				case (1) :arbre_courant=arbre_courant->noir;break;
				case (-1):arbre_courant=arbre_courant->blanc;break;
				case (2) :arbre_courant=arbre_courant->bord;break;
				case (0) :arbre_courant=arbre_courant->vide;break;
				default: cout<<"Erreur : the_goban mal formé ! ";break;
			}
		if (!(arbre_courant)) break;
		if(arbre_courant->pattern_associe) {
			urgences[i]=arbre_courant->pattern_associe->p_joue_sachant_match_blanc;
			//arbre_courant->pattern_associe->ecrire_pattern_2d(&cout);
		}
	}
}}
}*/


//**********************************************************************************************
//******************************   afficher urgences   ******************************************
//**********************************************************************************************
/*
void Forme_kppv::ecrire_urgences(void){
int i;
int j;
int Cho=0;
for (i=0;i<19;i++){
	cout << endl;
	for (j=0;j<19;j++){
	if (urgences[Cho]!=0){cout<< (int)(urgences[Cho]*1000); if (urgences[Cho]<100) cout<<" ";}
	else cout << " . ";
	Cho++;
	}
}
cout << endl;
}
*/
//*********************************************************************************************
//****************************** Afficher arbre *******************************************
//*********************************************************************************************

//**********************************************************************************************
//******************************  CONSTRUIRE LIENS **************************************
//**********************************************************************************************

/*
void Forme_kppv::construire_liens(Arbre * arbre_de_depart,Arbre * mon_arbre,int mon_no,int couleur){
int k,i;

if (mon_arbre) {
	goban_construction[table_pos_idx_vers_idx_construction[mon_no]]=couleur;

	mon_arbre->lien[1]=arbre_de_depart;
	mon_arbre->lien[2]=arbre_de_depart;
	mon_arbre->lien[3]=arbre_de_depart;
	mon_arbre->lien[4]=arbre_de_depart;
	mon_arbre->lien_hauteur_faite[1]=0;
	mon_arbre->lien_hauteur_faite[2]=0;
	mon_arbre->lien_hauteur_faite[3]=0;
	mon_arbre->lien_hauteur_faite[4]=0;

	lier_pattern(mon_arbre,arbre_de_depart,1,1,0);
	lier_pattern(mon_arbre,arbre_de_depart,1,2,0);
	lier_pattern(mon_arbre,arbre_de_depart,1,3,0);
	lier_pattern(mon_arbre,arbre_de_depart,1,4,0);

	construire_liens(arbre_de_depart,mon_arbre->noir,mon_no+1,1);
	for (i=mon_no+1;i<1369;i++) {goban_construction[table_pos_idx_vers_idx_construction[i]]=3;}
	construire_liens(arbre_de_depart,mon_arbre->blanc,mon_no+1,-1);
	for (i=mon_no+1;i<1369;i++) {goban_construction[table_pos_idx_vers_idx_construction[i]]=3;}
	construire_liens(arbre_de_depart,mon_arbre->vide,mon_no+1,0);
	for (i=mon_no+1;i<1369;i++) {goban_construction[table_pos_idx_vers_idx_construction[i]]=3;}
	construire_liens(arbre_de_depart,mon_arbre->bord,mon_no+1,2);
	for (i=mon_no+1;i<1369;i++) {goban_construction[table_pos_idx_vers_idx_construction[i]]=3;}
}
}

void Forme_kppv::lier_pattern(Arbre * gros_arbre, Arbre * petit_arbre,int haut_pa,int no_lien,double urgence_courante){
// la hauteur ds le pattern est tjrs en avance de 1 par rapport à l'arbre, par ex pour haut_pa=1, petit arbre=racine, dc il faut initialiser le lien de la racine
if (petit_arbre){// on pourra enlever cette condition
	switch 	(goban_construction[table_pos_idx_vers_idx_construction[haut_pa+no_lien*1369]]){
		case (0): 	
			if (petit_arbre->vide) {
				if (petit_arbre->vide->pattern_associe) {urgence_courante=petit_arbre->vide->pattern_associe->p_joue_sachant_match;}
				gros_arbre->lien[no_lien]=petit_arbre->vide;
				gros_arbre->lien_urgence[no_lien]=urgence_courante;
				gros_arbre->lien_hauteur_faite[no_lien]=haut_pa;
				lier_pattern(gros_arbre,petit_arbre->vide,haut_pa+1,no_lien,urgence_courante);
			}
		break;
		case (1):
			 if (petit_arbre->noir) {
				if (petit_arbre->noir->pattern_associe) {urgence_courante=petit_arbre->noir->pattern_associe->p_joue_sachant_match;}
				gros_arbre->lien[no_lien]=petit_arbre->noir;
				gros_arbre->lien_urgence[no_lien]=urgence_courante;
				gros_arbre->lien_hauteur_faite[no_lien]=haut_pa;
				lier_pattern(gros_arbre,petit_arbre->vide,haut_pa+1,no_lien,urgence_courante);
			}
		break;
		case (-1):
			if (petit_arbre->blanc) {
				if (petit_arbre->blanc->pattern_associe) {urgence_courante=petit_arbre->blanc->pattern_associe->p_joue_sachant_match;}
				gros_arbre->lien[no_lien]=petit_arbre->blanc;
				gros_arbre->lien_urgence[no_lien]=urgence_courante;
				gros_arbre->lien_hauteur_faite[no_lien]=haut_pa;
				lier_pattern(gros_arbre,petit_arbre->vide,haut_pa+1,no_lien,urgence_courante);
			}
		break;
		case (2):
			if (petit_arbre->bord) {
				if (petit_arbre->bord->pattern_associe) {urgence_courante=petit_arbre->bord->pattern_associe->p_joue_sachant_match;}
				gros_arbre->lien[no_lien]=petit_arbre->bord;
				gros_arbre->lien_urgence[no_lien]=urgence_courante;
				gros_arbre->lien_hauteur_faite[no_lien]=haut_pa;
				lier_pattern(gros_arbre,petit_arbre->vide,haut_pa+1,no_lien,urgence_courante);
			}
		break;
		case (3):break;
	}
}
}
*/


void Forme_kppv::verification(void){
/*int k,j;

for (k=0;k<1521;k++) goban_construction[k]=19*39;
for (k=0;k<1368;k++){goban_construction[table_pos_idx_vers_idx_construction[k]]=k;}

*Katia::maSortie <<endl;
for (j=0;j<39;j++){
for (k=0;k<39;k++){
	switch ((goban_construction[k+j*39]/39)){
		case (0): *Katia::maSortie <<"0 ";break;
		case (1): *Katia::maSortie <<"1 ";break;
		case (2): *Katia::maSortie <<"2 ";break;
		case (3): *Katia::maSortie <<"3 ";break;
		case (4): *Katia::maSortie <<"4 ";break;
		case (5): *Katia::maSortie <<"5 ";break;
		case (6): *Katia::maSortie <<"6 ";break;
		case (7): *Katia::maSortie <<"7 ";break;
		case (8): *Katia::maSortie <<"8 ";break;
		case (9): *Katia::maSortie <<"9 ";break;
		case (10): *Katia::maSortie <<"a ";break;
		case (11): *Katia::maSortie <<"b ";break;
		case (12): *Katia::maSortie <<"c ";break;
		case (13): *Katia::maSortie <<"d ";break;
		case (14): *Katia::maSortie <<"e ";break;
		case (15): *Katia::maSortie <<"f ";break;
		case (16): *Katia::maSortie <<"g ";break;
		case (17): *Katia::maSortie <<"h ";break;
		case (18): *Katia::maSortie <<"i ";break;
		case (19): *Katia::maSortie <<". ";break;
		default: *Katia::maSortie <<"- ";break;
	}
}
*Katia::maSortie <<endl;
}*/
}

//**********************************************************************************************
//******************************   MAIN    *****************************************************
//**********************************************************************************************
/*
int main(void){

double bonte;
double proba;
unsigned char i; // attention !
int j,k;
int tmp;
// bonte=1.42;
// proba=0.02;

// Procédures d'initialisation

Forme_kppv::creer_les_listes();
cout << " Les listes ont été faites "<<endl;
Forme_kppv::trouver_cercles(18);			// Déterminer la correspondance anneau-goban
cout << " L'ordre du pattern matching a été déterminé "<<endl;
Forme_kppv::calculer_table_pos_idx_vers_pos();
cout << " La table a été réalisée "<<endl;
Forme_kppv::calculer_table_pos_idx_vers_pos_construction();
cout << " La table a été réalisée pour les liens dans l'arbre "<<endl;

// Forme_kppv::symetries();//pas disponibles ici
// cout << " Les symétries ont été calculées "<<endl;

tmp=Forme_kppv::lire_tous_les_patterns_noir();		// Charger tous les patterns en mémoire vive
cout << " Les patterns NOIRS ont été lus, il y en avais : "<<tmp<<endl;
tmp=Forme_kppv::lire_tous_les_patterns_blanc();		// Charger tous les patterns en mémoire vive
cout << " Les patterns BLANCS ont été lus, il y en avais : "<<tmp<<endl;

tmp=Forme_kppv::lire_tous_les_patterns_77();		// Charger tous les patterns en mémoire vive
cout << " Les patterns pour le matching réduits ont été lus, il y en avais : "<<tmp<<endl;

Forme_kppv::construire_liens(Forme_kppv::racine_arbre_77,Forme_kppv::racine_arbre_77,0,0);
cout << "Les liens ont été construit pour l'arbre 77"<<endl;
Forme_kppv::construire_liens(Forme_kppv::racine_arbre_noir,Forme_kppv::racine_arbre_noir,0,0);
cout << "Les liens ont été construit pour l'arbre noir"<<endl;

Forme_kppv::verification();

//for (i=2;i<21;i++){	Forme_kppv::afficher_arbre(Forme_kppv::racine_arbre_77,0,0.01,1.4,i);}


//Forme_kppv::bien_lie(Forme_kppv::racine_arbre_noir);

Fichier_pattern::calculer_probas_toutes_les_parties();

// Résultats :
cout << endl;
//cout << " Nombre de patterns sauvés-lus : "<< somme-Forme_kppv::nb_de_patterns_lus<<endl;
cout << " Nombre de patterns créés : " << Forme_kppv::nb_de_patterns_total<<endl;
cout << " Nombre de patterns lus : "<< Forme_kppv::nb_de_patterns_lus<<endl;
//cout << " Nombre de patterns affichés : " << Forme_kppv::nb_de_patterns_affiches<<endl;
cout << endl;
cout << " Nombre de parties testées : " << Fichier_pattern::nombre_de_parties_testees<<endl;

cout << " Nombre de coups joués : " << Forme_kppv::n_joue<<endl;
//cout << " Nombre de coups joués : " << Fichier_pattern::nombre_de_coups_dans_parties_testees<<endl;

cout << " Nombre moyen de coups par parties : "<<(double)Fichier_pattern::nombre_de_coups_dans_parties_testees/(double)Fichier_pattern::nombre_de_parties_testees<<endl;
cout <<endl;
cout << " Nombre de pattern-matchings : " << Forme_kppv::n_test<<endl;
cout << " Nombre de comparaisons de pierres : " << Forme_kppv::nb_boucles<<endl;
cout << " Nombre de comparaisons par pattern-matching : " << (double)Forme_kppv::nb_boucles/(double)Forme_kppv::n_test<<endl;
cout << " Nombre de pattern-matching pour un goban: " << (double)Forme_kppv::n_test/(double)Fichier_pattern::nombre_de_coups_dans_parties_testees<<endl;
*/
/*
int x,y;
x=0;
y=1;
char a,b;
int multi;
int acceleration=0;
int n,p;

cout<<endl <<"Jouer contre la version limitée au 36 cases voisines ? (o/n) : ";
	cin>>b;


Forme_kppv::bien_lie(Forme_kppv::racine_arbre_77);

acceleration=b-96-14;
//acceleration=1;
if ((acceleration>1)||(acceleration<0)) {cout<< endl <<"Arf, il fallait rentrer 'o' ou 'n', bon tant pis on va dire qu'on joue avec le version globale"<<endl;}
n=0;p=1;
multi=1;
p=1;
	for (k=20;k<(GOBAN_SIZE_2-19);k++){Forme_kppv::the_goban[k]=0;} // Ici on met des 2 sur les bords du damier
	for (k=0;k<GOBAN_SIZE;k++){Forme_kppv::the_goban[k]=2;}
	for (k=0;k<GOBAN_SIZE;k++){Forme_kppv::the_goban[k*19]=2;}
	for (k=1;k<GOBAN_SIZE;k++){Forme_kppv::the_goban[k*19-1]=2;}
	for (k=0;k<GOBAN_SIZE;k++){Forme_kppv::the_goban[k+18*19]=2;}
	Forme_kppv::the_goban[GOBAN_SIZE_2]=0;

	Forme_kppv::match_noir(Forme_kppv::the_goban);
	p=Forme_kppv::trouver_urgence_max(Forme_kppv::urgences);
	Forme_kppv::ecrire_urgences();
	Forme_kppv::the_goban[p]=1;
	if (acceleration==1){Forme_kppv::match_reduit_lineaire(p);}

	while (p!=0){
//		joueur blanc
//		if (p!=0) {
//			for (k=0;k<GOBAN_SIZE_2;k++){Forme_kppv::urgences[k]=0;}
//			for (k=0;k<multi;k++){Forme_kppv::match_blanc(damier);}
//			Forme_kppv::ecrire_urgences();
//			cout << Forme_kppv::trouver_urgence_max()<<endl;
//			p=Forme_kppv::trouver_urgence_max();
//			damier[p]=-1;
//			n++;
//		}
cout << endl << "Le goban est maintenant : "<< endl;
		Forme_kppv::ecrire_damier(Forme_kppv::the_goban);
		cout << "Coup : ";cin >> a;cin >> y;
		//a='d';y=4;
		x=a-96;
		if (x>9) x-=1;
		x-=1;y=19-y;
		if (x==19){
			cout << " Première pierre à enlever : "<<endl;
			cout << "Coup : ";cin >> a;cin >> y;
			x=a-96;
			if (x>9) x-=1;
			x-=1;y=19-y;
			Forme_kppv::the_goban[x+y*19]=0;
			cout << " Deuxième pierre à enlever : "<<endl;
			cout << "Coup : ";cin >> a;cin >> y;
			x=a-96;
			if (x>9) x-=1;
			x-=1;y=19-y;
			Forme_kppv::the_goban[x+y*19]=0;
			cout << " Mettre une pierre blanche à la place : "<<endl;
			cout << "Coup : ";cin >> a;cin >> y;
			x=a-96;
			if (x>9) x-=1;
			x-=1;y=19-y;
			Forme_kppv::the_goban[x+y*19]=-1;
			cout << " Mettre une pierre noire à la place : "<<endl;
			cout << "Coup : ";cin >> a;cin >> y;
			x=a-96;
			if (x>9) x-=1;
			x-=1;y=19-y;
			Forme_kppv::the_goban[x+y*19]=1;
		}
		if (acceleration==1){
			Forme_kppv::match_recurrent(p);
			p=Forme_kppv::trouver_urgence_max(Forme_kppv::urgences);
		} else {
			for (k=0;k<GOBAN_SIZE_2;k++){Forme_kppv::urgences[k]=0;}
			Forme_kppv::match_blanc(Forme_kppv::the_goban);
			p=Forme_kppv::trouver_urgence_max(Forme_kppv::urgences);
			if (p==x+y*19) {cout <<"Le pattern matching aurait fait pareil ;)"<<endl;}
		}
		Forme_kppv::the_goban[x+y*19]=-1;
		Fichier_pattern::determiner_et_enlever_pierres_mortes(x+y*19,Forme_kppv::the_goban,-1);

		// joueur noir 
		if (acceleration==1){
			Forme_kppv::match_recurrent(x+y*19);
			p=Forme_kppv::trouver_urgence_max(Forme_kppv::urgences);
			Forme_kppv::ecrire_urgences();
		} else {
			for (k=0;k<GOBAN_SIZE_2;k++){Forme_kppv::urgences[k]=0;}
			Forme_kppv::match_noir(Forme_kppv::the_goban);
			p=Forme_kppv::trouver_urgence_max(Forme_kppv::urgences);
			Forme_kppv::ecrire_urgences();
		}
//		cout << Forme_kppv::trouver_urgence_max()<<endl;
		Forme_kppv::the_goban[p]=1;
		n++;
		Fichier_pattern::determiner_et_enlever_pierres_mortes(p,Forme_kppv::the_goban,1);
	}

cout << "Nombre de pattern matching de goban : "<< n <<endl;
*/

// } // fin du main

// Changer le k=0 ds la création du goban vierge...
