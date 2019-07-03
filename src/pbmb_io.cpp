
using namespace std;

# include "pbmb_io.h"
#include <string.h>

//****************************************************************************

bool pbmb_check_data ( int xsize, int ysize, int *barray )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_CHECK_DATA checks the data for a binary portable bit map file.
//
//  Modified:
//
//    17 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int XSIZE, YSIZE, the number of rows and columns of data.
//
//    Input, int *BARRAY, the array of XSIZE by YSIZE bits.
//
//    Output, bool PBMB_CHECK_DATA, is true if an error occurred.
//
{
  int i;
  int *indexb;
  int j;

  if ( xsize <= 0 )
  {
    cout << "\n";
    cout << "PBMB_CHECK_DATA - Fatal error!\n";
    cout << "  XSIZE <= 0\n";
    cout << "  XSIZE = " << xsize << "\n";
    return true;
  }

  if ( ysize <= 0 )
  {
    cout << "\n";
    cout << "PBMB_CHECK_DATA - Fatal error!\n";
    cout << "  YSIZE <= 0\n";
    cout << "  YSIZE = " << ysize << "\n";
    return true;
  }

  if ( barray == NULL )
  {
    cout << "\n";
    cout << "PBMB_CHECK_DATA - Fatal error!\n";
    cout << "  Null pointer to data.\n";
    return true;
  }

  indexb = barray;

  for ( j = 0; j < ysize; j++ )
  {
    for ( i = 0; i < xsize; i++ )
    {
      if ( *indexb != 0 && *indexb != 1 )
      {
        cout << "\n";
        cout << "PBMB_CHECK_DATA - Fatal error!\n";
        cout << "  b(" << i << "," << j << ") = "
          << *indexb << ".\n";
        return true;
      }

      indexb = indexb + 1;
    }
  }

  return false;
}
//****************************************************************************

bool pbmb_example ( int xsize, int ysize, int *barray )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_EXAMPLE sets up some sample PBMB data.
//
//  Discussion:
//
//    The data represents an ellipse.
//
//  Modified:
//
//    17 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int XSIZE, YSIZE, the number of rows and columns of data.
//    Values of 200 would be reasonable.
//
//    Output, int *BARRAY, the array of XSIZE by YSIZE data values.
//
//    Output, bool PBMB_EXAMPLE, is true if an error occurred.
//
{
  int i;
  int *indexb;
  int j;
  float r;
  float test;
  float x;
  float xc;
  float y;
  float yc;
 
  indexb = barray;
  if ( xsize < ysize )
  {
    r = ( float ) xsize / 3.0;
  }
  else
  {
    r = ( float ) ysize / 3.0;
  }
  xc = ( xsize ) / 2.0;
  yc = ( ysize ) / 2.0;

  for ( i = 0; i < ysize; i++ )
  {
    y = ( float ) i;
    for ( j = 0; j < xsize; j++ )
    {
      x = ( float ) j;
      test = r - sqrt ( ( x - xc ) * ( x - xc ) 
               + 0.75 * ( y - yc ) * ( y - yc ) );
      if ( fabs ( test ) <= 3.0 )
      {
        *indexb = 1;
      }
      else
      {
        *indexb = 0;
      }
      indexb = indexb + 1;
    }
  }

  return false;
}
//****************************************************************************

bool pbmb_read ( char *file_in_name, int *xsize, int *ysize, int **barray )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_READ reads the header and data from a binary portable bit map file.
// 
//  Discussion:
//
//    Thanks to Jonas Schwertfeger for pointing out that, especially on Microsoft
//    Windows systems, a binary file needs to be opened as a binary file!
//
//  Modified:
// 
//    02 April 2005
// 
//  Author:
// 
//    John Burkardt
//
//  Parameters:
//
//    Input, char *FILE_IN_NAME, the name of the file containing the binary
//    portable bit map data.
//
//    Output, int *XSIZE, *YSIZE, the number of rows and columns of data.
//
//    Output, int **BARRAY, the array of XSIZE by YSIZE data values.
//
//    Output, bool PBMB_READ, is true if an error occurred.
//
{
  bool error;
  ifstream file_in;

  file_in.open ( file_in_name, ios::binary );

  if ( !file_in )
  {
    cout << "\n";
    cout << "PBMB_READ: Fatal error!\n";
    cout << "  Cannot open the input file " << file_in_name << "\n";
    return true;
  }
//
//  Read the header.
//
  error = pbmb_read_header ( file_in, xsize, ysize );

  if ( error )
  {
    cout << "\n";
    cout << "PBMB_READ: Fatal error!\n";
    cout << "  PBMB_READ_HEADER failed.\n";
    return true;
  }
//
//  Allocate storage for the data.
//
  *barray = new int [ (*xsize) * (*ysize) ];
//
//  Read the data.
//
  error = pbmb_read_data ( file_in, *xsize, *ysize, *barray );

  if ( error )
  {
    cout << "\n";
    cout << "PBMB_READ: Fatal error!\n";
    cout << "  PBMB_READ_DATA failed.\n";
    return true;
  }
//
//  Close the file.
//
  file_in.close ( );

  return false;
}
//****************************************************************************

