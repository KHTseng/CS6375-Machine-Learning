//
//  main.cpp
//  DecisionTree
//
//  Created by TsengKai Han on 2/5/16.
//  Copyright © 2016 TsengKai Han. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <set>
#include <stack>
#include <time.h>
using namespace std;

////tree class
class node{
public:
    int attribute;
    node *left, *right, *parent;
    bool isLeaf, TorF;
    int lvl;
    node(){
        attribute = 0;
        left = NULL;
        right = NULL;
        parent = NULL;
        isLeaf = 0;
        lvl = 0;
        TorF = 0;
    }
};

class tree{
public:
    node* root;
};

////copy tree
void copyTree(node* n1, node* n2){
    if (n1->isLeaf == 1){//is leaf node
        n2->attribute = n1->attribute;
        n2->isLeaf = 1;
        n2->lvl = n1->lvl;
        n2->TorF = n1->TorF;
    }
    else{
        n2->attribute = n1->attribute;
        n2->isLeaf = 0;
        n2->lvl = n1->lvl;
        n2->TorF = n1->TorF;
        
        node* leftChild = new node; //create a left node to be child of new node
        n2->left = leftChild;
        leftChild->parent = n2;
        
        node* rightChild = new node; //create a right node
        n2->right = rightChild;
        rightChild->parent = n2;
        
        copyTree(n1->left, leftChild); //call copy function on subtree of left child
        copyTree(n1->right, rightChild); //call copy function on subtree of right child
    }
}
////readdata
void readData(string filename, vector<vector<bool> > &dataSet, vector<string> &attribute){// V
    //vector<vector<bool> > dataSet;
    //vector<string> attribute;
    vector<bool> temp;
    ifstream file(filename);
    string line, att;
    getline(file, line);//input attribute
    for (int i = 0; i < line.size(); i++) {//separate into vector
        if(line[i] == ','){
            attribute.push_back(att);
            att.clear();
        }
        else if(i == line.size() - 1){
            att.append(1, line[i]);
            attribute.push_back(att);
            att.clear();
        }
        else{
            att.append(1, line[i]);
        }
    }
    while (getline(file, line)) {//get one line
        for (int i = 0; i < line.size(); i++) {//separate into vector
            if(line[i] != ','){
                temp.push_back(bool(line[i] - 48));
            }
        }
        dataSet.push_back(temp);
        temp.resize(0);
    }
}

////calculate entropy
double entropy(vector<vector<bool> > s){//V
    double p = 0.0;
    double n = 0.0;
    for (int i = 0; i < s.size(); i++) {
        if (s[i].back() == 0) {
            n++;
        }
        else{
            p++;
        }
    }
    if (p == 0 || n == 0) {
        return 0;
    }
    else{
        return -1 * (p/(p+n)) * (log2(p/(p+n))) - ((n/(p+n)) * (log2(n/(p+n))));
    }
}

////calculate impurity
double impurity(vector<vector<bool> > s){
    double p = 0.0;
    double n = 0.0;
    for (int i = 0; i < s.size(); i++) {
        if (s[i].back() == 0) {
            n++;
        }
        else{
            p++;
        }
    }
    if (p == 0 || n == 0) {
        return 0;
    }
    else{
        return (p/s.size()) * (n/s.size());
    }
}

////calculate gain
double gain(vector<vector<bool> > s, int val, bool heuristc){//V
    vector<vector<bool> > s_p, s_n;
    for (int i = 0; i < s.size(); i++) {
        if (s[i][val] == 0) {//negitive
            s_n.push_back(s[i]);
        }
        else{//positive
            s_p.push_back(s[i]);
        }
    }
    if (heuristc == 0) {
        return entropy(s) - ((double(s_n.size())/double(s.size())) * entropy(s_n)) - ((double(s_p.size())/double(s.size())) * entropy(s_p));
    }
    else{
        return impurity(s) - ((double(s_n.size())/double(s.size())) * impurity(s_n)) - ((double(s_p.size())/double(s.size())) * impurity(s_p));
    }
}

