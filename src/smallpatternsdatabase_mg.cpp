#include "smallpatternsdatabase_mg.h"
#include "gotools_mg.h"
#include "yadtinterface_mg.h"
#include "goban_mg.h"

using namespace MoGo;







// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::SmallPatternsDatabase::SmallPatternsDatabase( ) {
  tree=0;
}

MoGo::SmallPatternsDatabase::~SmallPatternsDatabase( ) {/**assert(0);**/
  if (tree) delete tree;
}

bool MoGo::SmallPatternsDatabase::buildByGnugoDatabase( const char * name, int minPatternSize ) {/**assert(0);**/
  allPatterns.clear();
  Vector<Vector<int > >matrix;

  FILE *f=fopen(name,"r");

  if (!f) return false;
  while(true) {/**assert(0);**/
    std::string result;
    int wordFound=YaDTInterface::lookForLineWith("Pattern","Pattern",f,result);
    if (wordFound!=1) break;
    sscanf(result.c_str(),"%*s %s",YaDTInterface::tmpWord);

    while(true) {/**assert(0);**/
      fgets(YaDTInterface::tmpLine, 50000, f); // read empty lines
      //       GoTools::print("read line empty %s\n", YaDTInterface::tmpLine);
      char c;sscanf(YaDTInterface::tmpLine, "%c", &c);
      if (c=='#')
        continue;
      break;
    }

    if (!readPattern(f, matrix)) // convert the pattern in a matrix
      return false;
    //     GoTools::print("matrix =\n");
    //     GoTools::print(matrix);
    addPatternsFromTheMatrix(matrix, std::string(YaDTInterface::tmpWord), minPatternSize);
  }

  fclose(f);
  return true;
}



bool MoGo::SmallPatternsDatabase::matchOnePattern(const Goban &goban, Location location) const {/**assert(0);**/
  int treeAnswer=-1;
  if (tree) {/**assert(0);**/
    treeAnswer=tree->classify(goban, location);
    return treeAnswer;
  }

  for (int i=0;i<(int)allPatterns.size();i++)
    if (allPatterns[i].matchPattern(goban,location)) {/**assert(0);**/
//       assert(treeAnswer>=0);
      if (treeAnswer>=0)
        assert(treeAnswer==1);
      return true;
    }

  return false;
}



bool MoGo::SmallPatternsDatabase::readPattern( FILE * f, Vector< Vector < int > > & matrix ) {/**assert(0);**/
  matrix.clear();

  for (int i=0; ;i++) {/**assert(0);**/
    fgets(YaDTInterface::tmpLine, 50000, f);
    //     GoTools::print("read line %s\n", YaDTInterface::tmpLine);
    char c;
    sscanf(YaDTInterface::tmpLine, "%c", &c);
    //          GoTools::print("c1=%c %i\n", c,c);
    if (c==' ' || c=='\0'|| c=='\n' || c==9) break;
    matrix.push_back(Vector<int> ());
    for (int j=0; ;j++) {/**assert(0);**/
      matrix[i].push_back(c);
      sscanf(YaDTInterface::tmpLine+j+1, "%c", &c);
      //              GoTools::print("c2=%c %i\n", c,c);
      if (c==' '|| c=='\0'|| c=='\n' || c==9) break;
    }
  }
  return true;
}






MoGo::DummyPatternRepresentation::DummyPatternRepresentation( ) {/**assert(0);**/}

MoGo::DummyPatternRepresentation::~ DummyPatternRepresentation( ) {/**assert(0);**/}

bool MoGo::DummyPatternRepresentation::matchPattern( const Goban & goban, Location location ) const {/**assert(0);**/
  int x=goban.xIndex(location);
  int y=goban.yIndex(location);
  for (int i=0;i<(int)toMatch.size();i++)
    if (!matchPatternElement(goban, x, y, toMatch[i], positionsDiffX[i], positionsDiffY[i]))
      return false;
  return true;
}
bool MoGo::DummyPatternRepresentation::matchPatternElement( int v, int toMatchElement ) const {/**assert(0);**/
  int playerColor=1;
  switch (toMatchElement) {/**assert(0);**/
      case 0: //EMPTY
      return v==EMPTY;
      break;

      case 1: // opponent
      return v==3-playerColor;
      break;

      case 2: // player
      return v==playerColor;
      break;

      case 3: // opponent or empty
      return v==3-playerColor || v==EMPTY;
      break;

      case 4: // player or empty
      return v==playerColor || v==EMPTY;
      break;

      case 5: // outside
      return v==OUTSIDE;
      break;

      default:
      assert(0);
  }

  return false;
}

