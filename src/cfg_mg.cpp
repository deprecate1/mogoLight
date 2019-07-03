

//
// C++ Implementation: cfg_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cfg_mg.h"
#include "goban_mg.h"
#include "gotools_mg.h"

int MoGo::GobanBitmap::size = 0;
int MoGo::GobanBitmap::mask = 0;
Bitmap MoGo::GobanBitmap::backupGobanBitmap;
Bitmap MoGo::GobanBitmap::tmpBitmap;
Bitmap MoGo::GobanBitmap::oneLocationMask_;
Vector < Bitmap > MoGo::GobanBitmap::zoneBitmaps;
Vector < int > MoGo::GobanBitmap::zoneNumber_;

int MoGo::GobanLocation::height = 0;
int MoGo::GobanLocation::width = 0;
int MoGo::GobanLocation::size = 0;
Vector<int> MoGo::GobanLocation::xLocation_;
Vector<int> MoGo::GobanLocation::yLocation_;
Vector<Vector<Location> > MoGo::GobanLocation::locationXY_;
Location MoGo::GobanLocation::leftUpLocation = width+3;
Location MoGo::GobanLocation::rightDownLocation = (height+1)*(width+2)+width+1;
Vector<int> MoGo::GobanLocation::directionsArray;
Vector < Location > MoGo::GobanLocation::handicapStones_;

int MoGo::CFGGroup::distance1 = 2;
int MoGo::CFGGroup::distance2 = 1;


MoGo::GobanBitmap::GobanBitmap( ) {/**assert(0);**/}

MoGo::GobanBitmap::~ GobanBitmap( ) {/**assert(0);**/}

void MoGo::GobanBitmap::logicalOR( Bitmap &bitmap1, const Bitmap &bitmap2 ) {/**assert(0);**/
  if ((int)bitmap1.size() != (int)bitmap2.size()) {/**assert(0);**/
    showBitmap(bitmap1);
    showBitmap(bitmap2);
  }
  assert(bitmap1.size() == bitmap2.size());
  assert((int)bitmap1.size() == size);
  for (int i=0;i<size;i++)
    bitmap1[i] |= bitmap2[i];
}

void MoGo::GobanBitmap::logicalOR( Bitmap & bitmap, const Vector< Bitmap > & bitmaps ) {/**assert(0);**/
  assert(bitmaps.size());
  bitmap = bitmaps[0];
  for (int i=1;i<(int)bitmaps.size();i++)
    logicalOR(bitmap,bitmaps[i]);
}



void MoGo::GobanBitmap::logicalXOR( Bitmap &bitmap1, const Bitmap &bitmap2 ) {/**assert(0);**/
  assert(bitmap1.size() == bitmap2.size());
  assert((int)bitmap1.size() == size);
  for (int i=0;i<size;i++) {/**assert(0);**/
    bitmap1[i] ^= bitmap2[i];
  }
}

void MoGo::GobanBitmap::logicalAND( Bitmap &bitmap1, const Bitmap &bitmap2 ) {/**assert(0);**/
  assert(bitmap1.size() == bitmap2.size());
  assert((int)bitmap1.size() == size);
  for (int i=0;i<size;i++)
    bitmap1[i] &= bitmap2[i];
}

void MoGo::GobanBitmap::dilation( Bitmap & bitmap ) {/**assert(0);**/
  int here, left, right, up, down;
  tmpBitmap = bitmap;

  here=tmpBitmap[0];if (here) {/**assert(0);**/
    //bitmap[0] |= here;
    right = here>>1;right &= mask;
    bitmap[0] |= right;
    left = here<<1; left &= mask;
    bitmap[0] |= left;
  }

  down=bitmap[1];if (down) bitmap[0] |= down;

  for (int J=1;J<size-1;J++) {/**assert(0);**/
    up = tmpBitmap[J-1];if (up) bitmap[J] |= up;
    here = tmpBitmap[J];if (here) {/**assert(0);**/
      bitmap[J] |= here;
      right = here>>1;right &= mask;
      bitmap[J] |= right;
      left = here<<1; left &= mask;
      bitmap[J] |= left;
    }
    down = bitmap[J+1];
    if (down) bitmap[J] |= down;
  }
  up=tmpBitmap[size-2];if (up) bitmap[size-1] |= up;
  here = tmpBitmap[size-1];if (here) {/**assert(0);**/
    bitmap[size-1] |= here;
    right = here>>1;right &= mask;
    bitmap[size-1] |= right;
    left = here<<1; left &= mask;
    bitmap[size-1] |= left;
  }
}

void MoGo::GobanBitmap::setBitValue( Bitmap & bitmap, const int x, const int y, bool value ) {/**assert(0);**/
  assert((int)bitmap.size() == size);
  if (value)
    bitmap[x] |= oneLocationMask_[y];
  else
    bitmap[x] = bitmap[x]&(bitmap[x]^oneLocationMask_[y]);
}

void MoGo::GobanBitmap::setBitValue( Bitmap & bitmap, const Location location, bool value ) {/**assert(0);**/
  assert((int)bitmap.size() == size);
  int x = GobanLocation::xLocation(location);
  int y = GobanLocation::yLocation(location);
  if (value)
    bitmap[x] |= oneLocationMask_[y];
  else
    bitmap[x] = bitmap[x]&(bitmap[x]^oneLocationMask_[y]);
}

int MoGo::GobanBitmap::getBitValue( const Bitmap & bitmap, const int & x, const int & y ) {/**assert(0);**/
  assert((int)bitmap.size() == size);
  return bitmap[x]&oneLocationMask_[y];
}

int MoGo::GobanBitmap::getBitValue( const Bitmap & bitmap, const Location & location ) {/**assert(0);**/
  assert((int)bitmap.size() == size);
  return bitmap[GobanLocation::xLocation(location)]&oneLocationMask_[GobanLocation::yLocation(location)];

}





void MoGo::GobanBitmap::clear( Bitmap & bitmap ) {/**assert(0);**/
  if ((int)bitmap.size()!=size)
    bitmap.resize(size);
  for (int i=0;i<size;i++)
    bitmap[i] = 0;
}

void MoGo::GobanBitmap::fullfill( Bitmap & bitmap ) {/**assert(0);**/
  if ((int)bitmap.size()!=size)
    bitmap.resize(size);
  for (int i=0;i<size;i++)
    bitmap[i] = mask;
}


void MoGo::GobanBitmap::backupBitmap( const Bitmap & bitmap ) {/**assert(0);**/
  backupGobanBitmap = bitmap;
}

Bitmap MoGo::GobanBitmap::logicalOROnLastBackupBitmap( const Bitmap &bitmap1 ) {/**assert(0);**/
  logicalOR(backupGobanBitmap,bitmap1);
  return backupGobanBitmap;
}

Bitmap MoGo::GobanBitmap::logicalXOROnLastBackupBitmap( const Bitmap &bitmap1 ) {/**assert(0);**/
  logicalXOR(backupGobanBitmap,bitmap1);
  return backupGobanBitmap;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanBitmap::initiate( const Goban & goban ) {
  size = goban.height();
  mask = 1;

  oneLocationMask_.clear();
  for (int i=0;i<goban.width();i++) {
    oneLocationMask_.push_back(mask);
    mask *= 2;
  }
  //mask = mask*2-1;
  mask--;

  backupGobanBitmap.resize(goban.height());
  tmpBitmap.resize(goban.height());
  initiateZoneBitmap();

}

void MoGo::GobanBitmap::printBitmap(const Bitmap & bitmap) {/**assert(0);**/
  for (int i=0;i<(int)bitmap.size();i++)
    GoTools::print("%d ",bitmap[i]);
  GoTools::print("\n");
}

void MoGo::GobanBitmap::showBitmap( const Bitmap & bitmap ) {/**assert(0);**/
  assert((int)bitmap.size() == size);
  for (int i=0;i<size;i++) {/**assert(0);**/
    GoTools::print("\n");
    for (int j=0;j<(int)oneLocationMask_.size();j++)
      if (getBitValue(bitmap,i,j))
        GoTools::print("#");
      else GoTools::print(".");
  }
  GoTools::print("\n");
}

void MoGo::GobanBitmap::showBitmap( const Vector< Bitmap > & bitmaps ) {/**assert(0);**/
  int bitmapSize = (int)bitmaps.size();
  int numbersByLine = 120/(int)oneLocationMask_.size();
  for (int i=0;i<bitmapSize;i+=numbersByLine) {/**assert(0);**/
    for (int j=0;j<size;j++) {/**assert(0);**/
      GoTools::print("\n");
      for (int k=i; k<i+numbersByLine && k<bitmapSize ;k++) {/**assert(0);**/
        for (int l=0;l<(int)oneLocationMask_.size();l++)
          if (getBitValue(bitmaps[k],j,l))
            GoTools::print("#");
          else GoTools::print(".");
        GoTools::print(" ");
      }
    }
    GoTools::print("\n");
  }
}

void MoGo::GobanBitmap::showBitmap( const int bitLine ) {/**assert(0);**/
  for (int j=0;j<(int)oneLocationMask_.size();j++)
    if (bitLine&oneLocationMask_[j])
      GoTools::print("#");
    else GoTools::print(".");
  GoTools::print("\n");
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanBitmap::initiateZoneBitmap( ) {
  zoneBitmaps.clear();
  if (size == 13) {/**assert(0);**/
    assert(GobanLocation::height == 13 && GobanLocation::width == 13);
    zoneNumber_.resize(15*15);

    for (int i=0;i<13;i++)
      for (int j=0;j<13;j++)
        if (i<5) {/**assert(0);**/
          if (j<5) zoneNumber_[GobanLocation::locationXY(i,j)] = 0;
          else if (j<=j && j<8) zoneNumber_[GobanLocation::locationXY(i,j)] = 4;
          else zoneNumber_[GobanLocation::locationXY(i,j)] = 2;
        } else if (i>=5 && i<8) {/**assert(0);**/
          if (j<5) zoneNumber_[GobanLocation::locationXY(i,j)] = 5;
          else if (j<=j && j<8) zoneNumber_[GobanLocation::locationXY(i,j)] = 8;
          else zoneNumber_[GobanLocation::locationXY(i,j)] = 6;
        } else {/**assert(0);**/
          if (j<5) zoneNumber_[GobanLocation::locationXY(i,j)] = 1;
          else if (j<=j && j<8) zoneNumber_[GobanLocation::locationXY(i,j)] = 7;
          else zoneNumber_[GobanLocation::locationXY(i,j)] = 3;
        }


    zoneBitmaps.clear();
    Bitmap tmpBitmap;
    clear(tmpBitmap);
    for (int i=0;i<9;i++)//LEFTUP
      for (int j=0;j<9;j++)
        if (i+j<=8)
          setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);
    clear(tmpBitmap);
    for (int i=4;i<13;i++)//LEFTDOWN
      for (int j=0;j<9;j++)
        if (j-i<=-4)
          setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);
    clear(tmpBitmap);
    for (int i=0;i<9;i++)//RIGHTUP
      for (int j=4;j<13;j++)
        if (j-i>=4)
          setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);
    clear(tmpBitmap);
    for (int i=4;i<13;i++)//RIGHTDOWN
      for (int j=4;j<13;j++)
        if (i+j>=16)
          setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);
    clear(tmpBitmap);
    for (int i=0;i<7;i++)//UP
      for (int j=0;j<13;j++)
        if (j>=i && i+j<=12)
          setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);
    clear(tmpBitmap);
    for (int i=0;i<13;i++)//LEFT
      for (int j=0;j<7;j++)
        if (i>=j && i+j<=12)
          setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);
    clear(tmpBitmap);
    for (int i=0;i<13;i++)//RIGHT
      for (int j=6;j<13;j++)
        if (i+j>=12 && i<=j)
          setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);
    clear(tmpBitmap);
    for (int i=6;i<13;i++)//DOWN
      for (int j=0;j<13;j++)
        if (i+j>=12 && i>=j)
          setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);
    clear(tmpBitmap);
    for (int i=4;i<9;i++)//CENTER
      for (int j=4;j<9;j++)
        setBitValue(tmpBitmap,i,j);
    zoneBitmaps.push_back(tmpBitmap);


  }

}

