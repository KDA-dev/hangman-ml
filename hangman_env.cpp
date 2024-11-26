#include "hangman_env.h"
#include<string>
#include<vector>
#include<iostream>
#include<set>
using namespace std;
vector<string> get_gallows(int stage) {
    vector<string> res(gallows);
    switch (stage) {
    case 0:
        res[1][3] = ' ';
    case 1:
        res[2][3] = ' ';
    case 2:
        res[3][3] = ' ';
    case 3:
        res[3][2] = ' ';
    case 4:
        res[3][4] = ' ';
    case 5:
        res[4][2] = ' ';
    case 6:
        res[4][4] = ' ';
    default:
        break;
    }
    return res;
}
void hangman_env::draw_game() const {
    string filler(pattern.size() + 6, ' ');
    auto cur_gallows = get_gallows(damage);
    set<int> in_pattern;
    for (int i = 0; i < pattern.size(); i++) {
        if (pattern[i] != '*')
            in_pattern.insert(pattern[i] - 'a');
    }

    for (int i = 0; i < 2; i++)
        cout << filler << cur_gallows[i] << endl;
    cout << "\"" << pattern << "\"    " << cur_gallows[2] << endl;
    for (int i = 3; i < cur_gallows.size(); i++)
        cout << filler << cur_gallows[i] << endl;
    cout << "Used letters: ";
    for (int i = 0; i < used_letters.size(); i++) {
        if (used_letters[i] && in_pattern.count(i) == 0)
            cout << char(i + 'a') << ' ';
    }
    cout << endl;
}

hangman_env::hangman_env(const hangman_env& other) {
    damage = other.damage;
    pattern = other.pattern;
    target = other.target;
    used_letters = other.used_letters;
}

hangman_env::hangman_env() { damage = 0; }

hangman_env::hangman_env(string _target) {
    target = _target;
    used_letters.assign(ALPHABET_LEN, false);
    pattern = string(target.size(), '*');
    damage = 0;
}

vector<double> hangman_env::getDataML() {
    vector<double> v(ALPHABET_LEN * 2 + ALPHABET_LEN * MAX_WORD_LEN + 2);
    v[ALPHABET_LEN] = 1;
    for (int i = 0; i < ALPHABET_LEN; i++) {
        if (used_letters[i])
            v[i + ALPHABET_LEN + 1] = 1;
    }
    for (int i = 0; i < MAX_WORD_LEN; i++) {
        if (i < pattern.size() && pattern[i] != '*')
            v[i * ALPHABET_LEN + pattern[i] - 'a' + ALPHABET_LEN * 2 + 1] = 1;
    }
    v.back() = pattern.size();
    return v;
}

bool hangman_env::process_pattern(int c) {
    bool hit = false;
    for (int i = 0; i < target.size(); i++) {
        if (target[i] == c + 'a') {
            pattern[i] = c + 'a';
            hit = true;
        }
    }
    return hit;
}

string hangman_env::get_pattern() const {
    return pattern;
}

double hangman_env::make_turn(int c) {
    used_letters[c] = true;
    bool hit = process_pattern(c);
    if (pattern == target)
        return 10;
    if (hit)
        return 1;
    else {
        ++damage;
        return 0;
    }
}

bool hangman_env::finished() const {
    return damage == MAX_HEALTH || pattern == target;
}
vector<int> hangman_env::legal() const {
    vector<int> ans;
    for (int i = 0; i < ALPHABET_LEN; i++)
        if (!used_letters[i])
            ans.push_back(i);
    return ans;
}

bool pattern_match(const string& pattern, const string& word) {
    if (pattern.size() != word.size())
        return false;
    for (int i = 0; i < pattern.size(); i++) {
        if (pattern[i] == '*')
            continue;
        if (pattern[i] != word[i])
            return false;
    }
    return true;
}