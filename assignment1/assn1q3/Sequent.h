#pragma once
#include <vector>
#include <stack>
#include <unordered_set>
#include "Formula.h"

class Sequent;
class SolutionTree;
typedef std::unordered_set<Formula, std::size_t(*)(const Formula&)> FormulaSet;
typedef std::unordered_set<SolutionTree*> SolutionSet;
typedef bool(*RuleFunc)(const Sequent &, SolutionTree *);


class Sequent
{

private:
    FormulaSet left_fms{0, Formula::gethash};
    FormulaSet right_fms{0, Formula::gethash};
    std::string rule_name;
    std::string comp_name;

public:
    Sequent();
    Sequent(const Formula &fm);
    ~Sequent();

    FormulaSet::const_iterator left_begin() const;
    FormulaSet::const_iterator left_end() const;
    FormulaSet::const_iterator right_begin() const;
    FormulaSet::const_iterator right_end() const;
    
    void setRuleName(const std::string &name)
    {
        this->rule_name = name;
    }

    std::string getRuleName() const
    {
        return this->rule_name;
    }

    void setCompName(const std::string &name)
    {
        this->comp_name = name;
    }

    std::string getCompName() const
    {
        return this->comp_name;
    }

private:
    void copyFormulaToLeft(const Formula &fm);
    void copyFormulaToRight(const Formula &fm);

public:
    static void init();
    static bool executeProof(const Sequent & in);
    
    std::string toString() const;
    

private:
    static void printProof(const SolutionTree &tr, std::size_t &max_length, std::vector<std::string> &output, std::vector<std::string> &rules);
    static void printProof(const SolutionTree &tr);

    static bool rule_1(const Sequent &in, SolutionTree *tr);

    static bool execute_subrules(const Sequent & in, SolutionTree *tr);
    static bool execute_subrules(const Sequent & new_seq1, const Sequent & new_seq2, SolutionTree * tr);

    static bool rule_p2a(const Sequent & in, SolutionTree *tr);
    static bool rule_p2b(const Sequent & in, SolutionTree *tr);
    static bool rule_p3a(const Sequent & in, SolutionTree *tr);
    static bool rule_p3b(const Sequent & in, SolutionTree *tr);
    static bool rule_p4a(const Sequent & in, SolutionTree *tr);
    static bool rule_p4b(const Sequent & in, SolutionTree *tr);
    static bool rule_p5a(const Sequent & in, SolutionTree *tr);
    static bool rule_p5b(const Sequent & in, SolutionTree *tr);
    static bool rule_p6a(const Sequent & in, SolutionTree *tr);
    static bool rule_p6b(const Sequent & in, SolutionTree *tr);
};

class SolutionTree
{
public:
    std::string seq_1;
    std::string seq_2;
    std::string rule;
    SolutionTree *parent;
    SolutionSet children_1;
    SolutionSet children_2;

    SolutionTree(const Sequent &seq) :seq_1(seq.toString()), parent(NULL)
    {
    }

    SolutionTree(const Sequent &seq1, const Sequent &seq2) :seq_1(seq1.toString()), seq_2(seq2.toString()), parent(NULL)
    {
    }

    ~SolutionTree()
    {
        for (SolutionTree *tr : children_1)
        {
            delete tr;
        }

        for (SolutionTree *tr : children_2)
        {
            delete tr;
        }
    }

    void addChild_1(SolutionTree *subtree)
    {
        subtree->parent = this;
        children_1.insert(subtree);
    }

    void addChild_2(SolutionTree *subtree)
    {
        subtree->parent = this;
        children_2.insert(subtree);
    }
};

std::ostream & operator<<(std::ostream & os, const Sequent &sq);