void MoGo::GobanBitmap::showZoneBitmap( ) {/**assert(0);**/

  for (int i=0;i<(int)oneLocationMask_.size();i++)
    showBitmap(oneLocationMask_[i]);
  showBitmap(mask);
  if (!zoneBitmaps.size()) return ;
  showBitmap(zoneBitmaps);
  GoTools::print("\n");
  for (int i=0;i<GobanLocation::height;i++) {/**assert(0);**/
    for (int j=0;j<GobanLocation::width;j++)
      GoTools::print("%2d ",zoneNumber_[GobanLocation::locationXY(i,j)]);
    GoTools::print("\n");
  }
}

bool MoGo::GobanBitmap::isEqual( const Bitmap & bitmap1, const Bitmap & bitmap2 ) {/**assert(0);**/
  if ((int)bitmap1.size() != (int)bitmap2.size()) return false;
  for (int i=0;i<(int)bitmap1.size();i++)
    if (bitmap1[i]!=bitmap2[i]) return false;
  return true;
}

void MoGo::GobanBitmap::inversion( Bitmap & bitmap ) {/**assert(0);**/
  assert((int)bitmap.size() == size);
  for (int i=0;i<size;i++)
    bitmap[i] ^= mask;
}

bool MoGo::GobanBitmap::isEmpty( const Bitmap & bitmap ) {/**assert(0);**/
  assert((int)bitmap.size() == size);
  for (int i=0;i<size;i++)
    if (bitmap[i]) return false;
  return true;
}

int MoGo::GobanBitmap::getSize( Bitmap & bitmap ) {/**assert(0);**/
  int counter = 0;
  assert((int)bitmap.size() == size);
  for (int i=0;i<size;i++)
    for (int j=0;j<(int)oneLocationMask_.size();j++)
      counter+= getBitValue(bitmap,i,j)>0;
  return counter;

}

void MoGo::GobanBitmap::getRectangle( const Bitmap & bitmap, int & topLine, int & bottomLine, int & leftColumn, int & rightColumn ) {/**assert(0);**/
  topLine = -1;
  leftColumn = GobanLocation::width;
  rightColumn = 0;
  bottomLine = 0;
  for (int i=0;i<GobanLocation::height;i++)
    if (bitmap[i]) {/**assert(0);**/
      topLine = i;
      break;
    }

  if (topLine == -1)
    return;

  for (int i=GobanLocation::height-1; i>=0 ;i--)
    if (bitmap[i]) {/**assert(0);**/
      bottomLine = i;
      break;
    }
  assert(topLine<=bottomLine);
  for (int i=topLine;i<=bottomLine;i++)
    for (int j=0;j<GobanLocation::width;j++)
      if (GobanBitmap::getBitValue(bitmap,i,j)) {/**assert(0);**/
        if (j<leftColumn) leftColumn = j;
        if (j>rightColumn) rightColumn = j;
      }


}

void MoGo::GobanBitmap::setRectangle( Bitmap & bitmap, int topLine, int bottomLine, int leftColumn, int rightColumn ) {/**assert(0);**/
  for (int i=topLine;i<=bottomLine;i++)
    for (int j=leftColumn;j<=rightColumn;j++)
      setBitValue(bitmap,i,j);
}

Vector<int>  MoGo::GobanBitmap::removeIncludedBitmaps( Vector< Bitmap > & bitmaps, bool isRectangle ) {/**assert(0);**/
  //   showBitmap(bitmaps);

  Vector<int> toRemove;
  for (int i=0;i<(int)bitmaps.size()-1;i++)
    for (int j=i+1;j<(int)bitmaps.size();j++)
      if (isIncludedIn(bitmaps[i],bitmaps[j],isRectangle)) {/**assert(0);**/
        toRemove.push_back(i);
      } else if (isIncludedIn(bitmaps[j],bitmaps[i],isRectangle)) {/**assert(0);**/
        toRemove.push_back(j);
      }

  removeFromVectorIndexes(bitmaps, toRemove);
  //   showBitmap(bitmaps);
  return toRemove;
}
bool MoGo::GobanBitmap::isIncludedIn(const Bitmap & bitmap, const Bitmap & bitmap1, bool isRectangle) {/**assert(0);**/
  if (!isRectangle) {/**assert(0);**/
    Bitmap tmpBitmap = bitmap;
    logicalOR(tmpBitmap,bitmap1);
    return isEqual(tmpBitmap, bitmap1);
  } else {/**assert(0);**/
    int tL, tL1, bL , bL1, lC, lC1, rC, rC1;
    getRectangle(bitmap,tL,bL,lC,rC);
    getRectangle(bitmap1,tL1,bL1,lC1,rC1);
    return (tL>=tL1 && bL <= bL1 && lC >= lC1 && rC <= rC1) ;
  }
}

void MoGo::GobanBitmap::changeBitmapToLocations(const Bitmap & bitmap, Vector < Location > & location) {/**assert(0);**/
  for (int i=0;i<size;i++)
    if (bitmap[i])
      for (int j=0;j<(int)oneLocationMask_.size();j++)
        if (getBitValue(bitmap,i,j))
          location.push_back(GobanLocation::locationXY(i,j));
}








































MoGo::GobanLocation::GobanLocation( ) {/**assert(0);**/}