bool MoGo::DummyPatternRepresentation::matchPatternElement( const Goban & goban, int i, int j, int toMatchElement, int positionsDiffXElement, int positionsDiffYElement ) const {/**assert(0);**/
  int v=goban.positionWithChecking(i+positionsDiffXElement, j+positionsDiffYElement);
  int playerColor=goban.whoToPlay();
  switch (toMatchElement) {/**assert(0);**/
      case 0: //EMPTY
      return v==EMPTY;
      break;

      case 1: // opponent
      return v==3-playerColor;
      break;

      case 2: // player
      return v==playerColor;
      break;

      case 3: // opponent or empty
      return v==3-playerColor || v==EMPTY;
      break;

      case 4: // player or empty
      return v==playerColor || v==EMPTY;
      break;

      case 5: // outside
      return v==OUTSIDE;
      break;

      default:
      assert(0);
  }

  return false;
}

void MoGo::DummyPatternRepresentation::addElement( int toMatchElement, int positionsDiffXElement, int positionsDiffYElement, const std::string & elementName ) {/**assert(0);**/
  toMatch.push_back(toMatchElement);
  positionsDiffX.push_back(positionsDiffXElement);
  positionsDiffY.push_back(positionsDiffYElement);
  patternName=elementName;
}

void MoGo::SmallPatternsDatabase::addPatternsFromTheMatrix( const Vector< Vector < int > > & matrix, const std::string &name, int minPatternSize ) {/**assert(0);**/
  int iIndex, jIndex;
  findPointToPlay(matrix, iIndex, jIndex);
  if (iIndex<0 || jIndex<0) return;

  int patternSize=0;
  int indexAllPatterns=allPatterns.size();
  for (int s=0;s<8;s++) allPatterns.push_back(DummyPatternRepresentation());

  for (int i=0;i<(int)matrix.size();i++) {/**assert(0);**/
    for (int j=0;j<(int)matrix[i].size();j++) {/**assert(0);**/
      int pattern=patternElementFromChar(matrix[i][j]);
      if (pattern<0) continue;
      int iIndexAfter=i-iIndex; int jIndexAfter=j-jIndex;
      patternSize++;
      for (int s=0;s<8;s++) {/**assert(0);**/
        allPatterns[indexAllPatterns+s].addElement(pattern, iIndexAfter, jIndexAfter, name);
        doOneSymetry((s+1)%2,iIndexAfter, jIndexAfter, iIndexAfter, jIndexAfter);
      }
    }
  }
  if (patternSize<minPatternSize)
    for (int s=0;s<8;s++) allPatterns.pop_back();
}

void MoGo::SmallPatternsDatabase::findPointToPlay( const Vector< Vector < int > > & matrix, int & iIndex, int & jIndex ) {/**assert(0);**/
  iIndex=-1;
  jIndex=-1;
  for (int i=0;i<(int)matrix.size();i++) {/**assert(0);**/
    for (int j=0;j<(int)matrix[i].size();j++) {/**assert(0);**/
      if (matrix[i][j]=='*') {/**assert(0);**/
        iIndex=i; jIndex=j;
        return;
      }
    }
  }
}

void MoGo::SmallPatternsDatabase::doOneSymetry( bool rotation, int iIndex, int jIndex, int & iIndexAfter, int & jIndexAfter ) {/**assert(0);**/
  if (rotation) {/**assert(0);**/
    iIndexAfter=-jIndex;
    jIndexAfter=iIndex;
  } else {/**assert(0);**/ // vertical mirror
    iIndexAfter=-iIndex;
    jIndexAfter=jIndexAfter;
  }
}


