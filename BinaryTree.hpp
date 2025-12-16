#pragma once

#include "List_header.hpp"
#include "MyQueue.hpp"

const string branch_right = "|--- ";
const string branch_left = "|__ ";

template<typename T>
struct TreeNode{
    T key;
    TreeNode* left;
    TreeNode* right;

    TreeNode() : key(T()), left(nullptr), right(nullptr) {}
    TreeNode(T value, TreeNode* leftptr, TreeNode* rightptr)
            : key(value), left(leftptr), right(rightptr){}
};

template<typename T>
class FBTree{
 private:
    TreeNode<T>* root;
    int size = 0;

 public:
    explicit FBTree() : root(nullptr) {}

    void destroy_tree(TreeNode<T>* tree_node) {
        if (tree_node != nullptr) {
            destroy_tree(tree_node -> left);
            destroy_tree(tree_node -> right);
            delete tree_node;
        }
    }

    ~FBTree() {
        destroy_tree(root);
        size = 0;
    }
    // прямой обход для поиска элемента
    bool preorder_search(const TreeNode<T>* root, T key) const {
        if (root != nullptr) {
                if (root->key == key) {
                    return true;
                }

                if (preorder_search(root->left, key)) {
                    return true;
                }
                if (preorder_search(root->right, key)) {
                    return true;
                }
            }
            return false;
    }

    bool TISMEMBER(const FBTree<T>& tree, T key) const {   // поиск элементач
        return preorder_search(tree.root, key);
    }

    void TINSERT(T key) {
        TreeNode<T>* newNode = new TreeNode<T>{key, nullptr, nullptr};
        if (root == nullptr) {
            root = newNode;
            size++;
            return;
        }

        TreeNode<T>* current = root;

        while (current) {
            if (key > current->key) {
                if (current->right != nullptr) {
                    current = current->right;
                } else {
                    current->right = newNode;
                    size++;
                    return;
                }
            } else if (key < current->key) {
                if (current->left != nullptr) {
                    current = current->left;
                } else {
                    current->left = newNode;
                    size++;
                    return;
                }
            } else {
                delete newNode;
                return;
            }
        }
    }

    bool TISFULL(TreeNode<T>* current) const {
        if ((current -> left == nullptr) ^ (current -> right == nullptr) == 1) {
            return false;
        } else if ((current -> left == nullptr
                    && current -> right == nullptr)) {
            return TISFULL(current -> left) && TISFULL(current -> right);
        }
        return true;
    }

    // обходы дерева в глубину
    void pre_order(const TreeNode<T>* root) const {  // прямой обход
        if (root != nullptr) {
            cout << root -> key << ' ';
            pre_order(root -> left);
            pre_order(root -> right);
        }
    }

    void in_order(const TreeNode<T>* root) const {  // центрированный обход
        if (root != nullptr) {
            in_order(root -> left);
            cout << root -> key << ' ';
            in_order(root -> right);
        }
    }

    void post_order(const TreeNode<T>* root) const {  // обратный обход
        if (root != nullptr) {
            post_order(root -> left);
            post_order(root -> right);
            cout << root -> key << ' ';
        }
    }

    void PRINT_preorder() const {
        pre_order(root);
        cout << endl;
    }

    void PRINT_inorder() const {
        in_order(root);
        cout << endl;
    }

    void PRINT_postorder() const {
        post_order(root);
        cout << endl;
    }

    void PRINT_BFS() const {    // вывод через обход в ширину (BFS)
        if (root == nullptr) {
            throw out_of_range("Tree is empty");
        }

        MyQueue<TreeNode<T>*> q;
        q.QPUSH(root);

        while (!q.QEMPTY()) {
            TreeNode<T>* current = q.get_head();
            q.QPOP();

            cout << current -> key << ' ';
            if (current -> left != nullptr) {
                q.QPUSH(current -> left);
            }
            if (current -> right != nullptr) {
                q.QPUSH(current -> right);
            }
        }
        cout << endl;
    }

    void print_tree_recursive(TreeNode<T>* node,
        const string& prefix, bool isLeft) const {
        if (node != nullptr) {
            cout << prefix;
            cout << (isLeft ? branch_left : branch_right);
            cout << node->key << endl;
            print_tree_recursive(node->right, prefix
                                + (isLeft ? "|   " : "    "), false);
            print_tree_recursive(node->left, prefix
                                + (isLeft ? "|   " : "    "), true);
        }
    }

    void PRINT(TreeNode<T>* root,
        const string& prefix = "", bool isLeft = true) const {
        if (root == nullptr) {
            return;
        }

        cout << prefix;
        cout << (isLeft ? "├──" : "└──");
        cout << root->key << endl;

        PRINT(root->left, prefix + (isLeft ? "│   " : "    "), true);
        PRINT(root->right, prefix + (isLeft ? "│   " : "    "), false);
    }

    void pre_order_write_file(const TreeNode<T>* root, ofstream& file) const {
        if (root != nullptr) {
            file << root -> key << ' ';
            pre_order_write_file(root -> left, file);
            pre_order_write_file(root -> right, file);
        }
    }

    // запись дерева в файл
    void tree_write_file(const string& filename) const {
        ofstream file(filename);
        if (file.is_open()) {
            file << size << ' ';
            pre_order_write_file(root, file);
        }
        file.close();
    }


    void tree_read_file(const string& filename) {   // чтение из файла
        destroy_tree();
        ifstream file(filename);
        if (is_file_empty(filename)) {
            return;
        }
        root = nullptr;

        int listsize;
        file >> listsize;
        for (int i = 0; i < listsize; i++) {
            T data;
            file >> data;
            TINSERT(data);
        }
        file.close();
    }
};