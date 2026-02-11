#include "Generator.hpp"

std::string Generator::generate()
{
    // Tear down main
    tearDownStackFrame();
    set0("ecx");
    emit("call", "ExitProcess");

    generateGlobalData();

    std::stringstream output;
    output << "option casemap:none\n";
    output << "includelib kernel32.lib\n";
    output << "EXTERN GetStdHandle:PROC\n";
    output << "EXTERN WriteConsoleA:PROC\n";
    output << "EXTERN ExitProcess:PROC\n\n";

    output << ".data\n" << _dataSection.str() << "\n";

    output << ".code\n";
    output << "main PROC\n";

    output << _codeSection.str();

    output << "main ENDP\n";
    output << "END\n";

    return output.str();
}

void Generator::generateGlobalData()
{
    _dataSection << "negative_flag db 0\n";
    _dataSection << "global_buffer db 24 dup(0)\n";
}

void Generator::asmFromStatement(std::shared_ptr<Node> root)
{
    switch (root->type())
    {
    case NodeType::Assignment:
        generateAssignment(root);
        break;
    case NodeType::Print:
        generatePrint(root);
        break;
    default:
        throw std::runtime_error("root is not valid type for statement");
    }
}

void Generator::generateExpr(const std::shared_ptr<Node> node)
{
    generateExprOperand(node->left());
    push("rax");

    generateExprOperand(node->right());
    mov("rbx", "rax");
    pop("rax");

    if (node->value() == "+") add("rax", "rbx");
    else if (node->value() == "-") sub("rax", "rbx");
    else throw std::runtime_error("unrecognised operator");
}

void Generator::generateExprOperand(const std::shared_ptr<Node> node)
{
    if (node->is(NodeType::IntLiteral))
        mov("rax", node->value());
    else if (node->is(NodeType::Variable))
    {
        if (!_symbolTable.contains(node->value()))
            throw std::runtime_error("attempted to operate on unrecognised symbol");
        mov("rax", "[rbp + " + _symbolTable.getStackOffset(node->value()) + ']');
    }
    else if (node->is(NodeType::BinExpr))
        generateExpr(node);
    else
        throw std::runtime_error("node type is not valid operand");
}

void Generator::generateAssignment(const std::shared_ptr<Node> node)
{
    std::shared_ptr<Node> leftNode = node->left();
    std::shared_ptr<Node> rightNode = node->right();

    if (!leftNode->is(NodeType::Variable))
        throw std::runtime_error("can only assign to variables");
    if (!rightNode->is({ NodeType::Variable, NodeType::IntLiteral, NodeType::BinExpr }))
        throw std::runtime_error("invalid value type for assignment");

    std::string variableName = leftNode->value();
    if (!_symbolTable.contains(variableName))
        _symbolTable.addLocalVariable(variableName);

    switch (rightNode->type())
    {
    case NodeType::Variable:
        if (!_symbolTable.contains(rightNode->value()))
            throw std::runtime_error("attempted to assign using unrecognised symbol");
        mov("rax", "[rbp + " + _symbolTable.getStackOffset(rightNode->value()) + ']');
        mov("[rbp + " + _symbolTable.getStackOffset(variableName) + ']', "rax");
        break;
    case NodeType::BinExpr:
        generateExpr(rightNode);
        mov("[rbp + " + _symbolTable.getStackOffset(variableName) + ']', "rax");
        break;
    }
}

void Generator::generatePrint(std::shared_ptr<Node> node)
{
    getStdOutHandle();

    // Evaluate the expression into RAX
    generateExprOperand(node->right());

    // Convert RAX to string (fills global_buffer and sets RCX as length)
    intToString();

    // Setup Windows API call (WriteConsoleA)
    // RDX = Buffer, R8 = Length, RCX = Handle, R9 = NULL
    mov("rdx", "rdi");      // rdi was set by intToString to the start of the string
    mov("r8", "rcx");       // rcx was set by intToString as the count
    mov("rcx", "[hStdOut]");
    set0("r9");

    // Shadow space is already reserved in main's prologue
    emit("call", "WriteConsoleA");
}

void Generator::intToString()
{
    // Convert integer in RAX to string, filling in characters from the end
    // RDI: Current pos in buffer, RCX: chars written
    std::string labelId = std::to_string(_labelCounter++);
    std::string posLabel = "pos_" + labelId;
    std::string loopLabel = "conv_" + labelId;
    std::string endLabel = "done_" + labelId;

    // Reset negative flag
    mov("byte ptr [negative_flag]", "0");
    emit("cmp", "rax, 0");
    emit("jge", posLabel);

    // Handle negative
    emit("neg", "rax");
    mov("byte ptr [negative_flag]", "1");

    emit(posLabel + ":");
    emit("lea", "rdi, [global_buffer + 22]"); // Leave space for newline
    mov("rbx", "10");
    set0("rcx");
    mov("byte ptr [rdi + 1]", "0Ah");        // Place newline
    emit("inc", "rcx");

    // Conversion Loop
    emit(loopLabel + ":");
    set0("rdx");
    emit("div", "rbx");
    add("dl", "'0'");
    mov("[rdi]", "dl");
    emit("dec", "rdi");
    emit("inc", "rcx");
    emit("test", "rax, rax");
    emit("jnz", loopLabel);

    // Add '-' if negative
    emit("cmp", "byte ptr [negative_flag], 1");
    emit("jne", endLabel);
    mov("byte ptr [rdi]", "'-'");
    emit("dec", "rdi");
    emit("inc", "rcx");

    emit(endLabel + ":");
    emit("inc", "rdi"); // Points RDI to the first character to print
}

void Generator::getStdOutHandle()
{
    if (!_hasStdOutHandle)
    {
        _hasStdOutHandle = true;

        // Define the variables in the data section
        _dataSection << "STD_OUTPUT_HANDLE equ -11\n";
        _dataSection << "hStdOut dq 0\n";

        // Emit the assembly to fetch the handle
        mov("rcx", "STD_OUTPUT_HANDLE");
        emit("call", "GetStdHandle");
        mov("[hStdOut]", "rax");
    }
}

void Generator::setupStackFrame()
{
    push("rbp"); // Save the old base pointer
    mov("rbp", "rsp"); // Set the new base pointer to the current stack pointer
    push("rdi"); // Save non-volatile registers
    push("rbx");

    // Temp: Using fixed size buffer space - will need to calculate to fit locals
    sub("rsp", "48");
}

void Generator::tearDownStackFrame()
{
    add("rsp", "48"); // Remove buffer space
    pop("rbx"); // Restore non-volatile registers
    pop("rdi");
    pop("rbp");
}

void Generator::emit(const std::string& instr, const std::string& ops)
{
    _codeSection << "    " << std::left << std::setw(8) << instr << ops << "\n";
}

void Generator::push(const std::string& reg)
{
    emit("push", reg);
}

void Generator::pop(const std::string& reg)
{
    emit("pop", reg);
}

void Generator::mov(const std::string& dest, const std::string& src)
{
    emit("mov", dest + ", " + src);
}

void Generator::add(const std::string& dest, const std::string& src)
{
    emit("add", dest + ", " + src);
}

void Generator::sub(const std::string& dest, const std::string& src)
{
    emit("sub", dest + ", " + src);
}

void Generator::set0(const std::string& reg)
{
    emit("xor", reg + ", " + reg);
}