int MoGo::SmallPatternsDatabase::patternElementFromChar( int charRepresentation ) {/**assert(0);**/
  if (charRepresentation=='?' || charRepresentation=='!')
    return -2;
  else if (charRepresentation=='.')
    return 0;
  else if (charRepresentation=='X')
    return 1;
  else if (charRepresentation=='Y')
    return 1;
  else if (charRepresentation=='O')
    return 2;
  else if (charRepresentation=='x')
    return 3;
  else if (charRepresentation=='o')
    return 4;
  else if (charRepresentation=='*')
    return -1;
  else if (charRepresentation=='-')
    return 5;
  else if (charRepresentation=='+')
    return 5;
  else if (charRepresentation=='|')
    return 5;

  GoTools::print("charRepresentation %c\n", charRepresentation);
  assert(0);
  return -1;
}

void MoGo::DummyPatternRepresentation::printPattern( ) const {/**assert(0);**/
  GoTools::print("Pattern %s\n", patternName.c_str());
  for (int i=0;i<(int)toMatch.size();i++)
    printPatternElement(i);
}

void MoGo::DummyPatternRepresentation::printPatternElement(int i) const {/**assert(0);**/
  GoTools::print("match %i i=%i, j=%i\n", toMatch[i], positionsDiffX[i], positionsDiffY[i]);
}

void MoGo::SmallPatternsDatabase::printDatabase( ) const {/**assert(0);**/
  for (int i=0;i<(int)allPatterns.size();i++)
    allPatterns[i].printPattern();
  if (tree) tree->print();
}








int MoGo::SPDecisionTree::classify( const Goban & goban, Location location ) const {/**assert(0);**/
  int x=goban.xIndex(location);
  int y=goban.yIndex(location);
  const SPDecisionTreeNode *leaf=root->iterate(goban, x, y);
  assert(leaf->status<0);

  return -leaf->status-1;
}

MoGo::SPDecisionTree::~ SPDecisionTree( ) {/**assert(0);**/}

void MoGo::SPDecisionTree::print( ) const {/**assert(0);**/
  root->print(0);
}

MoGo::SPDecisionTreeNode::~SPDecisionTreeNode( ) {/**assert(0);**/}

const SPDecisionTreeNode * MoGo::SPDecisionTreeNode::iterate( const Goban & goban, int x, int y) const {/**assert(0);**/
  if (status<0) return this;
  int v=goban.positionWithChecking(x+positionsDiffXElement, y+positionsDiffYElement);
  if (v==goban.whoToPlay()) v=1;
  else if (v==3-goban.whoToPlay()) v=2;
  assert(childs.size()==4);
  return childs[v]->iterate(goban, x, y);
}

void MoGo::SPDecisionTreeNode::print( int depth ) const {/**assert(0);**/
  for (int i=0;i<depth;i++)
    GoTools::print("|   ");
  GoTools::print("dx %i, dy %i, s %i\n", positionsDiffXElement, positionsDiffYElement, status);
  for (int i=0;i<(int)childs.size();i++)
    childs[i]->print(depth+1);
}

void MoGo::DummyPatternRepresentation::deleteElement( int index ) {/**assert(0);**/
  assert(index>=0);assert(index<(int)toMatch.size());
  assert(toMatch.size()==positionsDiffX.size());
  assert(toMatch.size()==positionsDiffY.size());

  toMatch[index]=toMatch[toMatch.size()-1];
  positionsDiffX[index]=positionsDiffX[toMatch.size()-1];
  positionsDiffY[index]=positionsDiffY[toMatch.size()-1];

  toMatch.pop_back();
  positionsDiffX.pop_back();
  positionsDiffY.pop_back();
}

int MoGo::DummyPatternRepresentation::indexOf( int positionsDiffXElement, int positionsDiffYElement ) const {/**assert(0);**/
  for (int i=0;i<(int)toMatch.size();i++)
    if (positionsDiffXElement==positionsDiffX[i] && positionsDiffYElement==positionsDiffY[i])
      return i;
  return -1;
}





void MoGo::SmallPatternsDatabase::compile( ) {/**assert(0);**/
  if (tree) delete tree;

  SPDecisionTreeNode *root=buildTree(allPatterns);
  tree= new SPDecisionTree(root);
}




