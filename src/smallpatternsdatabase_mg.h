//
// C++ Interface: scoreevaluator_mg
//
// Description:
//
//
// Author: Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SMALLPATTERNSDATABASE_MG_H
#define SMALLPATTERNSDATABASE_MG_H

#include "typedefs_mg.h"
#include <string>
#include "yadtinterface_mg.h"

namespace MoGo {
  class Goban;
  class SPDecisionTreeNode;
  /**
  @author Sylvain Gelly
  */

  class SPDecisionTree {
    friend class SPDecisionTreeNode;
  public:
    int classify(const Goban &goban, Location location) const;
    SPDecisionTree(SPDecisionTreeNode *root) {this->root=root;};
    ~SPDecisionTree();
    void print() const;
  private:
    SPDecisionTreeNode *root;
  };
  class SPDecisionTreeNode {
    friend class SPDecisionTree;
    friend class SmallPatternsDatabase;
  public:
    SPDecisionTreeNode() {};
    SPDecisionTreeNode(int score) {
      status=-score-1;positionsDiffXElement=0;positionsDiffYElement=0;
    }
    ~SPDecisionTreeNode();
    inline int result() const {
      if (status>=0) return -1;
      else return -status-1;
    }
    inline const SPDecisionTreeNode *iterate(const Goban &goban, int x, int y) const;
    void addChild(SPDecisionTreeNode *child, int pos);
    void print(int depth) const;
  private:
    int status;
    int positionsDiffXElement;
    int positionsDiffYElement;
    Vector<SPDecisionTreeNode *> childs;
  };

  class DummyPatternRepresentation {
    friend class SmallPatternsDatabase;
  public:
    DummyPatternRepresentation();
    void addElement(int toMatchElement, int positionsDiffXElement, int positionsDiffYElement, const std::string &elementName);
    ~DummyPatternRepresentation();
    bool matchPattern(const Goban &goban, Location location) const;
    void printPattern() const;
    void printPatternElement(int i) const;
    void deleteElement(int index);
    int indexOf(int positionsDiffXElement, int positionsDiffYElement) const;
  private:
    bool matchPatternElement(const Goban &goban, int i, int j,
                             int toMatchElement, int positionsDiffXElement, int positionsDiffYElement) const;
    bool matchPatternElement(int value, int toMatchElement) const;
    Vector<int> toMatch;
    Vector<Location> positionsDiffX;
    Vector<Location> positionsDiffY;
    std::string patternName;
  };

  class SmallPatternsDatabase {
  public:
    SmallPatternsDatabase();
    ~SmallPatternsDatabase();

    bool buildByGnugoDatabase(const char *name, int minPatternSize=-1);
    bool matchOnePattern(const Goban &goban, Location location) const;

    static void doOneSymetry(bool rotation, int iIndex, int jIndex, int &iIndexAfter, int &jIndexAfter);
    void printDatabase() const;
    void compile();
    static SPDecisionTreeNode *buildTree(const Vector<DummyPatternRepresentation> &patterns);
  private:
    static bool readPattern(FILE *f, Vector<Vector<int> > &matrix);
    void addPatternsFromTheMatrix(const Vector<Vector<int> > &matrix, const std::string &name, int minPatternSize);
    static void findPointToPlay(const Vector<Vector<int> > &matrix, int & iIndex, int & jIndex);
    static int patternElementFromChar(int charRepresentation);
    Vector<DummyPatternRepresentation> allPatterns;
    SPDecisionTree *tree;
  };
}
#endif