MoGo::GobanLocation::~ GobanLocation( ) {/**assert(0);**/}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanLocation::resize( const Goban & goban ) {
  if (height == goban.height() &&
      width == goban.width())
    return;
  height = goban.height();
  width = goban.width();
  size = height*width;
  xLocation_.resize((height+2)*(width+2));
  yLocation_.resize((height+2)*(width+2));
  locationXY_.resize(height);
  //      indexOnArray(int x, int y) returns int v=(x+1)*(widthOfGoban+2)+(y+1);

  leftUpLocation = width+3;
  rightDownLocation = (height+1)*(width+2)+width+1;
  Location location = goban.leftUpLocation();

  for (int i=0;i<height;i++,location+=2) {
    locationXY_[i].resize(width);
    for (int j=0;j<width;j++,location++) {
      xLocation_[location] = i;
      yLocation_[location] = j;
      locationXY_[i][j] = location;
    }
  }
  assert((int)xLocation_.size() == (int)goban.getGobanState().size());
  for (int i=0;i<(int)goban.getGobanState().size();i++)
    if (goban.getGobanState()[i] == OUTSIDE) {
      xLocation_[i] = -1;
      yLocation_[i] = -1;
    }

  recreateDirectionsArray();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanLocation::recreateDirectionsArray( ) {
  directionsArray.resize(20);
  //
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  //
  directionsArray[0] = -1;//LEFT
  directionsArray[1] = -(width+2);//UP
  directionsArray[2] = 1;//RIGHT
  directionsArray[3] = width+2 ;//DOWN
  directionsArray[4] = -(width+1);//RIGHTUP
  directionsArray[5] = -(width+3);//LEFTUP
  directionsArray[6] = width+1;//LEFTDOWN
  directionsArray[7] = width+3;//RIGHTDOWN

  directionsArray[8] = directionsArray[LEFT_]+directionsArray[LEFT_];
  directionsArray[9] = directionsArray[UP_]+directionsArray[UP_];
  directionsArray[10] = directionsArray[RIGHT_]+directionsArray[RIGHT_];
  directionsArray[11] = directionsArray[DOWN_]+directionsArray[DOWN_];
  directionsArray[12] = directionsArray[LEFT_]+directionsArray[UP_]+directionsArray[UP_];
  directionsArray[13] = directionsArray[RIGHT_]+directionsArray[UP_]+directionsArray[UP_];
  directionsArray[14] = directionsArray[UP_]+directionsArray[RIGHT_]+directionsArray[RIGHT_];
  directionsArray[15] = directionsArray[DOWN_]+directionsArray[RIGHT_]+directionsArray[RIGHT_];
  directionsArray[16] = directionsArray[RIGHT_]+directionsArray[DOWN_]+directionsArray[DOWN_];
  directionsArray[17] = directionsArray[LEFT_]+directionsArray[DOWN_]+directionsArray[DOWN_];
  directionsArray[18] = directionsArray[DOWN_]+directionsArray[LEFT_]+directionsArray[LEFT_];
  directionsArray[19] = directionsArray[UP_]+directionsArray[LEFT_]+directionsArray[LEFT_];


}


void MoGo::GobanLocation::textModeShowPosition( const Location location ) {/**assert(0);**/
  if (location==PASSMOVE) {/**assert(0);**/
    GoTools::print("pass ");
    return;
  }
  int x = xLocation(location);
  int y = yLocation(location);
  if (y>=8)
    GoTools::print("%c%i ",(char)('A'+y+1),height-x);
  else
    GoTools::print("%c%i ",(char)('A'+y),height-x);
  if (height>9 && height-x<10) GoTools::print(" ");
  return;

}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
int MoGo::GobanLocation::getSideNumberFast( const Location location ) {
  int x = xLocation_[location];
  int y = yLocation_[location];

  if (x == 0 && y!=0 && y!=width-1) return 0;
  if (y == width-1 && x!=0 && x!=height-1) return 1;
  if (x == height-1 && y!=0 && y!=width-1) return 2;
  if (y == 0 && x!=0 && x!=height-1) return 3;
  return 4;//TODO 4 is might be a corner, might be the center, not defined yet.
}

bool MoGo::GobanLocation::setHandicapStones( int number ) {/**assert(0);**/
  handicapStones_.clear();
  if (height != width || height < 13) return false;
  int size = height;
  handicapStones_.push_back(locationXY_[size-4][3]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[3][size-4]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[3][3]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[size-4][size-4]);
  if ((int)handicapStones_.size() == number) return true;
  if (size%2 == 0) return false;
  int middle = size/2;
  if (number == 5) {/**assert(0);**/
    handicapStones_.push_back(locationXY_[middle][middle]);
    return true;
  }
  handicapStones_.push_back(locationXY_[middle][3]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[middle][size-4]);
  if ((int)handicapStones_.size() == number) return true;
  if (number == 7) {/**assert(0);**/
    handicapStones_.push_back(locationXY_[middle][middle]);
    return true;
  }
  handicapStones_.push_back(locationXY_[3][middle]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[size-4][middle]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[middle][middle]);
  return true;
}

const Vector< Location > & MoGo::GobanLocation::getHandicapStones( ) {/**assert(0);**/
  return handicapStones_;
}

bool MoGo::GobanLocation::setHandicapStones( int number,  Vector< Location > &handicapStones_ ) {/**assert(0);**/
  handicapStones_.clear();
  if (height != width || height < 13) return false;
  int size = height;
  handicapStones_.push_back(locationXY_[size-4][3]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[3][size-4]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[3][3]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[size-4][size-4]);
  if ((int)handicapStones_.size() == number) return true;
  if (size%2 == 0) return false;
  int middle = size/2;
  if (number == 5) {/**assert(0);**/
    handicapStones_.push_back(locationXY_[middle][middle]);
    return true;
  }
  handicapStones_.push_back(locationXY_[middle][3]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[middle][size-4]);
  if ((int)handicapStones_.size() == number) return true;
  if (number == 7) {/**assert(0);**/
    handicapStones_.push_back(locationXY_[middle][middle]);
    return true;
  }
  handicapStones_.push_back(locationXY_[3][middle]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[size-4][middle]);
  if ((int)handicapStones_.size() == number) return true;
  handicapStones_.push_back(locationXY_[middle][middle]);
  return true;
}

Location MoGo::GobanLocation::changeToLocation( const char * token ) {/**assert(0);**/
  if (!strncmp(token,"PASS",4) || !strncmp(token,"pass",4) || !strncmp(token,"Pass",4)) return PASSMOVE;
  int i,j=0;
  if ('A'<=token[0] && token[0]<='Z') {/**assert(0);**/
    j=(int)(token[0]-(int)'A');
    if ((int)token[0]>=(int)'I') j--;// Be careful! Why there is no 'I' in the GNUGO? Or for all the protocol(I dont think so)?
  } else
    if ('a'<=token[0] && token[0]<='z') {/**assert(0);**/
      j=(int)(token[0]-'a');
      if ((int)token[0]>=(int)'i') j--;
    } else assert(0);

  i=height-atoi(token+1);
  return locationXY_[i][j];
}
































/////////////////////
// class CFGString //
/////////////////////



MoGo::CFGString::CFGString( ) {/**assert(0);**/
  clear();
}

MoGo::CFGString::~ CFGString( ) {/**assert(0);**/}

void MoGo::CFGString::initiate(int size ) {/**assert(0);**/
  //freeLocationCFG.adjacentBlackStrNbs.resize(size);
  //freeLocationCFG.adjacentWhiteStrNbs.resize(size);
  //freeLocationCFG.adjacentFreeLocations.resize(size);
  stringNumber.resize(size);


  maxNumberOfString = 0;
  //GoTools::print("CFG initiate OK!\n");
  clear();
}


void MoGo::CFGString::clear( ) {/**assert(0);**/
  adjacentStrNbs.clear();
  liberties.clear();
  stones.clear();
  maxNumberOfString = 0;
  adjacentStrNbs.push_back(LocationList());
  liberties.push_back(LocationList());
  stones.push_back(FastSmallVector<Location>());
  for (int i=0;i<(int)stringNumber.size();i++)
    stringNumber[i] = 0;

  resetStringPreview();

}

void MoGo::CFGString::resetStringPreview( ) {/**assert(0);**/
  lastStringPreviewLocation=-1;
  lastStringPreviewResult=false;
}

void MoGo::CFGString::updateStringNumber( Goban *goban, const Location & location, const LocationList * adjacentFriendStrNbs, const LocationList * adjacentEnemyStrNbs,const FastSmallVector< Location > & emptyLocations ) {/**assert(0);**/
  StringNumber strNb = 0;
  //assert(0);
  if (adjacentFriendStrNbs->size()) {/**assert(0);**/

    strNb = getBiggestLibertiesStrNumber( location, adjacentFriendStrNbs );
    //assert(0);
    for (int i=0;i<(int)adjacentFriendStrNbs->size();i++) {/**assert(0);**/
      StringNumber friendStrNb = (*adjacentFriendStrNbs)[i];
      if (friendStrNb != strNb) {/**assert(0);**/
        for (int j=0;j<(int)adjacentStrNbs[friendStrNb].size();j++)//FIXME here addLocation checks the repeating case.
        {/**assert(0);**/
          adjacentStrNbs[strNb].addLocation(adjacentStrNbs[friendStrNb][j]);
          adjacentStrNbs[adjacentStrNbs[friendStrNb][j]].deleteLocation(friendStrNb);
          adjacentStrNbs[adjacentStrNbs[friendStrNb][j]].addLocation(strNb);
        }
        for (int j=0;j<(int)liberties[friendStrNb].size();j++)
          liberties[strNb].addLocation(liberties[friendStrNb][j]);
        for (int k=0;k<(int)stones[friendStrNb].size();k++) {/**assert(0);**/
          stones[strNb].push_back(stones[friendStrNb][k]);
          stringNumber[stones[friendStrNb][k]] = strNb;
        }

        liberties[friendStrNb].clear();//necessary?
        stones[friendStrNb].clear();
      }
      liberties[strNb].deleteLocation(location);//FIXME deleteLocation takes care of the repeating case. stupid!!!
    }

    //for (int i=0;i<(int)freeLocationCFG.adjacentFreeLocations[location].size();i++)
    //stringCFG.liberties[strNb].addLocation(freeLocationCFG.adjacentFreeLocations[location][i]);
    for (int i=0;i<(int)emptyLocations.size();i++)
      liberties[strNb].addLocation(emptyLocations[i]);
  } else //no friend adjacent string.
  {/**assert(0);**/
    strNb = liberties.size();
    maxNumberOfString++;
    assert(maxNumberOfString == strNb);//FIXME CFG to be deleted
    adjacentStrNbs.push_back(*adjacentEnemyStrNbs);
    //stringCFG.liberties.push_back(freeLocationCFG.adjacentFreeLocations[location]);
    //stringCFG.liberties.push_back(emptyLocations);
    LocationList tmpLocationList;
    for (int i=0;i<(int)emptyLocations.size();i++)
      tmpLocationList.addLocation(emptyLocations[i]);
    liberties.push_back(tmpLocationList);
    assert((int)liberties.size() == strNb+1);


    stones.push_back(FastSmallVector< Location >());
    assert((int)stones.size() == strNb+1);
  }



  stringNumber[location] = strNb;
  stones[strNb].push_back(location);

  for (int i=0;i<(int)adjacentEnemyStrNbs->size();i++) {/**assert(0);**/
    StringNumber enemyStrNb = (*adjacentEnemyStrNbs)[i];
    if (!liberties[enemyStrNb].isLocationIn(location)) {/**assert(0);**/
      GoTools::print("enemyStrNb: %d",enemyStrNb);
      GobanLocation::textModeShowPosition(location);
      GoTools::print("\n");
    }
    if (!liberties[enemyStrNb].isLocationIn(location))
      printInformation();
    if (!liberties[enemyStrNb].isLocationIn(location)) {/**assert(0);**/
      goban->textModeShowGoban(); goban->textModeShowPosition(location);
      assert(0);
    }
    liberties[enemyStrNb].deleteLocation(location);

    adjacentStrNbs[strNb].addLocation(enemyStrNb);
    adjacentStrNbs[enemyStrNb].addLocation(strNb);
    if (!liberties[enemyStrNb].size()) {/**assert(0);**/
      goban->gobanState[location] = goban->playerColor;//trick
      updateErasingString(goban,enemyStrNb);
      goban->gobanState[location] = EMPTY;
    }

  }


}

StringNumber MoGo::CFGString::getBiggestLibertiesStrNumber( const Location & , const LocationList * adjacentFriendStrNbs) {/**assert(0);**/
  assert(adjacentFriendStrNbs->size());
  StringNumber strNb = (*adjacentFriendStrNbs)[0];
  for (int i=1;i<(int)adjacentFriendStrNbs->size();i++)
    if (stones[(*adjacentFriendStrNbs)[i]].size()>stones[strNb].size())
      strNb = (*adjacentFriendStrNbs)[i];
  return strNb;
}

void MoGo::CFGString::updateErasingString( Goban * goban, const StringNumber & strNb ) {/**assert(0);**/
  Location tmpLocation = stones[strNb][0];
  StoneColor color = 3-goban->gobanState[tmpLocation];
  if (color == 3) {/**assert(0);**///has been cleared already by stringInformation.
    for (int i=0;i<(int)stones[strNb].size();i++) {/**assert(0);**///it's possible that one stones is surrounded by the same color stones, or the outside.
      tmpLocation = stones[strNb][i];
      for (int j=0;j<4;j++)
        if (goban->gobanState[tmpLocation+GobanLocation::directionsArray[j]] == BLACK ||
            goban->gobanState[tmpLocation+GobanLocation::directionsArray[j]] == WHITE) {/**assert(0);**/
          color = goban->gobanState[tmpLocation+GobanLocation::directionsArray[j]];
          break;
        }
      if (color != 3) break;
    }
    assert(color == BLACK || color == WHITE);
  }
  //  Vector < LocationList > * adjacentStrNbs = 0;
  //   if (color == BLACK)
  //     adjacentStrNbs = &freeLocationCFG.adjacentBlackStrNbs;
  //   else
  //     adjacentStrNbs = &freeLocationCFG.adjacentWhiteStrNbs;

  for (int i=0;i<(int)stones[strNb].size();i++) {/**assert(0);**/
    Location location = stones[strNb][i];
    //freeLocationCFG.adjacentBlackStrNbs[location].clear();
    //freeLocationCFG.adjacentWhiteStrNbs[location].clear();
    //freeLocationCFG.adjacentFreeLocations[location].clear();
    for (int j=0;j<4;j++)
      if (goban->gobanState[location+goban->direction(j)] == color) {/**assert(0);**/
        if (!stringNumber[location+goban->direction(j)])
          GobanLocation::textModeShowPosition(location+goban->direction(j));
        assert(stringNumber[location+goban->direction(j)] != 0);
        liberties[stringNumber[location+goban->direction(j)]].addLocation(location);
        //(*adjacentStrNbs)[location].addLocation(stringNumber[location+goban->direction(j)]);
      } //else if (goban->gobanState[location+goban->direction(j)] != OUTSIDE)
    //freeLocationCFG.adjacentFreeLocations[location].addLocation(location+goban->direction(j));
    stringNumber[location] = 0;
    tmpErasedStones.push_back(location);

  }
  for (int i=0;i<(int)adjacentStrNbs[strNb].size();i++)
    adjacentStrNbs[adjacentStrNbs[strNb][i]].deleteLocation(strNb);
  stones[strNb].clear();
}

void MoGo::CFGString::printInformation( ) const {/**assert(0);**/
  //textModeShowGoban();
  for ( int i=0;i<(int)stones.size();i++)
    if (stones[i].size() ) {/**assert(0);**/
      GoTools::print("string number %d: %d adjacent string number: ",i,(int)adjacentStrNbs[i].size());
      for (int j=0;j<(int)adjacentStrNbs[i].size();j++)
        GoTools::print(" %d",adjacentStrNbs[i][j]);

      GoTools::print("|| %d string stones: ",stones[i].size());
      for (int j=0;j<(int)stones[i].size();j++)
        GobanLocation::textModeShowPosition(stones[i][j]);

      GoTools::print("|| %d liberties: ",liberties[i].size());
      for (int j=0;j<(int)liberties[i].size();j++)
        GobanLocation::textModeShowPosition(liberties[i][j]);
      GoTools::print("\n");

    }
  if (liberties[0].size()) {/**assert(0);**/
    GoTools::print("Warning! liberties[0] not empty!\n");
    for (int i=0;i<(int)liberties[0].size();i++)
      GobanLocation::textModeShowPosition(liberties[0][i]);
  }

}

void MoGo::CFGString::calculateAllFromGobanState( const Goban * goban, Vector < Location > & gobanState ) {/**assert(0);**/
  clear();
  /*  maxNumberOfString=0;
    stringLiberty.clear();stringLiberty.push_back(0);
    stringLength.clear();stringLength.push_back(0);*/
  //stringSelfKillLocation.clear();stringSelfKillLocation.push_back(0);
  //stringEyeNumber.clear();stringEyeNumber.push_back(0);
  Location location = GobanLocation::leftUpLocation;
  for (int i=0;i<goban->heightOfGoban;i++,location+=2)
    for (int j=0;j<goban->widthOfGoban;j++,location++)
      if (stringNumber[location]==0 &&
          (gobanState[location]==BLACK || gobanState[location] == WHITE))
        calculateOneFromGobanState(goban,location);

  calculateAdjacentStringNumbers(gobanState,GobanLocation::directionsArray);

}

void MoGo::CFGString::calculateAdjacentStringNumbers(Vector < Location > &gobanState, Vector < int > &directionsArray) {/**assert(0);**/
  for (int i=1;i<=maxNumberOfString;i++) {/**assert(0);**/
    StoneColor color = 3 - gobanState[stones[i][0]];
    for (int j=0;j<(int)stones[i].size();j++)
      for (int k=0;k<4;k++)
        if (gobanState[stones[i][j]+directionsArray[k]] == color)
          adjacentStrNbs[i].addLocation(stringNumber[stones[i][j]+directionsArray[k]]);
  }
}


void MoGo::CFGString::calculateOneFromGobanState(const Goban * goban, const Location location) {/**assert(0);**/
  /*  maxNumberOfString++;
    stringLiberty.push_back(0);
    stringLength.push_back(0);*/
  //stringSelfKillLocation.push_back(0);
  //stringEyeNumber.push_back(0);
  maxNumberOfString++;
  adjacentStrNbs.push_back(LocationList());
  liberties.push_back(LocationList());
  stones.push_back(FastSmallVector<int>());
  iterateCalculateOneFromGobanState(goban,location,goban->gobanState[location],goban->gobanState);
}


void MoGo::CFGString::iterateCalculateOneFromGobanState(const Goban * goban, const Location location,const StoneColor stringColor, const Vector < Location > &gobanState) {/**assert(0);**/
  assert(maxNumberOfString >= 0);
  assert(maxNumberOfString == (int)liberties.size()-1);

  if (gobanState[location]!=stringColor) return;// other player's stone or outside. iteration never falls on an empty location.
  if (stringNumber[location]==maxNumberOfString) return;
  for (int i=0;i<4;i++)
    if (gobanState[location+GobanLocation::directionsArray[i]]==EMPTY)
      liberties[maxNumberOfString].addLocation(location+GobanLocation::directionsArray[i]);
  stringNumber[location]=maxNumberOfString;
  stones[maxNumberOfString].push_back(location);

  for (int i=0;i<4;i++)
    if (gobanState[location+GobanLocation::directionsArray[i]]==stringColor)
      iterateCalculateOneFromGobanState(goban,location+GobanLocation::directionsArray[i],stringColor,gobanState);
  return;
}

void MoGo::CFGString::playMoveOnGoban( Goban * goban, const Location & location ) {/**assert(0);**/
  tmpErasedStones.clear();
  updateStringNumber(goban,location,&tmpAdjacentFriendStrNbs,&tmpAdjacentEnemyStrNbs,tmpAdjacentFreeLocations);

}

int MoGo::CFGString::basicTest( const Goban * goban, const Location location ) const {/**assert(0);**/
  assert(location!=PASSMOVE);
  testKo=0;
  if (goban->gobanState[location]==OUTSIDE) return OUTGOBAN;//FIXME
  if (goban->gobanState[location]==WHITE || goban->gobanState[location]==BLACK) return ALREADYTAKEN;
  if (location==goban->ko) return KOILL;
  if (!stringPreview(location, goban->gobanState, goban->directionsArray, goban->playerColor))
    return SELFKILLMOVE;
  if (testLocationIsNotKo) return LEGAL;
  int deadStone = 0;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmp = location+goban->direction(i);
    if (goban->gobanState[tmp] == goban->playerColor ||
        goban->gobanState[tmp] == EMPTY) return LEGAL;
    else if (goban->gobanState[tmp] == 3-goban->playerColor)
      if (liberties[stringNumber[tmp]].size() == 1) {/**assert(0);**/
        if (stones[stringNumber[tmp]].size() >= 2) return LEGAL;
        if (deadStone) return LEGAL;
        deadStone = tmp;
      }
  }
  if (deadStone == 0) return LEGAL;
  testKo=deadStone;
  return LEGAL;
}

