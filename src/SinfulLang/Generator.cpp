#include "Generator.hpp"

const std::string Generator::generate() const
{
    std::stringstream output;
    output << "option casemap:none\n";
    output << "\n";
    output << "includelib kernel32.lib\n";
    output << "\n";
    output << "EXTERN GetStdHandle:PROC\n";
    output << "EXTERN WriteConsoleA:PROC\n";
    output << "EXTERN ExitProcess:PROC\n";
    output << "\n";
}

void Generator::asmFromStatement(std::shared_ptr<Node> root)
{
    if (root->is(NodeType::Assignment))
        generateAssignment(root);
}

void Generator::generateExpr(std::shared_ptr<Node> node)
{

}

void Generator::generateAssignment(std::shared_ptr<Node> node)
{
    std::shared_ptr<Node> leftNode = node->left();
    std::shared_ptr<Node> rightNode = node->right();

    if (!leftNode->is(NodeType::Variable))
        throw std::runtime_error("can only assign to variables");
    if (!rightNode->is({ NodeType::Variable, NodeType::IntLiteral, NodeType::BinExpr }))
        throw std::runtime_error("invalid value type for assignment");

    std::string variableName = leftNode->value();
    if (!_symbolTable.contains(variableName))
    {
        _dataSection << variableName
            << "  DQ  "
            << rightNode->is(NodeType::IntLiteral)
                ? rightNode->value()
                : "0"
            << "\n";
        _symbolTable.addLocalVariable(variableName);
    }

    switch (rightNode->type())
    {
    case NodeType::Variable:
        if (!_symbolTable.contains(rightNode->value()))
            throw std::runtime_error("attempted to assign using unrecognised symbol");
        _codeSection << "mov     rax, " << rightNode->value() << "\n";
        _codeSection << "mov     " << variableName << ", rax\n";
        break;
    case NodeType::BinExpr:
        generateExpr(rightNode);
        _codeSection << "mov     " << variableName << ", rax\n";
        break;
    }
}
