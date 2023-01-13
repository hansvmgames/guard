/*
 *
 * This file is part of guard.
 *
 * guard is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version.
 * 
 * guard is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with guard. 
 * If not, see <https://www.gnu.org/licenses/>. 
 * 
 */

#include "rb_tree.h"
#include "status.h"

#include <assert.h>
#include <stdlib.h>

void init_rb_tree(struct rb_tree * tree, rb_tree_cmp cmp, rb_tree_destroy destroy_key, rb_tree_destroy destroy_value) {
  assert(tree != NULL);
  assert(cmp != NULL);
  
  tree->root = NULL;
  tree->cmp = cmp;
  tree->destroy_key = destroy_key;
  tree->destroy_value = destroy_value;
}


/**
 * Finds a node in the red black tree
 * \param tree the tree
 * \param key the key
 * \return the node or NULL if not found
 */
static struct rb_node * find_rb_node(const struct rb_tree * tree, const void * key) {
  assert(tree != NULL);
  assert(key != NULL);
  
  struct rb_node * node = tree->root;
  while(node != NULL) {
    int result = (*tree->cmp)(key, node->key);
    if(result < 0) {
      node = node->left;
    } else if (result > 0) {
      node = node->right;
    } else {
      break;
    }
  }
  return node;
}

/**
 * Rotates a subtree clockwise
 * \param tree the tree
 * \param p the parent
 * \param n the left child of p
 */
static void rotate_rb_node_cw(struct rb_tree * tree, struct rb_node * p, struct rb_node * n) {
  assert(tree != NULL);
  assert(p != NULL);
  assert(n != NULL);
  assert(n->parent == p);
  assert(p->left == n);

  if(p->parent == NULL) {
    tree->root = n;
  } else if(p->parent->left == p){
    p->parent->left = n;
  } else {
    assert(p->parent->right == p);
    p->parent->right = n;
  }
  n->parent = p->parent;
  
  struct rb_node * c = n->right;
  n->right = p;
  p->parent = n;
  p->left = c;
  if(c != NULL) {
    c->parent = p;
  }
}

/**
 * Rotates a subtree counter-clockwise
 * \param tree the tree
 * \param p the parent
 * \param n the right child of p
 */
static void rotate_rb_node_ccw(struct rb_tree * tree, struct rb_node * p, struct rb_node * n) {
  assert(tree != NULL);
  assert(p != NULL);
  assert(n != NULL);
  assert(n->parent == p);
  assert(p->right == n);
  
  if(p->parent == NULL) {
    tree->root = n;
  } else if(p->parent->left == p){
    p->parent->left = n;
  } else {
    assert(p->parent->right == p);
    p->parent->right = n;
  }
  n->parent = p->parent;

  struct rb_node * c = n->left;
  n->left = p;
  p->parent = n;
  p->right = c;
  if(c != NULL) {
    c->parent = p;
  }
}

/**
 * Fixes the red black tree after insert
 * \param tree the tree
 * \param n the node just inserted
 */