int MoGo::CFGString::uselessnessTest(const Goban * goban, const Location location) const//executed ALWAYS after basicTest(then stringPreview) returns LEGAL, nobody except yizao understands these terrible codes... TOO BAD.
{/**assert(0);**/
  assert(location!=PASSMOVE);

  if (testLocationIsNotEyeFillMove) return LEGAL;

  for ( int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)] == 3-goban->playerColor ||
        goban->gobanState[location+goban->direction(i)] == EMPTY) return LEGAL;

  //now, necessary connecting move or eyefill move.
  int j = 0;
  for(int i=4;i<8;i++)
    if ( goban->gobanState[location+goban->direction(i)] == 3-goban->playerColor ) j++;
  if (j>1) return LEGAL;
  if (j == 0) return EYEFILLMOVE;
  else if (goban->isOnGobanSide(location)) return LEGAL;//FIXME here LocationInformation is called.
  return EYEFILLMOVE;

}

bool MoGo::CFGString::stringPreview(const Location location, const Vector<Location> &gobanState, const Vector<int> &directionsArray, const int &playerColor) const {/**assert(0);**/
  if (location == lastStringPreviewLocation)
    return lastStringPreviewResult;
  bool isLegalMove = false;
  stringPreviewClear();
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmp = location+directionsArray[i];
    if (gobanState[tmp]==EMPTY) // empty
    {/**assert(0);**/
      tmpAdjacentFreeLocations.push_back(tmp);
      assert(tmpAdjacentFreeLocations.size());
      testLocationIsNotKo = true;
      testLocationIsNotEyeFillMove = true;
      isLegalMove = true;
    } else if (gobanState[tmp]==playerColor) // friend
    {/**assert(0);**/
      tmpAdjacentFriendStrNbs.addLocation(stringNumber[tmp]);
      if (!isLegalMove &&
          liberties[stringNumber[tmp]].size() >= 2) {/**assert(0);**/
        testLocationIsNotKo = true;
        testLocationIsNotEyeFillMove = false;
        isLegalMove = true;
      }
    } else if (gobanState[tmp]==3-playerColor) // enemy
    {/**assert(0);**/
      tmpAdjacentEnemyStrNbs.addLocation(stringNumber[tmp]);
      if (!isLegalMove &&
          liberties[stringNumber[tmp]].size() == 1) {/**assert(0);**/
        if (stones[stringNumber[tmp]].size() == 1)
          testLocationIsNotKo = false;
        else
          testLocationIsNotKo = true;
        testLocationIsNotEyeFillMove = true;
        isLegalMove = true;
      }
    }
  }
  lastStringPreviewLocation = location;
  lastStringPreviewResult = isLegalMove;
  return isLegalMove;
}

void MoGo::CFGString::stringPreviewClear() const {/**assert(0);**/
  tmpAdjacentFriendStrNbs.clear();
  tmpAdjacentEnemyStrNbs.clear();
  tmpAdjacentFreeLocations.clear();
}

void MoGo::CFGString::freeLocationUpdateAfterPlayMoveOnGoban( Goban * goban, const Location location, GobanLocationLists &gobanLists, LocationList *locationListIncludingThisLocation ) const {/**assert(0);**/

  assert(locationListIncludingThisLocation);
  //deleteLocationFromList(location);
  //for the location played.
  //textModeShowPosition(location);GoTools::print("erased...");
  locationListIncludingThisLocation->deleteLocation(location);
  for (int i=0;i<(int)tmpErasedStones.size();i++) {/**assert(0);**/
    if (gobanLists.rangeLocation.isLocationIn(tmpErasedStones[i]))
      gobanLists.freeLocation.addLocation(tmpErasedStones[i]);
    goban->gobanState[tmpErasedStones[i]] = EMPTY;
  }
}




void MoGo::CFGString::selfKillLocationUpdate( Goban & , const Vector < Location > &gobanState, GobanLocationLists &gobanLists, const StoneColor & playerColor, Vector < int > &directionsArray ) const {/**assert(0);**/
  //when there are stones eaten.
  LocationList *locationList = 0;
  if (playerColor==BLACK)
    locationList = &gobanLists.blackSelfKillLocation;
  else if (playerColor==WHITE)
    locationList = &gobanLists.whiteSelfKillLocation;
  else assert(0);
  for (int i=0;i<(int)locationList->size();i++) {/**assert(0);**/
    for (int j=0;j<4;j++)
      if (gobanState[(*locationList)[i]+directionsArray[j]]==playerColor)
        if (liberties[stringNumber[(*locationList)[i]+directionsArray[j]]].size()>1) {/**assert(0);**/
          assert(!gobanLists.freeLocation.isLocationIn((*locationList)[i]));
          if (gobanLists.rangeLocation.isLocationIn((*locationList)[i])) {/**assert(0);**/
            gobanLists.freeLocation.addLocation((*locationList)[i]);
            locationList->deleteLocation((*locationList)[i]);
            i--;
            break;
          }
        }
  }
}

void MoGo::CFGString::specialLocationUpdate( const Goban * goban, const Location location, const Vector < Location > &, GobanLocationLists &gobanLists ) const {/**assert(0);**/
  //When a string has but one liberty left, then his eye, if he has one, is no longer meaningful.
  //also the self atari move. this two situation leads to the delete of the previous and the add to the //new selfKillLocation list.
  for (int i=0;i<(int)tmpAdjacentEnemyStrNbs.size();i++)
    if (liberties[tmpAdjacentEnemyStrNbs[i]].size() == 1) {/**assert(0);**/
      specialLocationUpdateForOneLibertyString(goban,tmpAdjacentEnemyStrNbs[i],3-goban->playerColor,gobanLists);
    }
  if (liberties[stringNumber[location]].size() == 1)
    specialLocationUpdateForOneLibertyString(goban,stringNumber[location],goban->playerColor,gobanLists);
}

void MoGo::CFGString::specialLocationUpdateForOneLibertyString( const Goban * , const StringNumber strNb, const StoneColor stringColor, GobanLocationLists &gobanLists ) const {/**assert(0);**/
  //here we have a string, his color given, knowing that he has only one liberty.
  //if one eye tested before belongs to this string...
  Location lastLiberty=liberties[strNb][0];
  if (!gobanLists.rangeLocation.isLocationIn(lastLiberty)) return;

  if (gobanLists.eyeLocation.isLocationIn(lastLiberty))
    gobanLists.eyeLocation.deleteLocation(lastLiberty);
  gobanLists.freeLocation.addLocation(lastLiberty);

  if (stringColor==BLACK) {/**assert(0);**/
    if (gobanLists.whiteSelfKillLocation.isLocationIn(lastLiberty))
      gobanLists.whiteSelfKillLocation.deleteLocation(lastLiberty);
    gobanLists.whiteEatingLocation.addLocation(lastLiberty);
  } else {/**assert(0);**/
    if (gobanLists.blackSelfKillLocation.isLocationIn(lastLiberty))
      gobanLists.blackSelfKillLocation.deleteLocation(lastLiberty);
    gobanLists.blackEatingLocation.addLocation(lastLiberty);
  }

}

void MoGo::CFGString::updateEatingMove( const Goban * goban, const Location location, GobanLocationLists &gobanLists) const {/**assert(0);**/
  //This function is executed before the changement of player.
  EatingLocationListType *playerEatingLocation, *opponentEatingLocation;
  if (goban->isBlacksTurn()) {/**assert(0);**/
    playerEatingLocation=&gobanLists.blackEatingLocation;
    opponentEatingLocation=&gobanLists.whiteEatingLocation;
  } else {/**assert(0);**/
    playerEatingLocation=&gobanLists.whiteEatingLocation;
    opponentEatingLocation=&gobanLists.blackEatingLocation;
  }
  Location selfEatingLocation=-1;
  if (liberties[stringNumber[location]].size() == 1)
    //the move played right away is a self-atari
  {/**assert(0);**/
    selfEatingLocation=liberties[stringNumber[location]][0];
    if (gobanLists.rangeLocation.isLocationIn(selfEatingLocation))
      opponentEatingLocation->addLocation(selfEatingLocation);
  }
  for (int i=0;i<(int)tmpAdjacentEnemyStrNbs.size();i++) {/**assert(0);**/
    if (liberties[tmpAdjacentEnemyStrNbs[i]].size()==1) {/**assert(0);**/
      Location eatingEnemyLocation = liberties[tmpAdjacentEnemyStrNbs[i]][0];
      if (gobanLists.rangeLocation.isLocationIn(eatingEnemyLocation))
        playerEatingLocation->addLocation(eatingEnemyLocation);
    }
  }
}

void MoGo::CFGString::showOneLibertyStringsInGTPMode( const Goban * goban ) const {/**assert(0);**/
  int totalStringNumber=maxNumberOfString+1;
  int test = 1;
  for (int i=0;i<totalStringNumber;i++)
    if (liberties[i].size() == 1) {/**assert(0);**/
      test = 0;
      for (int j=0;j<(int)stones[i].size();j++)
        goban->textModeShowPosition(stones[i][j]);
      GoTools::print("\n");
    }
  if (test) GoTools::print("\n");

}

void MoGo::CFGString::addStringToBitmap( const StringNumber strNb, Bitmap & bitmap ) const {/**assert(0);**/
  for (int j=0;j<(int)stones[strNb].size();j++) {/**assert(0);**/
    Location location = stones[strNb][j];
    GobanBitmap::setBitValue(bitmap,GobanLocation::xLocation(location),GobanLocation::yLocation(location));
  }
}

