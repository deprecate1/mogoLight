#include "Element_kppv.h"
#include <stdlib.h>

Element_kppv::Element_kppv(unsigned short int pi,unsigned char  pc,Element_kppv * ep) {
couleur=pc;
position=pi;
suivant=NULL;
if (ep!=NULL) {ep->suivant= this;}
}

Element_kppv::~Element_kppv()
{
	if (suivant) delete suivant;
}