static void fix_rb_tree_after_insert(struct rb_tree * tree, struct rb_node * n) {
  assert(tree != NULL);
  assert(n != NULL);
  assert(n->red);
  assert(n->left == NULL);
  assert(n->right == NULL);

  // Rules:
  // 1) Every node is red or black
  // 2) Leaf nodes (NULL) are black
  // 3) A red node can not have a red child
  // 4) For all subtrees, all paths to a leaf node passes the same amount of black nodes
  
  struct rb_node * p;

  while(true){
    p = n->parent;
    if(p == NULL) {
      assert(tree->root == n);
      // All rules satisfied, nothing to be done
      break;
    } else if(!p->red) {
      // Rule 3 and 4 are satisfied, nothing to be done
      break;
    } else {
      if(p == tree->root) {
	// Rule 3 violated but parent is the root
	p->red = false; // color swap
	break;
      } else {
	// Rule 3 is violated and parent is not root
	struct rb_node * gp = p->parent;
	assert(gp != NULL);
	assert(!gp->red); // if parent is red, then grandparent must be black
	struct rb_node * u = gp->left == p ? gp->right : gp->left;
	
	if(u != NULL && u->red){
	  // both parent and uncle are red
	  // flipping gp, p and u's colors will satisfy rule 3 and 4 for this subtree
	  gp->red = true;
	  u->red = false;
	  p->red = false;

	  //now gp may violate rule 3 because its parent may be red
	  // go again from gp's position 
	  n = gp;
	} else {
	  // uncle is black, so color flips won't work
	  // we need to rotate the parent to the grandparent's position
	  // this will not work if n is an inner grandchild
	  if(p == gp->left && n == p->right){
	    // rotate counter clockwise
	    rotate_rb_node_ccw(tree, p, n);
	    // node and parent are now switched
	    n = p;
	    p = n->parent;
	  } else if(p == gp->right && n == p->left) {
	    // rotate clockwise
	    rotate_rb_node_cw(tree, p, n);
	    // node and parent are now switched
	    n = p;
	    p = n->parent;
	  }

	  // n is now always an outer grandchild
	  assert((p == gp->left && n == p->left) || (p == gp->right && n == p->right));
	  
	  // now we rotate the parent into the grand parent's place
	  if(p == gp->left) {
	    rotate_rb_node_cw(tree, gp, p);
	  } else {
	    rotate_rb_node_ccw(tree, gp, p);
	  }
	  // now p is in gp's place, p and n are red, gp is black, u is black
	  // we paint p black and gp red
	  p->red = false;
	  gp->red = true;
	  // Rule 3 and 4 are now satisfied
	  return;
	}
      }
    }
  }
}

/**
 * Finds a node for the specified key or emplaces one with a NULL value
 * Sets the following status codes:
 * STATUS_CODE_BAD_ALLOC if memory allocation fails
 * \param tree the treee
 * \param key the key
 * \return an existing node with a value, a new node with a NULL value or NULL if allocation failed
 */
static struct rb_node * find_or_emplace_rb_node(struct rb_tree * tree, void * key) {
  assert(tree != NULL);
  
  struct rb_node * parent = NULL;
  struct rb_node * cur = tree->root;
  int result = 0;
  
  while(cur != NULL) {
    result = (*tree->cmp)(key, cur->key);
    if(result < 0) {
      parent = cur;
      cur = cur->left;
    } else if(result > 0){
      parent = cur;
      cur = cur->right;
    } else {
      return cur;
    }
  }

  struct rb_node * node = (struct rb_node *) malloc(sizeof(struct rb_node));
  if(node == NULL) {
    set_status(STATUS_CODE_BAD_ALLOC);
    return NULL;
  }
  node->red = true;
  node->key = key;
  node->value = NULL;
  node->parent = parent;
  node->left = NULL;
  node->right = NULL;
  
  if(parent == NULL) {
    tree->root = node;
  } else if(result < 0){
    assert(parent->left == NULL);
    parent->left = node;
  } else {
    assert(result > 0);
    assert(parent->right == NULL);
    parent->right = node;
  }
  fix_rb_tree_after_insert(tree, node);
  return node;
}

int insert_into_rb_tree(struct rb_tree * tree, void * key, void * value) {
  assert(tree != NULL);

  struct rb_node * node = find_or_emplace_rb_node(tree, key);
  if(node == NULL) {
    return -1;
  }
  
  if(node->value != NULL) {
    set_status(STATUS_CODE_ALREADY_EXISTS);
    return -1;
  }
  
  node->value = value;
  return 0;
}


int insert_or_replace_into_rb_tree(struct rb_tree * tree, void * key, void * value) {
  assert(tree != NULL);

  struct rb_node * node = find_or_emplace_rb_node(tree, key);
  if(node == NULL) {
    return -1;
  }
  
  if(node->value != NULL && tree->destroy_value != NULL) {
    (*tree->destroy_value)(node->value);
  }
  
  node->value = value;
  return 0;
}

