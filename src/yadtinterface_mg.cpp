//
// C++ Implementation: yadtinterface_mg
//
// Description:
//
//
// Author: Sylvain Gelly and Yizao Wang <sylvain.gelly@lri.fr and yizao.wang@polytechnique.edu>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "yadtinterface_mg.h"
#include "gotools_mg.h"

using namespace MoGo;
using namespace std;


namespace MoGo {
  char *YaDTInterface::tmpLine=new char[100000];
  char *YaDTInterface::tmpWord=new char[100000];
  char *YaDTInterface::tmpWord2=new char[100000];
  char *YaDTInterface::tmpWord3=new char[100000];

  YaDTInterface::YaDTInterface() {/**assert(0);**/}


  YaDTInterface::~YaDTInterface() {/**assert(0);**/}}



int MoGo::DecisionTree::classify( const Vector< int > & entry ) const {/**assert(0);**/
  assert(root);
  DecisionTreeNode *node=root;
  //   while (node->result()<0) {/**assert(0);**/
  while (node->status>=0) {/**assert(0);**/
    node=node->iterate(entry);
    // node->print(0);
    //  GoTools::print("node %i, %i\n", node->result());
  }
  return node->result();
}

MoGo::DecisionTree::~DecisionTree( ) {/**assert(0);**/
  if (root) delete root;
}

MoGo::DecisionTreeNode::~DecisionTreeNode( ) {/**assert(0);**/
  for (int i=0;i<(int)childs.size();i++)
    delete childs[i];
}


Vector< int > MoGo::YaDTInterface::stringToVectorInt( const std::string & s ) {/**assert(0);**/
  Vector<int> res(s.length());
  for (int i=0;i<(int)s.length();i++)
    res[i]=(int)s[i];
  return res;
}

DecisionTree * MoGo::YaDTInterface::readTreeFromXMLFile( const char * name ) {/**assert(0);**/
  FILE *f=fopen(name,"r");
  if (!f)
    return 0;
  YaDTInterface interface;
  if (!interface.readIdentifiers(f))
    return false;
  string result;
  int wordFound=lookForLineWith("<Node","</Node>",f,result); if (wordFound!=1) return 0;
  wordFound=lookForLineWith("<Node","</Node>",f,result); if (wordFound!=1) return 0; // first real node (the root !)

  Vector<DecisionTreeNode *> currentSequence;currentSequence.push_back(0);
  Vector<int> currentSequencePos;
  DecisionTreeNode *node=interface.nodeFromXML(f, result, currentSequence, currentSequencePos);
  if (!node) return 0;
  DecisionTree *tree=new DecisionTree(node);

  bool newNode=true;
  while (true) {/**assert(0);**/
    wordFound=lookForLineWith("<Node","</Node>",f,result);
    //     GoTools::print("fdfs %i, %i, %i\n",wordFound, currentSequence.size(), currentSequencePos.size());
    if (wordFound==0)
      break;
    if (wordFound==2) {/**assert(0);**/
      newNode=false;
      if (currentSequence.size()==0) {/**assert(0);**/
        delete tree; return 0;
      }
      currentSequence.pop_back();
      if (currentSequencePos.size()>0)
        currentSequencePos.pop_back();
      continue;
    }
    if (newNode)
      currentSequence.push_back(0);
    node=interface.nodeFromXML(f, result, currentSequence, currentSequencePos);
    if (!node) {/**assert(0);**/
      delete tree; return 0;
    }
    newNode=true;
  }

  return tree;
}

DecisionTreeNode * MoGo::YaDTInterface::nodeFromXML(FILE *f, const std::string & nodeLine, Vector<DecisionTreeNode *> &currentSequence, Vector<int> &currentSequencePos) {/**assert(0);**/
  assert(currentSequence.size()>0);

  // <Node score="0" recordCount="24">
  sscanf (nodeLine.c_str(),"%*s %s",tmpWord); string fieldScore=extractBetweenQuote(tmpWord, 50000);
  int score=-1; if (fieldScore.length()>0) sscanf(fieldScore.c_str(), "%i", &score);

  string result;
  // <SimplePredicate field="b21" operator="equal" value="3" />
  int wordFound=lookForLineWith("<SimplePredicate","</Node>",f,result); if (wordFound!=1) return 0;
  sscanf(result.c_str(),"%*s %s %s %s",tmpWord,tmpWord2,tmpWord3);
  string fieldField=extractBetweenQuote(tmpWord, 50000); Vector<int> fieldFieldVI=stringToVectorInt(fieldField);
  string fieldOp=extractBetweenQuote(tmpWord2, 50000);
  string fieldVal=extractBetweenQuote(tmpWord3, 50000); int val; sscanf(fieldVal.c_str(), "%i", &val);

  if (identifierToPos.count(fieldFieldVI)==0)
    return 0;

  int status=DecisionTreeNode::statusOfString(fieldOp);
  DecisionTreeNode *node=currentSequence.back();
  if (node==0) {/**assert(0);**/ // we have to create a new node
    node=new DecisionTreeNode();
    node->status=abs(status);
    if (node->status!=0)
      node->cutValue=val;
    else
      node->cutValue=0;
    node->numVariable=identifierToPos[fieldFieldVI];
    currentSequence.back()=node;
    if (currentSequencePos.size()>0) {/**assert(0);**/
      assert(currentSequence.size()>=2);
      currentSequence[currentSequence.size()-2]->addChild(node,currentSequencePos.back());
    }
  } else {/**assert(0);**/
    if (node->numVariable!=identifierToPos[fieldFieldVI])
      return 0;
    if (node->status!=abs(DecisionTreeNode::statusOfString(fieldOp)))
      return 0;
  }
  int pos=DecisionTreeNode::posInChild(status, val);
  currentSequencePos.push_back(pos);
  if (score>=0) {/**assert(0);**/
    DecisionTreeNode *child=new DecisionTreeNode(score);
    node->addChild(child, pos);
    currentSequence.push_back(child);
  }
  return node;
}


