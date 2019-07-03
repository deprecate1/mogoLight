// pbmb_io.h
# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <fstream>
# include <cmath>

bool pbmb_check_data ( int xsize, int ysize, int *barray );
bool pbmb_example ( int xsize, int ysize, int *barray );

bool pbmb_read ( char *file_in_name, int *xsize, int *ysize, int **barray );
bool pbmb_read_data ( std::ifstream &file_in, int xsize, int ysize, int *barray );
bool pbmb_read_header ( std::ifstream &file_in, int *xsize, int *ysize );
bool pbmb_read_test ( char *file_in_name );

bool pbmb_write ( char *file_out_name, int xsize, int ysize, int *barray );
bool pbmb_write_data ( std::ofstream &file_out, int xsize, int ysize, int *barray )
;
bool pbmb_write_header ( std::ofstream &file_out, int xsize, int ysize );
bool pbmb_write_test ( char *file_out_name );