/**
 * Destroys the key and value stored in a node
 * \param tree the tree
 * \param node the node
 */
static void destroy_rb_key_value(struct rb_tree * tree, struct rb_node * node) {
  assert(tree != NULL);
  assert(node != NULL);
    
  if(tree->destroy_key != NULL) {
    (*tree->destroy_key)(node->key);
  }
  if(tree->destroy_value != NULL) {
    (*tree->destroy_value)(node->value);
  }
}

int remove_from_rb_tree(struct rb_tree * tree, const void * key) {

  assert(tree != NULL);
  
  // Rules:
  // 1) Every node is red or black
  // 2) Leaf nodes (NULL) are black
  // 3) A red node can not have a red child
  // 4) For all subtrees, all paths to a leaf node passes the same amount of black nodes

  struct rb_node * n = find_rb_node(tree, key);
  if(n == NULL) {
    return -1; //not found
  }
  destroy_rb_key_value(tree, node);
  
  // if n has two non leaf children then we need to find another one to delete
  if(n->left != NULL && n->right != NULL) {
    // find the in order successor and put its value and key into n
    struct rb_node * o = min_rb_node(n->right);
    n->key = o->key; //note that the original key and value are already destroyed
    n->value = o->key;
    n = o;
  }

  // n is now a node that has at most one child
  assert(n->left == NULL || n->right == NULL);

  struct rb_node * c;
  if(n->left != NULL) {
    c = n->left;
  } else if(n->right != NULL) {
    v = n->right;
  } else {
    c = NULL;
  }
  if(c != NULL) {
    // n has one child c then because of rule 4 c must be red
    // if c is red then n must be black because of rule 3
    assert(c->red);
    assert(!n->red);
    // we can now simply swap n with c without altering the black height of the tree
    if(n->parent == NULL){
      tree->root = c;
    } else if(n->parent->left == n) {
      n->parent->left = c;
    } else {
      assert(n->parent->right == n);
      n->parent->right = c;
    }
    c->parent = n->parent;
    free(n);
    return 0;
  }

  // n has no none leaf children
  assert(n->left == NULL && n->right == NULL);

  //if n is red, it can simply be removed without altering the black height of the tree
  if(n->red) {
    if(n->parent == NULL) {
      tree->root = NULL;
    } else if(n->parent->left == n){
      n->parent->left = NULL;
    } else {
      assert(n->parent->right == n);
      n->parent->right = NULL;
    }
    free(n);
  }

  // n is a black node with no non leaf children
  assert(!n->red);

  if(n == tree->root) {
    // just delete n and the tree is empty
    free(n);
    tree->root = NULL;
  }

  assert(n != tree->root);
  // n is a non-root black node with two leaf children
  // this is the difficult case

  // store the node that needs to be deleted for later and remove it from the parent
  struct rb_node * del = n;
  
  struct rb_node * p = n->parent;

  // we store whether n was the left child as we're going to remove n from p
  bool left;
  if(p->left == n) {
    p->left = NULL;
    left = true;
  } else {
    assert(p->right = n);
    p->right = NULL;
    left = false;
  }

  struct rb_node * s; // sibling
  //c will be the close nephew
  struct rb_node * d; // distant nephew
  
  // we have now decreased the black height of the subtree of p by one
  // we now have to rebalance all the other trees recursively

  // The loop invariant is:
  // If n has / had a sibling, then the black height of the subtree through s is one higher
  // than the one through n
  // n itself is a leaf in the first iteration (the original deletion has replaced n by NULL) or a non leaf black node
  // rule 4 must be restored at the point where the loop ends

  while(true) {

    // set s, c and d
    if(left) {
      s = p->right;
      c = s->left;
      d = s->right;
    } else {
      s = p->left;
      c = s->right;
      d = s->left;
    }

    assert(s != NULL); // sibling can not be NULL because then then the black height would be equal
    if(s->red) {
      // p, c and d are all black because of rule 3
      assert(!p->red);
      assert(c == NULL || !c->red);
      assert(d == NULL || !d->red);
      
    }
  }
  
  
  return 0;
}


