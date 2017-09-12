#include "FormulaBuilder.h"
#include <sstream>

FormulaBuilder::FormulaBuilder()
{
}


FormulaBuilder::~FormulaBuilder()
{
}

bool FormulaBuilder::handleFormula(std::stack<SyntaxItem> &stk, const SyntaxItem &item)
{
    if (stk.empty())
    {
        stk.push(item);
    }
    else
    {
        SyntaxItem top = stk.top();
        if (top.isFormula())
        {
            return false;
        }
        else if (top.getString() == "and"
            || top.getString() == "or"
            || top.getString() == "imp"
            || top.getString() == "iff")
        {
            stk.pop();
            SyntaxItem left = stk.top();

            if (!left.isFormula())
            {
                return false;
            }

            stk.pop();

            Operation op;
            if (top.getString() == "and")
            {
                op = Operation::_and;
            }
            else if (top.getString() == "or")
            {
                op = Operation::_or;
            }
            else if (top.getString() == "imp")
            {
                op = Operation::imp;
            }
            else
            {
                op = Operation::iff;
            }

            std::shared_ptr<Formula> new_fm = std::make_shared<Formula>(left.getFormula(), item.getFormula(), op);
            SyntaxItem fm_item(new_fm);
            stk.push(fm_item);
        }
        else if (top.getString() == "comma")
        {
            stk.push(item);
        }
        else if (top.getString() == "seq")
        {
            stk.pop();

            if (stk.empty() || !stk.top().isFormula())
            {
                std::shared_ptr<Formula> new_fm = std::make_shared<Formula>(item.getFormula(), Operation::seq);
                SyntaxItem fm_item(new_fm);
                stk.push(fm_item);

                return true;
            }

            SyntaxItem left = stk.top();
            stk.pop();

            std::shared_ptr<Formula> new_fm = std::make_shared<Formula>(left.getFormula(), item.getFormula(), Operation::seq);
            SyntaxItem fm_item(new_fm);
            stk.push(fm_item);
        }
        else if (top.getString() == "neg")
        {
            stk.pop();
            std::shared_ptr<Formula> new_fm = std::make_shared<Formula>(item.getFormula(), Operation::neg);
            SyntaxItem fm_item(new_fm);
            handleFormula(stk, fm_item);
        }
        else if (top.getString() == "]" || top.getString() == ")")
        {
            return false;
        }
        else
        {
            stk.push(item);
        }
    }

    return true;
}

