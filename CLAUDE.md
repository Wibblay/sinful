# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Scope

Sinful is a compiled language targeting x86 and MASM64 (Windows x64 ABI), built around two core design goals:

**Performance and control.** The language is intended for developers who want close-to-hardware tooling — direct control over memory, predictable codegen, and the ability to write high-performance code without fighting the language to do it.

**Human-readable expressability.** Sinful is designed for an era of AI-assisted coding: syntax should be quick to read and review at a glance, and complex operations should be expressible in few lines. The language favours clarity and density of intent over verbosity.

## Version Control

All non-trivial changes must be made on a feature branch, not directly on `main`/`master`. Branch from `main`, make changes, then open a PR. Do not commit directly to `main`.

## Build & Test

Requires CMake >= 3.25, Ninja, MSVC, vcpkg at `C:/vcpkg` (or `VCPKG_ROOT` env var).

**The build must run from a Windows environment with MSVC in PATH.** From WSL, invoke via `cmd.exe` with `vcvarsall.bat` sourced first. A reusable batch file for this lives at `C:\Windows\Temp\sinful_build.bat` (see below).

```bash
# From WSL — drive the build via cmd.exe
cmd.exe /c "C:\Windows\Temp\sinful_build.bat"
cmd.exe /c "C:\Windows\Temp\sinful_test.bat"

# Or directly from a VS Developer Command Prompt (recommended for interactive use)
cmake --preset x64-debug
cmake --build --preset x64-debug
ctest --preset x64-debug

# Run a specific test suite
out/build/x64-debug/tests/SinfulTests.exe --gtest_filter="LexerTest.*"
```

Other presets: `x64-release`, `x86-debug`, `x86-release`. Test filters: `ParserTest.*`, `GeneratorTest.*`, `SymbolTableTest.*`.

**vcpkg manifest mode** is active (`vcpkg.json` at project root). GTest is installed into `out/build/x64-debug/vcpkg_installed/` automatically during `cmake --preset`. If the install directory is missing, delete `CMakeCache.txt` from the build dir and re-run configure.

**Batch file template** (write to `C:\Windows\Temp\sinful_build.bat` before building from WSL):
```bat
@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set "PATH=%PATH%;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"
set "PATH=%PATH%;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
set VCPKG_ROOT=C:\vcpkg
cd /d "C:\Users\willb\Documents\coding-projects\Sinful\Sinful"
cmake --preset x64-debug && cmake --build --preset x64-debug
```

## Agent Usage

Use specialised agents proactively. All agents must output their progress step by step — not just a final result — so the user can see what is happening and intervene early if needed. Always include in the prompt: *"Output your progress step by step (e.g. '[1/N] Doing X...') so the user can follow along."*

**Cheap first.** Before invoking any agent, ask whether a Read, Grep, or Bash command would answer the question directly. Agents are for tasks that genuinely require multi-step reasoning or execution — not for simple lookups that can be done inline.

### Agent directory

| Agent | When to invoke |
|---|---|
| `dev-ops` | Build tool not found, wrong PATH, missing SDK, broken environment config — anything where the tool chain itself is the problem rather than the code |
| `debugger` | Logic error, test failure, or unexpected runtime behaviour where the root cause isn't obvious from reading the code. If the failure looks environmental, prefer `dev-ops` |
| `test-runner` | After any non-trivial code change, after a bug fix, after a refactor — to verify nothing regressed |
| `code-reviewer` | Before opening a PR or after a significant implementation |
| `refactor` | When code works but has become hard to read or maintain |
| `orchestrator` | Non-trivial features or changes involving architectural choices or cross-cutting concerns |
| `implementer` | Executing a spec produced by the orchestrator or debugger — do not use for open-ended tasks |
| `git-ops` | All git operations: committing, branching, pushing, PRs |
| `docs-writer` | After a feature or API change is complete and needs documentation updated |

### Typical workflows

**Bug fix:** `debugger` → `implementer` → `test-runner` → `code-reviewer` → `git-ops`

**Environment/build failure:** `dev-ops` → `test-runner` (to confirm fix)

**New feature:** `orchestrator` → `implementer` → `test-runner` → `code-reviewer` → `git-ops`

**Refactor:** `refactor` → `test-runner`

## Architecture

Pipeline: `src/main.cpp` → `Compiler::Compile()` → `Lexer` → `Parser` → `Generator` → assembly.

**Non-obvious design decisions:**

- AST nodes (`Node.hpp`) are a `std::variant` — traverse with `std::visit`, not virtual dispatch.
- `SymbolTableManager` holds a global table plus a scope stack; lookup walks innermost to outermost.
- `Generator` is two-pass: data section and text section are built independently then merged at the end.
- `Parser` does type checking inline during the recursive descent parse (not a separate pass).
- `Generator` targets Windows x64 ABI: shadow space required, I/O via `WriteConsoleA` from `kernel32`.
- `CompilerException` carries source location; `ErrorReporter` renders colored caret diagnostics.
- `src/main.cpp` accepts `<input.sin>` and optional `<output.asm>`; output defaults to alongside the input file.