void MoGo::CFGString::addStringLibertiesToBitmap( const StringNumber strNb, Bitmap & bitmap ) const {/**assert(0);**/
  for (int j=0;j<(int)liberties[strNb].size();j++) {/**assert(0);**/
    Location location = liberties[strNb][j];
    GobanBitmap::setBitValue(bitmap,GobanLocation::xLocation(location),GobanLocation::yLocation(location));
  }
}

void MoGo::CFGString::calculateTerritory( const Goban * goban ) const {/**assert(0);**/
  if ((int)territoryNumber.size() != (int)stringNumber.size())
    territoryNumber.resize(stringNumber.size());
  else for (int i=0;i<(int)territoryNumber.size();i++)
      territoryNumber[i] = 0;

  maxTerritoryNumber = 0;
  territoryAdjacentStrNbs.clear();territoryAdjacentStrNbs.push_back(LocationList());
  territoryLocations.clear();territoryLocations.push_back(LocationList());
  territoryOwnerColor.clear();territoryOwnerColor.push_back(EMPTY);

  for (int i=0;i<(int)stringNumber.size();i++)
    if (goban->gobanState[i] == EMPTY && territoryNumber[i] == 0)
      calculateOneTerritory(goban,i);
}

void MoGo::CFGString::calculateOneTerritory( const Goban * goban, const Location location ) const {/**assert(0);**/
  maxTerritoryNumber++;
  territoryAdjacentStrNbs.push_back(LocationList());
  territoryLocations.push_back(LocationList());
  assert((int)territoryAdjacentStrNbs.size() == maxTerritoryNumber+1);
  assert((int)territoryLocations.size() == maxTerritoryNumber+1);
  territoryOwnerColor.push_back(EMPTY);
  assert((int)territoryOwnerColor.size() == maxTerritoryNumber+1);
  iterateOneTerritory(goban,location,maxTerritoryNumber);
}

void MoGo::CFGString::iterateOneTerritory( const Goban * goban , const Location location, const int terriNb ) const {/**assert(0);**/
  if (goban->gobanState[location] == EMPTY) {/**assert(0);**/
    if (territoryNumber[location] == 0) {/**assert(0);**/
      territoryNumber[location] = terriNb;
      territoryLocations[terriNb].addLocation(location,true);
      for (int i=0;i<4;i++)
        iterateOneTerritory(goban,location+goban->directionsArray[i],terriNb);
    }
    return;
  } else if (goban->gobanState[location] != OUTSIDE) {/**assert(0);**/
    territoryAdjacentStrNbs[terriNb].addLocation(stringNumber[location]);
    if (territoryOwnerColor[terriNb] == EMPTY) {/**assert(0);**/
      if (territoryAdjacentStrNbs[terriNb].size() == 1) {/**assert(0);**/
        assert(territoryOwnerColor[terriNb] == EMPTY);
        territoryOwnerColor[terriNb] = goban->gobanState[location];
      }
    } else if (territoryOwnerColor[terriNb] != goban->gobanState[location]) {/**assert(0);**/
      assert((territoryOwnerColor[terriNb] == BLACK && goban->gobanState[location] == WHITE) ||
             (territoryOwnerColor[terriNb] == WHITE && goban->gobanState[location] == BLACK));
      territoryOwnerColor[terriNb] = EMPTY;
    }
    return;
  }
  return;

}

void MoGo::CFGString::showTerritory(const Goban * goban) const {/**assert(0);**/
  if ((int)territoryNumber.size() != (int)stringNumber.size()) {/**assert(0);**/
    GoTools::print("territory not calculated.\n");
    return;
  }
  int HorSideWidth=6;
  //textModeShowPosition(location);
  GoTools::print("\n");
  int mode = 1;

  for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
  for(int i=0;i<goban->widthOfGoban;i++)
    if (i>=8)
      GoTools::print(" %c ",(char)('A'+i+1));
    else
      GoTools::print(" %c ",(char)('A'+i));
  if (mode==1) {/**assert(0);**/
    GoTools::print(" ");
    for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
    for(int i=0;i<goban->widthOfGoban;i++)
      if (i>=8)
        GoTools::print(" %c ",(char)('A'+i+1));
      else
        GoTools::print(" %c ",(char)('A'+i));
  }
  GoTools::print("\n");
  for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
  for(int i=0;i<goban->widthOfGoban;i++) GoTools::print("---"); GoTools::print("+");
  if (mode==1) {/**assert(0);**/
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<goban->widthOfGoban;i++) GoTools::print("---"); GoTools::print("+");
  }
  for(int i=0;i<goban->heightOfGoban;i++) {/**assert(0);**/
    GoTools::print("\n%2d",goban->heightOfGoban-i);
    //if (i<9) GoTools::print(" ");
    for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
    GoTools::print("|");
    for(int j=0;j<goban->widthOfGoban;j++) {/**assert(0);**/
      int llocation=goban->indexOnArray(i,j);



      if (territoryNumber[llocation] >= 1)
        GoTools::print("[");
      else
        GoTools::print(" ");

      /////////////
      if (goban->gobanState[llocation]==WHITE) GoTools::print("O");
      else if (goban->gobanState[llocation]==BLACK) GoTools::print("@");
      else if (goban->gobanState[llocation]==EMPTY)
        GoTools::print(" ");
      else if (goban->gobanState[llocation] == OUTSIDE) //now possible with location simulation!
        GoTools::print("#");
      ///////////
      if (territoryNumber[llocation] >= 1)
        GoTools::print("]");
      else
        GoTools::print(" ");

    }
    GoTools::print("| ");

    if (mode==1) {/**assert(0);**/
      for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
      GoTools::print(" |");
      for(int j=0;j<goban->widthOfGoban;j++) {/**assert(0);**/
        int llocation=goban->indexOnArray(i,j);
        //GoTools::print("[%d %d %d]",showGroupNumber,isCFGOn,cfg->isGroupUpToDate());

        if (territoryNumber[llocation]<100) GoTools::print(" ");
        GoTools::print("%d",territoryNumber[llocation]);
        if (territoryNumber[llocation]<10) GoTools::print(" ");

      }
      GoTools::print("|");
    }
  }
  GoTools::print("\n");
  for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
  for(int i=0;i<goban->widthOfGoban;i++) GoTools::print("---");
  GoTools::print("+");

  if (mode==1) {/**assert(0);**/
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<goban->widthOfGoban;i++) GoTools::print("---");
    GoTools::print("+");
  }
  GoTools::print("\n");


}

void MoGo::CFGString::showTerritoryInformation( const Goban * goban ) const {/**assert(0);**/
  for (int i=1;i<=maxTerritoryNumber;i++)
    if (territoryOwnerColor[i] != EMPTY) {/**assert(0);**/
      GoTools::print("Territory %d ,",i);
      if (territoryOwnerColor[i] == BLACK)
        GoTools::print("BLACK");
      else
        GoTools::print("WHITE");
      GoTools::print(", size %d :",territoryLocations[i].size());
      for (int j=0;j<(int)territoryLocations[i].size();j++)
        goban->textModeShowPosition(territoryLocations[i][j]);

      GoTools::print("\n%d adjacent strings :",territoryAdjacentStrNbs[i].size());
      for (int j=0;j<(int)territoryAdjacentStrNbs[i].size();j++)
        GoTools::print("%d ",territoryAdjacentStrNbs[i][j]);
      GoTools::print("\n");
    }
}











































MoGo::CFGFreeLocation::CFGFreeLocation( ) {/**assert(0);**/}

MoGo::CFGFreeLocation::~ CFGFreeLocation( ) {/**assert(0);**/}









































////////////////////
// class CFGGroup //
////////////////////


MoGo::CFGGroup::CFGGroup( int distance1, int distance2 ) {/**assert(0);**/
  this->distance1 = distance1;
  this->distance2 = distance2;
  isUpToDate = false;
  maxNumberOfGroup = 0;
}

MoGo::CFGGroup::~ CFGGroup( ) {/**assert(0);**/}

void MoGo::CFGGroup::initiate( int size ) {/**assert(0);**/
  groupNumber.resize(size);
  clear();
}


void MoGo::CFGGroup::clear() {/**assert(0);**/
  maxNumberOfGroup = 0;
  //adjacentGrNbs.clear();
  liberties.clear();liberties.push_back(LocationList());
  strNbs.clear();strNbs.push_back(LocationList());
  adjacentEnemyStrNbs.clear();adjacentEnemyStrNbs.push_back(LocationList());
  for (int i=0;i<(int)groupNumber.size();i++)
    groupNumber[i] = 0;
  isUpToDate = true;

}

void MoGo::CFGGroup::calculateAllGroupsFromCFGString( const Vector < Location > & gobanState, const CFGString & stringCFG ) {/**assert(0);**/
  clear();
  for (int i=1;i<=stringCFG.maxNumberOfString;i++)
    if (stringCFG.stones[i].size() &&
        groupNumber[stringCFG.stones[i][0]] == 0)
      calculateOneGroupFromCFGString(gobanState,stringCFG,i);
  isUpToDate = true;
}

