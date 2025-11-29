#pragma once

#include <fstream>
#include <vector>

#include "Token.hpp"

class Generator
{
public:
	static void WriteAsm(const std::string& path, const Token& token)
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
        out << "msg     db \"" << token.GetLabel() << "\", 0Ah, 0\n";
        out << asmCode2;
	}
};