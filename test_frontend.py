#!/usr/bin/env python3

"""
Frontend testing script for minic compiler
Tests flex+bison, antlr4, and recursive descent frontends
"""

import os
import sys
import subprocess
import tempfile
import shutil
import time
from pathlib import Path
from dataclasses import dataclass
from typing import Optional, List, Tuple
import argparse

@dataclass
class TestResult:
    """Test result for a single test case and frontend"""
    test_name: str
    frontend: str
    compile_success: bool
    compile_time: float
    ir_generated: bool
    runtime_success: bool
    runtime_time: float
    output_matches_reference: bool
    exit_code_matches_reference: bool
    error_message: Optional[str] = None

class FrontendTester:
    def __init__(self, project_root: str, timeout: int = 1, test_runtime: bool = True, frontends: List[str] = None, 
                 include_for: bool = False, include_cfg: bool = False):
        self.project_root = Path(project_root)
        self.timeout = timeout
        self.test_runtime = test_runtime
        self.frontends = frontends or ["flex_bison", "antlr4", "recursive_descent"]
        self.include_for = include_for
        self.include_cfg = include_cfg
        
        self.minic = self.project_root / "build" / "minic"
        self.ir_compiler = self.project_root / "tools" / "IRCompiler" / "Linux-aarch64" / "Ubuntu-22.04" / "IRCompiler"
        self.test_dir = self.project_root / "tests" / "commonclasstestcases" / "function"
        
        # Create temporary directory inside project
        self.temp_dir = self.project_root / "test_results"
        self.temp_dir.mkdir(exist_ok=True)
        
        # Create subdirectories with timestamp
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        self.run_dir = self.temp_dir / f"run_{timestamp}"
        self.log_dir = self.run_dir / "logs"
        self.ir_dir = self.run_dir / "ir"
        self.output_dir = self.run_dir / "output"
        
        # Create subdirectories
        self.run_dir.mkdir(exist_ok=True)
        self.log_dir.mkdir(exist_ok=True)
        self.ir_dir.mkdir(exist_ok=True)
        self.output_dir.mkdir(exist_ok=True)
        
        test_types = ["basic (1-143)"]
        if self.include_for:
            test_types.append("for (144-160)")
        if self.include_cfg:
            test_types.append("cfg (161-162)")
        print(f"Test types: {', '.join(test_types)}")
        print(f"Test directory: {self.run_dir}")
        print(f"Logs: {self.log_dir}")
        
    def cleanup(self):
        """Cleanup temporary directory if requested"""
        # No automatic cleanup - let user decide
    
    def run_with_timeout(self, cmd: List[str], timeout: int, 
                        stdout_file: Optional[str] = None, 
                        stderr_file: Optional[str] = None,
                        env: Optional[dict] = None) -> Tuple[int, float]:
        """Run command with timeout and return (exit_code, elapsed_time)"""
        start_time = time.time()
        
        stdout_handle = open(stdout_file, 'w') if stdout_file else subprocess.PIPE
        stderr_handle = open(stderr_file, 'w') if stderr_file else subprocess.PIPE
        
        try:
            # Use Popen for better control over process termination
            process = subprocess.Popen(
                cmd,
                stdout=stdout_handle,
                stderr=stderr_handle,
                cwd=self.project_root,
                env=env or os.environ
            )
            
            try:
                exit_code = process.wait(timeout=timeout)
                elapsed = time.time() - start_time
                return exit_code, elapsed
            except subprocess.TimeoutExpired:
                # Force kill the process and its children
                process.kill()
                try:
                    process.wait(timeout=5)  # Give it 5 seconds to die
                except subprocess.TimeoutExpired:
                    pass  # Process is really stuck, let it go
                elapsed = time.time() - start_time
                return -999, elapsed  # Special code for timeout
                
        finally:
            if stdout_file and stdout_handle != subprocess.PIPE:
                stdout_handle.close()
            if stderr_file and stderr_handle != subprocess.PIPE:
                stderr_handle.close()

    def run_with_timeout_and_input(self, cmd: List[str], timeout: int, 
                                  input_file: str, 
                                  stdout_file: Optional[str] = None, 
                                  stderr_file: Optional[str] = None) -> Tuple[int, float]:
        """Run command with timeout and input file, return (exit_code, elapsed_time)"""
        start_time = time.time()
        
        stdout_handle = open(stdout_file, 'w') if stdout_file else subprocess.PIPE
        stderr_handle = open(stderr_file, 'w') if stderr_file else subprocess.PIPE
        
        try:
            # Use Popen for better control over process termination
            with open(input_file, 'r') as input_handle:
                process = subprocess.Popen(
                    cmd,
                    stdin=input_handle,
                    stdout=stdout_handle,
                    stderr=stderr_handle,
                    cwd=self.project_root
                )
                
                try:
                    exit_code = process.wait(timeout=timeout)
                    elapsed = time.time() - start_time
                    return exit_code, elapsed
                except subprocess.TimeoutExpired:
                    # Force kill the process and its children
                    process.kill()
                    try:
                        process.wait(timeout=5)  # Give it 5 seconds to die
                    except subprocess.TimeoutExpired:
                        pass  # Process is really stuck, let it go
                    elapsed = time.time() - start_time
                    return -999, elapsed  # Special code for timeout
                    
        finally:
            if stdout_file and stdout_handle != subprocess.PIPE:
                stdout_handle.close()
            if stderr_file and stderr_handle != subprocess.PIPE:
                stderr_handle.close()
    
    def compile_test(self, test_file: Path, frontend: str, test_name: str) -> TestResult:
        """Compile a test file with specified frontend"""
        result = TestResult(
            test_name=test_name,
            frontend=frontend,
            compile_success=False,
            compile_time=0.0,
            ir_generated=False,
            runtime_success=False,
            runtime_time=0.0,
            output_matches_reference=False,
            exit_code_matches_reference=False
        )
        
        # Determine frontend flag
        frontend_flag = []
        if frontend == "antlr4":
            frontend_flag = ["-A"]
        elif frontend == "recursive_descent":
            frontend_flag = ["-D"]
        # flex+bison uses no flag (default)
        
        # Prepare file paths
        ir_file = self.ir_dir / f"{test_name}_{frontend}.ir"
        compile_log = self.log_dir / f"{test_name}_{frontend}_compile.log"
        
        # Compile command
        cmd = [str(self.minic), "-S", "-I"] + frontend_flag + [str(test_file), "-o", str(ir_file)]
        
        # Set environment to disable debug output
        env = os.environ.copy()
        env['DEBUG'] = '0'  # Disable debug output
        
        # Run compilation (capture stderr for debugging, stdout usually empty for compilation)
        exit_code, compile_time = self.run_with_timeout(
            cmd, 
            self.timeout,
            stdout_file=str(compile_log),  # Compilation stdout is usually empty
            stderr_file=str(compile_log),  # Capture errors and debug output
            env=env
        )
        
        result.compile_time = compile_time
        
        if exit_code == -999:
            result.error_message = f"Compilation timeout ({self.timeout}s)"
            return result
        elif exit_code != 0:
            result.error_message = f"Compilation failed with exit code {exit_code}"
            return result
        
        result.compile_success = True
        
        # Check if IR file was generated
        if ir_file.exists() and ir_file.stat().st_size > 0:
            result.ir_generated = True
        else:
            result.error_message = "IR file not generated or empty"
            return result
        
        return result
    
    def run_ir_test(self, ir_file: Path, test_name: str, frontend: str) -> Tuple[bool, float, Optional[str], int]:
        """Run IR file and return (success, time, output, exit_code)"""
        output_file = self.output_dir / f"{test_name}_{frontend}.out"
        runtime_log = self.log_dir / f"{test_name}_{frontend}_runtime.log"
        
        # Check for input file
        input_file = self.test_dir / f"{test_name}.in"
        
        cmd = [str(self.ir_compiler), "-R", str(ir_file)]
        
        # Handle input file if it exists
        if input_file.exists():
            exit_code, runtime_time = self.run_with_timeout_and_input(
                cmd,
                self.timeout,
                input_file=str(input_file),
                stdout_file=str(output_file),
                stderr_file=str(runtime_log)
            )
        else:
            exit_code, runtime_time = self.run_with_timeout(
                cmd,
                self.timeout,
                stdout_file=str(output_file),
                stderr_file=str(runtime_log)
            )
        
        if exit_code == -999:
            return False, runtime_time, "Runtime timeout", -999
        
        # Read output
        output = ""
        if output_file.exists():
            try:
                output = output_file.read_text().strip()
            except:
                output = ""
        
        return True, runtime_time, output, exit_code
    
    def test_single_file(self, test_file: Path) -> List[TestResult]:
        """Test a single file with specified frontends"""
        test_name = test_file.stem
        results = []
        
        print(f"Testing {test_name}...")
        
        # Generate reference with flex+bison if it's in the test list
        reference_output = None
        reference_exit_code = None
        
        if "flex_bison" in self.frontends:
            print(f"  Compiling with flex+bison...", end="", flush=True)
            reference_result = self.compile_test(test_file, "flex_bison", test_name)
            
            if reference_result.compile_success:
                print(f" OK ({reference_result.compile_time:.2f}s)")
            else:
                print(f" FAILED ({reference_result.error_message})")
            
            results.append(reference_result)
            
            if self.test_runtime and reference_result.compile_success and reference_result.ir_generated:
                ir_file = self.ir_dir / f"{test_name}_flex_bison.ir"
                success, runtime_time, output, exit_code = self.run_ir_test(ir_file, test_name, "flex_bison")
                reference_result.runtime_success = success
                reference_result.runtime_time = runtime_time
                reference_result.output_matches_reference = True  # Reference matches itself
                reference_result.exit_code_matches_reference = True
                
                if success:
                    reference_output = output
                    reference_exit_code = exit_code
        
        # Test other frontends
        for frontend in self.frontends:
            if frontend == "flex_bison":
                continue  # Already tested above
            
            print(f"  Compiling with {frontend}...", end="", flush=True)
            result = self.compile_test(test_file, frontend, test_name)
            
            if result.compile_success:
                print(f" OK ({result.compile_time:.2f}s)")
            else:
                print(f" FAILED ({result.error_message})")
            
            if self.test_runtime and result.compile_success and result.ir_generated:
                # Check if input file exists and show info
                input_file = self.test_dir / f"{test_name}.in"
                has_input = input_file.exists()
                if has_input:
                    print(f"    Running with input file...", end="", flush=True)
                else:
                    print(f"    Running without input...", end="", flush=True)
                
                ir_file = self.ir_dir / f"{test_name}_{frontend}.ir"
                success, runtime_time, output, exit_code = self.run_ir_test(ir_file, test_name, frontend)
                result.runtime_success = success
                result.runtime_time = runtime_time
                
                if success:
                    print(f" OK ({runtime_time:.2f}s)")
                else:
                    print(f" FAILED (Runtime timeout)" if exit_code == -999 else f" FAILED")
                
                # Compare with reference if available
                if reference_output is not None and reference_exit_code is not None:
                    result.output_matches_reference = (output == reference_output)
                    result.exit_code_matches_reference = (exit_code == reference_exit_code)
                else:
                    # No reference available, mark as successful if runtime worked
                    result.output_matches_reference = success
                    result.exit_code_matches_reference = success
            else:
                # Not testing runtime
                result.runtime_success = True
                result.output_matches_reference = True
                result.exit_code_matches_reference = True
            
            results.append(result)
        
        return results
    
    def run_tests(self, test_pattern: str = "*.c", max_tests: Optional[int] = None, max_test_number: Optional[int] = None) -> List[TestResult]:
        """Run tests on all matching files"""
        test_files = list(self.test_dir.glob(test_pattern))
        
        # Determine test number ranges based on enabled test types
        if max_test_number is None:
            max_test_number = 143  # Basic tests always included
            if self.include_cfg:
                max_test_number = 162  # Include cfg tests (161-162)
            elif self.include_for:
                max_test_number = 160  # Include for tests (144-160)
        
        # Filter by test number and test type inclusion
        filtered_files = []
        for test_file in test_files:
            # Skip non-test files
            if test_file.name in ['std.c', 'std.h', 'minicrun.sh', 'readme.md']:
                continue
                
            # Extract test number from filename (e.g., "123_test_name.c" -> 123)
            try:
                test_number = int(test_file.name.split('_')[0])
                
                # Check if this test should be included
                include_test = False
                if test_number <= 143:
                    include_test = True  # Basic tests always included
                elif 144 <= test_number <= 160 and self.include_for:
                    include_test = True  # For tests
                elif 161 <= test_number <= 162 and self.include_cfg:
                    include_test = True  # CFG tests
                
                if include_test and test_number <= max_test_number:
                    filtered_files.append(test_file)
            except (ValueError, IndexError):
                # Skip files that don't follow the naming convention
                continue
        
        test_files = sorted(filtered_files, key=lambda f: f.name)
        
        if max_tests:
            test_files = test_files[:max_tests]
        
        print(f"Found {len(test_files)} test files")
        
        all_results = []
        for i, test_file in enumerate(test_files, 1):
            print(f"[{i}/{len(test_files)}] ", end="")
            try:
                start_time = time.time()
                results = self.test_single_file(test_file)
                elapsed = time.time() - start_time
                print(f"  Total time: {elapsed:.2f}s")
                all_results.extend(results)
            except KeyboardInterrupt:
                print(f"Interrupted at {test_file.name}")
                raise
            except Exception as e:
                print(f"ERROR testing {test_file.name}: {e}")
        
        return all_results
    
    def print_summary(self, results: List[TestResult]):
        """Print test summary"""
        print("\n" + "="*80)
        print("TEST SUMMARY")
        print("="*80)
        
        for frontend in self.frontends:
            frontend_results = [r for r in results if r.frontend == frontend]
            if not frontend_results:
                continue
                
            total = len(frontend_results)
            compile_success = len([r for r in frontend_results if r.compile_success])
            ir_generated = len([r for r in frontend_results if r.ir_generated])
            
            print(f"\n{frontend.upper()}:")
            print(f"  Compilation success: {compile_success}/{total} ({100*compile_success/total:.1f}%)")
            print(f"  IR generated:        {ir_generated}/{total} ({100*ir_generated/total:.1f}%)")
            
            if self.test_runtime:
                runtime_success = len([r for r in frontend_results if r.runtime_success])
                output_matches = len([r for r in frontend_results if r.output_matches_reference])
                exit_code_matches = len([r for r in frontend_results if r.exit_code_matches_reference])
                
                print(f"  Runtime success:     {runtime_success}/{total} ({100*runtime_success/total:.1f}%)")
                if frontend != "flex_bison" and "flex_bison" in self.frontends:  # Don't compare reference with itself
                    print(f"  Output matches ref:  {output_matches}/{total} ({100*output_matches/total:.1f}%)")
                    print(f"  Exit code matches:   {exit_code_matches}/{total} ({100*exit_code_matches/total:.1f}%)")
        
        # Show failures
        print(f"\nFAILURES:")
        if self.test_runtime:
            failures = [r for r in results if not (r.compile_success and r.ir_generated and r.runtime_success and r.output_matches_reference and r.exit_code_matches_reference)]
        else:
            failures = [r for r in results if not (r.compile_success and r.ir_generated)]
        
        if failures:
            for result in failures:
                print(f"  {result.test_name} ({result.frontend}): {result.error_message or 'Output/exit code mismatch'}")
        else:
            print("  No failures!")
        
        print(f"\nResults saved in: {self.run_dir}")
        print(f"Log files: {self.log_dir}")
        print(f"IR files: {self.ir_dir}")
        if self.test_runtime:
            print(f"Output files: {self.output_dir}")