bool pbmb_read_data ( ifstream &file_in, int xsize, int ysize, int *barray )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_READ_DATA reads the data in a binary portable bit map file.
//
//  Modified:
//
//    16 December 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, ifstream &FILE_IN, a pointer to the file containing the binary
//    portable bit map data.
//
//    Input, int XSIZE, YSIZE, the number of rows and columns of data.
//
//    Output, int *BARRAY, the array of XSIZE by YSIZE data values.
//
//    Output, bool PBMB_READ_DATA, is true if an error occurred.
//
{
  int bit;
  char c;
  unsigned char c2=0;
  int i;
  int *indexb;
  int j;
  int k;
  int numbyte;

  indexb = barray;
  numbyte = 0;

  for ( j = 0; j < ysize; j++ )
  {
    for ( i = 0; i < xsize; i++ )
    {
      if ( i%8 == 0 )
      {
        file_in.read ( &c, 1 );
        c2 = ( unsigned char ) c;
        if ( file_in.eof() )
        {
          cout << "\n";
          cout << "PBMB_CHECK_DATA - Fatal error!\n";
          cout << "  Failed reading byte " << numbyte << "\n";
          return true;
        }
        numbyte = numbyte + 1;
      }

      k = 7 - i%8;
      bit = ( c2 >> k )%2;

      *indexb = bit;
      indexb = indexb + 1;
    }
  }
  return false;
}
//****************************************************************************

bool pbmb_read_header ( ifstream &file_in, int *xsize, int *ysize )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_READ_HEADER reads the header of a binary portable bit map file.
//
//  Modified:
//
//    17 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, ifstream &FILE_IN, a pointer to the file containing the binary
//    portable bit map data.
//
//    Output, int *XSIZE, *YSIZE, the number of rows and columns of data.
//
//    Output, bool PBMB_READ_HEADER, is true if an error occurred.
//
{
  int count;
  char line[255];
  char *next;
  int step;
  int width;
  char word[255];

  step = 0;

  while ( 1 )
  {
    file_in.getline ( line, sizeof ( line ) );

    if ( file_in.eof() )
    {
      cout << "\n";
      cout << "PBMB_READ_HEADER - Fatal error!\n";
      cout << "  End of file.\n";
      return true;
    }

    next = line;

    if ( line[0] == '#' )
    {
      continue;
    }

    if ( step == 0 )
    {
      count = sscanf ( next, "%s%n", word, &width );
      if ( count == EOF )
      {
        continue;
      }
      next = next + width;
      if ( strcmp ( word, "P4" ) != 0 && strcmp ( word, "p4" ) != 0 )
      {
        cout << "\n";
        cout << "PBMB_READ_HEADER - Fatal error.\n";
        cout << "  Bad magic number = \"" << word << "\".\n";
        return true;
      }
      step = 1;
    }

    if ( step == 1 )
    {

      count = sscanf ( next, "%d%n", xsize, &width );
      next = next + width;
      if ( count == EOF )
      {
        continue;
      }
      step = 2;
    }

    if ( step == 2 )
    {
      count = sscanf ( next, "%d%n", ysize, &width );
      next = next + width;
      if ( count == EOF )
      {
        continue;
      }
      break;
    }

  }

  return false;
}
//****************************************************************************

bool pbmb_read_test ( char *file_in_name )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_READ_TEST tests the binary portable bit map read routines.
//
//  Modified:
//
//    17 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *FILE_IN_NAME, the name of the file containing the binary
//    portable bit map data.
//
//    Output, bool PBMB_READ_TEST, is true if an error occurred.
//
{
  int *barray;
  bool error;
  int xsize;
  int ysize;

  barray = NULL;
//
//  Read the data.
//
  error = pbmb_read ( file_in_name, &xsize, &ysize, &barray );

  if ( error )
  {
    cout << "\n";
    cout << "PBMB_READ_TEST: Fatal error!\n";
    cout << "  PBMB_READ failed.\n";
    delete [] barray;
    return true;
  }
//
//  Check the data.
//
  error = pbmb_check_data ( xsize, ysize, barray );

  delete [] barray;

  if ( error )
  {
    cout << "\n";
    cout << "  PBMB_CHECK_DATA reports bad data from the file.\n";
    return true;
  }

  cout << "\n";
  cout << "  PBMB_CHECK_DATA passes the data from the file.\n";

  return false;
}
//****************************************************************************

