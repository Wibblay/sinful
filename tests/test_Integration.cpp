#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <windows.h>

namespace fs = std::filesystem;

static std::string toNativePath(const char* p)
{
    std::string s(p);
    for (char& c : s) if (c == '/') c = '\\';
    return s;
}

static int runCommand(const std::string& cmd, std::string& output)
{
    output.clear();
    // cmd.exe /c requires the entire command to be wrapped in outer quotes when
    // the command itself starts with a quoted token (e.g. a quoted executable path).
    // Without the outer quotes cmd.exe strips the first/last quote and misparses the args.
    const bool needsOuterQuotes = !cmd.empty() && cmd.front() == '"';
    const std::string fullCmd = needsOuterQuotes
        ? ("\"" + cmd + "\" 2>&1")
        : (cmd + " 2>&1");
    FILE* pipe = _popen(fullCmd.c_str(), "r");
    if (!pipe) return -1;
    std::array<char, 256> buf{};
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
        output += buf.data();
    return _pclose(pipe);
}

static bool executableOnPath(const std::string& name)
{
    std::string out;
    return runCommand("where " + name, out) == 0;
}

static fs::path makeTempDir()
{
    char base[MAX_PATH];
    GetTempPathA(MAX_PATH, base);

    const ::testing::TestInfo* info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::string leaf = std::string("sinful_integration_") +
                       (info ? info->name() : "unknown");

    fs::path dir = fs::path(base) / leaf;
    fs::create_directories(dir);
    return dir;
}

static std::string readFile(const fs::path& p)
{
    std::ifstream f(p, std::ios::binary);
    return { std::istreambuf_iterator<char>(f), {} };
}

class IntegrationTest : public ::testing::Test
{
protected:
    static constexpr const char* kSinfulApp  = SINFUL_APP_PATH;
    static constexpr const char* kScriptsDir = INTEGRATION_SCRIPTS_DIR;

    void SetUp() override
    {
        if (!executableOnPath("ml64"))
            GTEST_SKIP() << "ml64.exe not on PATH — skipping integration test";
        if (!executableOnPath("link"))
            GTEST_SKIP() << "link.exe not on PATH — skipping integration test";
    }

    void runIntegrationTest(const fs::path& sinFile)
    {
        ASSERT_TRUE(fs::exists(sinFile))
            << "Source file not found: " << sinFile;

        fs::path expectedFile = sinFile;
        expectedFile.replace_extension(".expected");
        ASSERT_TRUE(fs::exists(expectedFile))
            << "Expected output file not found: " << expectedFile;

        fs::path tmpDir  = makeTempDir();
        fs::path asmFile = tmpDir / sinFile.stem().concat(".asm");
        fs::path objFile = tmpDir / sinFile.stem().concat(".obj");
        fs::path exeFile = tmpDir / sinFile.stem().concat(".exe");

        auto np = [](const fs::path& p) {
            return fs::path(p).make_preferred().string();
        };

        // Step 1: compile .sin -> .asm
        std::string compileCmd =
            std::string("\"") + toNativePath(kSinfulApp) + "\" \"" +
            np(sinFile) + "\" \"" + np(asmFile) + "\"";

        std::string compileOut;
        int compileRet = runCommand(compileCmd, compileOut);
        ASSERT_EQ(compileRet, 0)
            << "SinfulApp failed (exit " << compileRet << "):\n" << compileOut;
        ASSERT_TRUE(fs::exists(asmFile))
            << "SinfulApp exited 0 but no .asm produced";

        // Step 2: assemble .asm -> .obj
        std::string assembleCmd =
            "ml64 /nologo /c /Fo\"" + np(objFile) +
            "\" \"" + np(asmFile) + "\"";

        std::string assembleOut;
        int assembleRet = runCommand(assembleCmd, assembleOut);
        ASSERT_EQ(assembleRet, 0)
            << "ml64 failed (exit " << assembleRet << "):\n" << assembleOut;

        // Step 3: link .obj -> .exe
        std::string linkCmd =
            "link /nologo /subsystem:console /entry:main "
            "kernel32.lib \"" + np(objFile) +
            "\" /out:\"" + np(exeFile) + "\"";

        std::string linkOut;
        int linkRet = runCommand(linkCmd, linkOut);
        ASSERT_EQ(linkRet, 0)
            << "link failed (exit " << linkRet << "):\n" << linkOut;

        // Step 4: run and capture stdout
        std::string runOut;
        int runRet = runCommand("\"" + np(exeFile) + "\"", runOut);
        ASSERT_EQ(runRet, 0)
            << "Compiled executable exited with non-zero code: " << runRet;

        // Step 5: compare output
        std::string expected = readFile(expectedFile);
        EXPECT_EQ(runOut, expected)
            << "Output mismatch for: " << sinFile.filename();
    }
};

TEST_F(IntegrationTest, BasicArithmetic)
{
    runIntegrationTest(fs::path(kScriptsDir) / "basic_arithmetic.sin");
}

TEST_F(IntegrationTest, VariableAssignmentAndReuse)
{
    runIntegrationTest(fs::path(kScriptsDir) / "variable_reuse.sin");
}

TEST_F(IntegrationTest, UnaryMinus)
{
    runIntegrationTest(fs::path(kScriptsDir) / "unary_minus.sin");
}

TEST_F(IntegrationTest, ComparisonOperators)
{
    runIntegrationTest(fs::path(kScriptsDir) / "comparisons.sin");
}

TEST_F(IntegrationTest, BooleanLogic)
{
    runIntegrationTest(fs::path(kScriptsDir) / "boolean_logic.sin");
}

TEST_F(IntegrationTest, CompoundExpression)
{
    runIntegrationTest(fs::path(kScriptsDir) / "compound_expression.sin");
}