////ID3
void ID3(node *now, vector<vector<bool> > s, set<int> avalible, bool heuristic){
    ////end condition
    int count = 0;
    for (int i = 0; i < s.size(); i++) {//count p&n class
        if (s[i].back() == 1) {
            count++;
        }
        else{
            count--;
        }
    }
    if (count == s.size()) {// all the positvie
        now->attribute = 1;
        now->isLeaf = 1;
        //cout<<"true ";
        return;
    }
    if (count == -1 * s.size()) {// all the nigitive
        now->attribute = 0;
        now->isLeaf = 1;
        //cout<<"false ";
        return;
    }
    if (avalible.size() == 0) {//avalible empty
        now->isLeaf = 1;
        if (count >= 0) {
            now->attribute = 1;
        }
        else{
            now->attribute = 1;
        }
        return;
    }
    
    ////find best attribute
    double maxGain = -1.0;
    int val = 0;
    for (set<int>::iterator it = avalible.begin(); it != avalible.end(); it++) {
        if ( gain(s, *it, heuristic) > maxGain) {//bigger and not used
            maxGain = gain(s, *it, heuristic);
            val = *it;
        }
    }
    ////update attribute to tree
    now->attribute = val;
    
    ////create new node to next lvl
    node* new_l = (node*) new node;
    node* new_r = (node*) new node;
    now->left = new_l;
    now->right = new_r;
    new_l->parent = now;
    new_r->parent = now;
    new_l->lvl = now->lvl + 1;
    new_r->lvl = now->lvl + 1;
    new_l->TorF = 0;
    new_r->TorF = 1;
    
    ////undate s
    vector<vector<bool> > s_n, s_p;
    for (int i = 0; i < s.size(); i++) {
        if (s[i][val] == 0) {//negitive
            s_n.push_back(s[i]);
        }
        else{//positive
            s_p.push_back(s[i]);
        }
    }
    
    ////update avalible
    avalible.erase(val);
    
    ////recursive
    ID3(new_l, s_n, avalible, heuristic);
    ID3(new_r, s_p, avalible, heuristic);
    
}

////print tree
void printTree(tree t, vector<string> attribute){
    stack<node> dfs;//imply dfs
    dfs.push(*t.root->right);
    dfs.push(*t.root->left);
    while (!dfs.empty()) {
        node temp = dfs.top();
        dfs.pop();
        ////print
        for (int i = 0; i < temp.parent->lvl; i++) {
            cout<<"| ";
        }
        cout<<attribute[temp.parent->attribute]<<" = "<<temp.TorF<<" : ";
        if (temp.isLeaf == 1) {
            cout<<temp.attribute;
        }
        cout<<endl;
        ////push nonleaf node into stack
        if (temp.isLeaf == 0) {
            dfs.push(*temp.right);
            dfs.push(*temp.left);
        }
    }
}

