#include "Sequent.h"
#include <iostream>
#include <queue>

Sequent::Sequent()
{
}

Sequent::Sequent(const Formula & fm)
{
    if (fm.getOperation() != Operation::seq)
    {
        return;
    }

    std::shared_ptr<Formula> left = fm.getLeftPointer();
    std::shared_ptr<Formula> right = fm.getRightPointer();

    if (left)
    {
        this->copyFormulaToLeft(*left);
    }

    if (right)
    {
        this->copyFormulaToRight(*right);
    }
}

Sequent::~Sequent()
{
}

FormulaSet::const_iterator Sequent::left_begin() const
{
    return this->left_fms.begin();
}

FormulaSet::const_iterator Sequent::left_end() const
{
    return this->left_fms.end();
}

FormulaSet::const_iterator Sequent::right_begin() const
{
    return this->right_fms.begin();
}

FormulaSet::const_iterator Sequent::right_end() const
{
    return this->right_fms.end();
}

void Sequent::copyFormulaToLeft(const Formula & fm)
{
    if (fm.isAtom())
    {
        this->left_fms.insert(fm);
        return;
    }

    if (Operation::comma == fm.getOperation())
    {
        std::shared_ptr<Formula> left = fm.getLeftPointer();
        std::shared_ptr<Formula> right = fm.getRightPointer();

        copyFormulaToLeft(*left);
        copyFormulaToLeft(*right);
    }
    else
    {
        this->left_fms.insert(fm);
    }
}

void Sequent::copyFormulaToRight(const Formula & fm)
{
    if (fm.isAtom())
    {
        this->right_fms.insert(fm);
        return;
    }

    if (Operation::comma == fm.getOperation())
    {
        std::shared_ptr<Formula> left = fm.getLeftPointer();
        std::shared_ptr<Formula> right = fm.getRightPointer();

        copyFormulaToRight(*left);
        copyFormulaToRight(*right);
    }
    else
    {
        this->right_fms.insert(fm);
    }
}

std::vector<RuleFunc> rule_funcs;

void Sequent::init()
{   
    rule_funcs.push_back(rule_p2a);
    rule_funcs.push_back(rule_p2b);
    rule_funcs.push_back(rule_p3a);
    rule_funcs.push_back(rule_p3b);
    rule_funcs.push_back(rule_p4a);
    rule_funcs.push_back(rule_p4b);
    rule_funcs.push_back(rule_p5a);
    rule_funcs.push_back(rule_p5b);
    rule_funcs.push_back(rule_p6a);
    rule_funcs.push_back(rule_p6b);
}

bool Sequent::executeProof(const Sequent & in)
{
    SolutionTree the_tree (in);

    bool success = false;
    for (int i = 0; i < 10; i++)
    {
        if (rule_funcs[i](in, &the_tree))
        {
            success = true;
            break;
        }
    }

    if (success)
    {
        std::cout << "true\n";
        printProof(the_tree);
    }
    else
    {
        std::cout << "false\n";
    }

    return success;
}

std::string Sequent::toString() const
{

    std::stringstream ss;
    ss << *this;

    return ss.str();

}

void Sequent::printProof(const SolutionTree &tr)
{
    std::size_t max_length = 0;
    std::vector<std::string> output;
    std::vector<std::string> rules;

    printProof(tr, max_length, output, rules);

    std::cout << "Proofs:\n";
    for (std::size_t i = 0; i < output.size() && i < rules.size(); i++)
    {
        std::cout << i + 1 << ".\t" << output[i];
        for (std::size_t j = 0; j < max_length + 8 - output[i].size(); j++)
        {
            std::cout << " ";
        }
        std::cout << rules[i] << "\n";
    }

    std::cout << "QED.\n";
}

void Sequent::printProof(const SolutionTree &tr, std::size_t &max_length, std::vector<std::string> &output, std::vector<std::string> &rules)
{
    for (SolutionTree *child : tr.children_1)
    {
        printProof(*child, max_length, output, rules);
        break;
    }

    for (SolutionTree *child : tr.children_2)
    {
        printProof(*child, max_length, output, rules);
        break;
    }

    std::stringstream ss;
    ss << tr.seq_1;

    if (tr.seq_2.size() > 0)
    {
        ss << "  - AND -  " << tr.seq_2;
    }

    std::string out = ss.str();
    if (out.size() > max_length)
    {
        max_length = out.size();
    }

    output.push_back(out);
    rules.push_back(tr.rule);
}

bool Sequent::rule_1(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.left_begin();
    while (iter != in.left_end())
    {
        if (!(*iter).isAtom())
        {
            return false;
        }
        iter++;
    }

    iter = in.right_begin();
    while (iter != in.right_end())
    {
        if (!(*iter).isAtom())
        {
            return false;
        }
        iter++;
    }

    iter = in.left_begin();
    while (iter != in.left_end())
    {
        if (in.right_fms.count(*iter))
        {
            tr->rule = "Rule P1";
            return true;
        }
        iter++;
    }

    return false;
}