def main():
    parser = argparse.ArgumentParser(description="Test minic frontend implementations")
    
    # Testing configuration
    parser.add_argument("--frontends", nargs="+", 
                       choices=["flex_bison", "antlr4", "recursive_descent"],
                       default=["flex_bison", "antlr4", "recursive_descent"],
                       help="Which frontends to test")
    parser.add_argument("--compile-only", action="store_true", 
                       help="Only test compilation, skip runtime testing")
    parser.add_argument("--include-for", action="store_true",
                       help="Include for loop tests (144-160)")
    parser.add_argument("--include-cfg", action="store_true", 
                       help="Include CFG tests (161-162)")
    
    # Test selection
    parser.add_argument("--pattern", type=str, default="*.c", 
                       help="Test file pattern (default: *.c)")
    parser.add_argument("--max-tests", type=int, 
                       help="Maximum number of tests to run")
    parser.add_argument("--max-test-number", type=int,
                       help="Maximum test number to include (default: 143, 160 with --include-for, 162 with --include-cfg)")
    parser.add_argument("--test-files", nargs="+",
                       help="Specific test files to run")
    
    # Performance options
    parser.add_argument("--timeout", type=int, default=15, 
                       help="Timeout for each compilation/execution (seconds)")
    
    # Output options
    parser.add_argument("--verbose", "-v", action="store_true",
                       help="Verbose output")
    
    args = parser.parse_args()
    
    # Get project root
    project_root = Path(__file__).parent.absolute()
    
    # Verify dependencies
    minic = project_root / "build" / "minic"
    if not minic.exists():
        print("ERROR: minic not found. Please run 'cd build && make' first.")
        sys.exit(1)
    
    # Check test directory exists
    test_dir = project_root / "tests" / "commonclasstestcases" / "function"
    if not test_dir.exists():
        print(f"ERROR: Test directory not found: {test_dir}")
        sys.exit(1)

    test_runtime = not args.compile_only
    if test_runtime:
        ir_compiler = project_root / "tools" / "IRCompiler" / "Linux-aarch64" / "Ubuntu-22.04" / "IRCompiler"
        if not ir_compiler.exists():
            print(f"ERROR: IRCompiler not found at {ir_compiler}")
            print("Use --compile-only to skip runtime testing")
            sys.exit(1)
    
    # Run tests
    tester = FrontendTester(
        str(project_root), 
        timeout=args.timeout,
        test_runtime=test_runtime,
        frontends=args.frontends,
        include_for=args.include_for,
        include_cfg=args.include_cfg
    )
    
    try:
        if args.test_files:
            # Test specific files
            test_files = [project_root / "tests" / "commonclasstestcases" / "function" / f for f in args.test_files]
            all_results = []
            for test_file in test_files:
                if test_file.exists():
                    results = tester.test_single_file(test_file)
                    all_results.extend(results)
                else:
                    print(f"WARNING: Test file not found: {test_file}")
        else:
            # Run pattern-based tests
            all_results = tester.run_tests(args.pattern, args.max_tests, args.max_test_number)
        
        tester.print_summary(all_results)
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        sys.exit(1)

if __name__ == "__main__":
    main() 