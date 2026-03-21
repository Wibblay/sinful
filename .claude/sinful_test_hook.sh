#!/bin/bash
# Runs build+tests when a source file is edited. Called by the PostToolUse hook.
fp=$(python3 -c "import json,sys; d=json.load(sys.stdin); print(d.get('tool_input',{}).get('file_path',''))")
case "$fp" in
  *.cpp|*.hpp|*.h|*.cmake) ;;
  *) exit 0 ;;
esac
echo "[Test Hook] $fp changed — running build+tests..."
cmd.exe /c "C:\Windows\Temp\sinful_buildtest.bat"