void MoGo::CFGGroup::calculateOneGroupFromCFGString( const GobanState &gobanState, const CFGString & stringCFG, const StringNumber strNb ) {/**assert(0);**/
  maxNumberOfGroup++;
  //adjacentGrNbs.push_back(LocationList());
  strNbs.push_back(LocationList());
  liberties.push_back(LocationList());
  adjacentEnemyStrNbs.push_back(LocationList());

  addStringToGroup(stringCFG,strNb,maxNumberOfGroup);
  StoneColor groupColor = gobanState[stringCFG.stones[strNb][0]];
  if (groupColor == 0) {/**assert(0);**/
    stringCFG.printInformation();
    printInformation();
    GobanLocation::textModeShowPosition(stringCFG.stones[strNb][0]);
  }
  assert(groupColor == BLACK || groupColor == WHITE);
  Bitmap bitmap,tmpBitmap;
  bool anotherLoop = true;
  //GoTools::print("Group Color %d\n",groupColor);
  while(anotherLoop) {/**assert(0);**/

    anotherLoop = false;
    //printInformation();
    GobanBitmap::clear(bitmap);
    addGroupStringsToBitmap(stringCFG,maxNumberOfGroup,bitmap);

    for (int d=0;d<distance1;d++) {/**assert(0);**/
      tmpBitmap = bitmap;
      GobanBitmap::dilation(bitmap);
      GobanBitmap::logicalXOR(tmpBitmap,bitmap);
      for (int i=0;i<GobanLocation::height;i++)
        for (int j=0;j<GobanLocation::width;j++)
          if (GobanBitmap::getBitValue(tmpBitmap,i,j)) {/**assert(0);**/

            Location location = GobanLocation::locationXY(i,j);
            if (gobanState[location] == groupColor) {/**assert(0);**/
              if (groupNumber[location] == 0) {/**assert(0);**/
                assert(stringCFG.stringNumber[location]);
                addStringToGroup(stringCFG,stringCFG.stringNumber[location],maxNumberOfGroup);
                anotherLoop = true;
              }
            } else if (gobanState[location] == 3-groupColor)
              GobanBitmap::setBitValue(bitmap,i,j,false);
          }
    }
  }

  GobanBitmap::clear(bitmap);
  addGroupStringsToBitmap(stringCFG,maxNumberOfGroup,bitmap);

  for (int d=0;d<distance2;d++) {/**assert(0);**/
    tmpBitmap = bitmap;
    GobanBitmap::dilation(bitmap);
    GobanBitmap::logicalXOR(tmpBitmap,bitmap);

    for (int i=0;i<GobanLocation::height;i++)
      for (int j=0;j<GobanLocation::width;j++)
        if (GobanBitmap::getBitValue(tmpBitmap,i,j)) {/**assert(0);**/
          Location location = GobanLocation::locationXY(i,j);
          if (gobanState[location] == 3-groupColor) {/**assert(0);**/
            GobanBitmap::setBitValue(bitmap,i,j,false);
            adjacentEnemyStrNbs[maxNumberOfGroup].addLocation(stringCFG.stringNumber[location]);
          }
        }
  }

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::CFGGroup::setGroupDistance(int distance1, int distance2) {
  MoGo::CFGGroup::distance1 = distance1;
  MoGo::CFGGroup::distance2 = distance2;
  if (distance1 < 2)
    GoTools::print("group distance warning! distance1 = %d\n",distance1);
}

void MoGo::CFGGroup::addStringToGroup(const CFGString & stringCFG, const StringNumber strNb, const GroupNumber grNb) {/**assert(0);**/
  if (!strNb)
    printInformation();
  assert(strNb);
  if (strNbs[grNb].isLocationIn(strNb))
    printInformation();

  assert(!strNbs[grNb].isLocationIn(strNb));
  strNbs[grNb].addLocation(strNb);
  for (int i=0;i<(int)stringCFG.liberties[strNb].size();i++)
    liberties[grNb].addLocation(stringCFG.liberties[strNb][i]);
  for (int i=0;i<(int)stringCFG.stones[strNb].size();i++)
    groupNumber[stringCFG.stones[strNb][i]] = grNb;
}

void MoGo::CFGGroup::printInformation() const {/**assert(0);**/
  if (!isUpToDate)
    GoTools::print("Warning! CFGGroup is not up to date.(%d)\n",maxNumberOfGroup);
  //textModeShowGoban();
  for ( int i=1;i<=maxNumberOfGroup;i++)
    if (strNbs[i].size() ) {/**assert(0);**/
      GoTools::print("group number %d: %d string numbers: ",i,strNbs[i].size());
      for (int j=0;j<(int)strNbs[i].size();j++)
        GoTools::print("%d ",strNbs[i][j]);

      GoTools::print("|| %d liberties: ",liberties[i].size());
      for (int j=0;j<(int)liberties[i].size();j++)
        GobanLocation::textModeShowPosition(liberties[i][j]);

      GoTools::print("|| %d adjacentEnemyStrNbs: ",adjacentEnemyStrNbs[i].size());
      for (int j=0;j<(int)adjacentEnemyStrNbs[i].size();j++)
        GoTools::print("%d ",adjacentEnemyStrNbs[i][j]);
      GoTools::print("\n");

    }
  if (strNbs[0].size()) {/**assert(0);**/
    GoTools::print("Warning, strNbs[0] not empty! ");
    for (int i=0;i<(int)strNbs[0].size();i++)
      GoTools::print("%d ",strNbs[0][i]);
    GoTools::print("\n");
  }
  if (liberties[0].size()) {/**assert(0);**/
    GoTools::print("Warning, liberties[0] not empty! ");
    for (int i=0;i<(int)liberties[0].size();i++)
      GobanLocation::textModeShowPosition(liberties[0][i]);
    GoTools::print("\n");

  }
  if (adjacentEnemyStrNbs[0].size()) {/**assert(0);**/
    GoTools::print("Warning, adjacentEnemyStrNbs[0] not empty! ");
    for (int i=0;i<(int)adjacentEnemyStrNbs[0].size();i++)
      GoTools::print("%d ",adjacentEnemyStrNbs[0][i]);
    GoTools::print("\n");
  }

}

void MoGo::CFGGroup::addGroupStringsToBitmap( const CFGString & stringCFG, const GroupNumber grNb, Bitmap & bitmap ) const {/**assert(0);**/
  for (int i=0;i<(int)strNbs[grNb].size();i++)
    stringCFG.addStringToBitmap(strNbs[grNb][i],bitmap);
}







































///////////////
// class CFG //
///////////////


MoGo::CFG::CFG( Goban * goban ) {/**assert(0);**/
  this->goban = goban;
  if (Goban::getCFGGlobal() == 0) return;
  initiate();
}

MoGo::CFG::CFG( ) {/**assert(0);**/
  this->goban = 0;
  if (Goban::getCFGGlobal() == 0) return;
}


MoGo::CFG::~ CFG( ) {/**assert(0);**/}

void MoGo::CFG::initiate( ) {/**assert(0);**/
  groupUpToDateMode = true;//FIXME GROUP
  int size = (goban->heightOfGoban+2)*(goban->widthOfGoban+2);
  stringCFG.initiate(size);
  groupCFG.initiate(size);
}

void MoGo::CFG::clear( ) {/**assert(0);**/
  stringCFG.clear();
}
void MoGo::CFG::getRelatedRange( const Location & /*location*/, Vector< Location > & /*CFGRelatedRange*/ ) const {/**assert(0);**/}

void MoGo::CFG::playMoveOnGoban( const Location & location ) {/**assert(0);**/
  stringCFG.playMoveOnGoban(goban,location);
  goban->gobanState[location] = goban->playerColor;
  if (groupUpToDateMode) {/**assert(0);**/
    // goban->textModeShowGoban();
    groupCFG.isUpToDate = false;
  }
}

void MoGo::CFG::printCFGInformation( ) const {/**assert(0);**/
  stringCFG.printInformation();
  groupCFG.printInformation();
}

StringNumber MoGo::CFG::getStringNumber( const Location & location ) const {/**assert(0);**/
  return stringCFG.stringNumber[location];
}

StringNumber MoGo::CFG::getMaxStringNumber( ) const {/**assert(0);**/
  return stringCFG.liberties.size();
}

GroupNumber MoGo::CFG::getGroupNumber(const Location & location) const {/**assert(0);**/
  return groupCFG.groupNumber[location];
}

int MoGo::CFG::getStringLength( const StringNumber & stringNumber ) const {/**assert(0);**/
  return stringCFG.stones[stringNumber].size();
}

int MoGo::CFG::getStringLiberty( const StringNumber & stringNumber ) const {/**assert(0);**/
  return stringCFG.liberties[stringNumber].size();
}

void MoGo::CFG::textModeShowGoban(const Location location, bool showGroupNumber ) const {/**assert(0);**/

  int HorSideWidth=6;
  //textModeShowPosition(location);
  GoTools::print("\n");
  int mode;
  if (goban->widthOfGoban>9) mode=0;
  else mode=1;

  for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
  for(int i=0;i<goban->widthOfGoban;i++)
    if (i>=8)
      GoTools::print(" %c ",(char)('A'+i+1));
    else
      GoTools::print(" %c ",(char)('A'+i));
  if (mode==1) {/**assert(0);**/
    GoTools::print(" ");
    for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
    for(int i=0;i<goban->widthOfGoban;i++)
      if (i>=8)
        GoTools::print(" %c ",(char)('A'+i+1));
      else
        GoTools::print(" %c ",(char)('A'+i));
  }
  GoTools::print("\n");
  for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
  for(int i=0;i<goban->widthOfGoban;i++) GoTools::print("---"); GoTools::print("+");
  if (mode==1) {/**assert(0);**/
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<goban->widthOfGoban;i++) GoTools::print("---"); GoTools::print("+");
  }
  for(int i=0;i<goban->heightOfGoban;i++) {/**assert(0);**/
    GoTools::print("\n%2d",goban->heightOfGoban-i);
    //if (i<9) GoTools::print(" ");
    for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
    GoTools::print("|");
    for(int j=0;j<goban->widthOfGoban;j++) {/**assert(0);**/
      int llocation=goban->indexOnArray(i,j);
  
      if (location==llocation) {/**assert(0);**/
        GoTools::print("(");
        if (goban->gobanState[llocation]==WHITE) GoTools::print("O");
        else if (goban->gobanState[llocation]==BLACK) GoTools::print("@");
        else {/**assert(0);**/
          GoTools::print("There is no stone at the given position!\n");
          assert(0);
        }

        GoTools::print(")");
      } else //Showing location different from the present move

        if (goban->gobanState[llocation]==EMPTY)
          GoTools::print(" . ");
        else {/**assert(0);**/
          GoTools::print(" ");
          if (goban->gobanState[llocation]==WHITE) GoTools::print("O");
          else GoTools::print("@");
          GoTools::print(" ");
        }

    }
    GoTools::print("| ");

    if (mode==1) {/**assert(0);**/
      if (i<9) GoTools::print(" ");
      for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
      GoTools::print("|");
      for(int j=0;j<goban->widthOfGoban;j++) {/**assert(0);**/
        int llocation=goban->indexOnArray(i,j);
        if (showGroupNumber &&
            groupCFG.isUpToDate) {/**assert(0);**/
          if (groupCFG.groupNumber[llocation]<100) GoTools::print(" ");
          GoTools::print("%d",groupCFG.groupNumber[llocation]);
          if (groupCFG.groupNumber[llocation]<10) GoTools::print(" ");
        } else {/**assert(0);**/
          if (stringCFG.stringNumber[llocation]<100) GoTools::print(" ");
          GoTools::print("%d",stringCFG.stringNumber[llocation]);
          if (stringCFG.stringNumber[llocation]<10) GoTools::print(" ");
        }
      }
      GoTools::print("|");
    }
    if (i == goban->heightOfGoban/2-1)
      GoTools::print("  Move: %d",goban->moveNumber_+1);
    else if (i==goban->heightOfGoban/2) {/**assert(0);**/
      if (goban->isBlacksTurn()) GoTools::print("  Black");
      else GoTools::print("  White");
      GoTools::print(" to play");
    } else if (i==goban->heightOfGoban/2+1)
      GoTools::print("  Black eaten stone(s): %d",goban->blackEatenStoneNumber);
    else if (i==goban->heightOfGoban/2+2)
      GoTools::print("  White eaten stone(s): %d",goban->whiteEatenStoneNumber);
  }
  GoTools::print("\n");
  for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
  for(int i=0;i<goban->widthOfGoban;i++) GoTools::print("---");
  GoTools::print("+");

  if (mode==1) {/**assert(0);**/
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<goban->widthOfGoban;i++) GoTools::print("---");
    GoTools::print("+");
  }
  GoTools::print("\n");

}


void MoGo::CFG::resetStringPreview( ) {/**assert(0);**/
  stringCFG.resetStringPreview();
}

void MoGo::CFG::calculateAllFromGobanState( Vector < Location > & gobanState ) {/**assert(0);**/
  stringCFG.calculateAllFromGobanState(goban,gobanState);
  return;

}

int MoGo::CFG::basicTest( const Location location ) const {/**assert(0);**/
  return stringCFG.basicTest(goban,location);
}
int MoGo::CFG::uselessnessTest( const Location location) const {/**assert(0);**/
  return stringCFG.uselessnessTest(goban,location);
}
bool MoGo::CFG::stringPreview(const Location location, const Vector<Location> &gobanState, const Vector<int> &directionsArray, const int &playerColor) const {/**assert(0);**/
  return stringCFG.stringPreview(location,gobanState,directionsArray,playerColor);
}

void MoGo::CFG::freeLocationUpdateAfterPlayMoveOnGoban( const Location location, GobanLocationLists &gobanLists, LocationList *locationListIncludingThisLocation ) const {/**assert(0);**/
  stringCFG.freeLocationUpdateAfterPlayMoveOnGoban(goban,location,gobanLists,locationListIncludingThisLocation);
}
void MoGo::CFG::selfKillLocationUpdate( Goban & goban , const Vector < Location > &gobanState, GobanLocationLists &gobanLists, const StoneColor & playerColor, Vector < int > &directionsArray ) const {/**assert(0);**/
  return stringCFG.selfKillLocationUpdate(goban,gobanState,gobanLists,playerColor,directionsArray);
}
void MoGo::CFG::specialLocationUpdate( const Location location, const Vector < Location > & tmp, GobanLocationLists &gobanLists ) const {/**assert(0);**/
  stringCFG.specialLocationUpdate(goban,location,tmp,gobanLists);
}
void MoGo::CFG::specialLocationUpdateForOneLibertyString( const StringNumber strNb, const StoneColor stringColor, GobanLocationLists &gobanLists ) const {/**assert(0);**/
  stringCFG.specialLocationUpdateForOneLibertyString(goban,strNb,stringColor,gobanLists);
}
void MoGo::CFG::updateEatingMove( const Location location, GobanLocationLists &gobanLists) const {/**assert(0);**/
  stringCFG.updateEatingMove(goban,location,gobanLists);
}






