#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

class Node {
public:
    int value;
    int rank;
    int max_rank;
    bool is_marked;
    Node *right_sibling;
    Node *left_sibling;
    Node *son;
    Node *predecessor;
    Node(int val, bool marked, Node *left, Node *right, Node *son, Node *predecessor):
        value(val), is_marked(marked), right_sibling(right),
        left_sibling(left), son(son), predecessor(predecessor), rank(0), max_rank(0) {};

    bool has_son() {return son != nullptr;}

    bool has_right_sibling() {return right_sibling != this;}

    bool has_left_sibling() {return left_sibling != this;}

    bool has_predecessor() {return predecessor != nullptr;}
};

class FibonacciHeap {
    Node *min = nullptr;
    void update_min();
    void make_node_child_of_parent(Node *parent, Node *child);
    void replace_min_with_its_children();
    void consolidate_nodes();
    void join_nodes_while_two_match_in_rank(unordered_map<int, Node*> *max_ranks, Node *node);
    Node *link(Node *node1, Node *node2);
    void setPredecessorToAllSiblings(Node *node);
    void cut_node_while_marked(Node *node);
    void split_node_with_its_predecessor(Node *node);
    void maybe_set_predecessors_son_to_nodes_sibling(Node *node);
    void dettach_node1_and_attach_next_to_node2(Node *node1, Node *node2);
    void detach_node_from_its_siblings(Node *node);
    void attach_node1_next_to_node2(Node *node1, Node *node2);
    void join_2_chains_each_with_node(Node *node1, Node *node2);
public:
    FibonacciHeap(void) {};
    void meld(FibonacciHeap *fbh);
    void insert(int);
    bool is_empty() {return min == nullptr;};
    Node *find_min() {return min;};
    int extract_min();
    void make_heap(vector<int> arr);
    void decrease_key(Node *node, int i);
    void delete_node(Node *node) {decrease_key(node, - std::numeric_limits<double>::infinity()); extract_min();}
};

void FibonacciHeap::decrease_key(Node *node, int i) {
    if(!node->has_predecessor()) {
        node->value = i;
        update_min();
        return;
    }

    Node *predecessor = node->predecessor;
    if(predecessor->value < i) {
        node->value = i;
        return;
    }

    node->value = i;
    split_node_with_its_predecessor(node);
    dettach_node1_and_attach_next_to_node2(node, min);
    cut_node_while_marked(predecessor);
    update_min();
}

void FibonacciHeap::cut_node_while_marked(Node *node) {
    if(node == nullptr || !node->has_predecessor()) {
        return;
    }

    while(node->is_marked) {
        node->is_marked = false;
        dettach_node1_and_attach_next_to_node2(node, min);
        Node * predecessor = node->predecessor;
        split_node_with_its_predecessor(node);
        node = predecessor;
    }
    if(node->has_predecessor()) {
        node->is_marked = true;
    }
}

void FibonacciHeap::detach_node_from_its_siblings(Node *node) {
    Node *nodes1_right = node->right_sibling;
    nodes1_right->left_sibling = node->left_sibling;
    node->left_sibling->right_sibling = nodes1_right;
}

void FibonacciHeap::attach_node1_next_to_node2(Node *node1, Node *node2) {
    Node *nodes2_right = node2->right_sibling;
    nodes2_right->left_sibling = node1;
    node2->right_sibling = node1;
    node1->right_sibling = nodes2_right;
    node1->left_sibling = node2;
}

void FibonacciHeap::dettach_node1_and_attach_next_to_node2(Node *node1, Node *node2) {
    if(node1->has_right_sibling()) {
        detach_node_from_its_siblings(node1);
    }
    attach_node1_next_to_node2(node1, node2);
}

void FibonacciHeap::split_node_with_its_predecessor(Node *node) {
    node->predecessor->rank -= 1;

    int max_rank_from_siblings = 0;
    int count = 0;
    Node *iter_begin = node->right_sibling;
    Node *iter_end = iter_begin->left_sibling;
    while(iter_begin != iter_end) {
        if(iter_begin->max_rank > max_rank_from_siblings) {
            max_rank_from_siblings = iter_begin->max_rank;
        }
        count++;
        iter_begin = iter_begin->right_sibling;
    }

    if(count > max_rank_from_siblings) {
        node->predecessor->max_rank = count;
    } else {
        node->predecessor->max_rank = max_rank_from_siblings;
    }

    maybe_set_predecessors_son_to_nodes_sibling(node);
    node->predecessor = nullptr;
}

void FibonacciHeap::maybe_set_predecessors_son_to_nodes_sibling(Node *node) {
    if(node == node->predecessor->son) {
        node->predecessor->son = node->right_sibling;
    }
}

void FibonacciHeap::meld(FibonacciHeap *fbh) {
    join_2_chains_each_with_node(fbh->find_min(), min);

    update_min();
}

