//
//

#ifndef HW1_AVLTREE_H
#define HW1_AVLTREE_H

#include <cassert>

const int EMPTY = -1;
const int INIT_HEIGHT = 1;

///the structure of each node in the avl tree
template <typename D>
struct AVL_Node{
    int key;
    D data;
    AVL_Node* parent;
    AVL_Node* left_son;
    AVL_Node* right_son;
    int height;

    AVL_Node() : key(EMPTY), left_son(nullptr), right_son(nullptr), height(INIT_HEIGHT){};

    explicit AVL_Node(const int key, const D& data = 0) : key(key), data(data), parent(nullptr), left_son(nullptr),
                                                          right_son(nullptr), height(INIT_HEIGHT){};
    /**
     *     the Dctr only delete the node without solving the ptr problem.
     *     assumes the user took care of that
     */
    ~AVL_Node() = default;
    void calcHeight(){
        if (!left_son && !right_son)
            height = INIT_HEIGHT;
        else if (!left_son)
            height = right_son->height + 1;
        else if (!right_son)
            height = left_son->height + 1;
        else
            height = (left_son->height > right_son->height) ? (left_son->height+1) : (right_son->height+1);
    }

    AVL_Node(const AVL_Node &copy) : key(copy.key), data(copy.data), parent(copy.parent), left_son(copy.left_son),
                                     right_son(copy.right_son), height(copy.height) {};
    AVL_Node(const AVL_Node *copy) : key(copy->key), data(copy->data), parent(copy->parent), left_son(copy->left_son),
                                     right_son(copy->right_son), height(copy->height) {};
};

/**
 * this generic class implement avl tree as studied at class
 */
 template <typename D>
class AVL_Tree{
    /**
     * tree varibles:
     * */
    AVL_Node<D>* root;
    int num_of_nodes;


    /**
     * helper functions for the rolls:
     */
    void leftRoll(AVL_Node<D>* to_roll){
        if (!to_roll || !to_roll->right_son)
            return;
        AVL_Node<D>* son = to_roll->right_son;

        if (!to_roll->parent)
            root = son;
        else{
            if (to_roll->parent->right_son == to_roll)
                to_roll->parent->right_son = son;
            else
                to_roll->parent->left_son = son;
        }
        to_roll->right_son = son->left_son;
        son->left_son = to_roll;
        son->parent = to_roll->parent;
        to_roll->parent = son;

        //update heights:
        to_roll->calcHeight();
        son->calcHeight();

    };

    void rightRoll(AVL_Node<D>* to_roll){
        if (!to_roll || !to_roll->left_son)
            return;
        AVL_Node<D>* son = to_roll->left_son;

        if (!to_roll->parent)
            root = son;
        else{
            if (to_roll->parent->right_son == to_roll)
                to_roll->parent->right_son = son;
            else
                to_roll->parent->left_son = son;
        }
        son->parent = to_roll->parent;
        to_roll->parent = son;
        to_roll->left_son = son->right_son;
        son->right_son = to_roll;



        //update heights:
        to_roll->calcHeight();
        son->calcHeight();
    };

    /**
     * the different rolls needed for the implementation of the avl tree
     */
    void LL(AVL_Node<D>* to_roll){
        rightRoll(to_roll);
    };
    void RR(AVL_Node<D>* to_roll){
        leftRoll(to_roll);
    };
    void LR(AVL_Node<D>* to_roll){
        leftRoll(to_roll->left_son);
        rightRoll(to_roll);
    };
    void RL(AVL_Node<D>* to_roll){
        rightRoll(to_roll->right_son);
        leftRoll(to_roll);
    };

    /**
     * this function updates all the heights in the tree from a given node till the tree's root
     */
    void updateHeights(AVL_Node<D>* start_node){
        if (!start_node)
            return;
        AVL_Node<D>* iter = start_node;
        while(iter){
            iter->calcHeight();
            iter = iter->parent;
        }
    };

