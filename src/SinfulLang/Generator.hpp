#pragma once

#include <fstream>
#include <sstream>
#include <vector>

#include "Node.hpp"
#include "SymbolTable.hpp"
#include "Token.hpp"

class Generator
{
public:

    const std::string generate() const;

    void asmFromStatement(std::shared_ptr<Node> root);
    

	static void WriteAsm(const std::string& path, int value)
	{
        static const char* asmCode1 =
            "option casemap:none\n"
            "\n"
            "includelib kernel32.lib\n"
            "\n"
            "EXTERN GetStdHandle:PROC\n"
            "EXTERN WriteConsoleA:PROC\n"
            "EXTERN ExitProcess:PROC\n"
            "\n"
            ".data\n";


        static const char* asmCode2 =
            "msgLen  equ ($ - msg)\n"
            "\n"
            ".code\n"
            "main PROC\n"
            "        sub     rsp, 28h\n"
            "        mov     ecx, -11\n"
            "        call    GetStdHandle\n"
            "\n"
            "        mov     rcx, rax\n"
            "        lea     rdx, msg\n"
            "        mov     r8, msgLen\n"
            "        xor     r9d, r9d\n"
            "        mov     qword ptr [rsp+20h], 0\n"
            "        call    WriteConsoleA\n"
            "\n"
            "        xor     ecx, ecx\n"
            "        call    ExitProcess\n"
            "main ENDP\n"
            "END\n";

        std::ofstream out(path);

        if (!out)
            throw std::runtime_error("Failed to open assembly output file: " + path);

        out << asmCode1;
        out << "msg     db \"" << std::to_string(value) << "\", 0Ah, 0\n";
        out << asmCode2;
	}  

private:

    void generateExpr(std::shared_ptr<Node> node);
    void generateAssignment(std::shared_ptr<Node> node);

    SymbolTable _symbolTable;
    std::stringstream _dataSection;
    std::stringstream _codeSection;
    std::stringstream _stackVars;
    int _labelCounter = 0;
};