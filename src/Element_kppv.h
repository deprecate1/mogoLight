#ifndef ELEMENT_KPPV_H
#define ELEMENT_KPPV_H

class Element_kppv {
public :
unsigned char couleur;
unsigned short int position;
Element_kppv * suivant;
Element_kppv(unsigned short int,unsigned char,Element_kppv *);
~Element_kppv();

};

#endif