    int calc_bf(AVL_Node<D>& to_calc){
        if (!to_calc.right_son && !to_calc.left_son)
            return 0;
        else if (!to_calc.right_son)
            return to_calc.left_son->height;
        else if (!to_calc.left_son)
            return (- to_calc.right_son->height);
        else
            return to_calc.left_son->height - to_calc.right_son->height;
    }

    void balanceTree(AVL_Node<D>* start_node){
        if (!start_node)
            return;
        AVL_Node<D>* iter = start_node;
        if (iter->height == INIT_HEIGHT)
            iter = iter->parent;
        int bf = 0;//calc_bf(*iter);
        while (iter != nullptr){
            bf = calc_bf(*iter);
            if (bf == 2){
                if (calc_bf(*iter->left_son) == -1)
                    LR(iter);
                else
                    LL(iter);
            } else if (bf == -2){
                if (calc_bf(*iter->right_son) == 1)
                    RL(iter);
                else
                    RR(iter);
            }
            updateHeights(iter);
            iter = iter->parent;
        }
    }

    /**
 * helper function to have a recursive in-order
 */
    void inorder_helper_for_delete(AVL_Node<D>** arr, int index, AVL_Node<D>* node){
        if (!node)
            return;
        else {
            //left son:
            if (node->left_son)
                inorder_helper_for_delete(arr, index, node->left_son);
            //visit node:
            arr[index] = node;
            index++;
            //right son:
            if (node->right_son)
                inorder_helper_for_delete(arr, index, node->right_son);
        }
    }


    /**
     * inorder tour of the tree
     * the function fills the given array with the node's keys by the
     * inorder order
     */
    bool inorder_for_delete(AVL_Node<D>** arr){
        if (!arr || !root)
            return false;
        else{
            int i=0;
            //left son:
            if (root->left_son)
                inorder_helper_for_delete(arr, i, root->left_son);
            //visit node:
            arr[i] = root;
            i++;
            //right son:
            if (root->right_son)
                inorder_helper_for_delete(arr, i, root->right_son);
            return true;
        }
    };

    /**
     * helper function for the delete node function.
     * this function changes the parents poiters and the son needed to change as well
     */
    void changeParentsSon(AVL_Node<D>* parent, AVL_Node<D>* change){
        if (parent){
            //change the parents son
            if (parent->right_son && parent->right_son == change->parent)
                parent->right_son = change;
            else
                assert(parent->left_son);
                parent->left_son = change;
            //change the nodes parent:
            change->parent = parent;
        } else {
            //only the root has null parent
            if (change)
                change->parent = nullptr;
            root = change;
        }
    }

    /**
     * helper function to delete to remove node with one son
     * @param to_delete - the node we want to remove
     */
    void remove_one_son(AVL_Node<D>* to_delete){
        if (to_delete->right_son){
            changeParentsSon(to_delete->parent, to_delete->right_son);
        } else {
            assert(to_delete->left_son);
            changeParentsSon(to_delete->parent, to_delete->left_son);
        }
    }

    void swipe_nodes(AVL_Node<D>* node1, AVL_Node<D>* node2){
        assert (node1 && node2);
        AVL_Node<D> temp = node1;
        *node1 = node2;
        *node2 = temp;
    }

    /**
     * helper function to delete node.
     * the function returns the next node in-order from a given node
     */
    AVL_Node<D>* next_node(AVL_Node<D>* original){
        AVL_Node<D>* iter = original;
        while (iter){
            if (iter->right_son)
                break;
            iter = iter->parent;
        }
        if (!iter)
            return nullptr;
        else {
            while (iter->left_son)
                iter = iter->left_son;
        }
        return iter;
    }

public:

    AVL_Tree() : root(nullptr), num_of_nodes(0) {};
    /**
     * we're going post-orderly on the tree from it's root and delete it's nodes
     */
    ~AVL_Tree(){
        if(!root)
            return;
        auto nodes = new AVL_Node<D>*[num_of_nodes];
        inorder_for_delete(nodes);
        for (int i = 0; i < num_of_nodes ; ++i) {
            delete nodes[i];
        }
        delete[] nodes;
    };