void MoGo::CFG::showOneLibertyStringsInGTPMode( ) const {/**assert(0);**/
  stringCFG.showOneLibertyStringsInGTPMode(goban);
}





void MoGo::CFG::getNewLocationsByDilation(Bitmap &bitmap1, const Bitmap &bitmap2, int &dilationTimes, Vector < Location > newLocations) {/**assert(0);**/
  for (int i=0;i<dilationTimes;i++)
    GobanBitmap::dilation(bitmap1);
  GobanBitmap::logicalXOR(bitmap1,bitmap2);
  goban->getLocationsFromBitmap(bitmap1,newLocations);
}

void MoGo::CFG::blockUpdateAddAdjacentStrNbs( LocationList &blockStrNbs, const StringNumber strNb, Bitmap & tmpBitmap, bool bitmapUpdate ) const {/**assert(0);**/
  if (blockStrNbs.isLocationIn(strNb)) return;
  blockStrNbs.addLocation(strNb);
  if (bitmapUpdate) {/**assert(0);**/
    int x = 0,y = 0;
    for (int j=0;j<(int)stringCFG.stones[strNb].size();j++) {/**assert(0);**/
      goban->getXYIndice(stringCFG.stones[strNb][j],x,y);
      MoGo::GobanBitmap::setBitValue(tmpBitmap,x,y);
    }
  }
  for (int i=0;i<(int)stringCFG.adjacentStrNbs[strNb].size();i++) {/**assert(0);**/
    StringNumber adjacentStrNb = stringCFG.adjacentStrNbs[strNb][i];
    blockUpdateAddAdjacentStrNbs(blockStrNbs,adjacentStrNb,tmpBitmap,bitmapUpdate);
  }
}

void MoGo::CFG::blockUpdateAddLiberties( LocationList & blockFreeLocations, const StringNumber strNb, Bitmap & , bool ) const {/**assert(0);**/
  for (int i=0;i<(int)stringCFG.liberties[strNb].size();i++)
    blockFreeLocations.addLocation(stringCFG.liberties[strNb][i]);
}

void MoGo::CFG::addOneMoveToBlock( const Location & location, const int & distance1, const int & distance2, Bitmap & bitmap, LocationList * blockStrNbs, LocationList * blockFreeLocations ) const {/**assert(0);**/
  if (location == PASSMOVE) return;
  int x = 0, y = 0;
  x = GobanLocation::xLocation(location);
  y = GobanLocation::yLocation(location);
  addOneMoveToBlock(location,x,y,distance1,distance2,bitmap,blockStrNbs,blockFreeLocations);
}

void MoGo::CFG::addOneMoveToBlock( const Location & location, const int &x, const int &y, const int & distance1, const int & distance2, Bitmap & bitmap, LocationList * blockStrNbs, LocationList * blockFreeLocations ) const {/**assert(0);**/
  bool blockStrNbsGiven = true;
  if (blockStrNbs == 0) {/**assert(0);**/
    blockStrNbs = new LocationList();
    blockStrNbsGiven = false;
  }// else blockStrNbs->deleteLocation(stringNumber[location]);
  //assert(GobanBitmap::getBitValue(bitmap,x,y)); //FIXME BLOCK for the tree later. Normaly we suppose that this new move is in the bitmap.
  //GobanBitmap::setBitValue(bitmap,x,y,false);
  //assert(!GobanBitmap::getBitValue(bitmap,x,y));
  Bitmap backupBitmap,newStringsBitmap0,newStringsBitmap;
  GobanBitmap::clear(newStringsBitmap0);
  GobanBitmap::setBitValue(newStringsBitmap0,x,y,true);
  blockUpdateAddAdjacentStrNbsForAddingOneMoveCase(*blockStrNbs,stringCFG.stringNumber[location],bitmap,newStringsBitmap0,true);

  bool anotherLoop = true;
  Vector < Location > newLocations;
  for (int loop = 0;anotherLoop;loop ++) {/**assert(0);**/
    anotherLoop = false;
    if (loop == 0) {/**assert(0);**/
      backupBitmap = newStringsBitmap0;
      for (int i=0;i<distance1+distance2;i++)
        GobanBitmap::dilation(backupBitmap);
      GobanBitmap::logicalXOR(backupBitmap,newStringsBitmap0);
    } else {/**assert(0);**/
      backupBitmap = newStringsBitmap;
      GobanBitmap::logicalOR(newStringsBitmap0,newStringsBitmap);
      for (int i=0;i<distance1+distance2;i++)
        GobanBitmap::dilation(backupBitmap);
      GobanBitmap::logicalXOR(backupBitmap,newStringsBitmap);
    }
    newLocations.clear();
    goban->getLocationsFromBitmap(backupBitmap,newLocations);
    GobanBitmap::clear(newStringsBitmap);
    for (int i=0;i<(int)newLocations.size();i++)
      if (stringCFG.stringNumber[newLocations[i]] &&
          !blockStrNbs->isLocationIn(stringCFG.stringNumber[newLocations[i]])) {/**assert(0);**/
        blockUpdateAddAdjacentStrNbsForAddingOneMoveCase(*blockStrNbs,stringCFG.stringNumber[newLocations[i]],bitmap,newStringsBitmap0,newStringsBitmap,true);
        anotherLoop = true;
      }
  }
  if (blockFreeLocations)
    backupBitmap = newStringsBitmap0;
  for (int i=0;i<distance1;i++)
    GobanBitmap::dilation(newStringsBitmap0);
  GobanBitmap::logicalOR(bitmap,newStringsBitmap0);
  if (blockFreeLocations) {/**assert(0);**/
    newLocations.clear();
    GobanBitmap::logicalXOR(backupBitmap,newStringsBitmap0);
    goban->getLocationsFromBitmap(backupBitmap,newLocations);
    for (int i=0;i<(int)newLocations.size();i++)
      if (stringCFG.stringNumber[newLocations[i]] == 0)
        blockFreeLocations->addLocation(newLocations[i],true);
  }
  if (!blockStrNbsGiven) delete blockStrNbs;
}


void MoGo::CFG::blockUpdateAddAdjacentStrNbsForAddingOneMoveCase( LocationList &blockStrNbs, const StringNumber strNb, Bitmap &bitmap, Bitmap &tmpBitmap, bool bitmapUpdate ) const {/**assert(0);**/
  if (blockStrNbs.isLocationIn(strNb)) return;
  int x = 0, y = 0;
  x = GobanLocation::xLocation(stringCFG.stones[strNb][0]);
  y = GobanLocation::yLocation(stringCFG.stones[strNb][0]);
  if (GobanBitmap::getBitValue(bitmap,x,y)) return;
  //blockStrNbs.addLocation(strNb);//FIXME BLOCK here i dont care for the moment ths stringNumber. I feel that it is probably useless in MoGo. the bitmap gives everything, fast.
  if (bitmapUpdate)
    for (int j=0;j<(int)stringCFG.stones[strNb].size();j++) {/**assert(0);**/
      x = GobanLocation::xLocation(stringCFG.stones[strNb][j]);
      y = GobanLocation::yLocation(stringCFG.stones[strNb][j]);
      GoTools::print("%d %d\n",x,y);
      MoGo::GobanBitmap::setBitValue(tmpBitmap,x,y);
      MoGo::GobanBitmap::setBitValue(bitmap,x,y);
    }
  for (int i=0;i<(int)stringCFG.adjacentStrNbs[strNb].size();i++) {/**assert(0);**/
    StringNumber adjacentStrNb = stringCFG.adjacentStrNbs[strNb][i];
    blockUpdateAddAdjacentStrNbsForAddingOneMoveCase(blockStrNbs,adjacentStrNb,bitmap,tmpBitmap,bitmapUpdate);
  }
}

void MoGo::CFG::blockUpdateAddAdjacentStrNbsForAddingOneMoveCase( LocationList &blockStrNbs, const StringNumber strNb, Bitmap &bitmap, Bitmap &bitmap0, Bitmap &tmpBitmap, bool bitmapUpdate ) const {/**assert(0);**/
  if (blockStrNbs.isLocationIn(strNb)) return;
  int x = 0, y = 0;
  x = GobanLocation::xLocation(stringCFG.stones[strNb][0]);
  y = GobanLocation::yLocation(stringCFG.stones[strNb][0]);
  if (GobanBitmap::getBitValue(bitmap,x,y)) return;
  //blockStrNbs.addLocation(strNb);//FIXME BLOCK here i dont care for the moment ths stringNumber. I feel that it is probably useless in MoGo. the bitmap gives everything, fast.
  if (bitmapUpdate)
    for (int j=0;j<(int)stringCFG.stones[strNb].size();j++) {/**assert(0);**/
      x = GobanLocation::xLocation(stringCFG.stones[strNb][j]);
      y = GobanLocation::yLocation(stringCFG.stones[strNb][j]);
      MoGo::GobanBitmap::setBitValue(tmpBitmap,x,y);
      MoGo::GobanBitmap::setBitValue(bitmap,x,y);
      MoGo::GobanBitmap::setBitValue(bitmap0,x,y);
    }
  for (int i=0;i<(int)stringCFG.adjacentStrNbs[strNb].size();i++) {/**assert(0);**/
    StringNumber adjacentStrNb = stringCFG.adjacentStrNbs[strNb][i];
    blockUpdateAddAdjacentStrNbsForAddingOneMoveCase(blockStrNbs,adjacentStrNb,bitmap,bitmap0,tmpBitmap,bitmapUpdate);
  }
}

void MoGo::CFG::setGroupDistance(int distance1, int distance2) const {/**assert(0);**/
  groupCFG.setGroupDistance(distance1,distance2);
}

void MoGo::CFG::groupUpdateAll( ) const {/**assert(0);**/
  groupCFG.calculateAllGroupsFromCFGString(goban->gobanState,stringCFG);
}

int MoGo::CFG::getGroupBlock( const Location & location, const int & distance1, const int &, Bitmap & bitmap, int zoneFreeLocationSize ) const {/**assert(0);**/
  assert(location!=PASSMOVE);
  if (!groupCFG.isUpToDate) {/**assert(0);**/
    groupCFG.calculateAllGroupsFromCFGString(goban->gobanState,stringCFG);
    groupCFG.isUpToDate = true;
  }
  GobanBitmap::clear(bitmap);
  GroupNumber grNb = groupCFG.groupNumber[location];
  if (!grNb) {/**assert(0);**/
    goban->textModeShowGoban(PASSMOVE);
    printCFGInformation();
    GobanLocation::textModeShowPosition(location);
    GoTools::print("%d %d\n",grNb,stringCFG.stringNumber[location]);
  }
  assert(grNb);
  //GoTools::print("%d",grNb);
  groupCFG.addGroupStringsToBitmap(stringCFG,grNb,bitmap);
  LocationList grNbs;
  grNbs.addLocation(grNb);
  //   for (int i=0;i<(int)grNbs.size();i++)
  //     GoTools::print("%d ",grNbs[i]);
  bool newGroupAdded = true;
  Bitmap tmpBitmap,tmpBitmap1;
  int freeLocationSize = 0;
  while(newGroupAdded) {/**assert(0);**///bitmap saves the groups(stones).
    //GoTools::print("?\n");
    newGroupAdded = false;
    int size = (int)grNbs.size();
    for (int i=0;i<size;i++) {/**assert(0);**/
      for (int j=0;j<(int)groupCFG.adjacentEnemyStrNbs[grNbs[i]].size();j++) {/**assert(0);**/
        //GoTools::print("(adESN %d)\n",groupCFG.adjacentEnemyStrNbs[grNbs[i]][j]);
        GroupNumber tmpGrNb = groupCFG.groupNumber[stringCFG.stones[groupCFG.adjacentEnemyStrNbs[grNbs[i]][j]][0]];
        //GoTools::print("!\n");
        if (!grNbs.isLocationIn(tmpGrNb)) {/**assert(0);**/
          grNbs.addLocation(tmpGrNb,true);
          groupCFG.addGroupStringsToBitmap(stringCFG,tmpGrNb,bitmap);
          newGroupAdded = true;
          //GoTools::print("hello\n");
        }
      }
    }

    //     for (int i=0;i<(int)grNbs.size();i++)
    //       GoTools::print("%d ",grNbs[i]);
    tmpBitmap = bitmap; // tmpBitmap saves the groups and the freeLocations.
    freeLocationSize = 0;
    for (int d=0;d<distance1;d++) {/**assert(0);**/
      tmpBitmap1 = tmpBitmap;
      GobanBitmap::dilation(tmpBitmap1);
      GobanBitmap::logicalXOR(tmpBitmap1,tmpBitmap);
      for (int i=0;i<GobanLocation::height;i++)
        for (int j=0;j<GobanLocation::width;j++)
          if (GobanBitmap::getBitValue(tmpBitmap1,i,j))
            if (groupCFG.groupNumber[GobanLocation::locationXY(i,j)] == 0)
              freeLocationSize++;
            else if (!grNbs.isLocationIn(groupCFG.groupNumber[GobanLocation::locationXY(i,j)]))
              GobanBitmap::setBitValue(tmpBitmap1,i,j,false);
      GobanBitmap::logicalOR(tmpBitmap,tmpBitmap1);
    }
    //GoTools::print("freeLocationSize = %d\n",freeLocationSize);
    if (freeLocationSize>zoneFreeLocationSize)
      break;

  }
  bitmap = tmpBitmap;

  return freeLocationSize;
}