void FibonacciHeap::join_2_chains_each_with_node(Node *node1, Node *node2) {
    Node *node1_right = node1->right_sibling;
    Node *node2_left = node2->left_sibling;

    node1->right_sibling = node2;
    node2->left_sibling = node1;
    node1_right->left_sibling = node2_left;
    node2_left->right_sibling = node1_right;
}

void FibonacciHeap::make_node_child_of_parent(Node *parent, Node *child) {
    detach_node_from_its_siblings(child);

    if (parent->has_son()) {
        attach_node1_next_to_node2(child, parent->son);
        return;
    }

    parent->son = child;
    child->left_sibling = child;
    child->right_sibling = child;
}

void FibonacciHeap::insert(int value) {
    Node *node = nullptr;
    if(!min) {
        node = new Node{value, false, nullptr, nullptr, nullptr, nullptr};
        node->left_sibling = node;
        node->right_sibling = node;
        min = node;
    } else {
        node = new Node{value, false, min, min->right_sibling, nullptr, nullptr};
        min->right_sibling->left_sibling = node;
        min->right_sibling = node;
    }

    if (min->value > node->value) {
        min = node;
    }
}

void FibonacciHeap::update_min() {
    if(is_empty()) {
        return;
    }
    Node *iter_begin = min;
    Node *iter_end = min->left_sibling;
    while(iter_begin != iter_end) {
        if(iter_begin->value < min->value) {
            min = iter_begin;
        }
        iter_begin = iter_begin->right_sibling;
    }
    if(iter_begin->value < min->value) {
        min = iter_begin;
    }
}

int FibonacciHeap::extract_min() {
    if (is_empty()) {
        throw runtime_error{"Cannot extract from empty heap"};
    }

    int result = min->value;

    replace_min_with_its_children();
    update_min();
    consolidate_nodes();

    return result;
}

void FibonacciHeap::replace_min_with_its_children() {
    if (min == nullptr) {
        return;
    }

    if(min->has_left_sibling()) {
        Node *result = min;
        min = min->left_sibling;
        detach_node_from_its_siblings(result);

        if (result->has_son()) {
            setPredecessorToAllSiblings(result->son);
            join_2_chains_each_with_node(result->son, min);
        }
    } else {
        if(min->has_son()) {
            min = min->son;
            min->predecessor = nullptr;
        } else {
            min = nullptr;
        }
    }
}

void FibonacciHeap::setPredecessorToAllSiblings(Node *node) {
    if(node == nullptr) {
        return;
    }
    Node *iter_begin = node;
    Node *iter_end = node->left_sibling;
    while(iter_begin != iter_end) {
        iter_begin->predecessor = nullptr;
        iter_begin = iter_begin->right_sibling;
    }
    iter_begin->predecessor = nullptr;
}

void FibonacciHeap::consolidate_nodes() {
    if(this->is_empty()) {
        return;
    }
    Node *iter_begin = min;
    Node *iter_end = min->left_sibling;
    unordered_map<int, Node*> max_ranks;
    while(iter_begin != iter_end) {
        auto found = max_ranks.find(iter_begin->max_rank);
        if (found != max_ranks.end()) {
            join_nodes_while_two_match_in_rank(&max_ranks, iter_begin);
            iter_begin = min;
            iter_end = min->left_sibling;
            max_ranks.clear();
        }
        max_ranks.insert({iter_begin->max_rank, iter_begin});
        iter_begin = iter_begin->right_sibling;
    }

    auto found = max_ranks.find(iter_begin->max_rank);
    if (found != max_ranks.end()) {
        join_nodes_while_two_match_in_rank(&max_ranks, iter_begin);
    }
}

void FibonacciHeap::join_nodes_while_two_match_in_rank(unordered_map<int, Node*> *max_ranks, Node *node) {
    auto found = max_ranks->find(node->max_rank);
    Node *tmp = node;
    while(found != max_ranks->end()) {
        tmp = link(found->second, tmp);
        max_ranks->erase(found->first);
        found = max_ranks->find(tmp->max_rank);
    }
}

Node *FibonacciHeap::link(Node *node1, Node *node2) {
    if(node1->value < node2->value) {
        make_node_child_of_parent(node1, node2);
        node2->predecessor = node1;
        node1->rank = node1->rank + 1; // carefull, what if updating rank on the xth level? need to update rank on 1-3 l
        node1->max_rank = max(node1->max_rank, node1->rank);
        return node1;
    }

    make_node_child_of_parent(node2, node1);
    node1->predecessor = node2;
    node2->rank = node2->rank + 1;
    node2->max_rank = max(node2->max_rank, node2->rank);
    return node2;
}

void FibonacciHeap::make_heap(vector<int> arr) {
    for (int i = 0; i < arr.size(); ++i) {
        insert(arr[i]);
    }
}

int main() {
    FibonacciHeap fbh{};
    fbh.make_heap(vector<int>{11, 12, 13,100, -1,0, 776, -2, 5, 7, 4, 9, 3, 6, 8, 2, 1, -3, 0 });

    while(!fbh.is_empty()) {
        cout << fbh.extract_min() << " ";
    }

    return 0;
}