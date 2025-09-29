#!/usr/bin/env python3
"""
Check coverage completeness by comparing gcovr output with git-tracked C++ files.
This script identifies C++ files in git that are not included in the coverage report.
"""

import subprocess
import sys
import json
import re
from pathlib import Path
import tempfile

def run_command(cmd, cwd=None):
    """Run a shell command and return stdout."""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, cwd=cwd)
        if result.returncode != 0:
            print(f"Error running command '{cmd}': {result.stderr}", file=sys.stderr)
            return None
        return result.stdout.strip()
    except Exception as e:
        print(f"Exception running command '{cmd}': {e}", file=sys.stderr)
        return None

def get_git_cpp_files():
    """Get all C++ files tracked by git."""
    cpp_extensions = ['*.cpp', '*.cc', '*.cxx', '*.c++']
    all_files = set()
    
    for ext in cpp_extensions:
        output = run_command(f"git ls-files '{ext}'")
        if output:
            files = [f.strip() for f in output.split('\n') if f.strip()]
            all_files.update(files)
    
    return sorted(all_files)

def get_coverage_files_from_gcovr():
    """Run gcovr and parse the output to get covered files."""
    # Generate coverage report in text format
    gcovr_cmd = """gcovr --gcov-executable "llvm-cov gcov" --gcov-ignore-errors=no_working_dir_found \
                  --exclude ".*_deps.*" --exclude ".*catch2.*" --root . \
                  build/nonHosted build/cjunk build/example_streambuffer build/middleware build/CMakeFiles/raii.dir"""
    
    output = run_command(gcovr_cmd)
    if not output:
        print("Failed to generate coverage report", file=sys.stderr)
        return set()
    
    # Parse the gcovr text output to extract file names
    covered_files = set()
    lines = output.split('\n')
    
    # Look for file lines in the coverage report (they contain coverage percentages)
    for line in lines:
        # Match lines that look like: "path/file.cpp    Lines    Exec  Cover   Missing"
        # Skip header lines and separator lines
        if ('Lines' in line and 'Exec' in line and 'Cover' in line) or line.startswith('---'):
            continue
        if 'TOTAL' in line:
            continue
            
        # Look for lines with file paths and coverage data
        # Format: filename.cpp    numbers    numbers   percentage%   [missing lines]
        match = re.match(r'([^\s]+\.(cpp|cc|cxx|c\+\+|hpp|hxx|h\+\+))\s+\d+\s+\d+\s+\d+%', line)
        if match:
            covered_files.add(match.group(1))
    
    return covered_files

def main():
    """Main function to compare git files with coverage report."""
    print("Checking coverage completeness...")
    print("=" * 60)
    
    # Get all C++ files from git
    git_files = get_git_cpp_files()
    print(f"Found {len(git_files)} C++ files in git:")
    for f in git_files:
        print(f"  {f}")
    print()
    
    # Get files from coverage report
    covered_files = get_coverage_files_from_gcovr()
    print(f"Found {len(covered_files)} C++ files in coverage report:")
    for f in sorted(covered_files):
        print(f"  {f}")
    print()
    
    # Find files in git but not in coverage
    missing_from_coverage = set(git_files) - covered_files
    
    # Find files in coverage but not in git (shouldn't happen, but worth checking)
    extra_in_coverage = covered_files - set(git_files)
    
    print("Analysis Results:")
    print("=" * 60)
    
    if missing_from_coverage:
        print(f"❌ {len(missing_from_coverage)} C++ files in git are NOT included in coverage report:")
        for f in sorted(missing_from_coverage):
            print(f"  - {f}")
        print()
    else:
        print("✅ All C++ files in git are included in coverage report!")
        print()
    
    if extra_in_coverage:
        print(f"⚠️  {len(extra_in_coverage)} files in coverage report are NOT tracked by git:")
        for f in sorted(extra_in_coverage):
            print(f"  - {f}")
        print()
    
    # Summary
    coverage_percentage = len(covered_files) / len(git_files) * 100 if git_files else 0
    print(f"Coverage completeness: {coverage_percentage:.1f}% ({len(covered_files)}/{len(git_files)} files)")
    
    # Exit with error code if files are missing from coverage
    return 1 if missing_from_coverage else 0

if __name__ == "__main__":
    sys.exit(main())