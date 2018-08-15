#ifndef __ACTION_TREE_INCLUDED__
#define __ACTION_TREE_INCLUDED__

#include "Arduino.h"

class Action;

class ActionNode {
  public:

    int height = 0;
    Action *data = NULL;

    ActionNode *left = NULL, *right = NULL;

    ActionNode();

    ActionNode(Action *action);

};

class ActionBinaryTree {

  private:

    ActionNode *root = NULL;

  public:

    ActionBinaryTree();

    bool isEmpty();

    void makeEmpty();

    void insert(Action *action);
    void insert();

    template <typename ... Args>
    void insert(Action *action, Args ... args) {
      insert(action);
      insert(args...);
    }

    int height(ActionNode *node);

    int maks(int lhs, int rhs);

    ActionNode *insert(Action *action, ActionNode *t);

    ActionNode *rotateWithLeftChild(ActionNode *k3);
    ActionNode *rotateWithRightChild(ActionNode *k1);

    ActionNode *doubleWithLeftChild(ActionNode *k3);

    ActionNode *doubleWithRightChild(ActionNode *k1);

    int countNodes();
    int countNodes(ActionNode *r);

    bool search(ActionNode *r, int val);
    bool search(int val);

    Action *retrieve(int val);
    Action *retrieve(ActionNode *r, int val);

	Action *retrieveRemote(int val);
	Action *retrieveRemote(ActionNode *r, int val);

    void inorder();
    void inorder(ActionNode *r);

    void preorder();
    void preorder(ActionNode *r);

    void postorder();
    void postorder(ActionNode *r);

};

#endif