bool Sequent::execute_subrules(const Sequent & new_seq, SolutionTree * tr)
{
    SolutionTree *new_tr = new SolutionTree(new_seq);
    tr->addChild_1(new_tr);
    tr->rule = new_seq.getRuleName();

    bool success = false;
    for (int i = 0; i < 10; i++)
    {
        if (rule_funcs[i](new_seq, new_tr))
        {
            success = true;
            break;
        }
    }

    if (!success)
    {
        tr->children_1.erase(new_tr);
        delete new_tr;
    }

    return success;
}

bool Sequent::execute_subrules(const Sequent & new_seq1, const Sequent & new_seq2, SolutionTree * tr)
{
    SolutionTree *new_tr_1 = new SolutionTree(new_seq1);
    SolutionTree *new_tr_2 = new SolutionTree(new_seq2);

    SolutionTree *tr_and = new SolutionTree(new_seq1, new_seq2);
    tr_and->addChild_1(new_tr_1);
    tr_and->addChild_2(new_tr_2);
    tr_and->rule = "AND";

    tr->addChild_1(tr_and);
    tr->rule = new_seq1.getRuleName();

    bool success1 = false;
    bool success2 = false;
    for (int i = 0; i < 10; i++)
    {
        if (rule_funcs[i](new_seq1, new_tr_1))
        {
            success1 = true;
            break;
        }
    }

    for (int i = 0; i < 10; i++)
    {
        if (rule_funcs[i](new_seq2, new_tr_2))
        {
            success2 = true;
            break;
        }
    }

    if (!success1 || !success2)
    {
        tr->children_1.erase(tr_and);
        delete tr_and;
        return false;
    }

    return true;
}