// case 1:
static void remove_rb_node_case_1(struct rb_node *n,
				  struct rb_node * p,
				  struct rb_node * s,
				  struct rb_node * c,
				  struct rb_node * d) {
  
}

// case 2:
static void remove_rb_node_case_2(struct rb_node *n,
				  struct rb_node * p,
				  struct rb_node * s,
				  struct rb_node * c,
				  struct rb_node * d) {
  
}

// case 3: s is red, so p, c and d are all black
static void remove_rb_node_case_3(struct rb_node *n,
				  struct rb_node * p,
				  struct rb_node * s,
				  struct rb_node * c,
				  struct rb_node * d) {
  assert(n != NULL);
  assert(s != NULL);
  assert(s->red);
  assert(p != NULL);
  assert(!p->red);
  assert(c == NULL || !c->red);
  assert(d == NULL || !d->red);
}

// case 4:
static void remove_rb_node_case_4(struct rb_node *n,
				  struct rb_node * p,
				  struct rb_node * s,
				  struct rb_node * c,
				  struct rb_node * d) {
  
}


// case 5:
static void remove_rb_node_case_5(struct rb_node *n,
				  struct rb_node * p,
				  struct rb_node * s,
				  struct rb_node * c,
				  struct rb_node * d) {
  
}

// case 6:
static void remove_rb_node_case_6(struct rb_node *n,
				  struct rb_node * p,
				  struct rb_node * s,
				  struct rb_node * c,
				  struct rb_node * d) {
  
}

void * find_in_rb_tree(const struct rb_tree * tree, const void * key) {
  assert(tree != NULL);

  struct rb_node * n = find_rb_node(tree, key);
  if(n == NULL) {
    return NULL;
  } else {
    return n->value;
  }
}

/**
 * Returns the minimum node in the specified subtree
 * \param node the root node of the subtree
 * \return the mininum node or NULL if the subtree root was NULL
 */
static struct rb_node * min_rb_node(struct rb_node * node) {
  if(node != NULL) {
    while(node->left != NULL) {
      node = node->left;
    }
  }
  return node;
}

/**
 * Returns the next node in the tree
 * \param node a node in the tree
 * \return the next node or NULL if the specified node was the last one or was NULL
 */
static struct rb_node * next_rb_node(struct rb_node * node) {
  if(node == NULL) {
    return NULL;
  }
  if(node->right != NULL) {
    return min_rb_node(node->right);
  } else {
    while(node->parent != NULL && node->parent->right == node) {
      node = node->parent;
    }
    return node->parent;
  }
}

int walk_rb_tree(const struct rb_tree * tree, int (*fn)(const void *, const void *)) {
  assert(tree != NULL);
  assert(fn != NULL);

  struct rb_node * node = min_rb_node(tree->root);
  while(node != NULL) {
    int result = (*fn)(node->key, node->value);
    if(result != 0) {
      return result;
    }
    node = next_rb_node(node);
  }
  return 0;
}

/**
 * No-op placeholder when destroy callbacks are NULL
 * \param arg not used
 */
static void noop_rb_tree_destroy(void * arg) {}
 
void dispose_rb_tree(struct rb_tree * tree) {
  assert(tree != NULL);

  struct rb_node * node = tree->root;

  rb_tree_destroy destroy_key = tree->destroy_key == NULL ? noop_rb_tree_destroy : tree->destroy_key;
  rb_tree_destroy destroy_value = tree->destroy_value == NULL ? noop_rb_tree_destroy : tree->destroy_key;

  struct rb_node * next;
  while(node != NULL) {
    if(node->left != NULL) {
      next = node->left;
      node->left = NULL;
    } else if(node->right != NULL) {
      next = node->right;
      node->right = NULL;
    } else {
      next = node->parent;
      (*destroy_key)(node->key);
      (*destroy_value)(node->value);
      free(node);
    }
    node = next;
  }
}