    /**
     * find node with a given key and returns it's data
     * @return the node's data or nullptr if didn't find it
     */
    AVL_Node<D>* find(const int key){
        if (!root)
            return nullptr;
        AVL_Node<D>* iter = root;
        while(iter){
            if (iter->key == key){
                return iter;
            } else if(key > iter->key){
                if (!iter->right_son)
                    return nullptr;
                else
                    iter = iter->right_son;
            } else {
                assert(key < iter->key);
                if (!iter->left_son)
                    return nullptr;
                else
                    iter = iter->left_son;
            }
        }
        return nullptr;
    };

    /**
     * function that hellps with finding a node in the tree while having the node.
     * this function is usefull for checking if a node exists in the tree already
     */
    AVL_Node<D>* find(AVL_Node<D>* find_me){ //todo: maybe change to find by data
        return find(find_me->key);
    }

    /**
     * function to insert another node to the tree with a given key and
     * possibly with data;
     * @return - true if succeeded and false else
     */
    bool insert(AVL_Node<D>* to_insert){
        if (find(to_insert)){
            return false;
        }
        AVL_Node<D>* iter = root;
        if (!root){
            root = to_insert;
            num_of_nodes++;
            return true;
        }
        while (true){
            if (to_insert->key > iter->key){
                if (!iter->right_son){
                    iter->right_son = to_insert;
                    to_insert->parent = iter;
                    break;
                } else {
                    iter = iter->right_son;
                }
            } else {
                assert(iter->key > to_insert->key);
                if (!iter->left_son){
                    iter->left_son = to_insert;
                    to_insert->parent = iter;
                    break;
                } else {
                    iter = iter->left_son;
                }
            }
        }

        num_of_nodes++;
        balanceTree(to_insert);
        return true;
    };

    /**
     * function to insert a node with key and data
     * @param key = the key of the new node
     * @param to_insert = the data of the new node
     * @return - true if succeeded and false else
     */
    bool insert (int key, D& to_insert){
        auto new_node = new AVL_Node<D>(key, to_insert);
        return insert(new_node);
    }

    /**
     * helper function to have a recursive in-order
     */
    int inorder_helper(AVL_Node<D>* arr, int index, const AVL_Node<D>* node){
        if (!node)
            return index;
        else {
            //left son:
            if (node->left_son) {
                index = inorder_helper(arr, index, node->left_son);
            }
            //visit node:
            arr[index] = *node;
            //right son:
            if (node->right_son)
                inorder_helper(arr, ++index, node->right_son);
        }
        return ++index;
    }


    /**
     * inorder tour of the tree
     * the function fills the given array with the node's keys by the
     * inorder order
     */
    bool inorder(AVL_Node<D>* arr){
        if (!arr || !root)
            return false;
        else{
            int i=0;
            //left son:
            if (root->left_son)
                i = inorder_helper(arr, i, root->left_son);
            //visit node:
            arr[i] = *root;
            //right son:
            if (root->right_son)
                inorder_helper(arr, ++i, root->right_son);
            return true;
        }
    };

    /**
     * function to delete the node with the given key. if the key node doesn't exist the function does nothing
     */
    void delete_node(const int key){
        AVL_Node<D>* to_delete = find(key);
        if (!to_delete)
            return;
        else {
            if (!to_delete->right_son && !to_delete->left_son) {
                if (!to_delete->parent) {
                    delete to_delete;
                    root = nullptr;
                    return;
                } else if (to_delete->parent->right_son == to_delete) {
                    to_delete->parent->right_son = nullptr;
                } else {
                    assert(to_delete->parent->left_son == to_delete);
                    to_delete->parent->left_son = nullptr;
                }
            } else if (to_delete->right_son || to_delete->left_son) {
                remove_one_son(to_delete);
            } else {
                AVL_Node<D>* nxt = next_node(to_delete);
                swipe_nodes(to_delete, nxt);
                remove_one_son(to_delete);
            }

            delete to_delete;
            num_of_nodes--;
        }
    };
};

#endif //HW1_AVLTREE_H
