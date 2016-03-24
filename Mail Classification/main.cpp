//
//  main.cpp
//  Text Classification
//
//  Created by TsengKai Han on 3/3/16.
//  Copyright © 2016 TsengKai Han. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include <math.h>
#include <dirent.h>

using namespace std;

////read files into map
void readDir(string filepath, map<string, int> &words, int &mail_num, int &words_num, map<string, int> stop_words){
    //declear
    DIR *D;
    struct dirent *Dirpath;
    ifstream fin;
    
    //read all files in this direction
    D = opendir(filepath.c_str());
    while ((Dirpath = readdir(D))){
        if( strcmp(Dirpath->d_name, ".") != 0 && strcmp(Dirpath->d_name, "..")!= 0 ){//exclude dots
            //get the path
            string file_in=filepath+"/"+Dirpath->d_name;
            //open file
            fin.open(file_in.c_str(),ios::in|ios::binary);
            if (fin.is_open()){
                mail_num++;//count mail's volume
                string word;
                while (fin >> word)//read word by word
                {
                    if (!stop_words.count(word)) {//not in stop words
                        words_num++;//count total words number
                        if (words.count(word)) {//exist in words, add 1
                            words[word] = words[word] + 1;
                        }
                        else{//not exist in words, insert new one
                            words.insert(pair<string, int>(word, 1));
                        }
                    }
                }
            }
            else{
                cout<<"file open error\n";
            }
            fin.close();
        }
    }
}

////naive bayes
void NB(int &correct, int &wrong, map<string, int> stop_words){
    //declear
    map<string, int> map_ham, map_spam;//words record
    int ham_num = 0, spam_num = 0;//ham/spam mail's volume
    int ham_words = 0, spam_words = 0;//total words in ham/spam
    DIR *D;
    struct dirent *Dirpath;
    ifstream fin;
    string filepath;
    
    ////read training files
    readDir("./train/spam", map_spam, spam_num, spam_words, stop_words);//read spam data
    readDir("./train/ham", map_ham, ham_num, ham_words, stop_words);//read ham data
    
    ////read spam test files
    filepath = "./test/spam";
    D = opendir(filepath.c_str());
    while ((Dirpath = readdir(D))){
        if( strcmp(Dirpath->d_name, ".") != 0 && strcmp(Dirpath->d_name, "..")!= 0 ){//exclude dots
            //get the path
            string file_in=filepath+"/"+Dirpath->d_name;
            //open file
            fin.open(file_in.c_str(),ios::in|ios::binary);
            double p_ham = 0, p_spam = 0;//probability
            p_ham = log(double(ham_num)/double(ham_num+spam_num));//multiply P(Y=ham)
            p_spam = log(double(spam_num)/double(ham_num+spam_num));//multiply P(Y=spam)
            if (fin.is_open()){
                string word;
                ////compute
                while (fin >> word){//read word by word
                    if (!stop_words.count(word)) {//not in stop words
                        //smoothing by k = 1
                        p_ham += log((double(map_ham[word])+1)/(double(ham_words)+map_ham.size()));//multiply porbability
                        p_spam += log((double(map_spam[word])+1)/(double(spam_words)+map_spam.size()));//multiply porbability
                    }
                }//end of the file
                ////classification
                if (p_ham < p_spam) {//NB says its ham and it is ham.
                    correct++;
                }
                else{//wrong classification
                    wrong++;
                }
            }
            else{
                cout<<"file open error\n";
            }
            fin.close();
        }
    }
    ////read ham test files
    filepath = "./test/ham";
    D = opendir(filepath.c_str());
    while ((Dirpath = readdir(D))){
        if( strcmp(Dirpath->d_name, ".") != 0 && strcmp(Dirpath->d_name, "..")!= 0 ){//exclude dots
            //get the path
            string file_in=filepath+"/"+Dirpath->d_name;
            //open file
            fin.open(file_in.c_str(),ios::in|ios::binary);
            double p_ham = 0, p_spam = 0;//probability
            p_ham = log(double(ham_num)/double(ham_num+spam_num));//multiply P(Y=ham)
            p_spam = log(double(spam_num)/double(ham_num+spam_num));//multiply P(Y=spam)
            if (fin.is_open()){
                string word;
                ////compute
                while (fin >> word){//read word by word
                    if (!stop_words.count(word)) {//not in stop words
                        //smoothing by k = 1
                        p_ham += log((double(map_ham[word])+1)/(double(ham_words)+map_ham.size()));//multiply porbability
                        p_spam += log((double(map_spam[word])+1)/(double(ham_words)+map_spam.size()));//multiply porbability
                    }
                }//end of the file
                ////classification
                if (p_ham > p_spam) {//NB says its ham and it is ham.
                    correct++;
                }
                else{//wrong classification
                    wrong++;
                }
            }
            else{
                cout<<"file open error\n";
            }
            fin.close();
        }
    }
}

