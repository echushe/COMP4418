#include "Formula.h"
#include <iostream>

Formula::Formula(const Formula &fm)
    :m_left(nullptr), m_right(nullptr), m_op(fm.m_op), m_atom(fm.m_atom)
{
    // std::cout << "Constructor of Formula! " << *this << std::endl;
    if (fm.m_left)
    {
        this->m_left = std::make_shared<Formula>(*fm.m_left);
    }

    if (fm.m_right)
    {
        this->m_right = std::make_shared<Formula>(*fm.m_right);
    }

}

Formula::Formula(Formula && fm)
    :m_left(std::move(fm.m_left)), m_right(std::move(fm.m_right)), m_op(fm.m_op), m_atom(fm.m_atom)
{
    // std::cout << "Constructor of Formula! " << *this << std::endl;
}

Formula::Formula(const std::string & atom)
    :m_left(nullptr), m_right(nullptr), m_op(Operation::none), m_atom(atom)
{
    // std::cout << "Constructor of Formula! " << *this << std::endl;
}

Formula::Formula()
    : m_left(nullptr), m_right(nullptr), m_op(Operation::none), m_atom("")
{
    // std::cout << "Constructor of Formula! " << *this << std::endl;
}

Formula::Formula(const Formula & left, const Formula & right, Operation op)
    : m_left(std::make_shared<Formula>(left)), m_right(std::make_shared<Formula>(right)), m_op(op), m_atom("")
{
    // std::cout << "Constructor of Formula! " << *this << std::endl;
}

Formula::Formula(const Formula & fm, Operation op, bool left)
    : m_left(nullptr), m_right(nullptr), m_op(op), m_atom("")
{
    // std::cout << "Constructor of Formula! " << *this << std::endl;
    if (left)
    {
        this->m_left = std::make_shared<Formula>(fm);
    }
    else
    {
        this->m_right = std::make_shared<Formula>(fm);
    }
}

// The following constructor assume that the pointers of formulae are
// pointing to formulae allocated in heaps.
Formula::Formula(const std::shared_ptr<Formula> &left, const std::shared_ptr<Formula> &right, Operation op)
    : m_left(left), m_right(right), m_op(op), m_atom("")
{
    // std::cout << "Constructor of Formula! " << *this << std::endl;
}

// The following constructor assume that the pointers of formulae are
// pointing to formulae allocated in heaps.
Formula::Formula(const std::shared_ptr<Formula> &fm, Operation op, bool left)
    : m_left(nullptr), m_right(nullptr), m_op(op), m_atom("")
{
    // std::cout << "Constructor of Formula! " << *this << std::endl;
    if (left)
    {
        this->m_left = fm;
    }
    else
    {
        this->m_right = fm;
    }
}

Formula & Formula::operator = (const Formula & fm)
{
    this->m_op = fm.m_op;
    this->m_atom = fm.m_atom;
    
    if (fm.m_left)
    {
        this->m_left = std::make_shared<Formula>(*fm.m_left);
    }

    if (fm.m_right)
    {
        this->m_right = std::make_shared<Formula>(*fm.m_right);
    }

    return *this;
}

Formula & Formula::operator = (Formula && fm)
{
    this->m_op = fm.m_op;
    this->m_atom = fm.m_atom;
    this->m_left = std::move(fm.m_left);
    this->m_right = std::move(fm.m_right);

    return *this;
}

Formula::~Formula()
{    
    // std::cout << "Destructor of Formula! " << *this << std::endl;
}

void Formula::traverseAndPrint(std::stringstream & ss, const Formula &fm, int depth) const
{
    if (!fm.isAtom() && depth > 0)
        ss << "(";

    if (fm.m_left)
    {
        traverseAndPrint(ss, *fm.m_left, depth + 1);
    }

    if (fm.m_op != Operation::none)
    {

        switch (fm.m_op)
        {
        case Operation::none:
            ss << "none";
        case Operation::neg:
            ss << "neg ";
            break;
        case Operation::_and:
            ss << " and ";
            break;
        case Operation::_or:
            ss << " or ";
            break;
        case Operation::imp:
            ss << " imp ";
            break;
        case Operation::iff:
            ss << " iff ";
            break;
        case Operation::seq:
            ss << " seq ";
            break;
        case Operation::comma:
            ss << ", ";
            break;
        }
    }

    if (fm.m_right)
    {
        traverseAndPrint(ss, *fm.m_right, depth + 1);
    }

    if (!fm.m_left && !fm.m_right)
    {
        // ss << " ";
        ss << fm.m_atom;
    }

    if (!fm.isAtom() && depth > 0)
        ss << ")";
}

bool Formula::isAtom() const
{
    if (!this->m_left && !this->m_right)
    {
        return true;
    }

    return false;
}

std::shared_ptr<Formula>  Formula::getLeftPointer() const
{
    return this->m_left;
}

std::shared_ptr<Formula>  Formula::getRightPointer() const
{
    return this->m_right;
}

std::string Formula::getAtom() const
{
    return this->m_atom;
}

Operation Formula::getOperation() const
{
    return this->m_op;
}

std::ostream & operator<<(std::ostream & os, const Formula & fm)
{
    std::stringstream ss;
    fm.traverseAndPrint(ss, fm, 0);

    os << ss.str();

    return os;
}

bool operator==(const Formula & left, const Formula & right)
{

    Operation l_op = left.getOperation();
    Operation r_op = right.getOperation();

    std::string l_atom = left.getAtom();
    std::string r_atom = right.getAtom();

    if (l_op != r_op || l_atom != r_atom)
    {
        return false;
    }

    if (left.getLeftPointer() && !right.getLeftPointer())
    {
        return false;
    }
    else if (!left.getLeftPointer() && right.getLeftPointer())
    {
        return false;
    }

    if (left.getRightPointer() && !right.getRightPointer())
    {
        return false;
    }
    else if (!left.getRightPointer() && right.getRightPointer())
    {
        return false;
    }

    if (!left.getLeftPointer() && !right.getRightPointer() 
        && !left.getLeftPointer() && !right.getRightPointer())
    {
        return true;
    }

    bool left_equal = true;
    bool right_equal = true;

    if (left.getLeftPointer() && right.getLeftPointer())
    {
        left_equal = *(left.getLeftPointer()) == *(right.getLeftPointer());
    }

    if (left.getRightPointer() && right.getRightPointer())
    {
        right_equal = *(left.getRightPointer()) == *(right.getRightPointer());
    }

    return left_equal && right_equal;
}
