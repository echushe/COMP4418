#pragma once
#include <string>
#include <ostream>
#include <sstream>
#include <memory>

enum class Operation
{
    none,
    neg,
    _and,
    _or,
    imp,
    iff,
    seq,
    comma
};

class Formula
{
private:
    std::shared_ptr<Formula> m_left;
    std::shared_ptr<Formula> m_right;
    Operation m_op;
    std::string m_atom;

public:
    Formula();
    Formula(const Formula &left, const Formula &right, Operation op);
    Formula(const Formula &fm, Operation op, bool left = false);
    Formula(const Formula &fm);
    Formula(Formula &&fm);
    Formula(const std::string &atom);

    Formula(const std::shared_ptr<Formula> &left, const std::shared_ptr<Formula> &right, Operation op);

    Formula(const std::shared_ptr<Formula> &fm, Operation op, bool left = false);

    Formula & operator = (const Formula &fm);
    Formula & operator = (Formula &&fm);

    ~Formula();

private:
    void traverseAndPrint(std::stringstream &ss, const Formula &fm, int depth) const;

public:
    bool isAtom() const;

    std::shared_ptr<Formula>  getLeftPointer() const;
    std::shared_ptr<Formula>  getRightPointer() const;

    std::string getAtom() const;
    Operation getOperation() const;

    static std::size_t gethash(const Formula &fm)
    {
        std::hash<std::size_t> hasher;
        std::hash<std::string> str_hasher;

        std::size_t seed = 0;
        std::size_t op = static_cast<std::size_t> (fm.getOperation());

        if (fm.getLeftPointer())
        {
            std::size_t sub_seed = gethash(*fm.getLeftPointer());
            seed ^= sub_seed + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        seed ^= hasher(op) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= str_hasher(fm.getAtom()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        if (fm.getRightPointer())
        {
            std::size_t sub_seed = gethash(*fm.getRightPointer());
            seed ^= sub_seed + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }

    friend std::ostream & operator<<(std::ostream & os, const Formula &fm);
};

std::ostream & operator<<(std::ostream & os, const Formula &fm);
bool operator == (const Formula &left, const Formula &right);

