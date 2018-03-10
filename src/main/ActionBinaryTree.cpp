#include "ActionBinaryTree.h"

#include "Action.h"

ActionNode::ActionNode() {

}

ActionNode::ActionNode(Action *action) {
  data = action;
}

ActionBinaryTree::ActionBinaryTree()

{

  root = NULL;

}



/* Function to check if tree is empty */

bool ActionBinaryTree::isEmpty()

{

  return root == NULL;

}



/* Make the tree logically empty */

void ActionBinaryTree::makeEmpty()

{

  root = NULL;

}



/* Function to insert data */
void ActionBinaryTree::insert() {}

void ActionBinaryTree::insert(Action *data)

{

  root = insert(data, root);

}



/* Function to get height of node */

int ActionBinaryTree::height(ActionNode *t)

{

  return t == NULL ? -1 : t->height;

}



/* Function to maks of left/right node */

int ActionBinaryTree::maks(int lhs, int rhs)

{

  return lhs > rhs ? lhs : rhs;

}



/* Function to insert data recursively */

ActionNode *ActionBinaryTree::insert(Action *x, ActionNode *t)

{

  if (t == NULL)

    t = new ActionNode(x);

  else if (x->code < t->data->code)

  {

    t->left = insert(x, t->left);

    if (height(t->left) - height(t->right) == 2)

      if (x < t->left->data)

        t = rotateWithLeftChild(t);

      else

        t = doubleWithLeftChild(t);

  }

  else if (x > t->data)

  {

    t->right = insert(x, t->right);

    if (height(t->right) - height(t->left) == 2)

      if (x->code > t->right->data->code)

        t = rotateWithRightChild(t);

      else

        t = doubleWithRightChild(t);

  }

  t->height = maks(height(t->left), height(t->right)) + 1;

  return t;

}



/* Rotate binary tree node with left child */

ActionNode *ActionBinaryTree::rotateWithLeftChild(ActionNode* k2)

{

  ActionNode *k1 = k2->left;

  k2->left = k1->right;

  k1->right = k2;

  k2->height = maks(height(k2->left), height(k2->right)) + 1;

  k1->height = maks(height(k1->left), k2->height) + 1;

  return k1;

}



/* Rotate binary tree node with right child */

ActionNode *ActionBinaryTree::rotateWithRightChild(ActionNode *k1)

{

  ActionNode *k2 = k1->right;

  k1->right = k2->left;

  k2->left = k1;

  k1->height = maks(height(k1->left), height(k1->right)) + 1;

  k2->height = maks(height(k2->right), k1->height) + 1;

  return k2;

}



/*

   Double rotate binary tree node: first left child

   with its right child; then node k3 with new left child

*/

ActionNode *ActionBinaryTree::doubleWithLeftChild(ActionNode *k3)

{

  k3->left = rotateWithRightChild(k3->left);

  return rotateWithLeftChild(k3);

}



/*

   Double rotate binary tree node: first right child

   with its left child; then node k1 with new right child

*/

ActionNode *ActionBinaryTree::doubleWithRightChild(ActionNode *k1)

{

  k1->right = rotateWithLeftChild(k1->right);

  return rotateWithRightChild(k1);

}



/* Functions to count number of nodes */

int ActionBinaryTree::countNodes()

{

  return countNodes(root);

}



int ActionBinaryTree::countNodes(ActionNode *r)

{

  if (r == NULL)

    return 0;

  else

  {

    int l = 1;

    l += countNodes(r->left);

    l += countNodes(r->right);

    return l;

  }

}



/* Functions to search for an element */

bool ActionBinaryTree::search(int val)

{

  return search(root, val);

}

Action *ActionBinaryTree::retrieve(int val)

{

  return retrieve(root, val);

}

Action *ActionBinaryTree::retrieve(ActionNode *r, int val)

{

  Action *found = NULL;

  while ((r != NULL) && !found)

  {

    int rval = r->data->code;

    if (val < rval)

      r = r->left;

    else if (val > rval)

      r = r->right;

    else

    {

      found = r->data;

      break;

    }

    found = retrieve(r, val);

  }

  return found;

}

Action *ActionBinaryTree::retrieveRemote(int val)

{

  return retrieveRemote(root, val);

}

Action *ActionBinaryTree::retrieveRemote(ActionNode *r, int val)

{

  Action *found = NULL;

  while ((r != NULL) && !found)

  {

    int rval = r->data->remoteCode;

    if (val < rval)

      r = r->left;

    else if (val > rval)

      r = r->right;

    else

    {

      found = r->data;

      break;

    }

    found = retrieve(r, val);

  }

  return found;

}

bool ActionBinaryTree::search(ActionNode *r, int val)

{

  bool found = false;

  while ((r != NULL) && !found)

  {

    int rval = r->data->code;

    if (val < rval)

      r = r->left;

    else if (val > rval)

      r = r->right;

    else

    {

      found = true;

      break;

    }

    found = search(r, val);

  }

  return found;

}



/* Function for inorder traversal */

void ActionBinaryTree::inorder()

{

  inorder(root);

}



void ActionBinaryTree::inorder(ActionNode *r)

{

  if (r != NULL)

  {

    inorder(r->left);

    inorder(r->right);

  }

}



/* Function for preorder traversal */

void ActionBinaryTree::preorder()

{

  preorder(root);

}

void ActionBinaryTree::preorder(ActionNode *r)

{

  if (r != NULL)

  {

    preorder(r->left);

    preorder(r->right);

  }

}



/* Function for postorder traversal */

void ActionBinaryTree::postorder()

{

  postorder(root);

}

void ActionBinaryTree::postorder(ActionNode *r)

{

  if (r != NULL)

  {

    postorder(r->left);

    postorder(r->right);

  }

}

