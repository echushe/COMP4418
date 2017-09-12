#pragma once
#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include "Formula.h"

class SyntaxItem
{
private:
    std::shared_ptr<Formula> m_fm;
    std::string m_str;

public:

    SyntaxItem(const std::shared_ptr<Formula> &fm)
        :m_fm(fm)
    {
    }

    SyntaxItem(const std::string &str)
        :m_fm(nullptr), m_str(str)
    {
    }

    bool isFormula()
    {
        if (this->m_fm)
        {
            return true;
        }

        return false;
    }

    std::shared_ptr<Formula> getFormula() const
    {
        return this->m_fm;
    }

    std::string getString() const
    {
        return this->m_str;
    }

    ~SyntaxItem()
    {
    }
};

class FormulaBuilder
{
public:
    FormulaBuilder();
    ~FormulaBuilder();

private:
    static bool handleFormula(std::stack<SyntaxItem> &stk, const SyntaxItem &item);
    static bool handleStr(std::stack<SyntaxItem> &stk, const SyntaxItem &item);
    static void consume_tmp(std::vector<SyntaxItem> &sv, std::string &tmp, bool &after_seq);

public:
    static void generateSyntaxVector(std::vector<SyntaxItem> &sv, const std::string &input);
    static bool buildFormula(const std::vector<SyntaxItem> &items, Formula &fm);
};

