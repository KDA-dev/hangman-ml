#pragma once
#include<string>
#include<vector>
#include "matrix.h"
using namespace std;

class hangman_env
{
public:
	vector<double> getDataML();
    void draw_game() const;
	hangman_env(const hangman_env& other);
	hangman_env(string _target);
    hangman_env();
    double make_turn(int c);
    bool finished() const;
    vector<int> legal() const;
    string get_pattern() const;
private:
    bool process_pattern(int c);
	int damage;
	string pattern, target;
	vector<bool> used_letters;
};
const int MAX_HEALTH = 7, ALPHABET_LEN = 26, MAX_WORD_LEN = 6;
const vector<string> gallows = {
    "   ______  ",
    "   |    |  ",
    "   0    |  ",
    "  /|\\   |  ",
    "  / \\   |  ",
    "        |  ",
    " _______|__",
    " |/      \\|"
};
vector<string> get_gallows(int stage);
bool pattern_match(const string& pattern, const string& word);