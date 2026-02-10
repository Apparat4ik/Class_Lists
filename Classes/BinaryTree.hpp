#pragma once

#include "List_header.hpp"
#include "MyQueue.hpp"
#include <sstream>

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

    // Вспомогательные методы для сериализации
    void serialize_text_helper(const TreeNode<T>* node, ofstream& out) const {
        if (node == nullptr) {
            out << "# ";
            return;
        }
        out << node->key << " ";
        serialize_text_helper(node->left, out);
        serialize_text_helper(node->right, out);
    }

    TreeNode<T>* deserialize_text_helper(istringstream& iss) {
        string value;
        if (!(iss >> value) || value == "#") {
            return nullptr;
        }

        T key;
        istringstream converter(value);
        converter >> key;

        TreeNode<T>* node = new TreeNode<T>(key, nullptr, nullptr);
        node->left = deserialize_text_helper(iss);
        node->right = deserialize_text_helper(iss);
        
        return node;
    }

    void serialize_binary_helper(const TreeNode<T>* node, ofstream& out) const {
        if (node == nullptr) {
            bool is_null = true;
            out.write(reinterpret_cast<const char*>(&is_null), sizeof(bool));
            return;
        }

        bool is_null = false;
        out.write(reinterpret_cast<const char*>(&is_null), sizeof(bool));
        out.write(reinterpret_cast<const char*>(&node->key), sizeof(T));
        
        serialize_binary_helper(node->left, out);
        serialize_binary_helper(node->right, out);
    }

    TreeNode<T>* deserialize_binary_helper(ifstream& in) {
        bool is_null;
        in.read(reinterpret_cast<char*>(&is_null), sizeof(bool));
        
        if (is_null) {
            return nullptr;
        }

        T key;
        in.read(reinterpret_cast<char*>(&key), sizeof(T));
        
        TreeNode<T>* node = new TreeNode<T>(key, nullptr, nullptr);
        node->left = deserialize_binary_helper(in);
        node->right = deserialize_binary_helper(in);
        
        return node;
    }

    int count_nodes(const TreeNode<T>* node) const {
        if (node == nullptr) {
            return 0;
        }
        return 1 + count_nodes(node->left) + count_nodes(node->right);
    }

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


     // Текстовая сериализация
    void SERIALIZE_TEXT(const string& filename) const {
        ofstream out(filename);
        if (!out.is_open()) {
            throw runtime_error("Cannot open file for writing: " + filename);
        }
        
        serialize_text_helper(root, out);
        out.close();
    }

    // Текстовая десериализация
    void DESERIALIZE_TEXT(const string& filename) {
        ifstream in(filename);
        if (!in.is_open()) {
            throw runtime_error("Cannot open file for reading: " + filename);
        }

        destroy_tree(root);
        root = nullptr;
        size = 0;

        stringstream buffer;
        buffer << in.rdbuf();
        string content = buffer.str();
        istringstream iss(content);
        
        root = deserialize_text_helper(iss);
        size = count_nodes(root);
        
        in.close();
    }

    // Бинарная сериализация
    void SERIALIZE_BINARY(const string& filename) const {
        ofstream out(filename, ios::binary);
        if (!out.is_open()) {
            throw runtime_error("Cannot open file for writing: " + filename);
        }
        

        out.write(reinterpret_cast<const char*>(&size), sizeof(int));
        

        serialize_binary_helper(root, out);
        out.close();
    }

    // Бинарная десериализация
    void DESERIALIZE_BINARY(const string& filename) {
        ifstream in(filename, ios::binary);
        if (!in.is_open()) {
            throw runtime_error("Cannot open file for reading: " + filename);
        }

        destroy_tree(root);
        root = nullptr;
        

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        

        root = deserialize_binary_helper(in);
        
        in.close();
    }
};