void MoGo::CFG::rangeBorderModifier( Bitmap & rangeBorderBitmap ) const {/**assert(0);**/
  int boardDistance = 2;
  for (int i=0;i<boardDistance;i++) {/**assert(0);**/
    for (int j=0;j<GobanLocation::width;j++) {/**assert(0);**/
      if (GobanBitmap::getBitValue(rangeBorderBitmap,i,j) &&
          stringCFG.stringNumber[GobanLocation::locationXY(i,j)] == 0)
        GobanBitmap::setBitValue(rangeBorderBitmap,i,j,false);
      if (GobanBitmap::getBitValue(rangeBorderBitmap,GobanLocation::height-1-i,j) &&
          stringCFG.stringNumber[GobanLocation::locationXY(GobanLocation::height-1-i,j)] == 0)
        GobanBitmap::setBitValue(rangeBorderBitmap,GobanLocation::height-1-i,j,false);
    }
  }

  for (int i=0;i<boardDistance;i++) {/**assert(0);**/
    for (int j=0;j<GobanLocation::height;j++) {/**assert(0);**/
      if (GobanBitmap::getBitValue(rangeBorderBitmap,j,i) &&
          stringCFG.stringNumber[GobanLocation::locationXY(j,i)] == 0)
        GobanBitmap::setBitValue(rangeBorderBitmap,j,i,false);
      if (GobanBitmap::getBitValue(rangeBorderBitmap,j,GobanLocation::width-1-i) &&
          stringCFG.stringNumber[GobanLocation::locationXY(j,GobanLocation::height-1-i)] == 0)
        GobanBitmap::setBitValue(rangeBorderBitmap,j,GobanLocation::width-1-i,false);
    }
  }


}


void MoGo::CFG::getLocalTerritoryBitmap( const Location location, Bitmap & bitmap, int distance ) const {/**assert(0);**/
  GobanBitmap::clear(bitmap);

  for (int i=0;i<8;i++) {/**assert(0);**/
    Location tmpLocation = location+goban->direction(i);
    if (goban->gobanState[tmpLocation] == OUTSIDE) continue;
    if (stringCFG.stringNumber[tmpLocation]) {/**assert(0);**/
      stringCFG.addStringToBitmap(stringCFG.stringNumber[tmpLocation],bitmap);
      stringCFG.addStringLibertiesToBitmap(stringCFG.stringNumber[tmpLocation],bitmap);
    } else
      GobanBitmap::setBitValue(bitmap,tmpLocation);
  }
  Bitmap tmpBitmap, newBorder = bitmap;
  Vector < Location > newBorderLocations;
  for (int d = 0;d<distance;d++) {/**assert(0);**/
    tmpBitmap = newBorder;
    //GoTools::print("before dilation:");
    //GobanBitmap::showBitmap(newBorder);
    //GobanBitmap::printBitmap(newBorder);
    //GobanBitmap::showBitmap(tmpBitmap);
    //GobanBitmap::showBitmap(bitmap);
    GobanBitmap::dilation(newBorder);
    //GoTools::print("dilation:");
    //GobanBitmap::showBitmap(newBorder);
    //GobanBitmap::printBitmap(newBorder);
    //GobanBitmap::showBitmap(tmpBitmap);

    GobanBitmap::logicalXOR(newBorder,tmpBitmap);
    //GobanBitmap::showBitmap(bitmap);
    //GobanBitmap::showBitmap(newBorder);

    newBorderLocations.clear();
    for (int i=0;i<GobanLocation::height;i++)
      if (newBorder[i])
        for (int j=0;j<GobanLocation::width;j++)
          if (GobanBitmap::getBitValue(newBorder,i,j))
            if (GobanBitmap::getBitValue(bitmap,i,j))
              GobanBitmap::setBitValue(newBorder,i,j,false);
            else if (stringCFG.stringNumber[GobanLocation::locationXY(i,j)])
              if (isStringDependentOfTerritory(stringCFG.stringNumber[GobanLocation::locationXY(i,j)],bitmap,distance-d)) {/**assert(0);**/
                stringCFG.addStringToBitmap(stringCFG.stringNumber[GobanLocation::locationXY(i,j)],bitmap);
                //newBorderLocations.push_back(GobanLocation::locationXY(i,j));
              } else
                GobanBitmap::setBitValue(newBorder,i,j,false);
            else {/**assert(0);**/
              newBorderLocations.push_back(GobanLocation::locationXY(i,j));
            }

    for (int i=0;i<(int)newBorderLocations.size();i++)
      if(!isExtendableBorder(bitmap,newBorder,newBorderLocations[i],distance-d)) {/**assert(0);**/
        //GoTools::print("!!!");GobanLocation::textModeShowPosition(newBorderLocations[i]);
        GobanBitmap::setBitValue(newBorder,newBorderLocations[i],false);
      }
    //GoTools::print("after dilation:\n");
    //GobanBitmap::showBitmap(newBorder);
    //GobanBitmap::showBitmap(bitmap);
    GobanBitmap::logicalOR(bitmap,newBorder);
  }
}



bool MoGo::CFG::isStringDependentOfTerritory(const StringNumber & strNb, Bitmap & bitmap, int distanceLeft) const {/**assert(0);**/
  if ((int)stringCFG.stones[strNb].size() <= distanceLeft-2) return true;
  int libertyNotInTerritory = 0;
  for (int i=0;i<(int)stringCFG.liberties[strNb].size();i++)
    if (!GobanBitmap::getBitValue(bitmap,stringCFG.liberties[strNb][i])) libertyNotInTerritory++;
  if (libertyNotInTerritory>=2) return false;
  return true;
}

void MoGo::CFG::getGlobalTerritoryBitmap( Vector< Bitmap > & bitmaps, int distance ) const {/**assert(0);**/

  int height = goban->height()-1;
  int width = goban->width()-1;
  Vector < Location > locations;
  //locations.resize(9);
  locations.push_back(GobanLocation::locationXY(0,0));
  //locations.push_back(GobanLocation::locationXY(0,width/2));
  locations.push_back(GobanLocation::locationXY(0,width));
  //locations.push_back(GobanLocation::locationXY(height/2,0));
  locations.push_back(GobanLocation::locationXY(height/2,width/2));
  //locations.push_back(GobanLocation::locationXY(height/2,width));
  locations.push_back(GobanLocation::locationXY(height,0));
  //locations.push_back(GobanLocation::locationXY(height,width/2));
  locations.push_back(GobanLocation::locationXY(height,width));
  bitmaps.resize(locations.size());


  for (int i=0;i<(int)bitmaps.size();i++) {/**assert(0);**/
    getLocalTerritoryBitmap(locations[i],bitmaps[i],distance);
  }

}

bool MoGo::CFG::isExtendableBorder( const Bitmap & bitmap, const Bitmap & newBorder, int x, int y, int distanceLeft ) const {/**assert(0);**/
  if (distanceLeft > 6) return true;

  assert(GobanBitmap::getBitValue(newBorder,x,y));
  int inBitmap = 0;
  int stonesOutsideBitmap = 0;
  if (x-1>=0) {/**assert(0);**/
    if (GobanBitmap::getBitValue(bitmap,x-1,y)) inBitmap++;
    else if (stringCFG.stringNumber[GobanLocation::locationXY(x-1,y)]) stonesOutsideBitmap++;
  } else stonesOutsideBitmap++;
  //else if (GobanBitmap::getBitValue(newBorder,x-1,y)) inBitmap++;
  if (y-1>=0) {/**assert(0);**/
    if (GobanBitmap::getBitValue(bitmap,x,y-1)) {/**assert(0);**/
      inBitmap++;
      if (inBitmap>=2) return true;
    } else if (stringCFG.stringNumber[GobanLocation::locationXY(x,y-1)]) stonesOutsideBitmap++;
  } else stonesOutsideBitmap++;
  //     else if (GobanBitmap::getBitValue(newBorder,x,y-1)) {/**assert(0);**/
  //       inBitmap++;
  //       if (inBitmap>=2) return true;
  //     }
  if (x+1<GobanLocation::height) {/**assert(0);**/
    if (GobanBitmap::getBitValue(bitmap,x+1,y)) {/**assert(0);**/
      inBitmap++;
      if (inBitmap>=2) return true;
    } else if (stringCFG.stringNumber[GobanLocation::locationXY(x+1,y)]) stonesOutsideBitmap++;
  } else stonesOutsideBitmap++;

  if (y+1<GobanLocation::width) {/**assert(0);**/
    if (GobanBitmap::getBitValue(bitmap,x,y+1)) {/**assert(0);**/
      inBitmap++;
      if (inBitmap>=2) return true;
    } else if (stringCFG.stringNumber[GobanLocation::locationXY(x,y+1)]) stonesOutsideBitmap++;
  } else stonesOutsideBitmap++;
 
  if (stonesOutsideBitmap>=2) return false;
  return true;

}

bool MoGo::CFG::isExtendableBorder( const Bitmap & bitmap, const Bitmap & newBorder, const Location location,int distanceLeft  ) const {/**assert(0);**/
  return isExtendableBorder(bitmap,newBorder,GobanLocation::xLocation(location),GobanLocation::yLocation(location),distanceLeft);
}





bool MoGo::CFG::isStringInterestingForBlockMode3( const StringNumber strNb ) const {/**assert(0);**/
  assert(stringCFG.stringNumber[strNb]);
  if ((int)stringCFG.stones[strNb].size()<=3 &&
      isStringIsolated(strNb)) return true;
  for (int i=0;i<(int)stringCFG.adjacentStrNbs[strNb].size();i++)
    if ((int)stringCFG.liberties[strNb].size()<(int)stringCFG.liberties[stringCFG.adjacentStrNbs[strNb][i]].size()) return true;
  return false;
}

bool MoGo::CFG::isStringIsolated( const StringNumber strNb ) const {/**assert(0);**/
  for (int i=0;i<(int)stringCFG.adjacentStrNbs[strNb].size();i++)
    if (goban->gobanState[stringCFG.stones[stringCFG.adjacentStrNbs[strNb][i]][0]] == 3- goban->gobanState[stringCFG.stones[strNb][0]]) return false;
  return true;
}




void MoGo::CFG::calculateTerritory( const Goban * goban ) const {/**assert(0);**/
  stringCFG.calculateTerritory(goban);
}

void MoGo::CFG::showTerritory( const Goban * goban ) const {/**assert(0);**/
  stringCFG.calculateTerritory(goban);
  stringCFG.showTerritory(goban);
}

void MoGo::CFG::showTerritoryInformation( const Goban * goban ) const {/**assert(0);**/
  stringCFG.showTerritoryInformation(goban);
}