bool Sequent::rule_p2a(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.right_begin();
    std::vector<Formula> to_move;

    while (iter != in.right_end())
    {
        if ((*iter).getOperation() == Operation::neg)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }

        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy{ in };
        Formula changed_fm{ *(fm.getRightPointer()) };

        seq_copy.left_fms.insert(changed_fm);
        seq_copy.right_fms.erase(fm);

        seq_copy.setRuleName("Rule P2a");
        seq_copy.setCompName("");

        if (execute_subrules(seq_copy, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p2b(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.left_begin();
    std::vector<Formula> to_move;

    while (iter != in.left_end())
    {
        if ((*iter).getOperation() == Operation::neg)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }
        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy{ in };
        Formula changed_fm{ *(fm.getRightPointer()) };

        seq_copy.right_fms.insert(changed_fm);
        seq_copy.left_fms.erase(fm);

        seq_copy.setRuleName("Rule P2b");
        seq_copy.setCompName("");

        if (execute_subrules(seq_copy, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p3a(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.right_begin();
    std::vector<Formula> to_move;

    while (iter != in.right_end())
    {
        if ((*iter).getOperation() == Operation::_and)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }
        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy_1{ in };
        Sequent seq_copy_2{ in };

        Formula changed_fm_1{ *(fm.getLeftPointer()) };
        Formula changed_fm_2{ *(fm.getRightPointer()) };

        seq_copy_1.right_fms.insert(changed_fm_1);
        seq_copy_2.right_fms.insert(changed_fm_2);

        seq_copy_1.right_fms.erase(fm);
        seq_copy_2.right_fms.erase(fm);

        seq_copy_1.setRuleName("Rule P3a");
        seq_copy_2.setRuleName("Rule P3a");

        if (execute_subrules(seq_copy_1, seq_copy_2, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p3b(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.left_begin();
    std::vector<Formula> to_move;

    while (iter != in.left_end())
    {
        if ((*iter).getOperation() == Operation::_and)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }
        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy{ in };
        Formula changed_fm_l{ *(fm.getLeftPointer()) };
        Formula changed_fm_r{ *(fm.getRightPointer()) };

        seq_copy.left_fms.insert(changed_fm_l);
        seq_copy.left_fms.insert(changed_fm_r);
        seq_copy.left_fms.erase(fm);

        seq_copy.setRuleName("Rule P3b");
        seq_copy.setCompName("");

        if (execute_subrules(seq_copy, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p4a(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.right_begin();
    std::vector<Formula> to_move;

    while (iter != in.right_end())
    {
        if ((*iter).getOperation() == Operation::_or)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }
        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy{ in };
        Formula changed_fm_l{ *(fm.getLeftPointer()) };
        Formula changed_fm_r{ *(fm.getRightPointer()) };

        seq_copy.right_fms.insert(changed_fm_l);
        seq_copy.right_fms.insert(changed_fm_r);
        seq_copy.right_fms.erase(fm);

        seq_copy.setRuleName("Rule P4a");
        seq_copy.setCompName("");

        if (execute_subrules(seq_copy, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p4b(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.left_begin();
    std::vector<Formula> to_move;

    while (iter != in.left_end())
    {
        if ((*iter).getOperation() == Operation::_or)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }
        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy_1{ in };
        Sequent seq_copy_2{ in };

        Formula changed_fm_1{ *(fm.getLeftPointer()) };
        Formula changed_fm_2{ *(fm.getRightPointer()) };

        seq_copy_1.left_fms.insert(changed_fm_1);
        seq_copy_2.left_fms.insert(changed_fm_2);

        seq_copy_1.left_fms.erase(fm);
        seq_copy_2.left_fms.erase(fm);

        seq_copy_1.setRuleName("Rule P4b");
        seq_copy_2.setRuleName("Rule P4b");

        if (execute_subrules(seq_copy_1, seq_copy_2, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p5a(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.right_begin();
    std::vector<Formula> to_move;

    while (iter != in.right_end())
    {
        if ((*iter).getOperation() == Operation::imp)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }
        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy{ in };
        
        Formula changed_fm_l{ *(fm.getLeftPointer()) };
        Formula changed_fm_r{ *(fm.getRightPointer()) };

        seq_copy.right_fms.insert(changed_fm_r);
        seq_copy.left_fms.insert(changed_fm_l);
        seq_copy.right_fms.erase(fm);

        seq_copy.setRuleName("Rule P5a");
        seq_copy.setCompName("");

        if (execute_subrules(seq_copy, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p5b(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.left_begin();
    std::vector<Formula> to_move;

    while (iter != in.left_end())
    {
        if ((*iter).getOperation() == Operation::imp)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }
        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy_1{ in };
        Sequent seq_copy_2{ in };

        Formula changed_fm_1{ *(fm.getLeftPointer()) };
        Formula changed_fm_2{ *(fm.getRightPointer()) };

        seq_copy_1.left_fms.insert(changed_fm_2);
        seq_copy_2.right_fms.insert(changed_fm_1);

        seq_copy_1.left_fms.erase(fm);
        seq_copy_2.left_fms.erase(fm);

        seq_copy_1.setRuleName("Rule P5b");
        seq_copy_2.setRuleName("Rule P5b");

        if (execute_subrules(seq_copy_1, seq_copy_2, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p6a(const Sequent & in, SolutionTree *tr)
{
    FormulaSet::const_iterator iter = in.right_begin();
    std::vector<Formula> to_move;

    while (iter != in.right_end())
    {
        if ((*iter).getOperation() == Operation::iff)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }
        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy_1{ in };
        Sequent seq_copy_2{ in };

        Formula changed_fm_1{ *(fm.getLeftPointer()) };
        Formula changed_fm_2{ *(fm.getRightPointer()) };

        seq_copy_1.left_fms.insert(changed_fm_1);
        seq_copy_1.right_fms.insert(changed_fm_2);

        seq_copy_2.left_fms.insert(changed_fm_2);
        seq_copy_2.right_fms.insert(changed_fm_1);

        seq_copy_1.right_fms.erase(fm);
        seq_copy_2.right_fms.erase(fm);

        seq_copy_1.setRuleName("Rule P6a");
        seq_copy_2.setRuleName("Rule P6a");

        if (execute_subrules(seq_copy_1, seq_copy_2, tr))
        {
            return true;
        }
    }

    return false;
}

bool Sequent::rule_p6b(const Sequent & in, SolutionTree *tr)
{

    FormulaSet::const_iterator iter = in.left_begin();
    std::vector<Formula> to_move;

    while (iter != in.left_end())
    {
        if ((*iter).getOperation() == Operation::iff)
        {
            to_move.push_back(*iter);
        }
        iter++;
    }

    if (to_move.empty())
    {
        if (rule_1(in, tr))
        {
            return true;
        }


        return false;
    }

    for (Formula fm : to_move)
    {
        Sequent seq_copy_1{ in };
        Sequent seq_copy_2{ in };

        Formula changed_fm_1{ *(fm.getLeftPointer()) };
        Formula changed_fm_2{ *(fm.getRightPointer()) };

        seq_copy_1.left_fms.insert(changed_fm_1);
        seq_copy_1.left_fms.insert(changed_fm_2);

        seq_copy_2.right_fms.insert(changed_fm_2);
        seq_copy_2.right_fms.insert(changed_fm_1);

        seq_copy_1.left_fms.erase(fm);
        seq_copy_2.left_fms.erase(fm);

        seq_copy_1.setRuleName("Rule P6b");
        seq_copy_2.setRuleName("Rule P6b");

        if (execute_subrules(seq_copy_1, seq_copy_2, tr))
        {
            return true;
        }
    }

    return false;
}

std::ostream & operator<<(std::ostream & os, const Sequent & sq)
{
    std::string sb_l = "[";
    std::string sb_r = "] ";
    std::stringstream ss;

    FormulaSet::const_iterator itr = sq.left_begin();
    int index = 0;
    while (itr != sq.left_end())
    {
        ss << *itr;
        itr++;
        index++;

        if (itr != sq.left_end())
        {
            ss << ", ";
        }
    }

    //if (index > 0)
    //{
        os << sb_l;
        os << ss.str();
        os << sb_r;
    //}

    os << "seq [";
    itr = sq.right_begin();
    while (itr != sq.right_end())
    {
        os << *itr;
        itr++;

        if (itr != sq.right_end())
        {
            os << ", ";
        }
    }

    os << "]";

    return os;
}