SPDecisionTreeNode * MoGo::SmallPatternsDatabase::buildTree( const Vector< DummyPatternRepresentation > & allPatterns ) {/**assert(0);**/
  if (allPatterns.size()==0) {/**assert(0);**/
    return new SPDecisionTreeNode(0);
  } else {/**assert(0);**/
    for (int i=0;i<(int)allPatterns.size();i++) {/**assert(0);**/
      if (allPatterns[i].toMatch.size()==0) // we found a rule (pattern) which is empty, so which matches
        return new SPDecisionTreeNode(1);
    }
  }

  MoGoHashMap(Vector<int>, Vector<int>, hashVectorInt, equalVectorInt) allMatchs;
  Vector<int> forMatch(2);
  for (int i=0;i<(int)allPatterns.size();i++) {/**assert(0);**/
    for (int j=0;j<(int)allPatterns[i].toMatch.size();j++) {/**assert(0);**/
      forMatch[0]=allPatterns[i].positionsDiffX[j];
      forMatch[1]=allPatterns[i].positionsDiffY[j];
      if (allMatchs.count(forMatch)==0)
        allMatchs[forMatch]=Vector<int>(5,0);
      bool oneMatches=false;
      for (int k=0;k<4;k++)
        if (allPatterns[i].matchPatternElement(k,allPatterns[i].toMatch[j])) {/**assert(0);**/
          allMatchs[forMatch][k]++;
          oneMatches=true;
        }
      if (oneMatches)
        allMatchs[forMatch][4]++;
    }
  }

  int min=1000000000; int minIndex=-1; int count=0; Vector<int> locDiffs;
  for (MoGoHashMap(Vector<int>, Vector<int>, hashVectorInt, equalVectorInt)::const_iterator it
       = allMatchs.begin() ; it != allMatchs.end() ; ++it) {/**assert(0);**/ // for all variables
    int max=-1;
    int rest=allPatterns.size()-it->second[4];
    assert(it->second.size()==5);
    for (int i=0;i<4;i++) {/**assert(0);**/
      if (it->second[i]+rest>max) {/**assert(0);**/
        max=it->second[i]+rest;
      }
    }
    if (max<min) {/**assert(0);**/
      min=max;
      minIndex=count;
      locDiffs=it->first;
    }
    count++;
  }

  assert(minIndex>=0);assert(locDiffs.size()==2);
  Vector<DummyPatternRepresentation> leftPatterns;
  Vector<Vector<DummyPatternRepresentation> > classifiedPatterns(4);
  for (int i=0;i<(int)allPatterns.size();i++) {/**assert(0);**/
    int index=allPatterns[i].indexOf(locDiffs[0],locDiffs[1]);
    if (index<0)
      leftPatterns.push_back(allPatterns[i]);
    else {/**assert(0);**/
      bool oneMatches=false;
      for (int k=0;k<4;k++)
        if (allPatterns[i].matchPatternElement(k,allPatterns[i].toMatch[index])) {/**assert(0);**/
          classifiedPatterns[k].push_back(allPatterns[i]);
          classifiedPatterns[k][classifiedPatterns[k].size()-1].deleteElement(index);
          oneMatches=true;
        }
      assert(oneMatches);
    }
  }
  Vector<SPDecisionTreeNode *>childs(4);
  for (int i=0;i<4;i++)
    concat(classifiedPatterns[i], leftPatterns);
  clearVector(leftPatterns);
  for (int i=0;i<4;i++) {/**assert(0);**/
    childs[i]=buildTree(classifiedPatterns[i]);
    clearVector(classifiedPatterns[i]);
  }

  SPDecisionTreeNode *root=new SPDecisionTreeNode();
  root->childs=childs;
  root->status=0;
  root->positionsDiffXElement=locDiffs[0];
  root->positionsDiffYElement=locDiffs[1];

  return root;
}




/*
#  ?     : don't care
#  .     : empty
#  X     : your piece,
#  O     : my piece,
#  x     : your piece or empty
#  o     : my piece or empty
#  *     : my next move
#  -, |  : edge of board
#  +     : corner of board
*/