////LR training
void LR_train(map<string, double> &words_weight, double &w0, string filepath, bool h_s, map<string, int> stop_words, double learning_rate, double penalty){
    ////constent
    //double learning_rate = 0.01;
    //double penalty = 0.1;
    //declear
    DIR *D;
    struct dirent *Dirpath;
    ifstream fin;
    
    //read all files in this direction
    D = opendir(filepath.c_str());
    while ((Dirpath = readdir(D))){
        if( strcmp(Dirpath->d_name, ".") != 0 && strcmp(Dirpath->d_name, "..")!= 0 ){//exclude dots
            //get the path
            string file_in=filepath+"/"+Dirpath->d_name;
            //open file
            fin.open(file_in.c_str(),ios::in|ios::binary);
            if (fin.is_open()){
                map<string, int> mail_words;//record words in this mail
                string word;
                while (fin >> word)//read word by word
                {
                    if (!stop_words.count(word)) {//not in stop words
                        if (mail_words.count(word)) {//exist in words, add 1
                            mail_words[word] = mail_words[word] + 1;
                        }
                        else{//not exist in words, insert new one
                            mail_words.insert(pair<string, int>(word, 1));
                        }
                    }
                }
                //end of the mail
                
                //compute P(Y|X)
                double prob = 0;
                double sum_xw = 0;
                for (map<string, int>::iterator it = mail_words.begin(); it != mail_words.end(); it++) {//sum xiwi
                    sum_xw = sum_xw + it->second * words_weight[it->first];
                }
                
                prob = exp(w0 + sum_xw)/(1 + exp(w0 + sum_xw));
                
                //update weight
                for (map<string, int>::iterator it = mail_words.begin(); it != mail_words.end(); it++) {
                    /*if (!words_weight.count(it->first)) {
                        words_weight.insert(pair<string, double>(it->first, 1.0));
                    }*/
                    words_weight[it->first] = words_weight[it->first] + learning_rate * (it->second) * (int(h_s) - prob) - learning_rate * penalty * words_weight[it->first];
                }
                
                //update w0
                w0 = w0 + learning_rate * (int(h_s) - prob) - learning_rate * penalty * w0;
                //end of update
            }
            else{
                cout<<"file open error\n";
            }
            fin.close();
        }
    }
}