bool FormulaBuilder::handleStr(std::stack<SyntaxItem> &stk, const SyntaxItem &item)
{
    std::string str = item.getString();

    if (str == "(" || str == "[")
    {
        stk.push(item);
    }
    else if (str == ")" || str == "]")
    {
        if (stk.empty())
        {
            return false;
        }

        SyntaxItem top = stk.top();
        if (top.isFormula())
        {
            stk.pop();

            if (stk.empty())
            {
                return false;
            }

            if (stk.top().getString() == "(" || stk.top().getString() == "[")
            {
                stk.pop();
                handleFormula(stk, top);
            }
            else if (stk.top().getString() == "comma")
            {   
                stk.pop();
                std::vector<SyntaxItem> popped_fm_list;
                popped_fm_list.push_back(top);

                std::shared_ptr<Formula> right = top.getFormula();
                
                while (!stk.empty() && stk.top().getString() != "(" && stk.top().getString() != "[")
                {
                    if (stk.top().isFormula())
                    {
                        std::shared_ptr<Formula> new_fm = std::make_shared<Formula>(stk.top().getFormula(), right, Operation::comma);
                        right = new_fm;

                        popped_fm_list.push_back(stk.top());
                    }

                    stk.pop();
                }

                if (stk.empty())
                {
                    return false;
                }

                if (stk.top().getString() == "(" || stk.top().getString() == "[")
                {
                    stk.pop();
                    SyntaxItem fm_item(right);
                    handleFormula(stk, fm_item);
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }           
        }
        else if (stk.top().getString() == "(" || stk.top().getString() == "[")
        {
            stk.pop();
        }
        else if (stk.top().getString() == "seq")
        {
            stk.pop();
            if (stk.empty() || !stk.top().isFormula())
            {
                return false;
            }
            SyntaxItem l_top = stk.top();
            stk.pop();

            if (stk.empty() || (stk.top().getString() != "(" && stk.top().getString() != "["))
            {
                return false;
            }

            stk.pop();

            std::shared_ptr<Formula> new_fm = std::make_shared<Formula>(l_top.getFormula(), Operation::seq, true);
            SyntaxItem fm_item(new_fm);
            handleFormula(stk, fm_item);
        }
        else
        {
            return false;
        }
    }
    else if (str == "neg")
    {
        stk.push(str);
    }
    else if (str == "seq")
    {
        stk.push(str);
    }
    else if (str == "and")
    {
        if (stk.empty())
        {
            return false;
        }

        SyntaxItem top = stk.top();

        if (!top.isFormula())
        {
            return false;
        }

        stk.push(str);
    }
    else if (str == "or")
    {
        if (stk.empty())
        {
            return false;
        }

        SyntaxItem top = stk.top();

        if (!top.isFormula())
        {
            return false;
        }

        stk.push(item);
    }
    else if (str == "comma")
    {
        stk.push(item);
    }
    else if (str == "imp")
    {
        if (stk.empty())
        {
            return false;
        }

        SyntaxItem top = stk.top();

        if (!top.isFormula())
        {
            return false;
        }

        stk.push(item);
    }
    else if (str == "iff")
    {
        if (stk.empty())
        {
            return false;
        }

        SyntaxItem top = stk.top();

        if (!top.isFormula())
        {
            return false;
        }

        stk.push(item);
    }
    else
    {
        return false;
    }

    return true;
}

bool FormulaBuilder::buildFormula(const std::vector<SyntaxItem> &items, Formula &fm)
{
    std::stack<SyntaxItem> stk;
    bool success = true;

    for (SyntaxItem item : items)
    {
        if (item.isFormula())
        {
            if (!handleFormula(stk, item))
            {
                success = false;
                break;
            }
        }
        else
        {
            if (!handleStr(stk, item.getString()))
            {
                success = false;
                break;
            }
            else
            {
                // Do nothing
            }
        }
    }

    if (!stk.empty() && stk.top().getString() == "seq")
    {
        stk.pop();
        if (stk.empty() || !stk.top().isFormula())
        {
            success = false;
        }
        else
        {
            std::shared_ptr<Formula> l_fm = stk.top().getFormula();
            stk.pop();

            std::shared_ptr<Formula> new_fm = std::make_shared<Formula>(l_fm, Operation::seq, true);
            SyntaxItem fm_item(new_fm);
            success = handleFormula(stk, fm_item);
        }
    }

    if (stk.size() > 1)
    {
        success = false;
    }

    if (!success)
    {
        while (!stk.empty())
        {
            SyntaxItem top = stk.top();
            stk.pop();
        }
    }
    else if (!stk.empty())
    {
        SyntaxItem top = stk.top();
        stk.pop();
        if (top.isFormula())
        {
            fm = std::move(*(top.getFormula()));
        }
        else
        {
            success = false;
        }
    }

    return success;
}


void FormulaBuilder::consume_tmp(std::vector<SyntaxItem> &sv, std::string &tmp, bool &after_seq)
{
    if (tmp.size() > 0)
    {
        if (tmp == "neg"
            || tmp == "and"
            || tmp == "or"
            || tmp == "imp"
            || tmp == "iff"
            || tmp == "seq")
        {
            if (tmp == "seq")
            {
                after_seq = true;
            }

            SyntaxItem item(tmp);
            sv.push_back(item);
        }
        else
        {
            std::shared_ptr<Formula> new_fm = std::make_shared<Formula>(tmp);
            SyntaxItem item(new_fm);
            sv.push_back(item);
        }
    }
}

void FormulaBuilder::generateSyntaxVector(std::vector<SyntaxItem> &sv, const std::string &input)
{
    // Read lines from the tream
    std::istringstream string_file(input);
    std::string line;
    bool after_seq = false;

    while (getline(string_file, line, ' '))
    {
        if (line.size() == 0)
        {
            continue;
        }

        if (line == "neg"
            || line == "and"
            || line == "or"
            || line == "imp"
            || line == "iff"
            || line == "seq"
            || line == "["
            || line == "]"
            || line == "("
            || line == ")")
        {
            if (line == "seq")
            {
                after_seq = true;
            }

            SyntaxItem item(line);
            sv.push_back(item);
        }
        else
        {
            std::string tmp;
            for (size_t i = 0; i < line.size(); i++)
            {
                if (line[i] == '[' || line[i] == ']' || line[i] == ')' || line[i] == '(')
                {
                    consume_tmp(sv, tmp, after_seq);
                    tmp = "";

                    SyntaxItem item{ std::string{ line[i] } };
                    sv.push_back(item);
                }
                else if (line[i] == ',')
                {
                    consume_tmp(sv, tmp, after_seq);

                    tmp = "";

                    SyntaxItem item("comma");
                    sv.push_back(item);
                }
                else
                {
                    tmp += line[i];
                }
            }

            consume_tmp(sv, tmp, after_seq);
        }
    }

    for (SyntaxItem item : sv)
    {
        if (item.isFormula())
        {
            //std::cout << item.getFormula().getAtom() << " ";
        }
        else
        {
            //std::cout << item.getString() << " ";
        }
    }
    //std::cout << std::endl;
}