bool MoGo::YaDTInterface::readIdentifiers( FILE * f ) {/**assert(0);**/
  identifierToPos.clear();
  fgets(tmpLine, 50000, f);
  string strTmp(tmpLine);
  if (strTmp != "<PMML version=\"2.0\">\n") {/**assert(0);**/
    return false;
  }
  string result;
  int count=0;
  while (true) {/**assert(0);**/
    int wordFound=lookForLineWith("<MiningField","</MiningSchema>",f,result);
    if (wordFound==0) return false;
    if (wordFound==2) break;
    assert(wordFound==1);
    // <MiningField name="a0" usageType="active" />
    sscanf (result.c_str(),"%*s %s %s",tmpWord,tmpWord2);
    string tmpStr2(tmpWord2);
    if (tmpStr2=="usageType=\"active\"") {/**assert(0);**/
      string field=extractBetweenQuote(tmpWord, 50000); Vector<int> fieldVI=stringToVectorInt(field);
      identifierToPos[fieldVI]=count;
      count++;
    }
  }

  return true;
}

string MoGo::YaDTInterface::extractBetweenQuote( const char * s, int maxSize ) {/**assert(0);**/
  int i=0;
  for (;s[i]!=0 && i<maxSize;i++) {/**assert(0);**/
    if (s[i]=='"')
      break;
  }
  if (s[i]!='"')
    return string();
  i++;
  int start=i;
  for (;s[i]!=0 && i<maxSize;i++) {/**assert(0);**/
    if (s[i]=='"')
      break;
  }

  if (s[i]!='"')
    return string();

  string tmp;
  for (int j=start;j<i;j++)
    tmp+=s[j];

  return tmp;
}

int MoGo::YaDTInterface::lookForLineWith( const char *s1, const char *s2, FILE * f, std::string &result ) {/**assert(0);**/
  string tmpWordSearched1(s1);
  string tmpWordSearched2(s2);
  while (fgets(tmpLine, 50000, f)) {/**assert(0);**/
    sscanf (tmpLine,"%s",tmpWord);
    string strTmp(tmpWord);
    if (strTmp == tmpWordSearched1) {/**assert(0);**/
      result=string(tmpLine);
      return 1;
    } else if (strTmp == tmpWordSearched2) {/**assert(0);**/
      result=string(tmpLine);
      return 2;
    }
  }
  return 0;
}

int MoGo::DecisionTreeNode::statusOfString( const string &fieldOp ) {/**assert(0);**/
  if (fieldOp=="equal")
    return 0;
  else if (fieldOp=="lessThan")
    return 1;
  else if (fieldOp=="lessOrEqual")
    return 2;
  else if (fieldOp=="greaterThan")
    return -2;
  else if (fieldOp=="greaterOrEqual")
    return -1;
  assert(0);return 0;
}

int MoGo::DecisionTreeNode::posInChild( int relativeStatus, int value ) {/**assert(0);**/
  if (relativeStatus==0)
    return value;
  else if (relativeStatus>0)
    return 0;
  else
    return 1;
}

void MoGo::DecisionTreeNode::addChild( DecisionTreeNode * child, int pos ) {/**assert(0);**/
  childs.resize(GoTools::max(childs.size(),pos+1));
  childs[pos]=child;
}

void MoGo::DecisionTree::print( ) const {/**assert(0);**/
  root->print(0);
}

void MoGo::DecisionTreeNode::print(int depth) const {/**assert(0);**/
  for (int i=0;i<depth;i++)
    GoTools::print("|   ");
  GoTools::print("nv %i, s %i, cv %i\n", numVariable, status, cutValue);
  for (int i=0;i<(int)childs.size();i++)
    childs[i]->print(depth+1);
}

bool MoGo::InterestingMoveFinderDecision::isInteresting( const Vector< int > & neighborhood ) const {/**assert(0);**/
  return tree->classify(neighborhood)==1;
}
// void MoGo::InterestingMoveFinderDecision::print( ) {/**assert(0);**/
//   tree->print();
// }
// void MoGo::InterestingMoveFinderDecision::printDetails( ) {/**assert(0);**/
//   tree->print();
// }
int MoGo::InterestingMoveFinderDecision::interestingInt( const Vector< int > & neighborhood ) const {/**assert(0);**/
  return tree->classify(neighborhood);
}