////count accuracy
double accuracy(tree t, vector<vector<bool> > set){
    int correct = 0;
    for (int i = 0; i < set.size(); i++) {//through all set
        node *ptr = t.root;
        while (ptr->isLeaf == 0) {// not leaf node
            if (set[i][ptr->attribute] == 0) {//false
                ptr = ptr->left;
            }
            else{//true
                ptr = ptr->right;
            }
        }
        if (ptr->attribute == set[i].back()) {//correct!!
            correct++;
        }
    }
    return double(correct)/double(set.size());
}
////post-pruning
tree post_pruning(tree t, vector<vector<bool> > v_set, int L, int K, vector<vector<bool> > t_set){
    tree bestTree;
    node* firstNode_b = new node;
    bestTree.root = firstNode_b;
    copyTree(t.root, bestTree.root);//copy best tree
    for (int i = 0; i < L; i++) {
        tree newTree;
        node* firstNode_n = new node;
        newTree.root = firstNode_n;
        copyTree(t.root, newTree.root);//copy new tree
        srand (int(time(NULL)));
        int M = rand() % K + 1;
        for (int j = 0; j < M; j++) {
            vector<node*> nodeArray;
            stack<node*> dfs;
            dfs.push(newTree.root);
            nodeArray.push_back(newTree.root);
            while (!dfs.empty()) {//mark all nodes in the tree
                node *temp;
                temp = dfs.top();
                dfs.pop();
                if (temp->isLeaf == 0) {//internal node
                    nodeArray.push_back(temp);
                    dfs.push(temp->left);
                    dfs.push(temp->right);
                }
            }
            ////random select node and prune
            srand (int(time(NULL)));
            int N = rand() % nodeArray.size();
            //// as true leaf node
            double acc_p = 0.0, acc_n = 0.0;
            nodeArray[N]->attribute = 1;
            nodeArray[N]->isLeaf = 1;
            acc_p = accuracy(newTree, t_set);
            ////as false leaf node
            nodeArray[N]->attribute = 0;
            acc_n = accuracy(newTree, t_set);
            if (acc_p > acc_n) {//more true
                nodeArray[N]->attribute = 1;
            }
            else{//more false
                nodeArray[N]->attribute = 0;
            }
        }
        if (accuracy(bestTree, v_set) < accuracy(newTree, v_set)) {
            bestTree = newTree;
        }
    }
    return bestTree;
}
int main(int argc, const char * argv[]) {
    ////declare
    vector<vector<bool> > test_set, training_set, validation_set;
    vector<string> attribute1, attribute2, attribute3;
    set<int> avalible;
    tree decisionTree_e, decisionTree_i;
    node firstNode_e, firstNode_i;
    decisionTree_e.root = &firstNode_e;//create tree
    decisionTree_i.root = &firstNode_i;//create tree
    int L = atoi(argv[1]), K = atoi(argv[2]);//input L K from command line
    bool toPrint = string(argv[6]).size() == 3? 1:0;
    
    ////read data
    readData(argv[5], test_set, attribute1);
    readData(argv[3], training_set, attribute2);
    readData(argv[4], validation_set, attribute3);
    
    ////create avalible
    for (int i = 0; i < attribute2.size() - 1; i++) {// not include "class"
        avalible.insert(i);
    }
    
    ////create decision tree
    ID3(&firstNode_e, training_set, avalible, 0);//heuristic = 0 => entropy
    ID3(&firstNode_i, training_set, avalible, 1);//heuristic = 1 => impurity
    
    ////print tree
    if (toPrint) {
        cout<<"Decision tree of entropy: "<<endl;
        printTree(decisionTree_e, attribute2);
        cout<<endl<<"Decision tree of impurity: "<<endl;
        printTree(decisionTree_i, attribute2);
    }
    
    ////accuracy
    cout<<"Accuracy by using entropy: "<<accuracy(decisionTree_e, test_set)<<endl;
    cout<<"Accuracy by using impurity: "<<accuracy(decisionTree_i, test_set)<<endl;
    
    ////pruning
    tree pruningTree_e, pruningTree_i;
    pruningTree_e = post_pruning(decisionTree_e, validation_set, L, K, training_set);
    pruningTree_i = post_pruning(decisionTree_i, validation_set, L, K, training_set);
    
    ////print pruning tree
    if (toPrint) {
        cout<<endl<<"Decision tree of entropy after pruning: "<<endl;
        printTree(pruningTree_e, attribute2);
        cout<<endl<<"Decision tree of impurity after pruning: "<<endl;
        printTree(pruningTree_i, attribute2);
    }
    
    ////accuracy after pruning
    cout<<"Accuracy by using impurity after pruning: "<<accuracy(pruningTree_e, test_set)<<endl;
    cout<<"Accuracy by using impurity after pruning: "<<accuracy(pruningTree_i, test_set)<<endl;
    
    return 0;
}