bool pbmb_write ( char *file_out_name, int xsize, int ysize, int *barray )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_WRITE writes the header and data for a binary portable bit map file.
// 
//  Discussion:
//
//    Thanks to Jonas Schwertfeger for pointing out that, especially on Microsoft
//    Windows systems, a binary file needs to be opened as a binary file!
//
//  Modified:
// 
//    02 April 2005
// 
//  Author:
// 
//    John Burkardt
//
//  Parameters:
//
//    Input, char *FILE_OUT_NAME, the name of the file to contain the binary
//    portable bit map data.
//
//    Input, int XSIZE, YSIZE, the number of rows and columns of data.
//
//    Input, int *BARRAY, the array of XSIZE by YSIZE data values.
//
//    Output, bool PBMB_WRITE, is true if an error occurred.
//
{
  bool error;
  ofstream file_out;

  file_out.open ( file_out_name, ios::binary );

  if ( !file_out )
  {
    cout << "\n";
    cout << "PBMB_WRITE: Fatal error!\n";
    cout << "  Cannot open the output file " << file_out_name << "\n";
    return true;
  }
//
//  Write the header.
//
  error = pbmb_write_header ( file_out, xsize, ysize );

  if ( error )
  {
    cout << "\n";
    cout << "PBMB_WRITE: Fatal error!\n";
    cout << "  PBMB_WRITE_HEADER failed.\n";
    return true;
  }
//
//  Write the data.
//
  error = pbmb_write_data ( file_out, xsize, ysize, barray );

  if ( error )
  {
    cout << "\n";
    cout << "PBMB_WRITE: Fatal error!\n";
    cout << "  PBMB_WRITE_DATA failed.\n";
    return true;
  }
//
//  Close the file.
//
  file_out.close ( );

  return false;
}
//****************************************************************************

bool pbmb_write_data ( ofstream &file_out, int xsize, int ysize, int *barray )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_WRITE_DATA writes the data for a binary portable bit map file.
//
//  Modified:
//
//    17 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, ofstream &FILE_OUT, a pointer to the file to contain the binary
//    portable bit map data.
//
//    Input, int XSIZE, YSIZE, the number of rows and columns of data.
//
//    Input, int *BARRAY, the array of XSIZE by YSIZE data values.
//
//    Output, bool PBMB_WRITE_DATA, is true if an error occurred.
//
{
  int bit;
  unsigned char c;
  int i;
  int *indexb;
  int j;
  int k;

  indexb = barray;
  c = 0;

  for ( j = 0; j < ysize; j++ )
  {
    for ( i = 0; i < xsize; i++ )
    {
      k = 7 - i%8;
      bit = (*indexb)%2;
      c = c | ( bit << k );

      indexb = indexb + 1;

      if ( (i+1)%8 == 0 || i == ( xsize - 1 ) )
      {
        file_out << c;
        c = 0;
      }

    }
  }
  return false;
}
//****************************************************************************

bool pbmb_write_header ( ofstream &file_out, int xsize, int ysize )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_WRITE_HEADER writes the header of a binary portable bit map file.
//
//  Modified:
//
//    17 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, ofstream &FILE_OUT, a pointer to the file to contain the binary
//    portable bit map data.
//
//    Input, int XSIZE, YSIZE, the number of rows and columns of data.
//
//    Output, bool PBMB_WRITE_HEADER, is true if an error occurred.
//
{
  file_out << "P4" << " "
           << xsize << " "
           << ysize << "\n";
 

  return false;
}
//****************************************************************************

bool pbmb_write_test ( char *file_out_name )

//****************************************************************************
//
//  Purpose:
//
//    PBMB_WRITE_TEST tests the binary portable bit map write routines.
//
//  Modified:
//
//    17 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *FILE_OUT_NAME, the name of the file to contain the binary
//    portable bit map data.
//
//    Output, bool PBMB_WRITE_TEST, is true if an error occurred.
//
{
  int *barray;
  bool error;
  int xsize;
  int ysize;
//
//  Set the data.
//
  xsize = 250;
  ysize = 150;
 
  barray = new int [ xsize * ysize ];

  if ( barray == NULL )
  {
    cout << "\n";
    cout << "PBMB_WRITE_TEST: Fatal error!\n";
    cout << "  Unable to allocate memory for data.\n";
    return true;
  }

  error = pbmb_example ( xsize, ysize, barray );

  if ( error )
  {
    cout << "\n";
    cout << "PBMB_WRITE_TEST: Fatal error!\n";
    cout << "  PBM_EXAMPLE failed.\n";
    return true;
  }

  error = pbmb_write ( file_out_name, xsize, ysize, barray );

  delete [] barray;

  if ( error )
  {
    cout << "\n";
    cout << "PBMB_WRITE_TEST: Fatal error!\n";
    cout << "  PBMB_WRITE failed.\n";
    return true;
  }

  return false;
}