////Logistic Regression
void LR(int &correct, int &wrong, map<string, int> stop_words, double learning_rate, double penalty, int iteration){
    //declear
    map<string, double> words_weight;
    double w0 = 0;
    DIR *D;
    struct dirent *Dirpath;
    ifstream fin;
    
    ////training
    for (int i = 0; i < iteration; i++) {
        LR_train(words_weight, w0, "./train/spam", 0, stop_words, learning_rate, penalty);//train spam mail
        LR_train(words_weight, w0, "./train/ham", 1, stop_words, learning_rate, penalty);//train ham mail
    }
    
    ////testing spam
    string filepath = "./test/spam";
    D = opendir(filepath.c_str());
    while ((Dirpath = readdir(D))){
        if( strcmp(Dirpath->d_name, ".") != 0 && strcmp(Dirpath->d_name, "..")!= 0 ){//exclude dots
            //get the path
            string file_in=filepath+"/"+Dirpath->d_name;
            //open file
            fin.open(file_in.c_str(),ios::in|ios::binary);
            if (fin.is_open()){
                map<string, int> mail_words;//record words in this mail
                string word;
                while (fin >> word)//read word by word
                {
                    if (!stop_words.count(word)) {//not in stop words
                        if (mail_words.count(word)) {//exist in words, add 1
                            mail_words[word] = mail_words[word] + 1;
                        }
                        else{//not exist in words, insert new one
                            mail_words.insert(pair<string, int>(word, 1));
                        }
                    }
                }
                //end of the mail
                
                //compute P(Y|X)
                double prob = 0;
                double sum_xw = 0;
                for (map<string, int>::iterator it = mail_words.begin(); it != mail_words.end(); it++) {//sum xiwi
                    sum_xw = sum_xw + it->second * words_weight[it->first];
                }
                prob = 1/(1 + exp(w0 + sum_xw));
                prob = 1 - prob;
                
                //evaluate
                if (prob < 0.5) {//its spam
                    correct++;
                }
                else{
                    wrong++;
                }
            }
            else{
                cout<<"file open error\n";
            }
            fin.close();
        }
    }
    ////testing ham
    filepath = "./test/ham";
    D = opendir(filepath.c_str());
    while ((Dirpath = readdir(D))){
        if( strcmp(Dirpath->d_name, ".") != 0 && strcmp(Dirpath->d_name, "..")!= 0 ){//exclude dots
            //get the path
            string file_in=filepath+"/"+Dirpath->d_name;
            //open file
            fin.open(file_in.c_str(),ios::in|ios::binary);
            if (fin.is_open()){
                map<string, int> mail_words;//record words in this mail
                string word;
                while (fin >> word)//read word by word
                {
                    if (!stop_words.count(word)) {//not in stop words
                        if (mail_words.count(word)) {//exist in words, add 1
                            mail_words[word] = mail_words[word] + 1;
                        }
                        else{//not exist in words, insert new one
                            mail_words.insert(pair<string, int>(word, 1));
                        }
                    }
                }
                //end of the mail
                
                //compute P(Y|X)
                double prob = 0;
                double sum_xw = 0;
                for (map<string, int>::iterator it = mail_words.begin(); it != mail_words.end(); it++) {//sum xiwi
                    sum_xw = sum_xw + it->second * words_weight[it->first];
                }
                prob = 1/(1 + exp(w0 + sum_xw));
                prob = 1 - prob;
                
                //evaluate
                if (prob > 0.5) {//its ham
                    correct++;
                }
                else{
                    wrong++;
                }
            }
            else{
                cout<<"file open error\n";
            }
            fin.close();
        }
    }
}

int main(int argc, const char * argv[]) {
    //stop word
    map<string, int> stop_words;
    stop_words.insert(pair<string, int>(",",1));
    stop_words.insert(pair<string, int>(".",1));
    stop_words.insert(pair<string, int>("-",1));
    stop_words.insert(pair<string, int>("/",1));
    stop_words.insert(pair<string, int>(":",1));
    stop_words.insert(pair<string, int>("{",1));
    stop_words.insert(pair<string, int>("}",1));
    stop_words.insert(pair<string, int>("|",1));
    stop_words.insert(pair<string, int>("?",1));
    stop_words.insert(pair<string, int>("~",1));
    
    if (atoi(argv[1])) {
        ifstream file;
        string file_in = "stopword_list.txt";
        file.open(file_in.c_str(),ios::in|ios::binary);
        string word;
        if (file.is_open()) {
            while (file >> word) {
                stop_words.insert(pair<string, int>(word, 1));
            }
        }
        else{
            cout<<"fail opening"<<endl;
        }
    }
    ////declear
    double learning_rate = stod(argv[2]);
    double penalty = stod(argv[3]);
    int correct_NB = 0, wrong_NB = 0, correct_LR = 0, wrong_LR = 0;
    NB(correct_NB, wrong_NB, stop_words);
    LR(correct_LR, wrong_LR, stop_words, learning_rate, penalty, atoi(argv[4]));
    cout<<"Naive Bayes: \t\t";
    cout<<"("<<correct_NB<<", "<<wrong_NB<<")\t ACCURACY: "<<double(correct_NB)/double(correct_NB + wrong_NB)<<endl;
    cout<<"Logistic Regression: \t";
    cout<<"("<<correct_LR<<", "<<wrong_LR<<")\t ACCURACY: "<<double(correct_LR)/double(correct_LR + wrong_LR)<<endl;
    
    return 0;
}
