//
// C++ Interface: yadtinterface_mg
//
// Description:
//
//
// Author: Sylvain Gelly and Yizao Wang <sylvain.gelly@lri.fr and yizao.wang@polytechnique.edu>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOYADTINTERFACE_MG_H
#define MOGOYADTINTERFACE_MG_H

#include "typedefs_mg.h"
#include <string>

namespace MoGo {
  class DecisionTree;
  class DecisionTreeNode;
  /**
  Interface to YADT Yet another Decision Tree builder
   
  @author Sylvain Gelly and Yizao Wang
  */
  class YaDTInterface {
  public:
    YaDTInterface();

    ~YaDTInterface();

    static DecisionTree *readTreeFromXMLFile(const char *name);
    static Vector<int> stringToVectorInt(const std::string &s);
    static std::string extractBetweenQuote(const char *s, int maxSize);
    static char *tmpLine;
    static char *tmpWord;
    static char *tmpWord2;
    static char *tmpWord3;
    static int lookForLineWith(const char *s1, const char *s2, FILE *f, std::string &result);
  private:
    bool iterateNodeOnXML(FILE *f);
    bool readIdentifiers(FILE *f);
    DecisionTreeNode *nodeFromXML(FILE *f, const std::string &nodeLine, Vector<DecisionTreeNode *> &currentSequence, Vector<int> &currentSequencePos);
    MoGoHashMap(Vector<int>, int, hashVectorInt, equalVectorInt) identifierToPos;
  };
  class DecisionTree {
    friend class DecisionTreeNode;
  public:
    int classify(const Vector<int> &entry) const;
    DecisionTree(DecisionTreeNode *root) {this->root=root;};
    ~DecisionTree();
    void print() const;
  private:
    DecisionTreeNode *root;
  };
  class DecisionTreeNode {
    friend class DecisionTree;
    friend class YaDTInterface;
  public:
    DecisionTreeNode() {};
    DecisionTreeNode(int score) {
      status=-score-1;cutValue=0; numVariable=-1;
    }
    ~DecisionTreeNode();
    inline int result() const {
      if (status>=0) return -1;
      else return -status-1;
    }
    inline DecisionTreeNode *iterate(const Vector<int> &entry) const {
      if (status==0)
        return childs[entry[numVariable]];
      if (status==1)
        return childs[entry[numVariable]>=cutValue];
      else
        return childs[entry[numVariable]>cutValue];
    }
    static int posInChild(int relativeStatus, int value);
    static int statusOfString(const std::string &fieldOp);
    void addChild(DecisionTreeNode *child, int pos);
    void print(int depth) const;
  private:
    /** <0 for class result, 0 for ==, 1 for <, 2 for <= */
    int status;
    /** if < or <= */
    int cutValue;
    int numVariable;
    Vector<DecisionTreeNode *> childs;
  };

/** Encapsulate the function saying if a move is (or not) an interesting move */
  class InterestingMoveFinder {
  public:
    virtual bool isInteresting(const Vector<int> &neighborhood) const=0;
    virtual int interestingInt(const Vector<int> &) const { return 0;}
    virtual void print() {};
    virtual void printDetails() {};
    virtual ~InterestingMoveFinder() {}}
  ;

  class InterestingMoveFinderDecision : public InterestingMoveFinder {
  public:
    InterestingMoveFinderDecision(DecisionTree *tree) { this->tree=tree;}
    virtual bool isInteresting(const Vector<int> &neighborhood) const;
    virtual int interestingInt(const Vector<int> &neighborhood) const;
    virtual void print() {};
    virtual void printDetails() {};
    virtual ~InterestingMoveFinderDecision() {}
  private:
    DecisionTree *tree;
  }

  ;
}

#endif
